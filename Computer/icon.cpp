#include <SDL.h>
#include <SDL_ttf.h>

#include "entity.h"
#include "icon.h"

Icon::Icon()
{
    
};

Icon::~Icon()
{
    
};

void Icon::generate(SDL_Renderer* renderer, TTF_Font* font, std::string text)
{
    SDL_Color colour = { 0x00, 0x00, 0x00 };
    textObj.generate(renderer, font, colour, text);
};

void Icon::draw(SDL_Renderer* renderer, bool selected, char desktopAlpha)
{
    SDL_Rect outlineRect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, 0x03, 0x36, 0x25, desktopAlpha);
    if (selected)
    {
        SDL_RenderFillRect(renderer, &outlineRect);
    }
    else
    {
        SDL_RenderDrawRect(renderer, &outlineRect);
    }
    textObj.x = x;
    textObj.y = y+h+5;
    textObj.draw(renderer);
}