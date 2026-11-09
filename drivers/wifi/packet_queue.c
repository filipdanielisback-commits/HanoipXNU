#include "packet_queue.h"

static int
hpx_wifi_packet_queue_valid(const struct hpx_wifi_packet_queue *queue)
{
    return queue != 0 && queue->slots != 0 && queue->capacity != 0;
}

static struct hpx_wifi_packet *
hpx_wifi_packet_find(struct hpx_wifi_packet_queue *queue, uint64_t ticket)
{
    size_t i;

    for (i = 0; i < queue->capacity; i++) {
        if (queue->slots[i].ticket == ticket &&
            queue->slots[i].state != HPX_WIFI_PACKET_FREE) {
            return &queue->slots[i];
        }
    }
    return 0;
}

static const struct hpx_wifi_packet *
hpx_wifi_packet_find_const(const struct hpx_wifi_packet_queue *queue,
                           uint64_t ticket)
{
    size_t i;

    for (i = 0; i < queue->capacity; i++) {
        if (queue->slots[i].ticket == ticket &&
            queue->slots[i].state != HPX_WIFI_PACKET_FREE) {
            return &queue->slots[i];
        }
    }
    return 0;
}

int
hpx_wifi_packet_queue_init(struct hpx_wifi_packet_queue *queue,
                           struct hpx_wifi_packet *slots,
                           size_t capacity,
                           enum hpx_wifi_packet_direction direction)
{
    size_t i;

    if (queue == 0 || slots == 0 || capacity == 0 ||
        (direction != HPX_WIFI_PACKET_TX && direction != HPX_WIFI_PACKET_RX)) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }

    queue->slots = slots;
    queue->capacity = capacity;
    queue->queued_count = 0;
    queue->in_flight_count = 0;
    queue->completed_count = 0;
    queue->next_ticket = 1U;
    queue->direction = direction;
    for (i = 0; i < capacity; i++) {
        slots[i].data = 0;
        slots[i].length = 0;
        slots[i].bytes_completed = 0;
        slots[i].ticket = 0U;
        slots[i].device_generation = 0U;
        slots[i].state = HPX_WIFI_PACKET_FREE;
        slots[i].completion_status = 0;
    }
    return HPX_WIFI_PACKET_OK;
}

int
hpx_wifi_packet_submit(struct hpx_wifi_packet_queue *queue,
                       void *buffer,
                       size_t length,
                       uint32_t device_generation,
                       uint64_t *ticket_out)
{
    size_t i;

    if (!hpx_wifi_packet_queue_valid(queue) || buffer == 0 || length == 0 ||
        device_generation == 0U || ticket_out == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    if (queue->next_ticket == 0U) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    if (queue->queued_count + queue->in_flight_count +
        queue->completed_count >= queue->capacity) {
        return HPX_WIFI_PACKET_FULL;
    }

    for (i = 0; i < queue->capacity; i++) {
        struct hpx_wifi_packet *slot = &queue->slots[i];
        if (slot->state == HPX_WIFI_PACKET_FREE) {
            slot->data = buffer;
            slot->length = length;
            slot->bytes_completed = 0;
            slot->ticket = queue->next_ticket;
            slot->device_generation = device_generation;
            slot->state = HPX_WIFI_PACKET_QUEUED;
            slot->completion_status = 0;
            *ticket_out = queue->next_ticket;
            if (queue->next_ticket == UINT64_MAX) {
                queue->next_ticket = 0U;
            } else {
                queue->next_ticket++;
            }
            queue->queued_count++;
            return HPX_WIFI_PACKET_OK;
        }
    }
    return HPX_WIFI_PACKET_FULL;
}

int
hpx_wifi_packet_acquire(struct hpx_wifi_packet_queue *queue,
                        uint64_t *ticket_out,
                        void **buffer_out,
                        size_t *length_out,
                        uint32_t *device_generation_out)
{
    size_t i;
    struct hpx_wifi_packet *oldest = 0;

    if (!hpx_wifi_packet_queue_valid(queue) || ticket_out == 0 ||
        buffer_out == 0 || length_out == 0 || device_generation_out == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    if (queue->queued_count == 0) {
        return HPX_WIFI_PACKET_EMPTY;
    }

    for (i = 0; i < queue->capacity; i++) {
        struct hpx_wifi_packet *slot = &queue->slots[i];
        if (slot->state == HPX_WIFI_PACKET_QUEUED &&
            (oldest == 0 || slot->ticket < oldest->ticket)) {
            oldest = slot;
        }
    }
    if (oldest == 0) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }

    oldest->state = HPX_WIFI_PACKET_IN_FLIGHT;
    queue->queued_count--;
    queue->in_flight_count++;
    *ticket_out = oldest->ticket;
    *buffer_out = oldest->data;
    *length_out = oldest->length;
    *device_generation_out = oldest->device_generation;
    return HPX_WIFI_PACKET_OK;
}

int
hpx_wifi_packet_complete(struct hpx_wifi_packet_queue *queue,
                         uint64_t ticket,
                         uint32_t device_generation,
                         size_t bytes_completed,
                         int completion_status)
{
    struct hpx_wifi_packet *slot;

    if (!hpx_wifi_packet_queue_valid(queue) || ticket == 0U ||
        device_generation == 0U) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    slot = hpx_wifi_packet_find(queue, ticket);
    if (slot == 0) {
        return HPX_WIFI_PACKET_STALE_TICKET;
    }
    if (slot->device_generation != device_generation) {
        return HPX_WIFI_PACKET_STALE_TICKET;
    }
    if (slot->state != HPX_WIFI_PACKET_IN_FLIGHT) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    if (bytes_completed > slot->length) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }

    slot->bytes_completed = bytes_completed;
    slot->completion_status = completion_status;
    slot->state = (completion_status == 0) ?
        HPX_WIFI_PACKET_COMPLETE : HPX_WIFI_PACKET_ERROR;
    queue->in_flight_count--;
    queue->completed_count++;
    return HPX_WIFI_PACKET_OK;
}

