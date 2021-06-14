#include <SDL.h>

#include "notice.h"

Notice::Notice()
{
    
};

Notice::~Notice()
{
    
};

void Notice::draw(SDL_Renderer* renderer, int x, int y, int w, int h)
{
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, 0x03, 0xFC, 0xA9, 0xFF);
    SDL_RenderFillRect(renderer, &rect);
    
    SDL_SetRenderDrawColor(renderer, 0x03, 0x36, 0x25, 0xFF);
    SDL_RenderDrawRect(renderer, &rect);
};