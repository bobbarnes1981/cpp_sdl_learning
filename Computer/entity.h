#ifndef __h_entity
#define __h_entity

class Entity
{
    public:
        Entity();
        ~Entity();
    protected:
        void renderText(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture);
};

#endif