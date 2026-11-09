#include "stream.h"

static int
hpx_audio_stream_valid(const struct hpx_audio_stream *stream)
{
    return stream != 0;
}

int
hpx_audio_stream_init(struct hpx_audio_stream *stream)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }

    stream->state = HPX_AUDIO_STREAM_CLOSED;
    stream->last_error = 0;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_open(struct hpx_audio_stream *stream)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (stream->state != HPX_AUDIO_STREAM_CLOSED) {
        return HPX_AUDIO_STREAM_INVALID_STATE;
    }

    stream->state = HPX_AUDIO_STREAM_READY;
    stream->last_error = 0;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_start(struct hpx_audio_stream *stream)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (stream->state != HPX_AUDIO_STREAM_READY) {
        return HPX_AUDIO_STREAM_INVALID_STATE;
    }

    stream->state = HPX_AUDIO_STREAM_RUNNING;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_pause(struct hpx_audio_stream *stream)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (stream->state != HPX_AUDIO_STREAM_RUNNING) {
        return HPX_AUDIO_STREAM_INVALID_STATE;
    }

    stream->state = HPX_AUDIO_STREAM_PAUSED;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_resume(struct hpx_audio_stream *stream)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (stream->state != HPX_AUDIO_STREAM_PAUSED) {
        return HPX_AUDIO_STREAM_INVALID_STATE;
    }

    stream->state = HPX_AUDIO_STREAM_RUNNING;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_stop(struct hpx_audio_stream *stream)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (stream->state != HPX_AUDIO_STREAM_RUNNING &&
        stream->state != HPX_AUDIO_STREAM_PAUSED &&
        stream->state != HPX_AUDIO_STREAM_FAILED) {
        return HPX_AUDIO_STREAM_INVALID_STATE;
    }

    stream->state = HPX_AUDIO_STREAM_READY;
    stream->last_error = 0;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_close(struct hpx_audio_stream *stream)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (stream->state != HPX_AUDIO_STREAM_READY &&
        stream->state != HPX_AUDIO_STREAM_FAILED) {
        return HPX_AUDIO_STREAM_INVALID_STATE;
    }

    stream->state = HPX_AUDIO_STREAM_CLOSED;
    stream->last_error = 0;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_fail(struct hpx_audio_stream *stream, int error_code)
{
    if (!hpx_audio_stream_valid(stream)) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (error_code == 0) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }
    if (stream->state == HPX_AUDIO_STREAM_CLOSED ||
        stream->state == HPX_AUDIO_STREAM_FAILED) {
        return HPX_AUDIO_STREAM_INVALID_STATE;
    }

    stream->last_error = error_code;
    stream->state = HPX_AUDIO_STREAM_FAILED;
    return HPX_AUDIO_STREAM_OK;
}

int
hpx_audio_stream_get_status(const struct hpx_audio_stream *stream,
                            enum hpx_audio_stream_state *state_out,
                            int *last_error_out)
{
    if (!hpx_audio_stream_valid(stream) || state_out == 0 ||
        last_error_out == 0) {
        return HPX_AUDIO_STREAM_INVALID_ARGUMENT;
    }

    *state_out = stream->state;
    *last_error_out = stream->last_error;
    return HPX_AUDIO_STREAM_OK;
}