int
hpx_wifi_packet_get_completion(
    const struct hpx_wifi_packet_queue *queue,
    uint64_t ticket,
    enum hpx_wifi_packet_state *state_out,
    size_t *bytes_completed_out,
    int *completion_status_out)
{
    const struct hpx_wifi_packet *slot;

    if (!hpx_wifi_packet_queue_valid(queue) || ticket == 0U ||
        state_out == 0 || bytes_completed_out == 0 ||
        completion_status_out == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    slot = hpx_wifi_packet_find_const(queue, ticket);
    if (slot == 0) {
        return HPX_WIFI_PACKET_STALE_TICKET;
    }
    if (slot->state != HPX_WIFI_PACKET_COMPLETE &&
        slot->state != HPX_WIFI_PACKET_ERROR) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    *state_out = slot->state;
    *bytes_completed_out = slot->bytes_completed;
    *completion_status_out = slot->completion_status;
    return HPX_WIFI_PACKET_OK;
}

int
hpx_wifi_packet_release(struct hpx_wifi_packet_queue *queue, uint64_t ticket)
{
    struct hpx_wifi_packet *slot;

    if (!hpx_wifi_packet_queue_valid(queue) || ticket == 0U) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    slot = hpx_wifi_packet_find(queue, ticket);
    if (slot == 0) {
        return HPX_WIFI_PACKET_STALE_TICKET;
    }
    if (slot->state != HPX_WIFI_PACKET_COMPLETE &&
        slot->state != HPX_WIFI_PACKET_ERROR) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }

    slot->data = 0;
    slot->length = 0;
    slot->bytes_completed = 0;
    slot->ticket = 0U;
    slot->device_generation = 0U;
    slot->state = HPX_WIFI_PACKET_FREE;
    slot->completion_status = 0;
    queue->completed_count--;
    return HPX_WIFI_PACKET_OK;
}

int
hpx_wifi_packet_cancel_queued(struct hpx_wifi_packet_queue *queue,
                              int completion_status,
                              size_t *cancelled_out)
{
    size_t i;
    size_t cancelled = 0;

    if (!hpx_wifi_packet_queue_valid(queue) || completion_status == 0 ||
        cancelled_out == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    for (i = 0; i < queue->capacity; i++) {
        struct hpx_wifi_packet *slot = &queue->slots[i];
        if (slot->state == HPX_WIFI_PACKET_QUEUED) {
            slot->bytes_completed = 0;
            slot->completion_status = completion_status;
            slot->state = HPX_WIFI_PACKET_ERROR;
            queue->queued_count--;
            queue->completed_count++;
            cancelled++;
        }
    }
    *cancelled_out = cancelled;
    return HPX_WIFI_PACKET_OK;
}

size_t
hpx_wifi_packet_queued(const struct hpx_wifi_packet_queue *queue)
{
    return hpx_wifi_packet_queue_valid(queue) ? queue->queued_count : 0U;
}

size_t
hpx_wifi_packet_in_flight(const struct hpx_wifi_packet_queue *queue)
{
    return hpx_wifi_packet_queue_valid(queue) ? queue->in_flight_count : 0U;
}

size_t
hpx_wifi_packet_completed(const struct hpx_wifi_packet_queue *queue)
{
    return hpx_wifi_packet_queue_valid(queue) ? queue->completed_count : 0U;
}

#endif
