#include "popups.h"

Popups::Popups()
{
    
};

Popups::~Popups()
{
    
};


bool Popups::exists(PopupType popup)
{
    for (int i = 0; i < P_NUMBER_OF_POPUPS - 1; i++)
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
        
        switch(popup)
        {
            case P_TERMINAL:
                order[0] = P_TERMINAL;
                
//                currentState = GS_TERM;
//                termBuffer.clear();
//                termLines = 0;
//                termBuffer.push_back(termPrompt);
//                terminalBufferGenerate();
//                popupTerminal.openedTicks = currentTicks;
                break;
            case P_HELP:
                order[0] = P_HELP;
                
//                currentState = GS_HELP;
//                popupHelp.openedTicks = currentTicks;
                break;
            case P_MANAGE:
                order[0] = P_MANAGE;
                
//                currentState = GS_MANAGE;
//                popupManage.openedTicks = currentTicks;
                break;
        }
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
            if (i > 0 && order[i-1] == P_NONE)
            {
                order[i-1] = order[i];
            }
        }
    }
}