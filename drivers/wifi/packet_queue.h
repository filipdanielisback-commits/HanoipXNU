#ifndef HPX_WIFI_PACKET_QUEUE_H
#define HPX_WIFI_PACKET_QUEUE_H

#include <stddef.h>
#include <stdint.h>

enum hpx_wifi_packet_direction {
    HPX_WIFI_PACKET_TX = 1,
    HPX_WIFI_PACKET_RX = 2
};

enum hpx_wifi_packet_state {
    HPX_WIFI_PACKET_FREE = 0,
    HPX_WIFI_PACKET_QUEUED,
    HPX_WIFI_PACKET_IN_FLIGHT,
    HPX_WIFI_PACKET_COMPLETE,
    HPX_WIFI_PACKET_ERROR
};

enum hpx_wifi_packet_result {
    HPX_WIFI_PACKET_OK = 0,
    HPX_WIFI_PACKET_INVALID_ARGUMENT = -1,
    HPX_WIFI_PACKET_FULL = -2,
    HPX_WIFI_PACKET_EMPTY = -3,
    HPX_WIFI_PACKET_STALE_TICKET = -4,
    HPX_WIFI_PACKET_INVALID_STATE = -5
};

/* The caller owns buffer memory and must keep it valid until release(). */
struct hpx_wifi_packet {
    void *data;
    size_t length;
    size_t bytes_completed;
    uint64_t ticket;
    uint32_t device_generation;
    enum hpx_wifi_packet_state state;
    int completion_status;
};

/* Metadata and descriptor storage are caller-owned; access must be serialized. */
struct hpx_wifi_packet_queue {
    struct hpx_wifi_packet *slots;
    size_t capacity;
    size_t queued_count;
    size_t in_flight_count;
    size_t completed_count;
    uint64_t next_ticket;
    enum hpx_wifi_packet_direction direction;
};

int hpx_wifi_packet_queue_init(struct hpx_wifi_packet_queue *queue,
                               struct hpx_wifi_packet *slots,
                               size_t capacity,
                               enum hpx_wifi_packet_direction direction);
/* TX length is packet bytes; RX length is writable buffer capacity. */
int hpx_wifi_packet_submit(struct hpx_wifi_packet_queue *queue,
                           void *buffer,
                           size_t length,
                           uint32_t device_generation,
                           uint64_t *ticket_out);
/* Backend acquires the oldest queued descriptor. */
int hpx_wifi_packet_acquire(struct hpx_wifi_packet_queue *queue,
                            uint64_t *ticket_out,
                            void **buffer_out,
                            size_t *length_out,
                            uint32_t *device_generation_out);
/* Complete exactly once; bytes_completed must not exceed submitted length. */
int hpx_wifi_packet_complete(struct hpx_wifi_packet_queue *queue,
                             uint64_t ticket,
                             uint32_t device_generation,
                             size_t bytes_completed,
                             int completion_status);
/* Completion details remain available until release(). */
int hpx_wifi_packet_get_completion(
    const struct hpx_wifi_packet_queue *queue,
    uint64_t ticket,
    enum hpx_wifi_packet_state *state_out,
    size_t *bytes_completed_out,
    int *completion_status_out);
/* Only COMPLETE or ERROR descriptors may be released. */
int hpx_wifi_packet_release(struct hpx_wifi_packet_queue *queue,
                            uint64_t ticket);
/* Mark queued (not in-flight) descriptors as failed; status must be nonzero. */
int hpx_wifi_packet_cancel_queued(struct hpx_wifi_packet_queue *queue,
                                  int completion_status,
                                  size_t *cancelled_out);

size_t hpx_wifi_packet_queued(const struct hpx_wifi_packet_queue *queue);
size_t hpx_wifi_packet_in_flight(const struct hpx_wifi_packet_queue *queue);
size_t hpx_wifi_packet_completed(const struct hpx_wifi_packet_queue *queue);

#endif
