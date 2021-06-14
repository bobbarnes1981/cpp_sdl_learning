#ifndef __h_icon
#define __h_icon

#include "entity.h"
#include "text.h"

class Icon : Entity
{
    private:
        Text textObj;
    public:
        int x;
        int y;
        int w;
        int h;
        Icon();
        ~Icon();
        void generate(SDL_Renderer* renderer, TTF_Font* font, std::string text);
        void draw(SDL_Renderer* renderer, bool selected, char desktopAlpha);
};

#endif