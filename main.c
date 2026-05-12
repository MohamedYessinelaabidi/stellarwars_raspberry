#include <stdio.h>
#include "main_menu.h"
#include "puzzle_game/game.h"
#include "serial_input.h"

/* Serial device the Arduino Nano is connected to on the Pi.
 * Change to /dev/ttyACM0 if the Nano uses a CDC-ACM USB chip. */
#ifndef ARDUINO_SERIAL_DEV
#define ARDUINO_SERIAL_DEV "/dev/ttyUSB0"
#endif

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    /* Open Arduino serial port (non-fatal: game works with keyboard if absent) */
    serial_input_open(ARDUINO_SERIAL_DEV);

    GameContext ctx;
    Uint32 result_started = 0;

    
    if (!game_init(&ctx)) {
        fprintf(stderr, "game_init failed – aborting.\n");
        game_destroy(&ctx);
        return 1;
    }

    if (run_main_menu(ctx.renderer) != MAIN_MENU_START) {
        game_destroy(&ctx);
        serial_input_close();
        return 0;
    }

    
    int running = 1;
    SDL_Event event;

    while (running) {
        /* Inject Arduino button presses as SDL keyboard events */
        serial_input_poll();

        game_render(&ctx);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }
            game_handle_event(&ctx, &event);
        }

        game_update(&ctx);

        if (ctx.state != STATE_PLAYING) {
            if (result_started == 0)
                result_started = SDL_GetTicks();

            if (SDL_GetTicks() - result_started >= 1200) {
                game_restart(&ctx);
                result_started = 0;
            }
        }
    }

    
    game_destroy(&ctx);
    serial_input_close();
    return 0;
}
