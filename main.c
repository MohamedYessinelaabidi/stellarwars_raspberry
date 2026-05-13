#include "header.h"
#include "main_menu.h"
#include "save_system.h"
#include "serial_input.h"

#define ARDUINO_SERIAL_DEV "/dev/ttyUSB0"

static SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    SDL_Texture *texture;

    if (surface == NULL)
        return NULL;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

static void render_text(SDL_Renderer *renderer, TTF_Font *font,
                        const char *text, int x, int y)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect dst;
    int maxW;

    if (font == NULL)
        return;

    surface = TTF_RenderText_Blended(font, text, (SDL_Color){255, 255, 255, 255});
    if (surface == NULL)
        return;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    dst = (SDL_Rect){x, y, surface->w, surface->h};
    maxW = SCREEN_W - x * 2;
    if (dst.w > maxW && dst.w > 0)
    {
        dst.h = (dst.h * maxW) / dst.w;
        dst.w = maxW;
    }
    SDL_FreeSurface(surface);

    if (texture != NULL)
    {
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_DestroyTexture(texture);
    }
}

static void run_main_game(SDL_Renderer *renderer, TTF_Font *font)
{
    CharacterSelection selection = {0, 0, 0, 0};
    const CharacterDefinition *characters;
    SDL_Texture *background;
    SDL_Event event;
    Joueur player;
    Bullet bullets[MAX_BULLETS];
    int characterCount = 0;
    int running = 1;

    characters = getCharacterDefinitions(&characterCount);
    if (!runCharacterSelectMenu(renderer, font, &selection))
        return;

    if (characterCount <= 0 ||
        selection.p1CharacterIndex < 0 ||
        selection.p1CharacterIndex >= characterCount)
        return;

    if (!initialiserJoueurAvecAssets(&player, renderer, 42, GROUND_Y,
            &characters[selection.p1CharacterIndex].outfits[selection.p1OutfitIndex]))
        return;

    initBullets(bullets, MAX_BULLETS);
    background = load_texture(renderer, "assets/mars_ship_level_full.png");
    stellarMusicStartGameplay();

    while (running)
    {
        const Uint8 *keys;
        Uint32 now;

        serial_input_poll();
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_SPACE)
                tirerBullet(bullets, MAX_BULLETS, &player, 1, SDL_GetTicks());
        }

        keys = serial_input_get_keyboard_state(NULL);
        now = SDL_GetTicks();

        gererEntreeJoueurClavier(&player, keys,
                                  SDL_SCANCODE_LEFT,
                                  SDL_SCANCODE_RIGHT,
                                  SDL_SCANCODE_UP);
        updateJoueur(&player, now);
        updateBullets(bullets, MAX_BULLETS);
        stellarMusicUpdateGameplay();

        SDL_SetRenderDrawColor(renderer, 8, 10, 22, 255);
        SDL_RenderClear(renderer);
        if (background != NULL)
            SDL_RenderCopy(renderer, background, NULL, NULL);

        SDL_SetRenderDrawColor(renderer, 80, 210, 255, 255);
        SDL_RenderDrawLine(renderer, 0, GROUND_Y + PLAYER_H, SCREEN_W, GROUND_Y + PLAYER_H);
        renderJoueur(renderer, &player);
        renderBullets(renderer, bullets, MAX_BULLETS);
        render_text(renderer, font, "Joystick: move/jump   Button1: shoot", 8, 8);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (background != NULL)
        SDL_DestroyTexture(background);
    libererJoueur(&player);
}

int main(void)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font;
    char savePath[SAVE_PATH_MAX];

    serial_input_open(ARDUINO_SERIAL_DEV);

    if (!initSDL(&window, &renderer))
    {
        serial_input_close();
        return 1;
    }

    font = TTF_OpenFont("assets_pluto/font.ttf", 18);
    if (font == NULL)
        font = TTF_OpenFont("puzzle_game/assets/fonts/SpaceMono-Regular.ttf", 18);

    while (run_main_menu(renderer) != MAIN_MENU_START)
        SDL_Delay(16);

    savePath[0] = '\0';
    prompt_select_save(renderer, font, savePath, sizeof(savePath));
    run_main_game(renderer, font);

    if (font != NULL)
        TTF_CloseFont(font);
    shutdownSDL(window, renderer);
    serial_input_close();
    return 0;
}
