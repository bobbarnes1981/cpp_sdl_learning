#ifndef __h_popups
#define __h_popups

#include "popup.h"

enum PopupType
{
    P_NONE,
    P_TERMINAL,
    P_MANAGE,
    P_HELP,
    P_BROWSER,
    P_NUMBER_OF_POPUPS
};

class Popups
{
    public:
        PopupType order[P_NUMBER_OF_POPUPS];
        Popup popups[P_NUMBER_OF_POPUPS];
        Popups();
        ~Popups();
        bool exists(PopupType popup);
        PopupType pop();
        PopupType peek();
        void push(PopupType popup);
        void select(PopupType popup);
        void remove(PopupType popup);
        void clear();
        PopupType clicked(int x, int y);
        PopupType dragged(int x, int y);
        void drag(PopupType popup, int x, int y, int currentTicks);
        void fakeDrag(PopupType popup, int x, int y);
};

#endif