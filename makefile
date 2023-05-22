CC = gcc
CFLAGS = -Wall -Werror -fpic
LDFLAGS = -shared
LIBRARY = st_reactor.so
TARGET = react_server
SRCS = react_server.c st_reactor.c
OBJS = $(SRCS:.c=.o)

all: $(LIBRARY) $(TARGET)

$(LIBRARY): st_reactor.o
	$(CC) $(LDFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L. -l:$(LIBRARY)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIBRARY) $(TARGET)
