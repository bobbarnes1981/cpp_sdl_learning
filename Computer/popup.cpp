#include <SDL.h>
#include <SDL_ttf.h>

#include "popup.h"

Popup::Popup()
{
    dragX = 0;
    dragY = 0;
};

Popup::~Popup()
{
    
};

void Popup::generate(SDL_Renderer* renderer, TTF_Font* font, std::string text)
{
    SDL_Color colour = { 0x00, 0x00, 0x00 };
    textObj.generate(renderer, font, colour, text);
};

bool Popup::draw(SDL_Renderer* renderer, unsigned int currentTicks)
{
    SDL_Rect fillRect = { x, y, w, 30 };
    SDL_SetRenderDrawColor(renderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
    
    textObj.x = x+5;
    textObj.y = y+5;
    textObj.draw(renderer);
    
    if (currentTicks - openedTicks > POPUP_DELAY)
    {
        fillRect = { x, y+30, w, h-30 };
        SDL_SetRenderDrawColor(renderer, 0x03, 0xFC, 0xA9, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);
        
        SDL_Rect outlineRect = { x, y, w, h };
        SDL_SetRenderDrawColor(renderer, 0x03, 0x36, 0x25, 0xFF);
        SDL_RenderDrawRect(renderer, &outlineRect);
        
        if (dragX != 0 && dragY != 0)
        {
            outlineRect = { x-dragX, y-dragY, w, h };
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderDrawRect(renderer, &outlineRect);
        }
        
        return true;
    }
    
    return false;
};

bool Popup::isClicked(int mX, int mY)
{
    return x < mX && x + w > mX && y < mY && y + h > mY;
}

bool Popup::isDragged(int mX, int mY)
{
    return x < mX && x + w > mX && y < mY && y + 30 > mY;
}

void Popup::drag(int offsetX, int offsetY)
{
    x -= offsetX;
    y -= offsetY;
    dragX = 0;
    dragY = 0;
}

void Popup::fakeDrag(int offsetX, int offsetY)
{
    dragX = offsetX;
    dragY = offsetY;
}