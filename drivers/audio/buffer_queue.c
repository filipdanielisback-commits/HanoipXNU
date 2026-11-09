#include "buffer_queue.h"

static int
hpx_audio_queue_valid(const struct hpx_audio_buffer_queue *queue)
{
    return queue != 0 && queue->slots != 0 && queue->capacity != 0;
}

static uint64_t
hpx_audio_make_ticket(uint32_t generation, size_t index)
{
    return ((uint64_t)generation << 32) | (uint32_t)index;
}

static const struct hpx_audio_buffer *
hpx_audio_find_ticket_const(const struct hpx_audio_buffer_queue *queue,
                            uint64_t ticket)
{
    uint32_t index = (uint32_t)(ticket & UINT32_MAX);
    uint32_t generation = (uint32_t)(ticket >> 32);
    const struct hpx_audio_buffer *slot;

    if (generation == 0 || (size_t)index >= queue->capacity) {
        return 0;
    }
    slot = &queue->slots[index];
    if (slot->ticket != ticket || slot->state == HPX_AUDIO_BUFFER_FREE) {
        return 0;
    }
    return slot;
}

static struct hpx_audio_buffer *
hpx_audio_find_ticket(struct hpx_audio_buffer_queue *queue, uint64_t ticket)
{
    return (struct hpx_audio_buffer *)
        hpx_audio_find_ticket_const(queue, ticket);
}

int
hpx_audio_buffer_queue_init(struct hpx_audio_buffer_queue *queue,
                            struct hpx_audio_buffer *slots,
                            size_t capacity)
{
    size_t i;

    if (queue == 0 || slots == 0 || capacity == 0 ||
        capacity > (size_t)UINT32_MAX) {
        return HPX_AUDIO_QUEUE_INVALID_ARGUMENT;
    }
    queue->slots = slots;
    queue->capacity = capacity;
    queue->queued_count = 0;
    queue->in_flight_count = 0;
    queue->completed_count = 0;
    queue->submit_cursor = 0;
    queue->dispatch_cursor = 0;
    queue->next_generation = 1;

    for (i = 0; i < capacity; ++i) {
        slots[i].data = 0;
        slots[i].length = 0;
        slots[i].bytes_transferred = 0;
        slots[i].ticket = 0;
        slots[i].state = HPX_AUDIO_BUFFER_FREE;
        slots[i].completion_status = 0;
    }
    return HPX_AUDIO_QUEUE_OK;
}

int
hpx_audio_buffer_submit(struct hpx_audio_buffer_queue *queue,
                        void *data,
                        size_t length,
                        uint64_t *ticket_out)
{
    size_t offset, i;
    struct hpx_audio_buffer *slot;
    uint32_t generation;

    if (!hpx_audio_queue_valid(queue) || data == 0 || length == 0 ||
        ticket_out == 0) {
        return HPX_AUDIO_QUEUE_INVALID_ARGUMENT;
    }
    if (queue->queued_count + queue->in_flight_count +
        queue->completed_count >= queue->capacity) {
        return HPX_AUDIO_QUEUE_FULL;
    }

    for (offset = 0; offset < queue->capacity; ++offset) {
        i = (queue->submit_cursor + offset) % queue->capacity;
        slot = &queue->slots[i];
        if (slot->state != HPX_AUDIO_BUFFER_FREE) {
            continue;
        }
        generation = queue->next_generation++;
        if (generation == 0) {
            generation = queue->next_generation++;
        }
        slot->data = data;
        slot->length = length;
        slot->bytes_transferred = 0;
        slot->ticket = hpx_audio_make_ticket(generation, i);
        slot->state = HPX_AUDIO_BUFFER_QUEUED;
        slot->completion_status = 0;
        queue->queued_count++;
        queue->submit_cursor = (i + 1) % queue->capacity;
        *ticket_out = slot->ticket;
        return HPX_AUDIO_QUEUE_OK;
    }
    return HPX_AUDIO_QUEUE_FULL;
}

int
hpx_audio_buffer_acquire(struct hpx_audio_buffer_queue *queue,
                         uint64_t *ticket_out,
                         void **data_out,
                         size_t *length_out)
{
    size_t offset, i;
    struct hpx_audio_buffer *slot;

    if (!hpx_audio_queue_valid(queue) || ticket_out == 0 ||
        data_out == 0 || length_out == 0) {
        return HPX_AUDIO_QUEUE_INVALID_ARGUMENT;
    }
    if (queue->queued_count == 0) {
        return HPX_AUDIO_QUEUE_EMPTY;
    }
    for (offset = 0; offset < queue->capacity; ++offset) {
        i = (queue->dispatch_cursor + offset) % queue->capacity;
        slot = &queue->slots[i];
        if (slot->state != HPX_AUDIO_BUFFER_QUEUED) {
            continue;
        }
        slot->state = HPX_AUDIO_BUFFER_IN_FLIGHT;
        queue->queued_count--;
        queue->in_flight_count++;
        queue->dispatch_cursor = (i + 1) % queue->capacity;
        *ticket_out = slot->ticket;
        *data_out = slot->data;
        *length_out = slot->length;
        return HPX_AUDIO_QUEUE_OK;
    }
    return HPX_AUDIO_QUEUE_EMPTY;
}

