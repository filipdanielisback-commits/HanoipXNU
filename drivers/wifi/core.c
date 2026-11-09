#include "core.h"

static int
hpx_wifi_core_valid(const struct hpx_wifi_core *core)
{
    return core != 0 && core->initialized != 0;
}

static struct hpx_wifi_packet_queue *
hpx_wifi_core_queue(struct hpx_wifi_core *core,
                    enum hpx_wifi_packet_direction direction)
{
    if (direction == HPX_WIFI_PACKET_TX) {
        return &core->tx_queue;
    }
    if (direction == HPX_WIFI_PACKET_RX) {
        return &core->rx_queue;
    }
    return 0;
}

static const struct hpx_wifi_packet_queue *
hpx_wifi_core_queue_const(const struct hpx_wifi_core *core,
                          enum hpx_wifi_packet_direction direction)
{
    if (direction == HPX_WIFI_PACKET_TX) {
        return &core->tx_queue;
    }
    if (direction == HPX_WIFI_PACKET_RX) {
        return &core->rx_queue;
    }
    return 0;
}

static int
hpx_wifi_core_cancel_queued(struct hpx_wifi_core *core, int status)
{
    size_t cancelled;

    if (hpx_wifi_packet_cancel_queued(&core->tx_queue, status, &cancelled) !=
        HPX_WIFI_PACKET_OK) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    if (hpx_wifi_packet_cancel_queued(&core->rx_queue, status, &cancelled) !=
        HPX_WIFI_PACKET_OK) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    return HPX_WIFI_PACKET_OK;
}

int
hpx_wifi_core_init(struct hpx_wifi_core *core,
                   struct hpx_wifi_packet *tx_slots, size_t tx_capacity,
                   struct hpx_wifi_packet *rx_slots, size_t rx_capacity)
{
    if (core == 0 || tx_slots == 0 || rx_slots == 0 ||
        tx_capacity == 0U || rx_capacity == 0U) {
        return HPX_WIFI_CORE_INVALID_ARGUMENT;
    }
    core->initialized = 0;
    if (hpx_wifi_state_init(&core->lifecycle) != HPX_WIFI_OK) {
        return HPX_WIFI_CORE_INVALID_STATE;
    }
    if (hpx_wifi_packet_queue_init(&core->tx_queue, tx_slots, tx_capacity,
                                   HPX_WIFI_PACKET_TX) != HPX_WIFI_PACKET_OK ||
        hpx_wifi_packet_queue_init(&core->rx_queue, rx_slots, rx_capacity,
                                   HPX_WIFI_PACKET_RX) != HPX_WIFI_PACKET_OK) {
        return HPX_WIFI_CORE_INVALID_ARGUMENT;
    }
    core->initialized = 1;
    return HPX_WIFI_CORE_OK;
}

