#include <SDL.h>

#include "notices.h"

Notices::Notices()
{
    
};

Notices::~Notices()
{
    
};

void Notices::add()
{
    if (numNotices < 5)
    {
        numNotices++;
    }
};

void Notices::remove()
{
    if (numNotices > 0)
    {
        numNotices--;
    }
}

void Notices::draw(SDL_Renderer* renderer, int x, int y)
{
    for (int i = 0; i < numNotices; i++)
    {
        SDL_Rect rect = { x - 100 - 5, y - (i*35) - 30 - 5, 100, 30 };
        SDL_SetRenderDrawColor(renderer, 0x03, 0xFC, 0xA9, 0xFF);
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_SetRenderDrawColor(renderer, 0x03, 0x36, 0x25, 0xFF);
        SDL_RenderDrawRect(renderer, &rect);
    }
};
