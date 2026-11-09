#include "core.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void
test_lifecycle_gates_and_packet_flow(void)
{
    struct hpx_wifi_core core;
    struct hpx_wifi_packet tx_slots[2], rx_slots[2];
    unsigned char tx[96], rx[256];
    uint64_t tx_ticket, rx_ticket, got;
    uint32_t generation;
    void *buffer;
    size_t length, bytes;
    int status;
    enum hpx_wifi_packet_state packet_state;

    assert(hpx_wifi_core_init(&core, tx_slots, 2, rx_slots, 2) ==
           HPX_WIFI_CORE_OK);
    assert(hpx_wifi_core_tx_submit(&core, tx, sizeof(tx), &tx_ticket) ==
           HPX_WIFI_PACKET_INVALID_STATE);
    assert(hpx_wifi_core_discover(&core) == HPX_WIFI_OK);
    generation = (uint32_t)core.lifecycle.generation;
    assert(hpx_wifi_core_begin_init(&core) == HPX_WIFI_OK);
    assert(hpx_wifi_core_mark_ready(&core, generation) == HPX_WIFI_OK);
    assert(hpx_wifi_core_rx_post(&core, rx, sizeof(rx), &rx_ticket) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_packet_acquire(&core, HPX_WIFI_PACKET_RX, &got,
                                        &buffer, &length, &generation) ==
           HPX_WIFI_PACKET_OK);
    assert(got == rx_ticket && buffer == rx && length == sizeof(rx));
    assert(hpx_wifi_core_packet_complete(&core, HPX_WIFI_PACKET_RX, rx_ticket,
                                         generation, 128, 0) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_packet_get_completion(&core, HPX_WIFI_PACKET_RX,
                                               rx_ticket, &packet_state,
                                               &bytes, &status) == HPX_WIFI_PACKET_OK);
    assert(packet_state == HPX_WIFI_PACKET_COMPLETE && bytes == 128 && status == 0);
    assert(hpx_wifi_core_packet_release(&core, HPX_WIFI_PACKET_RX, rx_ticket) ==
           HPX_WIFI_PACKET_OK);

    assert(hpx_wifi_core_connect_begin(&core, 41) == HPX_WIFI_OK);
    assert(hpx_wifi_core_connect_complete(&core, generation, 41, 0) == HPX_WIFI_OK);
    assert(hpx_wifi_core_tx_submit(&core, tx, sizeof(tx), &tx_ticket) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_packet_acquire(&core, HPX_WIFI_PACKET_TX, &got,
                                        &buffer, &length, &generation) ==
           HPX_WIFI_PACKET_OK);
    assert(got == tx_ticket && buffer == tx && length == sizeof(tx));
    assert(hpx_wifi_core_packet_complete(&core, HPX_WIFI_PACKET_TX, tx_ticket,
                                         generation, sizeof(tx), 0) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_packet_release(&core, HPX_WIFI_PACKET_TX, tx_ticket) ==
           HPX_WIFI_PACKET_OK);
}

static void
test_stop_cancels_queued_and_waits_for_drain(void)
{
    struct hpx_wifi_core core;
    struct hpx_wifi_packet tx_slots[2], rx_slots[2];
    unsigned char tx_a[32], tx_b[32], rx_a[64];
    uint64_t queued_ticket, active_ticket, rx_ticket;
    uint32_t generation;
    void *buffer;
    size_t length;
    int status;
    enum hpx_wifi_packet_state packet_state;

    assert(hpx_wifi_core_init(&core, tx_slots, 2, rx_slots, 1) ==
           HPX_WIFI_CORE_OK);
    assert(hpx_wifi_core_discover(&core) == HPX_WIFI_OK);
    generation = (uint32_t)core.lifecycle.generation;
    assert(hpx_wifi_core_begin_init(&core) == HPX_WIFI_OK);
    assert(hpx_wifi_core_mark_ready(&core, generation) == HPX_WIFI_OK);
    assert(hpx_wifi_core_connect_begin(&core, 7) == HPX_WIFI_OK);
    assert(hpx_wifi_core_connect_complete(&core, generation, 7, 0) == HPX_WIFI_OK);

    assert(hpx_wifi_core_tx_submit(&core, tx_a, sizeof(tx_a), &active_ticket) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_tx_submit(&core, tx_b, sizeof(tx_b), &queued_ticket) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_packet_acquire(&core, HPX_WIFI_PACKET_TX, &active_ticket,
                                        &buffer, &length, &generation) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_rx_post(&core, rx_a, sizeof(rx_a), &rx_ticket) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_begin_stop(&core) == HPX_WIFI_OK);
    assert(hpx_wifi_core_tx_submit(&core, tx_a, sizeof(tx_a), &queued_ticket) ==
           HPX_WIFI_PACKET_INVALID_STATE);
    assert(hpx_wifi_core_detach(&core) == HPX_WIFI_CORE_INVALID_STATE);
    assert(hpx_wifi_core_packet_get_completion(&core, HPX_WIFI_PACKET_TX,
                                               queued_ticket, &packet_state,
                                               &length, &status) ==
           HPX_WIFI_PACKET_OK);
    assert(packet_state == HPX_WIFI_PACKET_ERROR &&
           status == HPX_WIFI_CORE_CANCELLED);
    assert(hpx_wifi_core_packet_release(&core, HPX_WIFI_PACKET_TX,
                                        queued_ticket) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_packet_get_completion(&core, HPX_WIFI_PACKET_RX,
                                               rx_ticket, &packet_state,
                                               &length, &status) ==
           HPX_WIFI_PACKET_OK);
    assert(status == HPX_WIFI_CORE_CANCELLED);
    assert(hpx_wifi_core_packet_release(&core, HPX_WIFI_PACKET_RX, rx_ticket) ==
           HPX_WIFI_PACKET_OK);

    /* In-flight work is not fabricated as cancelled; backend must confirm it. */
    assert(hpx_wifi_core_packet_complete(&core, HPX_WIFI_PACKET_TX, active_ticket,
                                         generation, 0, -5) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_packet_release(&core, HPX_WIFI_PACKET_TX, active_ticket) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_detach(&core) == HPX_WIFI_OK);
}

