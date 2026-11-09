#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "engine.h"

int
main(void)
{
    struct hpx_audio_engine engine;
    struct hpx_audio_buffer_queue queue;
    struct hpx_audio_buffer slots[3];
    enum hpx_audio_buffer_state buffer_state;
    enum hpx_audio_stream_state stream_state;
    unsigned char data_a[16] = {0};
    unsigned char data_b[8] = {0};
    uint64_t ticket;
    void *acquired_data;
    size_t acquired_length, transferred;
    int completion_status, last_error;

    assert(hpx_audio_buffer_queue_init(&queue, slots, 3) == HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_engine_init(&engine, &queue) == HPX_AUDIO_ENGINE_OK);
    assert(hpx_audio_engine_open(&engine) == HPX_AUDIO_STREAM_OK);

    /* Prebuffer while READY, but do not acquire until RUNNING. */
    assert(hpx_audio_engine_submit(&engine, data_a, sizeof(data_a),
                                   &ticket) == HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_engine_acquire(&engine, &ticket, &acquired_data,
                                    &acquired_length) ==
           HPX_AUDIO_ENGINE_INVALID_STATE);
    assert(hpx_audio_engine_start(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_acquire(&engine, &ticket, &acquired_data,
                                    &acquired_length) == HPX_AUDIO_QUEUE_OK);
    assert(acquired_data == data_a && acquired_length == sizeof(data_a));

    /* Stop is blocked while a descriptor is in flight. */
    assert(hpx_audio_engine_stop(&engine) == HPX_AUDIO_ENGINE_BUSY);
    assert(hpx_audio_engine_complete(&engine, ticket, sizeof(data_a), 0) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_engine_get_completion(&engine, ticket, &buffer_state,
                                           &transferred, &completion_status) ==
           HPX_AUDIO_QUEUE_OK);
    assert(buffer_state == HPX_AUDIO_BUFFER_COMPLETE);
    assert(transferred == sizeof(data_a) && completion_status == 0);
    assert(hpx_audio_engine_release(&engine, ticket) == HPX_AUDIO_QUEUE_OK);

    assert(hpx_audio_engine_pause(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_submit(&engine, data_b, sizeof(data_b), &ticket) ==
           HPX_AUDIO_ENGINE_INVALID_STATE);
    assert(hpx_audio_engine_resume(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_submit(&engine, data_b, sizeof(data_b), &ticket) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_engine_acquire(&engine, &ticket, &acquired_data,
                                    &acquired_length) == HPX_AUDIO_QUEUE_OK);
    assert(acquired_data == data_b && acquired_length == sizeof(data_b));
    assert(hpx_audio_engine_complete(&engine, ticket, 4, -7) ==
           HPX_AUDIO_QUEUE_OK);
    assert(hpx_audio_engine_get_completion(&engine, ticket, &buffer_state,
                                           &transferred, &completion_status) ==
           HPX_AUDIO_QUEUE_OK);
    assert(buffer_state == HPX_AUDIO_BUFFER_ERROR);
    assert(transferred == 4 && completion_status == -7);
    assert(hpx_audio_engine_release(&engine, ticket) == HPX_AUDIO_QUEUE_OK);

    assert(hpx_audio_engine_stop(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_close(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_get_status(&engine, &stream_state, &last_error) ==
           HPX_AUDIO_STREAM_OK);
    assert(stream_state == HPX_AUDIO_STREAM_CLOSED && last_error == 0);

    /* Failure path preserves the error until stop, then permits close. */
    assert(hpx_audio_engine_open(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_start(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_fail(&engine, -42) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_get_status(&engine, &stream_state, &last_error) ==
           HPX_AUDIO_STREAM_OK);
    assert(stream_state == HPX_AUDIO_STREAM_FAILED && last_error == -42);
    assert(hpx_audio_engine_stop(&engine) == HPX_AUDIO_STREAM_OK);
    assert(hpx_audio_engine_close(&engine) == HPX_AUDIO_STREAM_OK);

    puts("audio engine coordinator test passed");
    return 0;
}
