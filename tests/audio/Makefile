CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic

AUDIO_DIR := ../../drivers/audio
RING_TEST := test_pcm_ring
STREAM_TEST := test_stream

.PHONY: all test clean

all: test

$(RING_TEST): test_pcm_ring.c $(AUDIO_DIR)/pcm_ring.c $(AUDIO_DIR)/pcm_ring.h
	$(CC) $(CFLAGS) -I$(AUDIO_DIR) $(AUDIO_DIR)/pcm_ring.c test_pcm_ring.c -o $(RING_TEST)

$(STREAM_TEST): test_stream.c $(AUDIO_DIR)/stream.c $(AUDIO_DIR)/stream.h
	$(CC) $(CFLAGS) -I$(AUDIO_DIR) $(AUDIO_DIR)/stream.c test_stream.c -o $(STREAM_TEST)

test: $(RING_TEST) $(STREAM_TEST)
	./$(RING_TEST)
	./$(STREAM_TEST)

clean:
	rm -f $(RING_TEST) $(STREAM_TEST)
