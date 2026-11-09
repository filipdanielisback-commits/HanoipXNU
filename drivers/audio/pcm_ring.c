#include "pcm_ring.h"

#include <string.h>

int
hpx_audio_pcm_ring_init(struct hpx_audio_pcm_ring *ring,
                        uint8_t *storage,
                        size_t capacity)
{
    if (ring == NULL || storage == NULL || capacity == 0) {
        return -1;
    }

    ring->storage = storage;
    ring->capacity = capacity;
    ring->read_pos = 0;
    ring->write_pos = 0;
    ring->used = 0;
    return 0;
}

void
hpx_audio_pcm_ring_reset(struct hpx_audio_pcm_ring *ring)
{
    if (ring == NULL || ring->storage == NULL || ring->capacity == 0) {
        return;
    }

    ring->read_pos = 0;
    ring->write_pos = 0;
    ring->used = 0;
}

size_t
hpx_audio_pcm_ring_available(const struct hpx_audio_pcm_ring *ring)
{
    if (ring == NULL || ring->storage == NULL || ring->capacity == 0 ||
        ring->used > ring->capacity) {
        return 0;
    }

    return ring->capacity - ring->used;
}

size_t
hpx_audio_pcm_ring_used(const struct hpx_audio_pcm_ring *ring)
{
    if (ring == NULL || ring->storage == NULL || ring->capacity == 0 ||
        ring->used > ring->capacity) {
        return 0;
    }

    return ring->used;
}

size_t
hpx_audio_pcm_ring_write(struct hpx_audio_pcm_ring *ring,
                         const uint8_t *source,
                         size_t length)
{
    size_t count;
    size_t first;

    if (ring == NULL || source == NULL || ring->storage == NULL ||
        ring->capacity == 0 || ring->used > ring->capacity) {
        return 0;
    }

    count = length;
    if (count > ring->capacity - ring->used) {
        count = ring->capacity - ring->used;
    }

    first = ring->capacity - ring->write_pos;
    if (first > count) {
        first = count;
    }

    if (first != 0) {
        memcpy(ring->storage + ring->write_pos, source, first);
        memcpy(ring->storage, source + first, count - first);
    }

    if (count >= ring->capacity - ring->write_pos) {
        ring->write_pos = count - (ring->capacity - ring->write_pos);
    } else {
        ring->write_pos += count;
    }
    ring->used += count;
    return count;
}

size_t
hpx_audio_pcm_ring_read(struct hpx_audio_pcm_ring *ring,
                        uint8_t *destination,
                        size_t length)
{
    size_t count;
    size_t first;

    if (ring == NULL || destination == NULL || ring->storage == NULL ||
        ring->capacity == 0 || ring->used > ring->capacity) {
        return 0;
    }

    count = length;
    if (count > ring->used) {
        count = ring->used;
    }

    first = ring->capacity - ring->read_pos;
    if (first > count) {
        first = count;
    }

    if (first != 0) {
        memcpy(destination, ring->storage + ring->read_pos, first);
        memcpy(destination + first, ring->storage, count - first);
    }

    if (count >= ring->capacity - ring->read_pos) {
        ring->read_pos = count - (ring->capacity - ring->read_pos);
    } else {
        ring->read_pos += count;
    }
    ring->used -= count;
    return count;
}
