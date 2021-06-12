#ifndef __h_text
#define __h_text

#include <string>

#include "entity.h"

class Text : Entity
{
    private:
        SDL_Surface* surface;
        SDL_Texture* texture;
    public:
        int x;
        int y;
        Text();
        ~Text();
        void generate(SDL_Renderer* renderer, TTF_Font* font, SDL_Color colour, std::string text);
        void generateWrapped(SDL_Renderer* renderer, TTF_Font* font, SDL_Color colour, std::string text, int length);
        void draw(SDL_Renderer* renderer);
};

#endif