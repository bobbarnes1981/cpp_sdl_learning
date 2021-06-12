#include <SDL.h>

#include "entity.h"
#include "popup.h"

Popup::Popup()
{
    
};

Popup::~Popup()
{
    
};

bool Popup::draw(SDL_Renderer* renderer, SDL_Texture* texture, unsigned int currentTicks)
{
    SDL_Rect fillRect = { x, y, w, 30 };
    SDL_SetRenderDrawColor(renderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
    
    renderText(renderer, x+5, y+5, texture);
    
    if (currentTicks - openedTicks > POPUP_DELAY)
    {
        fillRect = { x, y+30, w, h-30 };
        SDL_SetRenderDrawColor(renderer, 0x03, 0xFC, 0xA9, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);
        
        SDL_Rect outlineRect = { x, y, w, h };
        SDL_SetRenderDrawColor(renderer, 0x03, 0x36, 0x25, 0xFF);
        SDL_RenderDrawRect(renderer, &outlineRect);
        
        return true;
    }
    
    return false;
};