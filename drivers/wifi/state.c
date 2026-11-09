#include "state.h"

#include <limits.h>

static int
hpx_wifi_valid(const struct hpx_wifi_state_data *wifi)
{
    return wifi != 0;
}

static int
hpx_wifi_request_valid(unsigned int request_id)
{
    return request_id != 0U;
}

int
hpx_wifi_state_init(struct hpx_wifi_state_data *wifi)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }

    wifi->state = HPX_WIFI_ABSENT;
    wifi->resume_state = HPX_WIFI_ABSENT;
    wifi->generation = 0U;
    wifi->pending_request = 0U;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_discover(struct hpx_wifi_state_data *wifi)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_ABSENT || wifi->generation == UINT_MAX) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->generation++;
    wifi->state = HPX_WIFI_DISCOVERED;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_begin_init(struct hpx_wifi_state_data *wifi)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_DISCOVERED) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_INITIALIZING;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_mark_ready(struct hpx_wifi_state_data *wifi,
                    unsigned int generation)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_INITIALIZING ||
        wifi->generation != generation) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_READY;
    wifi->pending_request = 0U;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_begin_stop(struct hpx_wifi_state_data *wifi)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state == HPX_WIFI_ABSENT ||
        wifi->state == HPX_WIFI_STOPPING) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_STOPPING;
    wifi->pending_request = 0U;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_detach(struct hpx_wifi_state_data *wifi)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_STOPPING) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_ABSENT;
    wifi->resume_state = HPX_WIFI_ABSENT;
    wifi->pending_request = 0U;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_scan_begin(struct hpx_wifi_state_data *wifi,
                    unsigned int request_id)
{
    if (!hpx_wifi_valid(wifi) || !hpx_wifi_request_valid(request_id)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_READY) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_SCANNING;
    wifi->pending_request = request_id;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_scan_complete(struct hpx_wifi_state_data *wifi,
                       unsigned int generation, unsigned int request_id,
                       int result)
{
    if (!hpx_wifi_valid(wifi) || !hpx_wifi_request_valid(request_id)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_SCANNING ||
        wifi->generation != generation || wifi->pending_request != request_id) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_READY;
    wifi->pending_request = 0U;
    wifi->last_error = result;
    return HPX_WIFI_OK;
}

int
hpx_wifi_connect_begin(struct hpx_wifi_state_data *wifi,
                       unsigned int request_id)
{
    if (!hpx_wifi_valid(wifi) || !hpx_wifi_request_valid(request_id)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_READY) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_CONNECTING;
    wifi->pending_request = request_id;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_connect_complete(struct hpx_wifi_state_data *wifi,
                          unsigned int generation, unsigned int request_id,
                          int result)
{
    if (!hpx_wifi_valid(wifi) || !hpx_wifi_request_valid(request_id)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_CONNECTING ||
        wifi->generation != generation || wifi->pending_request != request_id) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = (result == 0) ? HPX_WIFI_LINK_UP : HPX_WIFI_READY;
    wifi->pending_request = 0U;
    wifi->last_error = result;
    return HPX_WIFI_OK;
}

int
hpx_wifi_disconnect_begin(struct hpx_wifi_state_data *wifi,
                          unsigned int request_id)
{
    if (!hpx_wifi_valid(wifi) || !hpx_wifi_request_valid(request_id)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_LINK_UP) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_DISCONNECTING;
    wifi->pending_request = request_id;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_disconnect_complete(struct hpx_wifi_state_data *wifi,
                             unsigned int generation,
                             unsigned int request_id, int result)
{
    if (!hpx_wifi_valid(wifi) || !hpx_wifi_request_valid(request_id)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_DISCONNECTING ||
        wifi->generation != generation || wifi->pending_request != request_id) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = (result == 0) ? HPX_WIFI_READY : HPX_WIFI_LINK_UP;
    wifi->pending_request = 0U;
    wifi->last_error = result;
    return HPX_WIFI_OK;
}

int
hpx_wifi_suspend_begin(struct hpx_wifi_state_data *wifi)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_READY && wifi->state != HPX_WIFI_LINK_UP) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->resume_state = wifi->state;
    wifi->state = HPX_WIFI_SUSPENDING;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_suspend_complete(struct hpx_wifi_state_data *wifi,
                          unsigned int generation, int result)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_SUSPENDING ||
        wifi->generation != generation) {
        return HPX_WIFI_INVALID_STATE;
    }

    if (result == 0) {
        wifi->state = HPX_WIFI_SUSPENDED;
    } else {
        wifi->state = wifi->resume_state;
    }
    wifi->last_error = result;
    return HPX_WIFI_OK;
}

int
hpx_wifi_resume_begin(struct hpx_wifi_state_data *wifi)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_SUSPENDED) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_RESUMING;
    wifi->last_error = 0;
    return HPX_WIFI_OK;
}

int
hpx_wifi_resume_complete(struct hpx_wifi_state_data *wifi,
                         unsigned int generation, int result)
{
    if (!hpx_wifi_valid(wifi)) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state != HPX_WIFI_RESUMING ||
        wifi->generation != generation) {
        return HPX_WIFI_INVALID_STATE;
    }

    if (result == 0) {
        wifi->state = wifi->resume_state;
    } else {
        wifi->state = HPX_WIFI_FAILED;
    }
    wifi->last_error = result;
    return HPX_WIFI_OK;
}

int
hpx_wifi_fail(struct hpx_wifi_state_data *wifi,
              unsigned int generation, int error_code)
{
    if (!hpx_wifi_valid(wifi) || error_code == 0) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }
    if (wifi->state == HPX_WIFI_ABSENT ||
        wifi->state == HPX_WIFI_FAILED ||
        wifi->state == HPX_WIFI_STOPPING ||
        wifi->generation != generation) {
        return HPX_WIFI_INVALID_STATE;
    }

    wifi->state = HPX_WIFI_FAILED;
    wifi->pending_request = 0U;
    wifi->last_error = error_code;
    return HPX_WIFI_OK;
}

int
hpx_wifi_get_status(const struct hpx_wifi_state_data *wifi,
                    enum hpx_wifi_state *state_out,
                    unsigned int *generation_out,
                    unsigned int *pending_request_out,
                    int *last_error_out)
{
    if (!hpx_wifi_valid(wifi) || state_out == 0 ||
        generation_out == 0 || pending_request_out == 0 ||
        last_error_out == 0) {
        return HPX_WIFI_INVALID_ARGUMENT;
    }

    *state_out = wifi->state;
    *generation_out = wifi->generation;
    *pending_request_out = wifi->pending_request;
    *last_error_out = wifi->last_error;
    return HPX_WIFI_OK;
}
