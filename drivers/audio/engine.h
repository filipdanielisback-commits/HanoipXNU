#ifndef HPX_AUDIO_ENGINE_H
#define HPX_AUDIO_ENGINE_H

#include "buffer_queue.h"
#include "stream.h"

enum hpx_audio_engine_result {
    HPX_AUDIO_ENGINE_OK = 0,
    HPX_AUDIO_ENGINE_INVALID_ARGUMENT = -100,
    HPX_AUDIO_ENGINE_INVALID_STATE = -101,
    HPX_AUDIO_ENGINE_BUSY = -102
};

/*
 * Hardware-independent coordinator. The queue and all audio data remain
 * caller-owned. Serialize every operation; this object has no internal locks.
 */
struct hpx_audio_engine {
    struct hpx_audio_stream stream;
    struct hpx_audio_buffer_queue *queue;
};

int hpx_audio_engine_init(struct hpx_audio_engine *engine,
                          struct hpx_audio_buffer_queue *queue);
int hpx_audio_engine_open(struct hpx_audio_engine *engine);
int hpx_audio_engine_start(struct hpx_audio_engine *engine);
int hpx_audio_engine_pause(struct hpx_audio_engine *engine);
int hpx_audio_engine_resume(struct hpx_audio_engine *engine);
int hpx_audio_engine_stop(struct hpx_audio_engine *engine);
int hpx_audio_engine_close(struct hpx_audio_engine *engine);
int hpx_audio_engine_fail(struct hpx_audio_engine *engine, int error_code);

/* Submission is allowed in READY for prebuffering, or while RUNNING. */
int hpx_audio_engine_submit(struct hpx_audio_engine *engine,
                            void *data, size_t length, uint64_t *ticket_out);
/* Acquisition is allowed only while RUNNING. */
int hpx_audio_engine_acquire(struct hpx_audio_engine *engine,
                             uint64_t *ticket_out, void **data_out,
                             size_t *length_out);
/* Completions may arrive while paused or failed; queue errors pass through. */
int hpx_audio_engine_complete(struct hpx_audio_engine *engine,
                              uint64_t ticket, size_t bytes_transferred,
                              int completion_status);
int hpx_audio_engine_get_completion(
    const struct hpx_audio_engine *engine, uint64_t ticket,
    enum hpx_audio_buffer_state *state_out, size_t *bytes_transferred_out,
    int *completion_status_out);
int hpx_audio_engine_release(struct hpx_audio_engine *engine,
                             uint64_t ticket);
int hpx_audio_engine_get_status(const struct hpx_audio_engine *engine,
                                enum hpx_audio_stream_state *state_out,
                                int *last_error_out);

#endif
