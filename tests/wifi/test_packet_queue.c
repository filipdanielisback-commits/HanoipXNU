#include "packet_queue.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static void
test_tx_fifo_capacity_and_ownership(void)
{
    struct hpx_wifi_packet slots[2];
    struct hpx_wifi_packet_queue queue;
    unsigned char a[64], b[64], c[64];
    uint64_t ta, tb, tc, got;
    uint32_t generation;
    void *buffer;
    size_t length;

    assert(hpx_wifi_packet_queue_init(&queue, slots, 2,
                                      HPX_WIFI_PACKET_TX) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_submit(&queue, a, sizeof(a), 7, &ta) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_submit(&queue, b, sizeof(b), 7, &tb) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_submit(&queue, c, sizeof(c), 7, &tc) == HPX_WIFI_PACKET_FULL);
    assert(hpx_wifi_packet_queued(&queue) == 2);

    assert(hpx_wifi_packet_acquire(&queue, &got, &buffer, &length,
                                   &generation) == HPX_WIFI_PACKET_OK);
    assert(got == ta && buffer == a && length == sizeof(a) && generation == 7);
    assert(hpx_wifi_packet_acquire(&queue, &got, &buffer, &length,
                                   &generation) == HPX_WIFI_PACKET_OK);
    assert(got == tb && buffer == b);
    assert(hpx_wifi_packet_acquire(&queue, &got, &buffer, &length,
                                   &generation) == HPX_WIFI_PACKET_EMPTY);

    /* Queue retains the caller's buffer; it never copies or frees it. */
    assert(hpx_wifi_packet_release(&queue, ta) == HPX_WIFI_PACKET_INVALID_STATE);
    assert(hpx_wifi_packet_complete(&queue, tb, 7, sizeof(b), 0) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_complete(&queue, ta, 7, sizeof(a), 0) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_release(&queue, tb) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_release(&queue, ta) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_queued(&queue) == 0);
    assert(hpx_wifi_packet_in_flight(&queue) == 0);
    assert(hpx_wifi_packet_completed(&queue) == 0);
    assert(hpx_wifi_packet_submit(&queue, c, sizeof(c), 7, &tc) == HPX_WIFI_PACKET_OK);
    assert(tc > tb);
}

static void
test_rx_completion_and_validation(void)
{
    struct hpx_wifi_packet slots[1];
    struct hpx_wifi_packet_queue queue;
    unsigned char rx[128];
    uint64_t ticket, got;
    uint32_t generation;
    void *buffer;
    size_t length, bytes;
    int status;
    enum hpx_wifi_packet_state state;

    assert(hpx_wifi_packet_queue_init(&queue, slots, 1,
                                      HPX_WIFI_PACKET_RX) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_submit(&queue, rx, sizeof(rx), 11, &ticket) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_get_completion(&queue, ticket, &state, &bytes,
                                          &status) == HPX_WIFI_PACKET_INVALID_STATE);
    assert(hpx_wifi_packet_acquire(&queue, &got, &buffer, &length,
                                   &generation) == HPX_WIFI_PACKET_OK);
    assert(got == ticket && buffer == rx && length == sizeof(rx) && generation == 11);
    assert(hpx_wifi_packet_complete(&queue, ticket, 11, sizeof(rx) + 1U, 0) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
    assert(hpx_wifi_packet_complete(&queue, ticket, 10, 64, 0) ==
           HPX_WIFI_PACKET_STALE_TICKET);
    assert(hpx_wifi_packet_complete(&queue, ticket, 11, 64, 0) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_complete(&queue, ticket, 11, 64, 0) ==
           HPX_WIFI_PACKET_INVALID_STATE);
    assert(hpx_wifi_packet_get_completion(&queue, ticket, &state, &bytes,
                                          &status) == HPX_WIFI_PACKET_OK);
    assert(state == HPX_WIFI_PACKET_COMPLETE && bytes == 64 && status == 0);
    assert(hpx_wifi_packet_release(&queue, ticket) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_get_completion(&queue, ticket, &state, &bytes,
                                          &status) == HPX_WIFI_PACKET_STALE_TICKET);
}

static void
test_cancel_and_error_release(void)
{
    struct hpx_wifi_packet slots[3];
    struct hpx_wifi_packet_queue queue;
    unsigned char buffers[3][32];
    uint64_t tickets[3], in_flight;
    uint32_t generation;
    void *buffer;
    size_t length, cancelled;
    int i, status;
    enum hpx_wifi_packet_state state;

    assert(hpx_wifi_packet_queue_init(&queue, slots, 3,
                                      HPX_WIFI_PACKET_TX) == HPX_WIFI_PACKET_OK);
    for (i = 0; i < 3; i++) {
        assert(hpx_wifi_packet_submit(&queue, buffers[i], sizeof(buffers[i]),
                                      19, &tickets[i]) == HPX_WIFI_PACKET_OK);
    }
    assert(hpx_wifi_packet_acquire(&queue, &in_flight, &buffer, &length,
                                   &generation) == HPX_WIFI_PACKET_OK);
    assert(in_flight == tickets[0]);
    assert(hpx_wifi_packet_cancel_queued(&queue, -77, &cancelled) == HPX_WIFI_PACKET_OK);
    assert(cancelled == 2);
    assert(hpx_wifi_packet_queued(&queue) == 0);
    assert(hpx_wifi_packet_in_flight(&queue) == 1);
    assert(hpx_wifi_packet_complete(&queue, in_flight, 19, 0, -9) == HPX_WIFI_PACKET_OK);
    for (i = 0; i < 3; i++) {
        assert(hpx_wifi_packet_get_completion(&queue, tickets[i], &state,
                                              &length, &status) == HPX_WIFI_PACKET_OK);
        assert(state == HPX_WIFI_PACKET_ERROR);
        assert(status == (i == 0 ? -9 : -77));
        assert(hpx_wifi_packet_release(&queue, tickets[i]) == HPX_WIFI_PACKET_OK);
    }
    assert(hpx_wifi_packet_cancel_queued(&queue, 0, &cancelled) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
}

static void
test_invalid_arguments(void)
{
    struct hpx_wifi_packet slots[1];
    struct hpx_wifi_packet_queue queue;
    unsigned char data[8];
    uint64_t ticket;

    assert(hpx_wifi_packet_queue_init(0, slots, 1, HPX_WIFI_PACKET_TX) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
    assert(hpx_wifi_packet_queue_init(&queue, slots, 0, HPX_WIFI_PACKET_TX) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
    assert(hpx_wifi_packet_queue_init(&queue, slots, 1, 0) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
    assert(hpx_wifi_packet_queue_init(&queue, slots, 1, HPX_WIFI_PACKET_TX) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_packet_submit(&queue, 0, sizeof(data), 1, &ticket) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
    assert(hpx_wifi_packet_submit(&queue, data, 0, 1, &ticket) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
    assert(hpx_wifi_packet_submit(&queue, data, sizeof(data), 0, &ticket) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
    assert(hpx_wifi_packet_submit(&queue, data, sizeof(data), 1, 0) ==
           HPX_WIFI_PACKET_INVALID_ARGUMENT);
}

int
main(void)
{
    test_tx_fifo_capacity_and_ownership();
    test_rx_completion_and_validation();
    test_cancel_and_error_release();
    test_invalid_arguments();
    puts("wifi packet queue tests: ok");
    return 0;
}
