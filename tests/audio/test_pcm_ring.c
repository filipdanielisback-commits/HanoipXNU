#include "pcm_ring.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int
main(void)
{
    uint8_t storage[8];
    uint8_t first[4];
    uint8_t second[8];
    const uint8_t input_a[] = { 0, 1, 2, 3, 4, 5 };
    const uint8_t input_b[] = { 6, 7, 8, 9, 10, 11 };
    const uint8_t expected[] = { 4, 5, 6, 7, 8, 9, 10, 11 };
    struct hpx_audio_pcm_ring ring;

    assert(hpx_audio_pcm_ring_init(&ring, storage, sizeof(storage)) == 0);
    assert(hpx_audio_pcm_ring_available(&ring) == 8);
    assert(hpx_audio_pcm_ring_write(&ring, input_a, sizeof(input_a)) == 6);
    assert(hpx_audio_pcm_ring_read(&ring, first, sizeof(first)) == 4);
    assert(first[0] == 0 && first[1] == 1 && first[2] == 2 && first[3] == 3);
    assert(hpx_audio_pcm_ring_write(&ring, input_b, sizeof(input_b)) == 6);
    assert(hpx_audio_pcm_ring_used(&ring) == 8);
    assert(hpx_audio_pcm_ring_read(&ring, second, sizeof(second)) == 8);

    for (size_t i = 0; i < sizeof(expected); ++i) {
        assert(second[i] == expected[i]);
    }
    assert(hpx_audio_pcm_ring_used(&ring) == 0);

    hpx_audio_pcm_ring_reset(&ring);
    assert(hpx_audio_pcm_ring_available(&ring) == 8);
    assert(hpx_audio_pcm_ring_init(&ring, storage, 0) == -1);

    puts("pcm ring tests passed");
    return 0;
}
