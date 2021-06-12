#ifndef __h_computer
#define __h_computer

#define STARTUP_FLASH 125
#define STARTUP_BLANK 600
#define STARTUP_FADE 900
#define STARTUP_ICON 1000
#define DOUBLE_CLICK 250
#define ICON_DELAY 125

enum GameState
{
    GS_START,
    GS_DESKTOP,
    GS_RUN,
    GS_SHUTDOWN,
    GS_TERM,
    GS_HELP,
    GS_MANAGE,
    GS_QUIT
};

#endif