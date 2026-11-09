#include "state.h"

#include <assert.h>
#include <stdio.h>

static void
expect_state(const struct hpx_wifi_state_data *wifi,
             enum hpx_wifi_state expected, unsigned int expected_generation,
             unsigned int expected_request, int expected_error)
{
    enum hpx_wifi_state state;
    unsigned int generation;
    unsigned int request;
    int error_code;

    assert(hpx_wifi_get_status(wifi, &state, &generation, &request,
                               &error_code) == HPX_WIFI_OK);
    assert(state == expected);
    assert(generation == expected_generation);
    assert(request == expected_request);
    assert(error_code == expected_error);
}

int
main(void)
{
    struct hpx_wifi_state_data wifi;

    assert(hpx_wifi_state_init(&wifi) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_ABSENT, 0U, 0U, 0);

    assert(hpx_wifi_mark_ready(&wifi, 1U) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_discover(&wifi) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_DISCOVERED, 1U, 0U, 0);
    assert(hpx_wifi_discover(&wifi) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_begin_init(&wifi) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_INITIALIZING, 1U, 0U, 0);
    assert(hpx_wifi_mark_ready(&wifi, 1U) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_READY, 1U, 0U, 0);

    assert(hpx_wifi_scan_begin(&wifi, 0U) == HPX_WIFI_INVALID_ARGUMENT);
    assert(hpx_wifi_scan_begin(&wifi, 10U) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_SCANNING, 1U, 10U, 0);
    assert(hpx_wifi_scan_complete(&wifi, 1U, 11U, 0) ==
           HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_scan_complete(&wifi, 2U, 10U, 0) ==
           HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_scan_complete(&wifi, 1U, 10U, -7) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_READY, 1U, 0U, -7);

    assert(hpx_wifi_connect_begin(&wifi, 20U) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_CONNECTING, 1U, 20U, 0);
    assert(hpx_wifi_connect_complete(&wifi, 1U, 20U, 0) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_LINK_UP, 1U, 0U, 0);

    assert(hpx_wifi_suspend_begin(&wifi) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_SUSPENDING, 1U, 0U, 0);
    assert(hpx_wifi_suspend_complete(&wifi, 2U, 0) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_suspend_complete(&wifi, 1U, 0) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_SUSPENDED, 1U, 0U, 0);
    assert(hpx_wifi_resume_begin(&wifi) == HPX_WIFI_OK);
    assert(hpx_wifi_resume_complete(&wifi, 2U, 0) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_resume_complete(&wifi, 1U, 0) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_LINK_UP, 1U, 0U, 0);

    assert(hpx_wifi_disconnect_begin(&wifi, 30U) == HPX_WIFI_OK);
    assert(hpx_wifi_disconnect_complete(&wifi, 1U, 30U, -8) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_LINK_UP, 1U, 0U, -8);
    assert(hpx_wifi_disconnect_begin(&wifi, 31U) == HPX_WIFI_OK);
    assert(hpx_wifi_disconnect_complete(&wifi, 1U, 31U, 0) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_READY, 1U, 0U, 0);

    assert(hpx_wifi_suspend_begin(&wifi) == HPX_WIFI_OK);
    assert(hpx_wifi_suspend_complete(&wifi, 1U, -9) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_READY, 1U, 0U, -9);

    assert(hpx_wifi_fail(&wifi, 2U, -42) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_fail(&wifi, 1U, -42) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_FAILED, 1U, 0U, -42);
    assert(hpx_wifi_fail(&wifi, 1U, -43) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_begin_stop(&wifi) == HPX_WIFI_OK);
    assert(hpx_wifi_detach(&wifi) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_ABSENT, 1U, 0U, 0);

    assert(hpx_wifi_discover(&wifi) == HPX_WIFI_OK);
    expect_state(&wifi, HPX_WIFI_DISCOVERED, 2U, 0U, 0);
    assert(hpx_wifi_begin_stop(&wifi) == HPX_WIFI_OK);
    assert(hpx_wifi_detach(&wifi) == HPX_WIFI_OK);

    assert(hpx_wifi_state_init(0) == HPX_WIFI_INVALID_ARGUMENT);
    assert(hpx_wifi_scan_begin(&wifi, 5U) == HPX_WIFI_INVALID_STATE);
    assert(hpx_wifi_get_status(&wifi, 0, 0, 0, 0) ==
           HPX_WIFI_INVALID_ARGUMENT);

    puts("wifi lifecycle state tests passed");
    return 0;
}