int
hpx_audio_buffer_complete(struct hpx_audio_buffer_queue *queue,
                          uint64_t ticket,
                          size_t bytes_transferred,
                          int completion_status)
{
    struct hpx_audio_buffer *slot;

    if (!hpx_audio_queue_valid(queue)) {
        return HPX_AUDIO_QUEUE_INVALID_ARGUMENT;
    }
    slot = hpx_audio_find_ticket(queue, ticket);
    if (slot == 0) {
        return HPX_AUDIO_QUEUE_STALE_TICKET;
    }
    if (slot->state != HPX_AUDIO_BUFFER_IN_FLIGHT) {
        return HPX_AUDIO_QUEUE_INVALID_STATE;
    }
    if (bytes_transferred > slot->length) {
        return HPX_AUDIO_QUEUE_INVALID_ARGUMENT;
    }
    slot->bytes_transferred = bytes_transferred;
    slot->completion_status = completion_status;
    slot->state = completion_status == 0 ?
                  HPX_AUDIO_BUFFER_COMPLETE : HPX_AUDIO_BUFFER_ERROR;
    queue->in_flight_count--;
    queue->completed_count++;
    return HPX_AUDIO_QUEUE_OK;
}

int
hpx_audio_buffer_get_completion(
    const struct hpx_audio_buffer_queue *queue,
    uint64_t ticket,
    enum hpx_audio_buffer_state *state_out,
    size_t *bytes_transferred_out,
    int *completion_status_out)
{
    const struct hpx_audio_buffer *slot;

    if (!hpx_audio_queue_valid(queue) || state_out == 0 ||
        bytes_transferred_out == 0 || completion_status_out == 0) {
        return HPX_AUDIO_QUEUE_INVALID_ARGUMENT;
    }
    slot = hpx_audio_find_ticket_const(queue, ticket);
    if (slot == 0) {
        return HPX_AUDIO_QUEUE_STALE_TICKET;
    }
    if (slot->state != HPX_AUDIO_BUFFER_COMPLETE &&
        slot->state != HPX_AUDIO_BUFFER_ERROR) {
        return HPX_AUDIO_QUEUE_INVALID_STATE;
    }
    *state_out = slot->state;
    *bytes_transferred_out = slot->bytes_transferred;
    *completion_status_out = slot->completion_status;
    return HPX_AUDIO_QUEUE_OK;
}

int
hpx_audio_buffer_release(struct hpx_audio_buffer_queue *queue,
                         uint64_t ticket)
{
    struct hpx_audio_buffer *slot;

    if (!hpx_audio_queue_valid(queue)) {
        return HPX_AUDIO_QUEUE_INVALID_ARGUMENT;
    }
    slot = hpx_audio_find_ticket(queue, ticket);
    if (slot == 0) {
        return HPX_AUDIO_QUEUE_STALE_TICKET;
    }
    if (slot->state != HPX_AUDIO_BUFFER_COMPLETE &&
        slot->state != HPX_AUDIO_BUFFER_ERROR) {
        return HPX_AUDIO_QUEUE_INVALID_STATE;
    }
    slot->data = 0;
    slot->length = 0;
    slot->bytes_transferred = 0;
    slot->ticket = 0;
    slot->state = HPX_AUDIO_BUFFER_FREE;
    slot->completion_status = 0;
    queue->completed_count--;
    return HPX_AUDIO_QUEUE_OK;
}

size_t
hpx_audio_buffer_queued(const struct hpx_audio_buffer_queue *queue)
{
    return hpx_audio_queue_valid(queue) ? queue->queued_count : 0;
}

size_t
hpx_audio_buffer_in_flight(const struct hpx_audio_buffer_queue *queue)
{
    return hpx_audio_queue_valid(queue) ? queue->in_flight_count : 0;
}

size_t
hpx_audio_buffer_completed(const struct hpx_audio_buffer_queue *queue)
{
    return hpx_audio_queue_valid(queue) ? queue->completed_count : 0;
}
