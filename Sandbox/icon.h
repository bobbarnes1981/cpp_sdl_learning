#ifndef __h_icon
#define __h_icon

#include "entity.h"

class Icon : Entity
{
    public:
        int x;
        int y;
        int w;
        int h;
        Icon();
        ~Icon();
        void draw(SDL_Renderer* renderer, SDL_Texture* texture);
};

#endif