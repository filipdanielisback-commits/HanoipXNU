#ifndef HPX_WIFI_STATE_H
#define HPX_WIFI_STATE_H

/*
 * Hardware-independent Wi-Fi lifecycle state machine.
 *
 * This module performs no I/O, allocation, locking, scheduling, or hardware
 * access. The caller must serialize access to each state object and must call
 * transitions only after receiving the corresponding backend confirmation.
 */

enum hpx_wifi_state {
    HPX_WIFI_ABSENT = 0,
    HPX_WIFI_DISCOVERED,
    HPX_WIFI_INITIALIZING,
    HPX_WIFI_READY,
    HPX_WIFI_SCANNING,
    HPX_WIFI_CONNECTING,
    HPX_WIFI_LINK_UP,
    HPX_WIFI_DISCONNECTING,
    HPX_WIFI_SUSPENDING,
    HPX_WIFI_SUSPENDED,
    HPX_WIFI_RESUMING,
    HPX_WIFI_FAILED,
    HPX_WIFI_STOPPING
};

enum hpx_wifi_result {
    HPX_WIFI_OK = 0,
    HPX_WIFI_INVALID_ARGUMENT = -1,
    HPX_WIFI_INVALID_STATE = -2
};

struct hpx_wifi_state_data {
    enum hpx_wifi_state state;
    enum hpx_wifi_state resume_state;
    unsigned int generation;
    unsigned int pending_request;
    int last_error;
};

/* Start in ABSENT. Caller serializes all access to the object. */
int hpx_wifi_state_init(struct hpx_wifi_state_data *wifi);

/* Device lifecycle. */
int hpx_wifi_discover(struct hpx_wifi_state_data *wifi);
int hpx_wifi_begin_init(struct hpx_wifi_state_data *wifi);
int hpx_wifi_mark_ready(struct hpx_wifi_state_data *wifi);
int hpx_wifi_begin_stop(struct hpx_wifi_state_data *wifi);
int hpx_wifi_detach(struct hpx_wifi_state_data *wifi);

/* Scan lifecycle. A request ID must be nonzero and unique while pending. */
int hpx_wifi_scan_begin(struct hpx_wifi_state_data *wifi,
                        unsigned int request_id);
int hpx_wifi_scan_complete(struct hpx_wifi_state_data *wifi,
                           unsigned int request_id, int result);

/* Connection lifecycle. Success must represent a confirmed backend event. */
int hpx_wifi_connect_begin(struct hpx_wifi_state_data *wifi,
                           unsigned int request_id);
int hpx_wifi_connect_complete(struct hpx_wifi_state_data *wifi,
                              unsigned int request_id, int result);
int hpx_wifi_disconnect_begin(struct hpx_wifi_state_data *wifi,
                              unsigned int request_id);
int hpx_wifi_disconnect_complete(struct hpx_wifi_state_data *wifi,
                                 unsigned int request_id, int result);

/* Suspend/resume are state-only; backend must confirm each operation. */
int hpx_wifi_suspend_begin(struct hpx_wifi_state_data *wifi);
int hpx_wifi_suspend_complete(struct hpx_wifi_state_data *wifi, int result);
int hpx_wifi_resume_begin(struct hpx_wifi_state_data *wifi);
int hpx_wifi_resume_complete(struct hpx_wifi_state_data *wifi, int result);

/* Fatal error from a verified backend/transport event; error must be nonzero. */
int hpx_wifi_fail(struct hpx_wifi_state_data *wifi, int error_code);

/* Read-only status snapshot. */
int hpx_wifi_get_status(const struct hpx_wifi_state_data *wifi,
                        enum hpx_wifi_state *state_out,
                        unsigned int *generation_out,
                        unsigned int *pending_request_out,
                        int *last_error_out);

#endif
