CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -g
LDFLAGS := 

TARGETS := client server

SRCS_COMMON := binder.c
OBJS_COMMON := $(SRCS_COMMON:.c=.o)

.PHONY: all clean run-client run-server

all: $(TARGETS)

# ---- Client ----
client: client.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ---- Server ----
server: server.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ---- Pattern rule ----
%.o: %.c binder.h
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Run helpers ----
run-server: server
	sudo ./server

run-client: client
	sudo ./client

# ---- Clean ----
clean:
	rm -f *.o $(TARGETS)
