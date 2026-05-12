#ifndef SERIAL_INPUT_H
#define SERIAL_INPUT_H

/*
 * serial_input – reads single chars from an Arduino Nano over UART and
 * injects them as synthetic SDL_KEYDOWN / SDL_KEYUP events so the rest
 * of the game needs zero changes to handle physical controls.
 *
 * Expected chars from Arduino:
 *   'W' / 'w'  →  SDLK_UP    (jump / navigate up)
 *   'S' / 's'  →  SDLK_DOWN  (crouch / navigate down)
 *   'A' / 'a'  →  SDLK_LEFT  (move left)
 *   'D' / 'd'  →  SDLK_RIGHT (move right)
 *   'C' / 'c'  →  SDLK_SPACE (shoot / confirm)
 *   ' '        →  SDLK_SPACE (legacy confirm)
 *   'Q' / 'q'  →  SDLK_ESCAPE (pause / back)
 *   'P' / 'p'  →  SDLK_a     (previous puzzle piece)
 *   'N' / 'n'  →  SDLK_d     (next puzzle piece)
 *
 * The Arduino should send the char while the button is held, then send
 * a release sentinel (uppercase = pressed, lowercase = released), OR
 * simply hold the char at ~60 Hz. Both protocols work because
 * serial_input_poll() issues a matching KEYUP 80 ms after the last
 * KEYDOWN for each key.
 */

#include <SDL2/SDL.h>

/* Open the serial port (e.g. "/dev/ttyUSB0") at 9600 baud.
 * Returns 1 on success, 0 on failure (game continues with keyboard only). */
int  serial_input_open(const char *device);

/* Close the serial port (call on shutdown). */
void serial_input_close(void);

/* Call once per frame (before SDL_PollEvent).
 * Reads all pending bytes from the serial port and pushes synthetic
 * SDL keyboard events into the SDL event queue. */
void serial_input_poll(void);

#endif /* SERIAL_INPUT_H */
