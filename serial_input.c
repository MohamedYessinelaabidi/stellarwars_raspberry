#include "serial_input.h"

#include <stdio.h>
#include <string.h>

/* ── POSIX serial headers (Linux / Raspberry Pi) ─────────────────────── */
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

/* ── Internal state ──────────────────────────────────────────────────── */

static int  s_fd = -1;   /* file descriptor, -1 = not open */

/* Each tracked key: SDL scancode, SDL keycode, last-seen timestamp.
 * We auto-release a key 80 ms after the last byte that activated it. */
#define SERIAL_KEY_COUNT 6
#define SERIAL_KEY_RELEASE_MS 80u

typedef struct {
    char      trigger_upper; /* uppercase char from Arduino → press   */
    char      trigger_lower; /* lowercase char from Arduino → release  */
    SDL_Keycode  keycode;
    SDL_Scancode scancode;
    int          pressed;    /* currently injected as held?            */
    Uint32       last_seen;  /* SDL_GetTicks() of last press byte      */
} SerialKey;

static SerialKey s_keys[SERIAL_KEY_COUNT] = {
    { 'W', 'w', SDLK_UP,     SDL_SCANCODE_UP,     0, 0 },
    { 'S', 's', SDLK_DOWN,   SDL_SCANCODE_DOWN,   0, 0 },
    { 'A', 'a', SDLK_LEFT,   SDL_SCANCODE_LEFT,   0, 0 },
    { 'D', 'd', SDLK_RIGHT,  SDL_SCANCODE_RIGHT,  0, 0 },
    { ' ', ' ', SDLK_SPACE,  SDL_SCANCODE_SPACE,  0, 0 },
    { 'Q', 'q', SDLK_ESCAPE, SDL_SCANCODE_ESCAPE, 0, 0 },
};

/* ── Helpers ──────────────────────────────────────────────────────────── */

static void push_key_event(SerialKey *k, Uint32 type)
{
    SDL_Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type                 = type;
    ev.key.state            = (type == SDL_KEYDOWN) ? SDL_PRESSED : SDL_RELEASED;
    ev.key.keysym.sym       = k->keycode;
    ev.key.keysym.scancode  = k->scancode;
    ev.key.repeat           = 0;
    SDL_PushEvent(&ev);
}

static void key_press(SerialKey *k, Uint32 now)
{
    k->last_seen = now;
    if (!k->pressed) {
        k->pressed = 1;
        push_key_event(k, SDL_KEYDOWN);
    }
}

static void key_release(SerialKey *k)
{
    if (k->pressed) {
        k->pressed = 0;
        push_key_event(k, SDL_KEYUP);
    }
}

/* ── Public API ───────────────────────────────────────────────────────── */

int serial_input_open(const char *device)
{
    struct termios tty;

    s_fd = open(device, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (s_fd < 0) {
        fprintf(stderr, "[serial] Cannot open %s: %s\n", device, strerror(errno));
        return 0;
    }

    if (tcgetattr(s_fd, &tty) != 0) {
        fprintf(stderr, "[serial] tcgetattr failed: %s\n", strerror(errno));
        close(s_fd);
        s_fd = -1;
        return 0;
    }

    /* Raw mode, 9600 baud */
    cfmakeraw(&tty);
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cc[VMIN]  = 0;   /* non-blocking read */
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(s_fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "[serial] tcsetattr failed: %s\n", strerror(errno));
        close(s_fd);
        s_fd = -1;
        return 0;
    }

    printf("[serial] Opened %s at 9600 baud\n", device);
    return 1;
}

void serial_input_close(void)
{
    int i;
    if (s_fd >= 0) {
        close(s_fd);
        s_fd = -1;
    }
    /* Release any still-held keys cleanly */
    for (i = 0; i < SERIAL_KEY_COUNT; i++)
        key_release(&s_keys[i]);
}

void serial_input_poll(void)
{
    unsigned char buf[64];
    ssize_t n;
    int i, j;
    Uint32 now = SDL_GetTicks();

    /* 1. Read all pending bytes */
    if (s_fd >= 0) {
        n = read(s_fd, buf, sizeof(buf));
        if (n > 0) {
            for (j = 0; j < (int)n; j++) {
                char c = (char)buf[j];
                for (i = 0; i < SERIAL_KEY_COUNT; i++) {
                    if (c == s_keys[i].trigger_upper) {
                        key_press(&s_keys[i], now);
                        break;
                    }
                    /* Explicit lowercase = release (optional Arduino protocol) */
                    if (c == s_keys[i].trigger_lower &&
                        s_keys[i].trigger_lower != s_keys[i].trigger_upper) {
                        key_release(&s_keys[i]);
                        break;
                    }
                }
            }
        }
    }

    /* 2. Auto-release keys not seen within the timeout window */
    for (i = 0; i < SERIAL_KEY_COUNT; i++) {
        if (s_keys[i].pressed &&
            (now - s_keys[i].last_seen) >= SERIAL_KEY_RELEASE_MS) {
            key_release(&s_keys[i]);
        }
    }
}