#ifndef __h_context
#define __h_context

class Context
{
    public:
        int numItems;
        int itemHeight;
        int itemWidth;
        int x;
        int y;
        bool show;
        Context();
        ~Context();
        void draw(SDL_Renderer* renderer, unsigned int elapsedTicks);
        int clicked(int mX, int mY);
};

#endif