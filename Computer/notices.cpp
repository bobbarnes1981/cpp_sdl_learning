#include <SDL.h>

#include "notices.h"

Notices::Notices()
{
    
};

Notices::~Notices()
{
    
};

void Notices::add()
{
    // not implemented
    if (numNotices < MAX_NOTICES)
    {
        numNotices++;
    }
};

void Notices::remove(int notice)
{
    // not implemented
    if (numNotices > 0)
    {
        numNotices--;
    }
}

void Notices::draw(SDL_Renderer* renderer)
{
    for (int i = 0; i < numNotices; i++)
    {
        notices[i].draw(renderer, x - 100 - 5, y - (i*35) - 30 - 5, 100, 30);
    }
};

int Notices::clicked(int mX, int mY)
{
    for (int i = 0; i < numNotices; i++)
    {
        if (mX > x - 100 -5 && mX < x - 100 - 5 +100 && mY > y - (i*35) - 30 - 5 && mY < y - (i*35) - 30 - 5 + 30)
        {
            return i;
        }
    }
    
    return -1;
};
