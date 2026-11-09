#ifndef HPX_WIFI_CORE_H
#define HPX_WIFI_CORE_H

#include "packet_queue.h"
#include "state.h"

#include <stddef.h>
#include <stdint.h>

enum hpx_wifi_core_result {
    HPX_WIFI_CORE_OK = 0,
    HPX_WIFI_CORE_INVALID_ARGUMENT = -1,
    HPX_WIFI_CORE_INVALID_STATE = -2
};

#define HPX_WIFI_CORE_CANCELLED (-1001)

struct hpx_wifi_core {
    struct hpx_wifi_state_data lifecycle;
    struct hpx_wifi_packet_queue tx_queue;
    struct hpx_wifi_packet_queue rx_queue;
    int initialized;
};

/*
 * The caller owns distinct TX/RX descriptor arrays and packet buffers. All
 * calls must be serialized. Reinitialization is permitted only after detach and after every
 * backend callback has been quiesced.
 */
int hpx_wifi_core_init(struct hpx_wifi_core *core,
                       struct hpx_wifi_packet *tx_slots, size_t tx_capacity,
                       struct hpx_wifi_packet *rx_slots, size_t rx_capacity);

int hpx_wifi_core_discover(struct hpx_wifi_core *core);
int hpx_wifi_core_begin_init(struct hpx_wifi_core *core);
int hpx_wifi_core_mark_ready(struct hpx_wifi_core *core, uint32_t generation);

int hpx_wifi_core_scan_begin(struct hpx_wifi_core *core, unsigned int request_id);
int hpx_wifi_core_scan_complete(struct hpx_wifi_core *core, uint32_t generation,
                                unsigned int request_id, int result);
int hpx_wifi_core_connect_begin(struct hpx_wifi_core *core, unsigned int request_id);
int hpx_wifi_core_connect_complete(struct hpx_wifi_core *core, uint32_t generation,
                                   unsigned int request_id, int result);
int hpx_wifi_core_disconnect_begin(struct hpx_wifi_core *core,
                                   unsigned int request_id);
int hpx_wifi_core_disconnect_complete(struct hpx_wifi_core *core,
                                      uint32_t generation,
                                      unsigned int request_id, int result);
int hpx_wifi_core_suspend_begin(struct hpx_wifi_core *core);
int hpx_wifi_core_suspend_complete(struct hpx_wifi_core *core,
                                   uint32_t generation, int result);
int hpx_wifi_core_resume_begin(struct hpx_wifi_core *core);
int hpx_wifi_core_resume_complete(struct hpx_wifi_core *core,
                                  uint32_t generation, int result);

/* TX submission requires LINK_UP. RX buffers may be posted in READY or LINK_UP. */
int hpx_wifi_core_tx_submit(struct hpx_wifi_core *core, void *buffer,
                            size_t length, uint64_t *ticket_out);
int hpx_wifi_core_rx_post(struct hpx_wifi_core *core, void *buffer,
                          size_t capacity, uint64_t *ticket_out);

/* Backend-side acquisition. TX requires LINK_UP; RX may run in READY/LINK_UP. */
int hpx_wifi_core_packet_acquire(struct hpx_wifi_core *core,
                                 enum hpx_wifi_packet_direction direction,
                                 uint64_t *ticket_out, void **buffer_out,
                                 size_t *length_out, uint32_t *generation_out);

/* Completions remain valid during FAILED/STOPPING so teardown can drain safely. */
int hpx_wifi_core_packet_complete(struct hpx_wifi_core *core,
                                  enum hpx_wifi_packet_direction direction,
                                  uint64_t ticket, uint32_t generation,
                                  size_t bytes_completed, int status);
int hpx_wifi_core_packet_get_completion(
    const struct hpx_wifi_core *core,
    enum hpx_wifi_packet_direction direction, uint64_t ticket,
    enum hpx_wifi_packet_state *state_out, size_t *bytes_completed_out,
    int *status_out);
int hpx_wifi_core_packet_release(struct hpx_wifi_core *core,
                                 enum hpx_wifi_packet_direction direction,
                                 uint64_t ticket);

/* Failure/stop cancel queued descriptors, but never pretend in-flight I/O stopped. */
int hpx_wifi_core_fail(struct hpx_wifi_core *core, uint32_t generation,
                       int error_code);
int hpx_wifi_core_begin_stop(struct hpx_wifi_core *core);
/* Detach succeeds only after every TX/RX descriptor has been released. */
int hpx_wifi_core_detach(struct hpx_wifi_core *core);

size_t hpx_wifi_core_tx_queued(const struct hpx_wifi_core *core);
size_t hpx_wifi_core_rx_queued(const struct hpx_wifi_core *core);
size_t hpx_wifi_core_in_flight(const struct hpx_wifi_core *core);
size_t hpx_wifi_core_completed(const struct hpx_wifi_core *core);

#endif
