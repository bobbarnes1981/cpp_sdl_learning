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
        unsigned int openedTicks;
        Popup();
        ~Popup();
        bool draw(SDL_Renderer* renderer, SDL_Texture* texture, unsigned int currentTicks);
};

#endif