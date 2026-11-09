CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic

AUDIO_DIR := ../../drivers/audio
TEST_BIN := test_pcm_ring

.PHONY: all test clean

all: test

$(TEST_BIN): test_pcm_ring.c $(AUDIO_DIR)/pcm_ring.c $(AUDIO_DIR)/pcm_ring.h
	$(CC) $(CFLAGS) -I$(AUDIO_DIR) $(AUDIO_DIR)/pcm_ring.c test_pcm_ring.c -o $(TEST_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(TEST_BIN)
