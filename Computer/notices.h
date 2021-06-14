#ifndef __h_notices
#define __h_notices

class Notices
{
    public:
        int x;
        int y;
        int numNotices;
        Notices();
        ~Notices();
        void add();
        void remove(int notice);
        void draw(SDL_Renderer* renderer);
        int clicked(int mX, int mY);
};

#endif