#include <SDL.h>

#include "context.h"

Context::Context()
{
    numItems = 4;
    itemWidth = 80;
    itemHeight = 30;
    show = false;
};

Context::~Context()
{
    
};

void Context::draw(SDL_Renderer* renderer, unsigned int elapsedTicks)
{
    int timeToShow = 500;
    
    SDL_Rect fillRect = { x, y, 15, (itemHeight*numItems)+1 };
    SDL_SetRenderDrawColor(renderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
    
    int itemTime = elapsedTicks;
    int itemsToDraw =  itemTime / (timeToShow / numItems);
    if (itemsToDraw > numItems)
    {
        itemsToDraw = numItems;
    }
    int mX, mY;
    SDL_GetMouseState(&mX, &mY);
    for (int i = 0; i < itemsToDraw; i++)
    {
        int x_ = x + 15;
        int y_ = y + (i*itemHeight);
        SDL_Rect rect = { x_, y_, itemWidth, itemHeight + 1 };
        if (mX > x_ && mX < x_ + itemWidth && mY > y_ && mY < y_ + itemHeight)
        {
            SDL_SetRenderDrawColor(renderer, 0x03, 0x46, 0x25, 0xFF);
            SDL_RenderFillRect(renderer, &rect);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0x03, 0xFC, 0xA9, 0xFF);
            SDL_RenderFillRect(renderer, &rect);
        }
        
        SDL_SetRenderDrawColor(renderer, 0x03, 0x36, 0x25, 0xFF);
        SDL_RenderDrawRect(renderer, &rect);
    }
}

int Context::clicked(int mX, int mY)
{
    if (show)
    {
        for (int i = 0; i < numItems; i++)
        {
            int x_ = x + 15;
            int y_ = y + (i*itemHeight);
            if (mX > x_ && mX < x_ + itemWidth && mY > y_ && mY < y_ + itemHeight)
            {
                return i;
            }
        }
    }
    
    return -1;
}