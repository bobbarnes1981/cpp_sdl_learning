#ifndef __h_icons
#define __h_icons

#define ICON_SELECT_TIMEOUT 125
#define ICON_SIZE 60

enum IconType
{
    I_TERMINAL,
    I_HELP,
    I_MANAGE,
    I_BROWSER,
    I_SHUTDOWN,
    I_NUMBER_OF_ICONS
};

class Icons
{
    public:
        Icon icons[I_NUMBER_OF_ICONS];
        int selected;
        int xOffset;
        int yOffset;
        Icons();
        ~Icons();
        void draw(SDL_Renderer* renderer, int iconsToDraw, char desktopAlpha, unsigned int elapsedTicks);
};

#endif