#ifndef __h_button
#define __h_button

#include "entity.h"

class Button : Entity
{
    public:
        int x;
        int y;
        int w;
        int h;
        Button();
        ~Button();
        bool draw(SDL_Renderer* renderer, bool mouseDown, int mouseX, int mouseY);
    private:
};

#endif