int
hpx_wifi_core_discover(struct hpx_wifi_core *core)
{
    return hpx_wifi_core_valid(core) ? hpx_wifi_discover(&core->lifecycle) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_begin_init(struct hpx_wifi_core *core)
{
    return hpx_wifi_core_valid(core) ? hpx_wifi_begin_init(&core->lifecycle) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_mark_ready(struct hpx_wifi_core *core, uint32_t generation)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_mark_ready(&core->lifecycle, (unsigned int)generation) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_scan_begin(struct hpx_wifi_core *core, unsigned int request_id)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_scan_begin(&core->lifecycle, request_id) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_scan_complete(struct hpx_wifi_core *core, uint32_t generation,
                            unsigned int request_id, int result)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_scan_complete(&core->lifecycle, (unsigned int)generation,
                               request_id, result) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_connect_begin(struct hpx_wifi_core *core, unsigned int request_id)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_connect_begin(&core->lifecycle, request_id) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_connect_complete(struct hpx_wifi_core *core, uint32_t generation,
                               unsigned int request_id, int result)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_connect_complete(&core->lifecycle, (unsigned int)generation,
                                  request_id, result) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_disconnect_begin(struct hpx_wifi_core *core,
                               unsigned int request_id)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_disconnect_begin(&core->lifecycle, request_id) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_disconnect_complete(struct hpx_wifi_core *core,
                                  uint32_t generation,
                                  unsigned int request_id, int result)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_disconnect_complete(&core->lifecycle,
                                     (unsigned int)generation, request_id,
                                     result) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_suspend_begin(struct hpx_wifi_core *core)
{
    return hpx_wifi_core_valid(core) ? hpx_wifi_suspend_begin(&core->lifecycle) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_suspend_complete(struct hpx_wifi_core *core, uint32_t generation,
                               int result)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_suspend_complete(&core->lifecycle, (unsigned int)generation,
                                  result) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_resume_begin(struct hpx_wifi_core *core)
{
    return hpx_wifi_core_valid(core) ? hpx_wifi_resume_begin(&core->lifecycle) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_resume_complete(struct hpx_wifi_core *core, uint32_t generation,
                              int result)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_resume_complete(&core->lifecycle, (unsigned int)generation,
                                 result) :
        HPX_WIFI_CORE_INVALID_ARGUMENT;
}

int
hpx_wifi_core_tx_submit(struct hpx_wifi_core *core, void *buffer, size_t length,
                        uint64_t *ticket_out)
{
    if (!hpx_wifi_core_valid(core) || buffer == 0 || ticket_out == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    if (core->lifecycle.state != HPX_WIFI_LINK_UP) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    return hpx_wifi_packet_submit(&core->tx_queue, buffer, length,
                                  (uint32_t)core->lifecycle.generation,
                                  ticket_out);
}

int
hpx_wifi_core_rx_post(struct hpx_wifi_core *core, void *buffer, size_t capacity,
                      uint64_t *ticket_out)
{
    if (!hpx_wifi_core_valid(core) || buffer == 0 || ticket_out == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    if (core->lifecycle.state != HPX_WIFI_READY &&
        core->lifecycle.state != HPX_WIFI_LINK_UP) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    return hpx_wifi_packet_submit(&core->rx_queue, buffer, capacity,
                                  (uint32_t)core->lifecycle.generation,
                                  ticket_out);
}

int
hpx_wifi_core_packet_acquire(struct hpx_wifi_core *core,
                             enum hpx_wifi_packet_direction direction,
                             uint64_t *ticket_out, void **buffer_out,
                             size_t *length_out, uint32_t *generation_out)
{
    struct hpx_wifi_packet_queue *queue;

    if (!hpx_wifi_core_valid(core)) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    queue = hpx_wifi_core_queue(core, direction);
    if (queue == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    if (direction == HPX_WIFI_PACKET_TX) {
        if (core->lifecycle.state != HPX_WIFI_LINK_UP) {
            return HPX_WIFI_PACKET_INVALID_STATE;
        }
    } else if (core->lifecycle.state != HPX_WIFI_READY &&
               core->lifecycle.state != HPX_WIFI_LINK_UP) {
        return HPX_WIFI_PACKET_INVALID_STATE;
    }
    return hpx_wifi_packet_acquire(queue, ticket_out, buffer_out, length_out,
                                   generation_out);
}

int
hpx_wifi_core_packet_complete(struct hpx_wifi_core *core,
                              enum hpx_wifi_packet_direction direction,
                              uint64_t ticket, uint32_t generation,
                              size_t bytes_completed, int status)
{
    struct hpx_wifi_packet_queue *queue;

    if (!hpx_wifi_core_valid(core)) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    queue = hpx_wifi_core_queue(core, direction);
    if (queue == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    return hpx_wifi_packet_complete(queue, ticket, generation, bytes_completed,
                                    status);
}

int
hpx_wifi_core_packet_get_completion(
    const struct hpx_wifi_core *core,
    enum hpx_wifi_packet_direction direction, uint64_t ticket,
    enum hpx_wifi_packet_state *state_out, size_t *bytes_completed_out,
    int *status_out)
{
    const struct hpx_wifi_packet_queue *queue;

    if (!hpx_wifi_core_valid(core)) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    queue = hpx_wifi_core_queue_const(core, direction);
    if (queue == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    return hpx_wifi_packet_get_completion(queue, ticket, state_out,
                                          bytes_completed_out, status_out);
}

int
hpx_wifi_core_packet_release(struct hpx_wifi_core *core,
                             enum hpx_wifi_packet_direction direction,
                             uint64_t ticket)
{
    struct hpx_wifi_packet_queue *queue;

    if (!hpx_wifi_core_valid(core)) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    queue = hpx_wifi_core_queue(core, direction);
    if (queue == 0) {
        return HPX_WIFI_PACKET_INVALID_ARGUMENT;
    }
    return hpx_wifi_packet_release(queue, ticket);
}

int
hpx_wifi_core_fail(struct hpx_wifi_core *core, uint32_t generation,
                   int error_code)
{
    int result;

    if (!hpx_wifi_core_valid(core) || error_code == 0) {
        return HPX_WIFI_CORE_INVALID_ARGUMENT;
    }
    result = hpx_wifi_fail(&core->lifecycle, (unsigned int)generation,
                           error_code);
    if (result != HPX_WIFI_OK) {
        return result;
    }
    return hpx_wifi_core_cancel_queued(core, error_code);
}

int
hpx_wifi_core_begin_stop(struct hpx_wifi_core *core)
{
    int result;

    if (!hpx_wifi_core_valid(core)) {
        return HPX_WIFI_CORE_INVALID_ARGUMENT;
    }
    result = hpx_wifi_begin_stop(&core->lifecycle);
    if (result != HPX_WIFI_OK) {
        return result;
    }
    return hpx_wifi_core_cancel_queued(core, HPX_WIFI_CORE_CANCELLED);
}

int
hpx_wifi_core_detach(struct hpx_wifi_core *core)
{
    if (!hpx_wifi_core_valid(core)) {
        return HPX_WIFI_CORE_INVALID_ARGUMENT;
    }
    if (hpx_wifi_packet_queued(&core->tx_queue) != 0U ||
        hpx_wifi_packet_in_flight(&core->tx_queue) != 0U ||
        hpx_wifi_packet_completed(&core->tx_queue) != 0U ||
        hpx_wifi_packet_queued(&core->rx_queue) != 0U ||
        hpx_wifi_packet_in_flight(&core->rx_queue) != 0U ||
        hpx_wifi_packet_completed(&core->rx_queue) != 0U) {
        return HPX_WIFI_CORE_INVALID_STATE;
    }
    return hpx_wifi_detach(&core->lifecycle);
}

size_t
hpx_wifi_core_tx_queued(const struct hpx_wifi_core *core)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_packet_queued(&core->tx_queue) : 0U;
}

size_t
hpx_wifi_core_rx_queued(const struct hpx_wifi_core *core)
{
    return hpx_wifi_core_valid(core) ?
        hpx_wifi_packet_queued(&core->rx_queue) : 0U;
}

size_t
hpx_wifi_core_in_flight(const struct hpx_wifi_core *core)
{
    if (!hpx_wifi_core_valid(core)) {
        return 0U;
    }
    return hpx_wifi_packet_in_flight(&core->tx_queue) +
           hpx_wifi_packet_in_flight(&core->rx_queue);
}

size_t
hpx_wifi_core_completed(const struct hpx_wifi_core *core)
{
    if (!hpx_wifi_core_valid(core)) {
        return 0U;
    }
    return hpx_wifi_packet_completed(&core->tx_queue) +
           hpx_wifi_packet_completed(&core->rx_queue);
}
