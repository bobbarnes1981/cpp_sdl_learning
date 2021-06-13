#ifndef __h_button
#define __h_button

#include "entity.h"

class Button : Entity
{
    public:
        bool pressed;
        int x;
        int y;
        int w;
        int h;
        Button();
        ~Button();
        bool update(bool mouseDown, int mouseX, int mouseY);
        void draw(SDL_Renderer* renderer);
    private:
};

#endif