#ifndef HPX_AUDIO_PCM_RING_H
#define HPX_AUDIO_PCM_RING_H

#include <stddef.h>
#include <stdint.h>

/* Caller-owned storage; this utility performs no allocation. */
struct hpx_audio_pcm_ring {
    uint8_t *storage;
    size_t capacity;
    size_t read_pos;
    size_t write_pos;
    size_t used;
};

/* Returns 0 on success, -1 for invalid arguments. */
int hpx_audio_pcm_ring_init(struct hpx_audio_pcm_ring *ring,
                            uint8_t *storage,
                            size_t capacity);
void hpx_audio_pcm_ring_reset(struct hpx_audio_pcm_ring *ring);
size_t hpx_audio_pcm_ring_available(const struct hpx_audio_pcm_ring *ring);
size_t hpx_audio_pcm_ring_used(const struct hpx_audio_pcm_ring *ring);

/*
 * Copy at most length bytes. Returns the number of bytes copied.
 * Source/destination memory must not overlap the ring's backing storage.
 */
size_t hpx_audio_pcm_ring_write(struct hpx_audio_pcm_ring *ring,
                                const uint8_t *source,
                                size_t length);
size_t hpx_audio_pcm_ring_read(struct hpx_audio_pcm_ring *ring,
                               uint8_t *destination,
                               size_t length);

#endif
