#include <SDL.h>
#include <SDL_ttf.h>

#include "popup.h"
#include "text.h"
#include "terminal.h"

Terminal::Terminal()
{
    
};

Terminal::~Terminal()
{
    
};

void Terminal::draw(SDL_Renderer* renderer, Popup& popupTerminal, Text& textTerminal, Text& textTerminalBuffer, unsigned int currentTicks)
{
    if (popupTerminal.draw(renderer, currentTicks))
    {
        SDL_Rect fillRect = { popupTerminal.x+5, popupTerminal.y+30+5, popupTerminal.w-(5*2), popupTerminal.h-30-(5*2) };
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);
        
        textTerminalBuffer.x = popupTerminal.x+5;
        textTerminalBuffer.y = popupTerminal.y+30+5;
        textTerminalBuffer.draw(renderer);
    }
    
    textTerminal.x = popupTerminal.x+5;
    textTerminal.y = popupTerminal.y+5;
    textTerminal.draw(renderer);
};
