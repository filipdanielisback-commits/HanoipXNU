#ifndef HPX_AUDIO_BUFFER_QUEUE_H
#define HPX_AUDIO_BUFFER_QUEUE_H

#include <stddef.h>
#include <stdint.h>

enum hpx_audio_buffer_state {
    HPX_AUDIO_BUFFER_FREE = 0,
    HPX_AUDIO_BUFFER_QUEUED,
    HPX_AUDIO_BUFFER_IN_FLIGHT,
    HPX_AUDIO_BUFFER_COMPLETE,
    HPX_AUDIO_BUFFER_ERROR
};

enum hpx_audio_queue_result {
    HPX_AUDIO_QUEUE_OK = 0,
    HPX_AUDIO_QUEUE_INVALID_ARGUMENT = -1,
    HPX_AUDIO_QUEUE_FULL = -2,
    HPX_AUDIO_QUEUE_EMPTY = -3,
    HPX_AUDIO_QUEUE_STALE_TICKET = -4,
    HPX_AUDIO_QUEUE_INVALID_STATE = -5
};

/* Buffer memory remains owned by the caller for the whole queued lifetime. */
struct hpx_audio_buffer {
    void *data;
    size_t length;
    size_t bytes_transferred;
    uint64_t ticket;
    enum hpx_audio_buffer_state state;
    int completion_status;
};

/* Queue metadata is caller-allocated; no internal allocation or locking. */
struct hpx_audio_buffer_queue {
    struct hpx_audio_buffer *slots;
    size_t capacity;
    size_t queued_count;
    size_t in_flight_count;
    size_t completed_count;
    size_t submit_cursor;
    size_t dispatch_cursor;
    uint32_t next_generation;
};

int hpx_audio_buffer_queue_init(struct hpx_audio_buffer_queue *queue,
                                struct hpx_audio_buffer *slots,
                                size_t capacity);
int hpx_audio_buffer_submit(struct hpx_audio_buffer_queue *queue,
                            void *data,
                            size_t length,
                            uint64_t *ticket_out);
int hpx_audio_buffer_acquire(struct hpx_audio_buffer_queue *queue,
                             uint64_t *ticket_out,
                             void **data_out,
                             size_t *length_out);
int hpx_audio_buffer_complete(struct hpx_audio_buffer_queue *queue,
                              uint64_t ticket,
                              size_t bytes_transferred,
                              int completion_status);
/* Read completion data before releasing the descriptor. */
int hpx_audio_buffer_get_completion(
    const struct hpx_audio_buffer_queue *queue,
    uint64_t ticket,
    enum hpx_audio_buffer_state *state_out,
    size_t *bytes_transferred_out,
    int *completion_status_out);
int hpx_audio_buffer_release(struct hpx_audio_buffer_queue *queue,
                             uint64_t ticket);

size_t hpx_audio_buffer_queued(const struct hpx_audio_buffer_queue *queue);
size_t hpx_audio_buffer_in_flight(const struct hpx_audio_buffer_queue *queue);
size_t hpx_audio_buffer_completed(const struct hpx_audio_buffer_queue *queue);

#endif
