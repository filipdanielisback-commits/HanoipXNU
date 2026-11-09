CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic

WIFI_DIR := ../../drivers/wifi
STATE_TEST := test_state
FAKE_TEST := test_fake_backend
PACKET_TEST := test_packet_queue
CORE_TEST := test_core

.PHONY: all test clean

all: test

$(STATE_TEST): test_state.c $(WIFI_DIR)/state.c $(WIFI_DIR)/state.h
	$(CC) $(CFLAGS) -I$(WIFI_DIR) $(WIFI_DIR)/state.c test_state.c -o $(STATE_TEST)

$(FAKE_TEST): test_fake_backend.c $(WIFI_DIR)/state.c $(WIFI_DIR)/state.h
	$(CC) $(CFLAGS) -I$(WIFI_DIR) $(WIFI_DIR)/state.c test_fake_backend.c -o $(FAKE_TEST)

$(PACKET_TEST): test_packet_queue.c $(WIFI_DIR)/packet_queue.c $(WIFI_DIR)/packet_queue.h
	$(CC) $(CFLAGS) -I$(WIFI_DIR) $(WIFI_DIR)/packet_queue.c test_packet_queue.c -o $(PACKET_TEST)

$(CORE_TEST): test_core.c $(WIFI_DIR)/core.c $(WIFI_DIR)/core.h $(WIFI_DIR)/state.c $(WIFI_DIR)/packet_queue.c
	$(CC) $(CFLAGS) -I$(WIFI_DIR) $(WIFI_DIR)/state.c $(WIFI_DIR)/packet_queue.c $(WIFI_DIR)/core.c test_core.c -o $(CORE_TEST)

test: $(STATE_TEST) $(FAKE_TEST) $(PACKET_TEST) $(CORE_TEST)
	./$(STATE_TEST)
	./$(FAKE_TEST)
	./$(PACKET_TEST)
	./$(CORE_TEST)

clean:
	rm -f $(STATE_TEST) $(FAKE_TEST) $(PACKET_TEST) $(CORE_TEST)
