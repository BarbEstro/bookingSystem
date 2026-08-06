
CC = gcc
CFLAGS = -Wall -Iinclude
RELEASE_FLAGS = -O2
DEBUG_FLAGS = -g -O0 -DDEBUG

CFLAGS += $(RELEASE_FLAGS)

SRC_DIR = src
TEST_DIR = tests

CLIENT_SRCS = $(SRC_DIR)/client.c $(SRC_DIR)/comunicazioneSocket.c $(SRC_DIR)/gestione_login.c $(SRC_DIR)/interfaccia_ui.c
SERVER_SRCS = $(SRC_DIR)/server.c $(SRC_DIR)/comunicazioneSocket.c $(SRC_DIR)/gestione_login.c $(SRC_DIR)/gestione_operazioni_server.c
INIT_SRCS = $(SRC_DIR)/init.c $(SRC_DIR)/gestione_login.c
TEST_SRCS = $(TEST_DIR)/test_gestione_login.c $(SRC_DIR)/gestione_login.c
TEST_UI_SRCS = $(TEST_DIR)/test_interfaccia_ui.c $(SRC_DIR)/interfaccia_ui.c

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
INIT_OBJS = $(INIT_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_UI_OBJS = $(TEST_UI_SRCS:.c=.o)

all: client server init test_login test_interfaccia_ui

debug: CFLAGS = -Wall -Iinclude $(DEBUG_FLAGS)
debug: client server init test_login test_interfaccia_ui

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o client $(CLIENT_OBJS)

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o server $(SERVER_OBJS)

test_login: $(TEST_OBJS)
	$(CC) $(CFLAGS) -o test_login $(TEST_OBJS)

test_interfaccia_ui: $(TEST_UI_OBJS)
	$(CC) $(CFLAGS) -o test_interfaccia_ui $(TEST_UI_OBJS)

init: $(INIT_OBJS)
	$(CC) $(CFLAGS) -o init $(INIT_OBJS)
	./init

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Default clean: remove object files and main binaries but keep test executable
clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(INIT_OBJS) $(TEST_OBJS) $(TEST_UI_OBJS) *.o client server init test_login test_interfaccia_ui

# Separate rule to clean test artifacts when desired
clean-tests:
	rm -f test_login test_interfaccia_ui $(TEST_OBJS) $(TEST_UI_SRCS:.c=.o)

.PHONY: all client server init test_login test_interfaccia_ui clean clean-tests