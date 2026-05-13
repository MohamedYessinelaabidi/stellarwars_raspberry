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
 *   'R' / 'r'  →  SDLK_r     (restart puzzle)
 *   'Q' / 'q'  →  SDLK_ESCAPE (legacy keyboard-only back)
 *   'P' / 'p'  →  SDLK_a     (previous puzzle piece)
 *   'N' / 'n'  →  SDLK_d     (next puzzle piece)
 *
 * The Arduino sends uppercase once when pressed and lowercase once when
 * released. serial_input_poll() mirrors that as SDL_KEYDOWN / SDL_KEYUP
 * and also keeps a virtual held-key state for gameplay code that reads
 * SDL_GetKeyboardState().
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

/* Returns SDL_GetKeyboardState() merged with the held keys received from
 * the Arduino. Use this for movement code that checks held scancodes. */
const Uint8 *serial_input_get_keyboard_state(int *numkeys);

#endif /* SERIAL_INPUT_H */
