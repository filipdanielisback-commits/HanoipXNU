#ifndef HPX_AUDIO_STREAM_H
#define HPX_AUDIO_STREAM_H

/*
 * Hardware-independent audio stream lifecycle.
 * This module performs no I/O, allocation, locking, or hardware control.
 * The caller must serialize access to each stream instance.
 */
enum hpx_audio_stream_state {
    HPX_AUDIO_STREAM_CLOSED = 0,
    HPX_AUDIO_STREAM_READY,
    HPX_AUDIO_STREAM_RUNNING,
    HPX_AUDIO_STREAM_PAUSED,
    HPX_AUDIO_STREAM_FAILED
};

enum hpx_audio_stream_result {
    HPX_AUDIO_STREAM_OK = 0,
    HPX_AUDIO_STREAM_INVALID_ARGUMENT = -1,
    HPX_AUDIO_STREAM_INVALID_STATE = -2
};

struct hpx_audio_stream {
    enum hpx_audio_stream_state state;
    int last_error;
};

/* Initialize a new stream object in CLOSED state. */
int hpx_audio_stream_init(struct hpx_audio_stream *stream);

/* Legal transitions: CLOSED->READY->RUNNING<->PAUSED. */
int hpx_audio_stream_open(struct hpx_audio_stream *stream);
int hpx_audio_stream_start(struct hpx_audio_stream *stream);
int hpx_audio_stream_pause(struct hpx_audio_stream *stream);
int hpx_audio_stream_resume(struct hpx_audio_stream *stream);

/*
 * Stop a RUNNING, PAUSED, or FAILED stream and return it to READY.
 * Closing requires READY or FAILED; active streams must be stopped first.
 */
int hpx_audio_stream_stop(struct hpx_audio_stream *stream);
int hpx_audio_stream_close(struct hpx_audio_stream *stream);

/* Mark a non-CLOSED, non-FAILED stream as failed with a nonzero error code. */
int hpx_audio_stream_fail(struct hpx_audio_stream *stream, int error_code);

/* Copy current state and last error to caller-provided outputs. */
int hpx_audio_stream_get_status(const struct hpx_audio_stream *stream,
                                enum hpx_audio_stream_state *state_out,
                                int *last_error_out);

#endif
