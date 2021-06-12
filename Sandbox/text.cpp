#include <SDL.h>

#include "entity.h"
#include "text.h"

Text::Text()
{
    
};

Text::~Text()
{
    
};

void Text::draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
}