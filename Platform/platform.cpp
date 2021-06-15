#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <cmath>

#include "platform.h"

Mix_Chunk* soundTest = NULL;

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool init();

bool loadMedia();

void close();

void handleKey();
void handleMouse(unsigned int type, int button);

SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

unsigned int currentTicks = 0;
unsigned int lastTicks = 0;
unsigned int elapsedTicks = 0;

bool mouseDown1 = false;
unsigned int mouseClick1 = 0;
bool mouseDown3 = false;
unsigned int mouseClick3 = 0;
int mouseX;
int mouseY;

bool quit = false;

bool init()
{
    bool success = true;
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }
        
        gWindow = SDL_CreateWindow( "Platform", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if(gWindow == NULL)
        {
            printf("Window could not be initialised! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                if (SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND) > 0)
                {
                    printf("Warning: Failed to set render draw blend mode!");
                }
                
                SDL_SetRenderDrawColor(gRenderer, 0xC0, 0xC0, 0xC0, 0xFF);
                
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialise! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
                
                if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
                {
                    printf("SDL_mixer could not initialise! SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }
                
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialise! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }
    
    return success;
}

bool loadMedia()
{
    bool success = true;

    gFont = TTF_OpenFont("font/FiraCode-Regular.ttf", 14);
    if (gFont == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }

    soundTest = Mix_LoadWAV("sound/test.wav" );
    if (soundTest == NULL )
    {
        printf("Failed to load test sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }
    
    return success;
}

void close()
{
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    
    TTF_CloseFont(gFont);
    gFont = NULL;
    
    Mix_FreeChunk(soundTest);
    soundTest = NULL;
    
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
        }
        
        SDL_FreeSurface(loadedSurface);
    }
    
    return newTexture;
}

int playerX = 0;
int playerY = 0;
double playerSpeed = 100;

void handleKey(SDL_Keycode keycode)
{
    switch (keycode)
    {
        case SDLK_ESCAPE:
            quit = true;
            break;
        case SDLK_w:
            playerY -= (playerSpeed * elapsedTicks / 100.0);
            break;
        case SDLK_a:
            playerX -= (playerSpeed * elapsedTicks / 100.0);
            break;
        case SDLK_s:
            playerY += (playerSpeed * elapsedTicks / 100.0);
            break;
        case SDLK_d:
            playerX += (playerSpeed * elapsedTicks / 100.0);
            break;
    }
}

void handleMouse(unsigned int type, int button)
{
    switch(type)
    {
        case SDL_MOUSEBUTTONDOWN:
            int x, y;
            SDL_GetMouseState(&x, &y);
            mouseX = x;
            mouseY = y;
            if (button == 1)
            {
                mouseDown1 = true;
                mouseClick1 = currentTicks;
            }
            if (button == 3)
            {
                mouseDown3 = true;
                mouseClick3 = currentTicks;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (button == 1)
            {
                mouseDown1 = false;
            }
            if (button == 3)
            {
                mouseDown3 = false;
            }
            break;
    }
}

int main(int argc, char* args[])
{
    if (!init())
    {
        printf("Failed to initialise\n");
    }
    else
    {
        if (!loadMedia())
        {
            printf("Failed to load media\n");
        }
        else
        {
            SDL_Event e;
            
            while(quit == false)
            {
                currentTicks = SDL_GetTicks();
                
                while(SDL_PollEvent(&e) != 0)
                {
                    switch (e.type)
                    {
                        case SDL_QUIT:
                            quit = true;
                            break;
                        case SDL_KEYDOWN:
                            handleKey(e.key.keysym.sym);
                            break;
                        case SDL_MOUSEBUTTONDOWN:
                            handleMouse(e.type, e.button.button);
                            break;
                        case SDL_MOUSEBUTTONUP:
                            handleMouse(e.type, e.button.button);
                            break;
                    }
                }
                
                // clear screen
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderClear(gRenderer);
                
                
                SDL_Rect fillRect = { playerX, playerY, 32, 32 };
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(gRenderer, &fillRect);
                
                
                SDL_RenderPresent(gRenderer);
                
                elapsedTicks = currentTicks - lastTicks;
                lastTicks = currentTicks;
                if (elapsedTicks < SCREEN_TICKS_PER_FRAME)
                {
                    SDL_Delay(SCREEN_TICKS_PER_FRAME - elapsedTicks);
                }
            }
        }
    }

    close();
    
    return 0;
}