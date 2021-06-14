#ifndef __h_notice
#define __h_notice

#include <string>

enum NoticeType
{
    NT_NONE,
    NT_INFO,
    NT_CRITICAL
};

class Notice
{
    public:
        std::string text;
        NoticeType type;
        Notice();
        ~Notice();
        void draw(SDL_Renderer* renderer, int x, int y, int w, int h);
};

#endif