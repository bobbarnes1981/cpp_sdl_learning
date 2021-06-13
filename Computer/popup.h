#ifndef __h_popup
#define __h_popup

#include "entity.h"

#define POPUP_DELAY 250

class Popup : Entity
{
    public:
        int x;
        int y;
        int w;
        int h;
        int dragX;
        int dragY;
        unsigned int openedTicks;
        Popup();
        ~Popup();
        bool draw(SDL_Renderer* renderer, unsigned int currentTicks);
        bool isClicked(int mX, int mY);
        void drag(int offsetX, int offsetY);
        void fakeDrag(int offsetX, int offsetY);
};

#endif