#include <SDL.h>
#include <SDL_ttf.h>

#include "notice.h"

Notice::Notice()
{
    
};

Notice::~Notice()
{
    
};

void Notice::generate(SDL_Renderer* renderer, TTF_Font* font)
{
    SDL_Color colour = { 0x00, 0x00, 0x00 };
    textObj.generate(renderer, font, colour, text);
};

void Notice::draw(SDL_Renderer* renderer, int x, int y, int w, int h)
{
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, 0x03, 0xFC, 0xA9, 0xFF);
    SDL_RenderFillRect(renderer, &rect);
    
    SDL_SetRenderDrawColor(renderer, 0x03, 0x36, 0x25, 0xFF);
    SDL_RenderDrawRect(renderer, &rect);
    
    textObj.x = x+5;
    textObj.y = y+5;
    textObj.draw(renderer);
};