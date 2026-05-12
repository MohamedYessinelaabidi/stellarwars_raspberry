#ifndef ASSETS_H
#define ASSETS_H

#ifndef PUZZLE_ASSET_PREFIX
#define PUZZLE_ASSET_PREFIX ""
#endif

#define ASSET_PATH(path) PUZZLE_ASSET_PREFIX path

#define FONT_PATH        ASSET_PATH("assets/fonts/SpaceMono-Regular.ttf")
#define FONT_BOLD_PATH   ASSET_PATH("assets/fonts/SpaceMono-Bold.ttf")

#define BG_IMAGE_PATHS { \
    ASSET_PATH("assets/images/space_bg.png"), \
    ASSET_PATH("assets/images/space_bg2.png"), \
    ASSET_PATH("assets/images/space_bg3.png"), \
    ASSET_PATH("assets/images/space_bg4.png") \
}
#define BG_IMAGE_COUNT 4

#define SFX_SUCCESS_PATH ASSET_PATH("assets/sounds/success.wav")
#define SFX_FAIL_PATH    ASSET_PATH("assets/sounds/fail.wav")
#define SFX_PICK_PATH    ASSET_PATH("assets/sounds/pick.wav")

#define WINDOW_TITLE  "Interstellar Puzzle"
#ifndef SCREEN_W
#define SCREEN_W 480
#endif
#ifndef SCREEN_H
#define SCREEN_H 320
#endif
#define WINDOW_W      SCREEN_W
#define WINDOW_H      SCREEN_H

#define MISSING_W     74
#define MISSING_H     74

#define MISSING_MIN_X  18
#define MISSING_MAX_X  (WINDOW_W - MISSING_W - 18)
#define MISSING_MIN_Y  18
#define MISSING_MAX_Y  (TRAY_Y - MISSING_H - 18)

#define TRAY_Y        218
#define TRAY_PIECE_W  MISSING_W
#define TRAY_PIECE_H  MISSING_H
#define TRAY_GAP      18

#define TIMER_BAR_X   18
#define TIMER_BAR_Y   300
#define TIMER_BAR_W   (WINDOW_W - 36)
#define TIMER_BAR_H   8
#define TIMER_SECONDS 60

#endif
