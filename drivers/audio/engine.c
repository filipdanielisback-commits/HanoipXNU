#include "engine.h"

static int
hpx_audio_engine_valid(const struct hpx_audio_engine *engine)
{
    return engine != 0 && engine->queue != 0 &&
           engine->queue->slots != 0 && engine->queue->capacity != 0;
}

static int
hpx_audio_engine_buffers_idle(const struct hpx_audio_engine *engine)
{
    return hpx_audio_buffer_queued(engine->queue) == 0 &&
           hpx_audio_buffer_in_flight(engine->queue) == 0 &&
           hpx_audio_buffer_completed(engine->queue) == 0;
}

int
hpx_audio_engine_init(struct hpx_audio_engine *engine,
                      struct hpx_audio_buffer_queue *queue)
{
    if (engine == 0 || queue == 0 || queue->slots == 0 ||
        queue->capacity == 0) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    engine->queue = queue;
    if (hpx_audio_stream_init(&engine->stream) != HPX_AUDIO_STREAM_OK) {
        engine->queue = 0;
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return HPX_AUDIO_ENGINE_OK;
}

int
hpx_audio_engine_open(struct hpx_audio_engine *engine)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_stream_open(&engine->stream);
}

int
hpx_audio_engine_start(struct hpx_audio_engine *engine)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_stream_start(&engine->stream);
}

int
hpx_audio_engine_pause(struct hpx_audio_engine *engine)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_stream_pause(&engine->stream);
}

int
hpx_audio_engine_resume(struct hpx_audio_engine *engine)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_stream_resume(&engine->stream);
}

int
hpx_audio_engine_stop(struct hpx_audio_engine *engine)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    if (!hpx_audio_engine_buffers_idle(engine)) {
        return HPX_AUDIO_ENGINE_BUSY;
    }
    return hpx_audio_stream_stop(&engine->stream);
}

int
hpx_audio_engine_close(struct hpx_audio_engine *engine)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    if (!hpx_audio_engine_buffers_idle(engine)) {
        return HPX_AUDIO_ENGINE_BUSY;
    }
    return hpx_audio_stream_close(&engine->stream);
}

int
hpx_audio_engine_fail(struct hpx_audio_engine *engine, int error_code)
{
    size_t cancelled;
    int result;

    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    result = hpx_audio_stream_fail(&engine->stream, error_code);
    if (result != HPX_AUDIO_STREAM_OK) {
        return result;
    }
    /*
     * Queued buffers cannot be acquired after failure. Complete them as
     * errors so callers can inspect and release them instead of deadlocking
     * shutdown. Already in-flight buffers must still report completion.
     */
    return hpx_audio_buffer_cancel_queued(engine->queue, error_code,
                                          &cancelled);
}

int
hpx_audio_engine_submit(struct hpx_audio_engine *engine,
                        void *data, size_t length, uint64_t *ticket_out)
{
    enum hpx_audio_stream_state state;
    int last_error;
    int result;

    if (!hpx_audio_engine_valid(engine) || data == 0 || length == 0 ||
        ticket_out == 0) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    result = hpx_audio_stream_get_status(&engine->stream, &state, &last_error);
    if (result != HPX_AUDIO_STREAM_OK) {
        return HPX_AUDIO_ENGINE_INVALID_STATE;
    }
    if (state != HPX_AUDIO_STREAM_READY &&
        state != HPX_AUDIO_STREAM_RUNNING) {
        return HPX_AUDIO_ENGINE_INVALID_STATE;
    }
    return hpx_audio_buffer_submit(engine->queue, data, length, ticket_out);
}

int
hpx_audio_engine_acquire(struct hpx_audio_engine *engine,
                         uint64_t *ticket_out, void **data_out,
                         size_t *length_out)
{
    enum hpx_audio_stream_state state;
    int last_error;
    int result;

    if (!hpx_audio_engine_valid(engine) || ticket_out == 0 ||
        data_out == 0 || length_out == 0) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    result = hpx_audio_stream_get_status(&engine->stream, &state, &last_error);
    if (result != HPX_AUDIO_STREAM_OK || state != HPX_AUDIO_STREAM_RUNNING) {
        return HPX_AUDIO_ENGINE_INVALID_STATE;
    }
    return hpx_audio_buffer_acquire(engine->queue, ticket_out,
                                    data_out, length_out);
}

int
hpx_audio_engine_complete(struct hpx_audio_engine *engine,
                          uint64_t ticket, size_t bytes_transferred,
                          int completion_status)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_buffer_complete(engine->queue, ticket,
                                     bytes_transferred, completion_status);
}

int
hpx_audio_engine_get_completion(
    const struct hpx_audio_engine *engine, uint64_t ticket,
    enum hpx_audio_buffer_state *state_out, size_t *bytes_transferred_out,
    int *completion_status_out)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_buffer_get_completion(engine->queue, ticket, state_out,
                                           bytes_transferred_out,
                                           completion_status_out);
}

int
hpx_audio_engine_release(struct hpx_audio_engine *engine, uint64_t ticket)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_buffer_release(engine->queue, ticket);
}

int
hpx_audio_engine_get_status(const struct hpx_audio_engine *engine,
                            enum hpx_audio_stream_state *state_out,
                            int *last_error_out)
{
    if (!hpx_audio_engine_valid(engine)) {
        return HPX_AUDIO_ENGINE_INVALID_ARGUMENT;
    }
    return hpx_audio_stream_get_status(&engine->stream, state_out,
                                       last_error_out);
}
