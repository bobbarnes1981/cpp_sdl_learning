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
