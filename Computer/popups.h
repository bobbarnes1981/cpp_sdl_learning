#ifndef __h_popups
#define __h_popups

enum PopupType
{
    P_NONE,
    P_TERMINAL,
    P_MANAGE,
    P_HELP,
    P_NUMBER_OF_POPUPS
};

class Popups
{
    public:
        PopupType order[P_NUMBER_OF_POPUPS];
        Popups();
        ~Popups();
        bool exists(PopupType popup);
        PopupType pop();
        PopupType peek();
        void push(PopupType popup);
        void select(PopupType popup);
        void remove(PopupType popup);
        void clear();
};

#endif