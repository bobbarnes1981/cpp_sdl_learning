#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "entity.h"
#include "text.h"

Text::Text()
{
    
};

Text::~Text()
{
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
};

void Text::generate(SDL_Renderer* renderer, TTF_Font* font, SDL_Color colour, std::string text)
{
    SDL_DestroyTexture(texture);
    surface = TTF_RenderText_Solid(font, text.c_str(), colour);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void Text::generateWrapped(SDL_Renderer* renderer, TTF_Font* font, SDL_Color colour, std::string text, int length)
{
    SDL_DestroyTexture(texture);
    surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), colour, length);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void Text::draw(SDL_Renderer* renderer)
{
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
}