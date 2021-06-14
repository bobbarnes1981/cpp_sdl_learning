#ifndef __h_notices
#define __h_notices

#include "notice.h"

#define MAX_NOTICES 5

class Notices
{
    public:
        int x;
        int y;
        int numNotices;
        Notice notices[MAX_NOTICES];
        Notices();
        ~Notices();
        void add();
        void remove(int notice);
        void draw(SDL_Renderer* renderer);
        int clicked(int mX, int mY);
};

#endif