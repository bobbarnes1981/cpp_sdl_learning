#include <SDL.h>
#include <SDL_ttf.h>

#include "icon.h"
#include "icons.h"

Icons::Icons()
{
    selected = -1;
    xOffset = 30;
    yOffset = 60;
};

Icons::~Icons()
{
    
};

void Icons::draw(SDL_Renderer* renderer, int iconsToDraw, char desktopAlpha, unsigned int elapsedTicks)
{
    // todo: this needs rewriting
    for (int i = 0; i < iconsToDraw; i++)
    {
        icons[i].x = xOffset;
        icons[i].y = yOffset+(i*(ICON_SIZE+30));
        icons[i].w = ICON_SIZE;
        icons[i].h = ICON_SIZE;
        
        icons[i].draw(renderer, selected == i, desktopAlpha);
        if (selected == i)
        {
            if (elapsedTicks > ICON_SELECT_TIMEOUT)
            {
                selected = -1;
            }
        }
        
/*        if (i == 0)
        {
            textTerminal.x = x;
            textTerminal.y = y+h+5;
            textTerminal.draw(renderer);
        }
        if (i == 1)
        {
            textHelp.x = x;
            textHelp.y = y+h+5;
            textHelp.draw(renderer);
        }
        if (i == 2)
        {
            textManage.x = x;
            textManage.y = y+h+5;
            textManage.draw(renderer);
        }
        if (i == 3)
        {
            textBrowser.x = x;
            textBrowser.y = y+h+5;
            textBrowser.draw(renderer);
        }
        if (i == 4)
        {
            textShutdown.x = x;
            textShutdown.y = y+h+5;
            textShutdown.draw(renderer);
        }*/
    }
};
