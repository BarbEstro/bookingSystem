CC = gcc
CFLAGS = -Wall -Iinclude
RELEASE_FLAGS = -O2
DEBUG_FLAGS = -g -O0 -DDEBUG

CFLAGS += $(RELEASE_FLAGS)

BIN_DIR = bin

SRC_DIR = src
TEST_DIR = tests

CLIENT_SRCS = $(SRC_DIR)/client.c $(SRC_DIR)/comunicazioneSocket.c $(SRC_DIR)/gestione_login.c $(SRC_DIR)/interfaccia_ui.c $(SRC_DIR)/gestione_operazioni_client.c
SERVER_SRCS = $(SRC_DIR)/server.c $(SRC_DIR)/comunicazioneSocket.c $(SRC_DIR)/gestione_login.c $(SRC_DIR)/gestione_operazioni_server.c $(SRC_DIR)/mappa_prenotazioni.c $(SRC_DIR)/predicati_prenotazioni.c
INIT_SRCS = $(SRC_DIR)/init.c $(SRC_DIR)/gestione_login.c
TEST_SRCS = $(TEST_DIR)/test_gestione_login.c $(SRC_DIR)/gestione_login.c
TEST_UI_SRCS = $(TEST_DIR)/test_interfaccia_ui.c $(SRC_DIR)/interfaccia_ui.c

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
INIT_OBJS = $(INIT_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_UI_OBJS = $(TEST_UI_SRCS:.c=.o)

all: client server init

debug: CFLAGS = -Wall -Iinclude $(DEBUG_FLAGS)
debug: client server init test_login test_interfaccia_ui

client: $(CLIENT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client $(CLIENT_OBJS)

server: $(SERVER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server $(SERVER_OBJS)

test_login: $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_login $(TEST_OBJS)

test_interfaccia_ui: $(TEST_UI_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_interfaccia_ui $(TEST_UI_OBJS)

init: $(INIT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/init $(INIT_OBJS)

run-init: init
	$(BIN_DIR)/init

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Default clean: remove object files and main binaries but keep test executable
clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(INIT_OBJS) $(TEST_OBJS) $(TEST_UI_OBJS) *.o
	rm -rf $(BIN_DIR)
	rm -rf dati

# Separate rule to clean test artifacts when desired
clean-tests:
	rm -f $(BIN_DIR)/test_login $(BIN_DIR)/test_interfaccia_ui $(TEST_OBJS) $(TEST_UI_SRCS:.c=.o)

run-server: server
	$(BIN_DIR)/server

run-client: client
	$(BIN_DIR)/client

.PHONY: all debug client server init run-init test_login test_interfaccia_ui clean clean-tests run-server run-client