static void
test_disconnect_cancels_unsent_tx(void)
{
    struct hpx_wifi_core core;
    struct hpx_wifi_packet tx_slots[2], rx_slots[1];
    unsigned char tx[24];
    uint64_t ticket;
    uint32_t generation;
    enum hpx_wifi_packet_state packet_state;
    size_t bytes;
    int status;

    assert(hpx_wifi_core_init(&core, tx_slots, 2, rx_slots, 1) == HPX_WIFI_CORE_OK);
    assert(hpx_wifi_core_discover(&core) == HPX_WIFI_OK);
    generation = (uint32_t)core.lifecycle.generation;
    assert(hpx_wifi_core_begin_init(&core) == HPX_WIFI_OK);
    assert(hpx_wifi_core_mark_ready(&core, generation) == HPX_WIFI_OK);
    assert(hpx_wifi_core_connect_begin(&core, 9) == HPX_WIFI_OK);
    assert(hpx_wifi_core_connect_complete(&core, generation, 9, 0) == HPX_WIFI_OK);
    assert(hpx_wifi_core_tx_submit(&core, tx, sizeof(tx), &ticket) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_disconnect_begin(&core, 10) == HPX_WIFI_OK);
    assert(hpx_wifi_core_packet_get_completion(&core, HPX_WIFI_PACKET_TX, ticket,
                                               &packet_state, &bytes, &status) ==
           HPX_WIFI_PACKET_OK);
    assert(packet_state == HPX_WIFI_PACKET_ERROR &&
           status == HPX_WIFI_CORE_CANCELLED);
    assert(hpx_wifi_core_packet_release(&core, HPX_WIFI_PACKET_TX, ticket) ==
           HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_disconnect_complete(&core, generation, 10, 0) == HPX_WIFI_OK);
}

static void
test_failure_cancels_queued(void)
{
    struct hpx_wifi_core core;
    struct hpx_wifi_packet tx_slots[1], rx_slots[1];
    unsigned char tx[16];
    uint64_t ticket;
    uint32_t generation;
    enum hpx_wifi_packet_state packet_state;
    size_t bytes;
    int status;

    assert(hpx_wifi_core_init(&core, tx_slots, 1, rx_slots, 1) == HPX_WIFI_CORE_OK);
    assert(hpx_wifi_core_discover(&core) == HPX_WIFI_OK);
    generation = (uint32_t)core.lifecycle.generation;
    assert(hpx_wifi_core_begin_init(&core) == HPX_WIFI_OK);
    assert(hpx_wifi_core_mark_ready(&core, generation) == HPX_WIFI_OK);
    assert(hpx_wifi_core_connect_begin(&core, 1) == HPX_WIFI_OK);
    assert(hpx_wifi_core_connect_complete(&core, generation, 1, 0) == HPX_WIFI_OK);
    assert(hpx_wifi_core_tx_submit(&core, tx, sizeof(tx), &ticket) == HPX_WIFI_PACKET_OK);
    assert(hpx_wifi_core_fail(&core, generation, -88) == HPX_WIFI_OK);
    assert(hpx_wifi_core_packet_get_completion(&core, HPX_WIFI_PACKET_TX, ticket,
                                               &packet_state, &bytes, &status) ==
           HPX_WIFI_PACKET_OK);
    assert(packet_state == HPX_WIFI_PACKET_ERROR && status == -88);
    assert(hpx_wifi_core_packet_release(&core, HPX_WIFI_PACKET_TX, ticket) ==
           HPX_WIFI_PACKET_OK);
}

static void
test_distinct_descriptor_storage_required(void)
{
    struct hpx_wifi_core core;
    struct hpx_wifi_packet slots[2];

    assert(hpx_wifi_core_init(&core, slots, 1, slots, 1) ==
           HPX_WIFI_CORE_INVALID_ARGUMENT);
}

int
main(void)
{
    test_lifecycle_gates_and_packet_flow();
    test_stop_cancels_queued_and_waits_for_drain();
    test_failure_cancels_queued();
    test_disconnect_cancels_unsent_tx();
    test_distinct_descriptor_storage_required();
    puts("wifi core coordinator tests: ok");
    return 0;
}
