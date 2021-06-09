#include <SDL.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();

bool loadMedia();

void close();

SDL_Surface* loadSurface( std::string path );

SDL_Window* gWindow = NULL;

SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gStretchedSurface = NULL;

bool init()
{
    bool success = true;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if(gWindow == NULL)
        {
            printf("Window could not be initialised! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }
    
    return success;
}

bool loadMedia()
{
    bool success = true;

    gStretchedSurface = loadSurface("stretch.bmp");
    if(gStretchedSurface == NULL)
    {
        printf("Failed to load stretching image!\n");
        success = false;
    }

    return success;
}

void close()
{
    SDL_FreeSurface(gStretchedSurface);
    gStretchedSurface = NULL;
    
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    
    SDL_Quit();
}

SDL_Surface* loadSurface(std::string path)
{
    SDL_Surface* optimisedSurface = NULL;

    SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
    }
    else
    {
        optimisedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimisedSurface == NULL)
        {
            printf("Unable to optimise image %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
        }
        
        SDL_FreeSurface(loadedSurface);
    }
    
    return optimisedSurface;
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
            bool quit = false;
            
            SDL_Event e;
            
            while(!quit)
            {
                while(SDL_PollEvent(&e) != 0)
                {
                    switch (e.type)
                    {
                        case SDL_QUIT:
                            quit = true;
                            break;
                    }
                }
                
                SDL_Rect stretchRect;
                stretchRect.x = 0;
                stretchRect.y = 0;
                stretchRect.w = SCREEN_WIDTH;
                stretchRect.h = SCREEN_HEIGHT;
                
                SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect);
                
                SDL_UpdateWindowSurface(gWindow);   
            }
        }
    }
    
    close();
    
    return 0;
}