#ifndef __h_text
#define __h_text

#include "entity.h"

class Text : Entity
{
    public:
        int x;
        int y;
        Text();
        ~Text();
        void draw(SDL_Renderer* renderer, SDL_Texture* texture);
};

#endif