#include <SDL.h>

#include "entity.h"

Entity::Entity()
{
    
};

Entity::~Entity()
{
    
};

void Entity::renderText(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture)
{
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
};