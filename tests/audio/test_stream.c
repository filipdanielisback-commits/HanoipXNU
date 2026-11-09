#include "stream.h"

#include <assert.h>
#include <stdio.h>

static void
expect_state(const struct hpx_audio_stream *stream,
             enum hpx_audio_stream_state expected_state,
             int expected_error)
{
    enum hpx_audio_stream_state state;
    int error_code;

    assert(hpx_audio_stream_get_status(stream, &state, &error_code) ==
           HPX_AUDIO_STREAM_OK);
    assert(state == expected_state);
    assert(error_code == expected_error);
}

int
main(void)
{
    struct hpx_audio_stream stream;

    assert(hpx_audio_stream_init(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_CLOSED, 0);

    assert(hpx_audio_stream_start(&stream) ==
           HPX_AUDIO_STREAM_INVALID_STATE);
    assert(hpx_audio_stream_open(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_READY, 0);

    assert(hpx_audio_stream_open(&stream) ==
           HPX_AUDIO_STREAM_INVALID_STATE);
    assert(hpx_audio_stream_pause(&stream) ==
           HPX_AUDIO_STREAM_INVALID_STATE);

    assert(hpx_audio_stream_start(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_RUNNING, 0);
    assert(hpx_audio_stream_pause(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_PAUSED, 0);
    assert(hpx_audio_stream_resume(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_RUNNING, 0);

    assert(hpx_audio_stream_close(&stream) ==
           HPX_AUDIO_STREAM_INVALID_STATE);
    assert(hpx_audio_stream_stop(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_READY, 0);
    assert(hpx_audio_stream_close(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_CLOSED, 0);

    assert(hpx_audio_stream_open(&stream) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_stream_fail(&stream, -42) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_FAILED, -42);
    assert(hpx_audio_stream_start(&stream) ==
           HPX_AUDIO_STREAM_INVALID_STATE);
    assert(hpx_audio_stream_stop(&stream) == HPX_AUDIO_STREAM_OK);
    expect_state(&stream, HPX_AUDIO_STREAM_READY, 0);
    assert(hpx_audio_stream_close(&stream) == HPX_AUDIO_STREAM_OK);

    assert(hpx_audio_stream_open(&stream) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_stream_fail(&stream, 0) ==
           HPX_AUDIO_STREAM_INVALID_ARGUMENT);
    assert(hpx_audio_stream_close(&stream) == HPX_AUDIO_STREAM_OK);

    assert(hpx_audio_stream_init(0) ==
           HPX_AUDIO_STREAM_INVALID_ARGUMENT);
    assert(hpx_audio_stream_get_status(&stream, 0, 0) ==
           HPX_AUDIO_STREAM_INVALID_ARGUMENT);

    puts("audio stream lifecycle tests passed");
    return 0;
}
