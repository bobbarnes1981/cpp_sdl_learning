#include <SDL.h>
#include <SDL_ttf.h>

#include "popups.h"

Popups::Popups()
{
    
};

Popups::~Popups()
{
    
};


bool Popups::exists(PopupType popup)
{
    for (int i = 0; i < P_NUMBER_OF_POPUPS; i++)
    {
        if (order[i] == popup)
        {
            return true;
        }
    }
    
    return false;
}

PopupType Popups::pop()
{
    PopupType p = order[0];
    for (int i = 0; i < P_NUMBER_OF_POPUPS - 1; i++)
    {
        order[i] = order[i+1];
    }
    order[P_NUMBER_OF_POPUPS-1] = P_NONE;
    
    return p;
}

PopupType Popups::peek()
{
    return order[0];
}

void Popups::push(PopupType popup)
{
    if (!exists(popup))
    {
        for (int i = P_NUMBER_OF_POPUPS; i > 0; i--)
        {
            order[i] = order[i-1];
        }
        order[0] = popup;
    }
}

void Popups::select(PopupType popup)
{
    if (exists(popup))
    {
        remove(popup);
        push(popup);
    }
}

void Popups::remove(PopupType popup)
{
    if (exists(popup))
    {
        for (int i = 0; i < P_NUMBER_OF_POPUPS; i++)
        {
            if (order[i] == popup)
            {
                order[i] = P_NONE;
            }
            if (order[i] == P_NONE && i < P_NUMBER_OF_POPUPS-1)
            {
                order[i] = order[i+1];
                order[i+1] = P_NONE;
            }
        }
    }
}

void Popups::clear()
{
    for (int i = 0; i < P_NUMBER_OF_POPUPS; i++)
    {
        order[i] = P_NONE;
    }
}

PopupType Popups::clicked(int x, int y)
{
    for (int i = 0; i < P_NUMBER_OF_POPUPS; i++)
    {
        switch (order[i])
        {
            case P_HELP:
                if (popups[P_HELP].isClicked(x, y))
                {
                    return P_HELP;
                }
                break;
            case P_MANAGE:
                if (popups[P_MANAGE].isClicked(x, y))
                {
                    return P_MANAGE;
                }
                break;
            case P_TERMINAL:
                if (popups[P_TERMINAL].isClicked(x, y))
                {
                    return P_TERMINAL;
                }
                break;
            case P_BROWSER:
                if (popups[P_BROWSER].isClicked(x, y))
                {
                    return P_BROWSER;
                }
                break;
        }
    }
    
    return P_NONE;
}

PopupType Popups::dragged(int x, int y)
{
    for (int i = 0; i < P_NUMBER_OF_POPUPS; i++)
    {
        switch (order[i])
        {
            case P_HELP:
                if (popups[P_HELP].isDragged(x, y))
                {
                    return P_HELP;
                }
                break;
            case P_MANAGE:
                if (popups[P_MANAGE].isDragged(x, y))
                {
                    return P_MANAGE;
                }
                break;
            case P_TERMINAL:
                if (popups[P_TERMINAL].isDragged(x, y))
                {
                    return P_TERMINAL;
                }
                break;
            case P_BROWSER:
                if (popups[P_BROWSER].isDragged(x, y))
                {
                    return P_BROWSER;
                }
                break;
        }
    }
    
    return P_NONE;
}

void Popups::drag(PopupType popup, int x, int y, int currentTicks)
{
    //printf("%d,%d\n", x, y);
    switch (popup)
    {
        case P_HELP:
            popups[P_HELP].drag(x, y);
            popups[P_HELP].openedTicks = currentTicks;
            break;
        case P_MANAGE:
            popups[P_MANAGE].drag(x, y);
            popups[P_MANAGE].openedTicks = currentTicks;
            break;
        case P_TERMINAL:
            popups[P_TERMINAL].drag(x, y);
            popups[P_TERMINAL].openedTicks = currentTicks;
            break;
        case P_BROWSER:
            popups[P_BROWSER].drag(x, y);
            popups[P_BROWSER].openedTicks = currentTicks;
            break;
    }
}

void Popups::fakeDrag(PopupType popup, int x, int y)
{
    switch (popup)
    {
        case P_HELP:
            popups[P_HELP].fakeDrag(x, y);
            break;
        case P_MANAGE:
            popups[P_MANAGE].fakeDrag(x, y);
            break;
        case P_TERMINAL:
            popups[P_TERMINAL].fakeDrag(x, y);
            break;
        case P_BROWSER:
            popups[P_BROWSER].fakeDrag(x, y);
            break;
    }
}
