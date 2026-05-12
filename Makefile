CC      = gcc
TARGET  = prog

CFLAGS  = -O2 -Wall -Wextra -Wpedantic -std=c11 \
          $(shell sdl2-config --cflags) \
          -D_DEFAULT_SOURCE \
          -DPUZZLE_ASSET_PREFIX=\"puzzle_game/\"
LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

OBJS    = main.o source.o main_menu.o save_system.o \
          puzzle_game/joueur.o puzzle_game/character_select.o serial_input.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

main.o: main.c header.h main_menu.h save_system.h serial_input.h
	$(CC) $(CFLAGS) -c -o $@ $<

main_menu.o: main_menu.c main_menu.h serial_input.h
	$(CC) $(CFLAGS) -c -o $@ $<

source.o: source.c header.h
	$(CC) $(CFLAGS) -c -o $@ $<

save_system.o: save_system.c save_system.h header.h serial_input.h
	$(CC) $(CFLAGS) -c -o $@ $<

puzzle_game/joueur.o: puzzle_game/joueur.c puzzle_game/header.h
	$(CC) $(CFLAGS) -c -o $@ $<

puzzle_game/character_select.o: puzzle_game/character_select.c puzzle_game/header.h
	$(CC) $(CFLAGS) -c -o $@ $<

serial_input.o: serial_input.c serial_input.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
