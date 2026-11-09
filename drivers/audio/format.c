#include "format.h"

#include <stdint.h>

int
hpx_audio_sample_bytes(enum hpx_audio_sample_format format, size_t *bytes_out)
{
    if (bytes_out == 0) {
        return HPX_AUDIO_FORMAT_INVALID_ARGUMENT;
    }

    switch (format) {
    case HPX_AUDIO_SAMPLE_U8:
        *bytes_out = 1;
        return HPX_AUDIO_FORMAT_OK;
    case HPX_AUDIO_SAMPLE_S16_LE:
        *bytes_out = 2;
        return HPX_AUDIO_FORMAT_OK;
    case HPX_AUDIO_SAMPLE_S24_LE:
        *bytes_out = 3;
        return HPX_AUDIO_FORMAT_OK;
    case HPX_AUDIO_SAMPLE_S32_LE:
        *bytes_out = 4;
        return HPX_AUDIO_FORMAT_OK;
    default:
        return HPX_AUDIO_FORMAT_UNSUPPORTED_SAMPLE_FORMAT;
    }
}

int
hpx_audio_format_frame_bytes(const struct hpx_audio_format *format,
                             size_t *bytes_out)
{
    size_t sample_bytes;
    int result;

    if (format == 0 || bytes_out == 0 || format->channels == 0) {
        return HPX_AUDIO_FORMAT_INVALID_ARGUMENT;
    }
    result = hpx_audio_sample_bytes(format->sample_format, &sample_bytes);
    if (result != HPX_AUDIO_FORMAT_OK) {
        return result;
    }
    if ((size_t)format->channels > SIZE_MAX / sample_bytes) {
        return HPX_AUDIO_FORMAT_SIZE_OVERFLOW;
    }
    *bytes_out = sample_bytes * (size_t)format->channels;
    return HPX_AUDIO_FORMAT_OK;
}

int
hpx_audio_format_validate(const struct hpx_audio_format *format)
{
    size_t frame_bytes;
    int result;

    if (format == 0) {
        return HPX_AUDIO_FORMAT_INVALID_ARGUMENT;
    }
    if (format->direction != HPX_AUDIO_PLAYBACK &&
        format->direction != HPX_AUDIO_CAPTURE) {
        return HPX_AUDIO_FORMAT_INVALID_ARGUMENT;
    }
    if (format->sample_rate < HPX_AUDIO_MIN_SAMPLE_RATE ||
        format->sample_rate > HPX_AUDIO_MAX_SAMPLE_RATE) {
        return HPX_AUDIO_FORMAT_UNSUPPORTED_RATE;
    }
    if (format->channels == 0 || format->channels > HPX_AUDIO_MAX_CHANNELS) {
        return HPX_AUDIO_FORMAT_UNSUPPORTED_CHANNELS;
    }
    if (format->period_frames < HPX_AUDIO_MIN_PERIOD_FRAMES ||
        format->period_frames > HPX_AUDIO_MAX_PERIOD_FRAMES) {
        return HPX_AUDIO_FORMAT_INVALID_PERIOD;
    }
    result = hpx_audio_format_frame_bytes(format, &frame_bytes);
    if (result != HPX_AUDIO_FORMAT_OK) {
        return result;
    }
    (void)frame_bytes;
    return HPX_AUDIO_FORMAT_OK;
}

int
hpx_audio_period_bytes(const struct hpx_audio_format *format,
                       size_t *bytes_out)
{
    size_t frame_bytes;
    int result;

    if (bytes_out == 0) {
        return HPX_AUDIO_FORMAT_INVALID_ARGUMENT;
    }
    result = hpx_audio_format_validate(format);
    if (result != HPX_AUDIO_FORMAT_OK) {
        return result;
    }
    result = hpx_audio_format_frame_bytes(format, &frame_bytes);
    if (result != HPX_AUDIO_FORMAT_OK) {
        return result;
    }
    if ((size_t)format->period_frames > SIZE_MAX / frame_bytes) {
        return HPX_AUDIO_FORMAT_SIZE_OVERFLOW;
    }
    *bytes_out = (size_t)format->period_frames * frame_bytes;
    return HPX_AUDIO_FORMAT_OK;
}

int
hpx_audio_buffer_bytes_validate(const struct hpx_audio_format *format,
                                size_t buffer_bytes)
{
    size_t frame_bytes;
    size_t period_bytes;
    int result;

    result = hpx_audio_format_validate(format);
    if (result != HPX_AUDIO_FORMAT_OK) {
        return result;
    }
    result = hpx_audio_format_frame_bytes(format, &frame_bytes);
    if (result != HPX_AUDIO_FORMAT_OK) {
        return result;
    }
    if (buffer_bytes == 0 || buffer_bytes % frame_bytes != 0) {
        return HPX_AUDIO_FORMAT_MISALIGNED_BUFFER;
    }
    result = hpx_audio_period_bytes(format, &period_bytes);
    if (result != HPX_AUDIO_FORMAT_OK) {
        return result;
    }
    if (buffer_bytes < period_bytes || buffer_bytes % period_bytes != 0) {
        return HPX_AUDIO_FORMAT_MISALIGNED_BUFFER;
    }
    return HPX_AUDIO_FORMAT_OK;
}
