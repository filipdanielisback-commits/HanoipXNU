#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "format.h"

int
main(void)
{
    struct hpx_audio_format format = {
        HPX_AUDIO_PLAYBACK, HPX_AUDIO_SAMPLE_S16_LE, 48000U, 2U, 240U
    };
    size_t bytes;

    assert(hpx_audio_format_validate(&format) == HPX_AUDIO_FORMAT_OK);
    assert(hpx_audio_format_frame_bytes(&format, &bytes) ==
           HPX_AUDIO_FORMAT_OK);
    assert(bytes == 4U);
    assert(hpx_audio_period_bytes(&format, &bytes) == HPX_AUDIO_FORMAT_OK);
    assert(bytes == 960U);
    assert(hpx_audio_buffer_bytes_validate(&format, 1920U) ==
           HPX_AUDIO_FORMAT_OK);
    assert(hpx_audio_buffer_bytes_validate(&format, 1000U) ==
           HPX_AUDIO_FORMAT_MISALIGNED_BUFFER);

    format.direction = HPX_AUDIO_CAPTURE;
    format.sample_format = HPX_AUDIO_SAMPLE_S24_LE;
    format.channels = 2U;
    format.period_frames = 128U;
    assert(hpx_audio_format_validate(&format) == HPX_AUDIO_FORMAT_OK);
    assert(hpx_audio_format_frame_bytes(&format, &bytes) ==
           HPX_AUDIO_FORMAT_OK);
    assert(bytes == 6U);
    assert(hpx_audio_period_bytes(&format, &bytes) == HPX_AUDIO_FORMAT_OK);
    assert(bytes == 768U);

    format.sample_rate = 7999U;
    assert(hpx_audio_format_validate(&format) ==
           HPX_AUDIO_FORMAT_UNSUPPORTED_RATE);
    format.sample_rate = 48000U;
    format.channels = 9U;
    assert(hpx_audio_format_validate(&format) ==
           HPX_AUDIO_FORMAT_UNSUPPORTED_CHANNELS);
    format.channels = 2U;
    format.sample_format = (enum hpx_audio_sample_format)99;
    assert(hpx_audio_format_validate(&format) ==
           HPX_AUDIO_FORMAT_UNSUPPORTED_SAMPLE_FORMAT);
    format.sample_format = HPX_AUDIO_SAMPLE_S16_LE;
    format.period_frames = 0U;
    assert(hpx_audio_format_validate(&format) ==
           HPX_AUDIO_FORMAT_INVALID_PERIOD);
    assert(hpx_audio_sample_bytes(HPX_AUDIO_SAMPLE_S16_LE, 0) ==
           HPX_AUDIO_FORMAT_INVALID_ARGUMENT);

    puts("audio PCM format test passed");
    return 0;
}
