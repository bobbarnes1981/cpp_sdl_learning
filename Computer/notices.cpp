#include <SDL.h>
#include <SDL_ttf.h>

#include "notices.h"

#define NOTICE_WIDTH 150

Notices::Notices()
{
    
};

Notices::~Notices()
{
    
};

void Notices::add(SDL_Renderer* renderer, TTF_Font* font, std::string text, NoticeType type)
{
    for (int i = MAX_NOTICES - 2; i >-1; i--)
    {
        notices[i+1].text = notices[i].text;
        notices[i+1].type = notices[i].type;
        notices[i+1].generate(renderer, font);
    }
    
    notices[0].text = text;
    notices[0].type = type;
    notices[0].generate(renderer, font);

    if (numNotices < MAX_NOTICES)
    {
        numNotices++;
    }
    
};

void Notices::remove(SDL_Renderer* renderer, TTF_Font* font, int notice)
{
    for (int i = notice; i < MAX_NOTICES - 1; i++)
    {
        notices[i].text = notices[i+1].text;
        notices[i].type = notices[i+1].type;
        notices[i].generate(renderer, font);
    }
    
    if (numNotices > 0)
    {
        numNotices--;
    }
}

void Notices::draw(SDL_Renderer* renderer)
{
    for (int i = 0; i < numNotices; i++)
    {
        notices[i].draw(renderer, x - NOTICE_WIDTH - 5, y - (i*35) - 30 - 5, NOTICE_WIDTH, 30);
    }
};

int Notices::clicked(int mX, int mY)
{
    for (int i = 0; i < numNotices; i++)
    {
        if (mX > x - NOTICE_WIDTH -5 && mX < x - NOTICE_WIDTH - 5 +100 && mY > y - (i*35) - 30 - 5 && mY < y - (i*35) - 30 - 5 + 30)
        {
            return i;
        }
    }
    
    return -1;
};
