#include "state.h"

#include <assert.h>
#include <stdio.h>

/*
 * Deterministic fake backend. It models confirmations only; it does not
 * emulate a chipset, bus, firmware, radio, or XNU networking stack.
 */
struct fake_backend {
    struct hpx_wifi_state_data wifi;
    int present;
    int init_result;
};

static void
fake_backend_init(struct fake_backend *backend)
{
    assert(hpx_wifi_state_init(&backend->wifi) == HPX_WIFI_OK);
    backend->present = 0;
    backend->init_result = 0;
}

static int
fake_backend_probe(struct fake_backend *backend)
{
    backend->present = 1;
    return hpx_wifi_discover(&backend->wifi);
}

static int
fake_backend_initialize(struct fake_backend *backend)
{
    int result;

    if (!backend->present) {
        return HPX_WIFI_INVALID_STATE;
    }
    result = hpx_wifi_begin_init(&backend->wifi);
    if (result != HPX_WIFI_OK) {
        return result;
    }

    if (backend->init_result != 0) {
        return hpx_wifi_fail(&backend->wifi, backend->wifi.generation,
                            backend->init_result);
    }
    return hpx_wifi_mark_ready(&backend->wifi, backend->wifi.generation);
}

static int
fake_backend_scan(struct fake_backend *backend, unsigned int request_id,
                  int completion_result)
{
    int result;

    result = hpx_wifi_scan_begin(&backend->wifi, request_id);
    if (result != HPX_WIFI_OK) {
        return result;
    }

    /* The fake backend supplies an explicit completion, not a fake success. */
    return hpx_wifi_scan_complete(&backend->wifi, backend->wifi.generation,
                                  request_id, completion_result);
}

int
main(void)
{
    struct fake_backend backend;
    enum hpx_wifi_state state;
    unsigned int generation;
    unsigned int old_generation;
    unsigned int pending;
    int error_code;

    fake_backend_init(&backend);
    assert(fake_backend_initialize(&backend) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_get_status(&backend.wifi, &state, &generation, &pending,
                               &error_code) == HPX_WIFI_OK);
    assert(state == HPX_WIFI_ABSENT);

    assert(fake_backend_probe(&backend) == HPX_WIFI_OK);
    backend.init_result = -100;
    assert(fake_backend_initialize(&backend) == HPX_WIFI_OK);
    assert(hpx_wifi_get_status(&backend.wifi, &state, &generation, &pending,
                               &error_code) == HPX_WIFI_OK);
    assert(state == HPX_WIFI_FAILED);
    assert(error_code == -100);

    assert(hpx_wifi_begin_stop(&backend.wifi) == HPX_WIFI_OK);
    assert(hpx_wifi_detach(&backend.wifi) == HPX_WIFI_OK);

    fake_backend_init(&backend);
    assert(fake_backend_probe(&backend) == HPX_WIFI_OK);
    assert(fake_backend_initialize(&backend) == HPX_WIFI_OK);
    assert(hpx_wifi_get_status(&backend.wifi, &state, &generation, &pending,
                               &error_code) == HPX_WIFI_OK);
    assert(state == HPX_WIFI_READY);

    assert(fake_backend_scan(&backend, 77U, 0) == HPX_WIFI_OK);
    assert(hpx_wifi_get_status(&backend.wifi, &state, &generation, &pending,
                               &error_code) == HPX_WIFI_OK);
    assert(state == HPX_WIFI_READY);
    assert(pending == 0U);
    assert(error_code == 0);

    assert(fake_backend_scan(&backend, 78U, -101) == HPX_WIFI_OK);
    assert(hpx_wifi_get_status(&backend.wifi, &state, &generation, &pending,
                               &error_code) == HPX_WIFI_OK);
    assert(state == HPX_WIFI_READY);
    assert(error_code == -101);

    assert(hpx_wifi_scan_begin(&backend.wifi, 90U) == HPX_WIFI_OK);
    old_generation = backend.wifi.generation;
    assert(hpx_wifi_begin_stop(&backend.wifi) == HPX_WIFI_OK);
    assert(hpx_wifi_detach(&backend.wifi) == HPX_WIFI_OK);

    assert(fake_backend_probe(&backend) == HPX_WIFI_OK);
    assert(fake_backend_initialize(&backend) == HPX_WIFI_OK);
    assert(backend.wifi.generation != old_generation);
    assert(hpx_wifi_scan_begin(&backend.wifi, 90U) == HPX_WIFI_OK);
    assert(hpx_wifi_scan_complete(&backend.wifi, old_generation, 90U, 0) ==
           HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_scan_complete(&backend.wifi, backend.wifi.generation,
                                  90U, 0) == HPX_WIFI_OK);

    assert(hpx_wifi_begin_stop(&backend.wifi) == HPX_WIFI_OK);
    assert(hpx_wifi_detach(&backend.wifi) == HPX_WIFI_OK);

    puts("wifi fake backend tests passed");
    return 0;
}
