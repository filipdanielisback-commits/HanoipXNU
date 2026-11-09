CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic

WIFI_DIR := ../../drivers/wifi
STATE_TEST := test_state
FAKE_TEST := test_fake_backend

.PHONY: all test clean

all: test

$(STATE_TEST): test_state.c $(WIFI_DIR)/state.c $(WIFI_DIR)/state.h
	$(CC) $(CFLAGS) -I$(WIFI_DIR) $(WIFI_DIR)/state.c test_state.c -o $(STATE_TEST)

$(FAKE_TEST): test_fake_backend.c $(WIFI_DIR)/state.c $(WIFI_DIR)/state.h
	$(CC) $(CFLAGS) -I$(WIFI_DIR) $(WIFI_DIR)/state.c test_fake_backend.c -o $(FAKE_TEST)

test: $(STATE_TEST) $(FAKE_TEST)
	./$(STATE_TEST)
	./$(FAKE_TEST)

clean:
	rm -f $(STATE_TEST) $(FAKE_TEST)
