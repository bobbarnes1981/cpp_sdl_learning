#ifndef __h_computer
#define __h_computer

#define BOOT_FLASH 125
#define BOOT_TEXT 600
#define DESKTOP_FADE_IN 900
#define DESKTOP_ICON_IN 1000
#define ICON_SELECT_TIMEOUT 125

enum GameState
{
    GS_BOOT,
    GS_DESKTOP,
    GS_RUN,
    GS_SHUTDOWN,
    GS_QUIT
};

#endif