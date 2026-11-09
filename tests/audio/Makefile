CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic

AUDIO_DIR := ../../drivers/audio
RING_TEST := test_pcm_ring
STREAM_TEST := test_stream
QUEUE_TEST := test_buffer_queue
ENGINE_TEST := test_engine

.PHONY: all test clean

all: test

$(RING_TEST): test_pcm_ring.c $(AUDIO_DIR)/pcm_ring.c $(AUDIO_DIR)/pcm_ring.h
	$(CC) $(CFLAGS) -I$(AUDIO_DIR) $(AUDIO_DIR)/pcm_ring.c test_pcm_ring.c -o $(RING_TEST)

$(STREAM_TEST): test_stream.c $(AUDIO_DIR)/stream.c $(AUDIO_DIR)/stream.h
	$(CC) $(CFLAGS) -I$(AUDIO_DIR) $(AUDIO_DIR)/stream.c test_stream.c -o $(STREAM_TEST)

$(QUEUE_TEST): test_buffer_queue.c $(AUDIO_DIR)/buffer_queue.c $(AUDIO_DIR)/buffer_queue.h
	$(CC) $(CFLAGS) -I$(AUDIO_DIR) $(AUDIO_DIR)/buffer_queue.c test_buffer_queue.c -o $(QUEUE_TEST)

$(ENGINE_TEST): test_engine.c $(AUDIO_DIR)/engine.c $(AUDIO_DIR)/engine.h $(AUDIO_DIR)/stream.c $(AUDIO_DIR)/buffer_queue.c
	$(CC) $(CFLAGS) -I$(AUDIO_DIR) $(AUDIO_DIR)/stream.c $(AUDIO_DIR)/buffer_queue.c $(AUDIO_DIR)/engine.c test_engine.c -o $(ENGINE_TEST)

test: $(RING_TEST) $(STREAM_TEST) $(QUEUE_TEST) $(ENGINE_TEST)
	./$(RING_TEST)
	./$(STREAM_TEST)
	./$(QUEUE_TEST)
	./$(ENGINE_TEST)

clean:
	rm -f $(RING_TEST) $(STREAM_TEST) $(QUEUE_TEST) $(ENGINE_TEST)
