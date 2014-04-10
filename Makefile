LUA_CFLAGS=$(shell pkg-config --cflags lua5.2)
CPPFLAGS=$(MY_CPPFLAGS)
CFLAGS=-fpic -Wall -Wextra -Werror $(LUA_CFLAGS) $(MY_CFLAGS)
ifeq ($(DEBUG),1)
    CFLAGS+=-g
else
    CFLAGS+=-O2
endif
LDFLAGS=-shared $(MY_LDFLAGS)
TARGET=lsched.so
OBJS=lsched.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

