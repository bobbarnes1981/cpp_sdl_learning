#include <SDL.h>

#include "entity.h"
#include "button.h"

Button::Button()
{
    
};

Button::~Button()
{
    
};

bool Button::update(bool mouseDown, int mouseX, int mouseY)
{
    bool newPressed = false;
    if (mouseDown && mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h)
    {
        newPressed = true;
    }
    if (newPressed == false && pressed == true)
    {
        pressed = newPressed;
        return true;
    }
    
    pressed = newPressed;
    return false;
}

void Button::draw(SDL_Renderer* renderer)
{
    SDL_Rect fillRect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
    
    if (pressed)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    SDL_RenderDrawLine(renderer, x, y, x+w, y);
    SDL_RenderDrawLine(renderer, x, y, x, y+h);
    if (pressed)
    {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    }
    SDL_RenderDrawLine(renderer, x, y+h, x+w, y+h);
    SDL_RenderDrawLine(renderer, x+w, y, x+w, y+h);
};