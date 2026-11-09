#ifndef HPX_AUDIO_FORMAT_H
#define HPX_AUDIO_FORMAT_H

#include <stddef.h>
#include <stdint.h>

enum hpx_audio_direction {
    HPX_AUDIO_PLAYBACK = 0,
    HPX_AUDIO_CAPTURE = 1
};

enum hpx_audio_sample_format {
    HPX_AUDIO_SAMPLE_U8 = 0,
    HPX_AUDIO_SAMPLE_S16_LE,
    HPX_AUDIO_SAMPLE_S24_LE,
    HPX_AUDIO_SAMPLE_S32_LE
};

enum hpx_audio_format_result {
    HPX_AUDIO_FORMAT_OK = 0,
    HPX_AUDIO_FORMAT_INVALID_ARGUMENT = -1,
    HPX_AUDIO_FORMAT_UNSUPPORTED_RATE = -2,
    HPX_AUDIO_FORMAT_UNSUPPORTED_CHANNELS = -3,
    HPX_AUDIO_FORMAT_UNSUPPORTED_SAMPLE_FORMAT = -4,
    HPX_AUDIO_FORMAT_INVALID_PERIOD = -5,
    HPX_AUDIO_FORMAT_SIZE_OVERFLOW = -6,
    HPX_AUDIO_FORMAT_MISALIGNED_BUFFER = -7
};

/*
 * Logical PCM parameters only. These limits are conservative scaffold
 * policy, not claims about the Moto G60 codec or supported hardware rates.
 */
#define HPX_AUDIO_MIN_SAMPLE_RATE 8000U
#define HPX_AUDIO_MAX_SAMPLE_RATE 192000U
#define HPX_AUDIO_MAX_CHANNELS 8U
#define HPX_AUDIO_MIN_PERIOD_FRAMES 1U
#define HPX_AUDIO_MAX_PERIOD_FRAMES 65536U

struct hpx_audio_format {
    enum hpx_audio_direction direction;
    enum hpx_audio_sample_format sample_format;
    uint32_t sample_rate;
    uint16_t channels;
    uint32_t period_frames;
};

int hpx_audio_sample_bytes(enum hpx_audio_sample_format format,
                           size_t *bytes_out);
int hpx_audio_format_frame_bytes(const struct hpx_audio_format *format,
                                 size_t *bytes_out);
int hpx_audio_format_validate(const struct hpx_audio_format *format);
int hpx_audio_period_bytes(const struct hpx_audio_format *format,
                           size_t *bytes_out);
int hpx_audio_buffer_bytes_validate(const struct hpx_audio_format *format,
                                    size_t buffer_bytes);

#endif
