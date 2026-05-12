CC      = gcc
TARGET  = prog
SCREEN_W ?= 480
SCREEN_H ?= 320

CFLAGS  = -O2 -Wall -Wextra -Wpedantic -std=c11 \
          $(shell sdl2-config --cflags) \
          -D_DEFAULT_SOURCE \
          -DSCREEN_W=$(SCREEN_W) -DSCREEN_H=$(SCREEN_H) \
          -DPUZZLE_ASSET_PREFIX=\"puzzle_game/\"
LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lm

OBJS    = main.o main_menu.o puzzle_game/game.o serial_input.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

main.o: main.c main_menu.h puzzle_game/game.h serial_input.h
	$(CC) $(CFLAGS) -c -o $@ $<

main_menu.o: main_menu.c main_menu.h serial_input.h
	$(CC) $(CFLAGS) -c -o $@ $<

puzzle_game/game.o: puzzle_game/game.c puzzle_game/game.h puzzle_game/assets.h
	$(CC) $(CFLAGS) -c -o $@ $<

serial_input.o: serial_input.c serial_input.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
