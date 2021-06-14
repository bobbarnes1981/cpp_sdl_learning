#ifndef __h_notice
#define __h_notice

#include <string>

#include "text.h"

enum NoticeType
{
    NT_NONE,
    NT_INFO,
    NT_CRITICAL
};

class Notice
{
    private:
        Text textObj;
    public:
        std::string text;
        NoticeType type;
        Notice();
        ~Notice();
        void generate(SDL_Renderer* renderer, TTF_Font* font);
        void draw(SDL_Renderer* renderer, int x, int y, int w, int h);
};

#endif