#ifndef __h_terminal
#define __h_terminal

class Terminal
{
    public:
        Terminal();
        ~Terminal();
        void draw(SDL_Renderer* renderer, Popup& popupTerminal, Text& textTerminalBuffer, unsigned int currentTicks);
};

#endif