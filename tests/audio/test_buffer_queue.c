#include "buffer_queue.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int
main(void)
{
    struct hpx_audio_buffer slots[2];
    struct hpx_audio_buffer_queue queue;
    uint8_t a[16] = { 0 };
    uint8_t b[8] = { 0 };
    uint64_t ticket_a;
    uint64_t ticket_b;
    uint64_t acquired;
    void *data;
    size_t length;

    assert(hpx_audio_buffer_queue_init(&queue, slots, 2) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_buffer_acquire(&queue, &acquired, &data, &length) ==
           HPX_AUDIO_QUEUE_EMPTY);

    assert(hpx_audio_buffer_submit(&queue, a, sizeof(a), &ticket_a) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_buffer_submit(&queue, b, sizeof(b), &ticket_b) ==
           HPX_AUDIO_QUEUE_OK);
    assert(ticket_a != ticket_b);
    assert(hpx_audio_buffer_submit(&queue, a, sizeof(a), &acquired) ==
           HPX_AUDIO_QUEUE_FULL);
    assert(hpx_audio_buffer_queued(&queue) == 2);

    assert(hpx_audio_buffer_acquire(&queue, &acquired, &data, &length) ==
           HPX_AUDIO_QUEUE_OK);
    assert(acquired == ticket_a);
    assert(data == a && length == sizeof(a));
    assert(hpx_audio_buffer_complete(&queue, ticket_a, 8, 0) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_buffer_complete(&queue, ticket_a, 8, 0) ==
           HPX_AUDIO_QUEUE_INVALID_STATE);
    assert(hpx_audio_buffer_release(&queue, ticket_a) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_buffer_release(&queue, ticket_a) ==
           HPX_AUDIO_QUEUE_STALE_TICKET);

    assert(hpx_audio_buffer_acquire(&queue, &acquired, &data, &length) ==
           HPX_AUDIO_QUEUE_OK);
    assert(acquired == ticket_b);
    assert(data == b && length == sizeof(b));
    assert(hpx_audio_buffer_complete(&queue, ticket_b, sizeof(b) + 1, 0) ==
           HPX_AUDIO_QUEUE_INVALID_ARGUMENT);
    assert(hpx_audio_buffer_complete(&queue, ticket_b, 3, -7) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_buffer_completed(&queue) == 1);
    assert(hpx_audio_buffer_release(&queue, ticket_b) ==
           HPX_AUDIO_QUEUE_OK);

    assert(hpx_audio_buffer_queued(&queue) == 0);
    assert(hpx_audio_buffer_in_flight(&queue) == 0);
    assert(hpx_audio_buffer_completed(&queue) == 0);
    assert(hpx_audio_buffer_queue_init(0, slots, 2) ==
           HPX_AUDIO_QUEUE_INVALID_ARGUMENT);
    assert(hpx_audio_buffer_queue_init(&queue, slots, 0) ==
           HPX_AUDIO_QUEUE_INVALID_ARGUMENT);

    puts("audio buffer queue tests passed");
    return 0;
}
