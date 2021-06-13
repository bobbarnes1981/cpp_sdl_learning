#ifndef __h_notices
#define __h_notices

class Notices
{
    public:
        int numNotices;
        Notices();
        ~Notices();
        void add();
        void remove();
        void draw(SDL_Renderer* renderer, int x, int y);
};

#endif