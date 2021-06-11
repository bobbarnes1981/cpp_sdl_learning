#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

#define STARTUP_FLASH 125
#define STARTUP_BLANK 65
#define STARTUP_FADE 2500
#define STARTUP_ICON 1000
#define DOUBLE_CLICK 250
#define ICON_DELAY 250

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int mouseX = -1;
int mouseY = -1;
bool mouseDown = false;
bool mouseDouble = false;
unsigned int mouseClick = 0;

bool init();

bool loadMedia();

void close();

void drawDesktop();

void drawIcons();

void drawExitPopup();

void drawPopup(int x, int y, int width, int height, SDL_Texture* texture);

bool drawButton(int x, int y, int width, int height, SDL_Texture* texture);

void handleKey();

void handleMouse(unsigned int type);

void generateText();

SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

SDL_Surface* buttonOKSurface = NULL;
SDL_Texture* buttonOKTexture = NULL;

SDL_Surface* buttonCancelSurface = NULL;
SDL_Texture* buttonCancelTexture = NULL;

SDL_Surface* textExitSurface = NULL;
SDL_Texture* textExitTexture = NULL;

enum GameState
{
    GS_START,
    GS_DESKTOP,
    GS_RUN,
    GS_EXIT,
    GS_QUIT
};

unsigned int lastTicks = 0;
GameState currentState = GS_START;

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
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }
        
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
                
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialise! SDL_image Error: %s\n", IMG_GetError());
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


    gFont = TTF_OpenFont("FiraCode-Regular.ttf", 14);
    if (gFont == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
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
    
    SDL_DestroyTexture(buttonOKTexture);
    SDL_FreeSurface(buttonOKSurface);
    
    SDL_DestroyTexture(buttonCancelTexture);
    SDL_FreeSurface(buttonCancelSurface);
    
    SDL_DestroyTexture(textExitTexture);
    SDL_FreeSurface(textExitSurface);
    
    TTF_CloseFont(gFont);
    gFont = NULL;
    
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

void drawDesktop()
{
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    
    char alpha = 0xFF;
    if (currentState == GS_START)
    {
        if (lastTicks < STARTUP_FLASH + STARTUP_BLANK)
        {
            if (lastTicks < STARTUP_FLASH)
            {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
        else
        {
            alpha = (256.0 / STARTUP_FADE) * (lastTicks - (STARTUP_FLASH + STARTUP_BLANK));
    
            if (lastTicks > STARTUP_FADE + STARTUP_FLASH)
            {
                printf("desktop\n");
                currentState = GS_DESKTOP;
            }
        }
    }
    
    SDL_RenderClear(gRenderer);

    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, 30 };
    SDL_SetRenderDrawColor(gRenderer, 0x0A, 0x8C, 0x61, alpha);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    fillRect = { 0, 30, SCREEN_WIDTH, SCREEN_HEIGHT-30 };
    SDL_SetRenderDrawColor(gRenderer, 0x03, 0xFC, 0xA9, alpha);
    SDL_RenderFillRect(gRenderer, &fillRect);
}

int iconSelected = -1;

void drawIcons()
{
    int iconSize = 60;
    int numIcons = 4;
    unsigned int iconsToDraw = 4;
    
    if (currentState == GS_DESKTOP)
    {
        if (lastTicks > STARTUP_FLASH + STARTUP_BLANK + STARTUP_FADE + STARTUP_ICON)
        {
            printf("run\n");
            currentState = GS_RUN;
        }
        else
        {
            int iconTime = lastTicks-(STARTUP_FLASH + STARTUP_BLANK + STARTUP_FADE);
            iconsToDraw =  iconTime / (STARTUP_ICON / numIcons);
        }
    }
    
    for (int i = 0; i < iconsToDraw; i++)
    {
        int x = 30;
        int y = 60+(i*(iconSize+30));
        int w = iconSize;
        int h = iconSize;
        SDL_Rect outlineRect = { x, y, w, h };
        
        if (currentState == GS_RUN)
        {
            if (mouseDown && mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h)
            {
                iconSelected = i;
                
                if (iconSelected == 3)
                {
                    currentState = GS_EXIT;
                }
            }
        }
        
        SDL_SetRenderDrawColor(gRenderer, 0x03, 0x36, 0x25, 0xFF);
        if (iconSelected == i)
        {
            SDL_RenderFillRect(gRenderer, &outlineRect);
            if (lastTicks - mouseClick > ICON_DELAY)
            {
                iconSelected = -1;
            }
        }
        else
        {
            SDL_RenderDrawRect(gRenderer, &outlineRect);
        }
        
        if (i == 3)
        {
            int texW = 0;
            int texH = 0;
            SDL_QueryTexture(textExitTexture, NULL, NULL, &texW, &texH);
            SDL_Rect dstrect = { x, y+h, texW, texH };
            SDL_RenderCopy(gRenderer, textExitTexture, NULL, &dstrect);
        }
    }
}

bool buttonOK = false;
bool buttonCancel = false;

void drawExitPopup()
{
    drawPopup(60, 60, 150, 100, textExitTexture);
    bool newState = drawButton(70, 120, 60, 30, buttonOKTexture);
    if (newState == false && buttonOK == true)
    {
        currentState = GS_QUIT;
    }
    buttonOK = newState;
    
    newState = drawButton(140, 120, 60, 30, buttonCancelTexture);
    if (newState == false && buttonCancel == true)
    {
        currentState = GS_RUN;
    }
    buttonCancel = newState;
}

void drawPopup(int x, int y, int width, int height, SDL_Texture* texture)
{
    SDL_Rect fillRect = { x, y, width, 30 };
    SDL_SetRenderDrawColor(gRenderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    fillRect = { x, y+30, width, height-30 };
    SDL_SetRenderDrawColor(gRenderer, 0x03, 0xFC, 0xA9, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    SDL_Rect outlineRect = { x, y, width, height };
    SDL_SetRenderDrawColor(gRenderer, 0x03, 0x36, 0x25, 0xFF);
    SDL_RenderDrawRect(gRenderer, &outlineRect);
    
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y+30, texW, texH };
    SDL_RenderCopy(gRenderer, texture, NULL, &dstrect);
}

bool drawButton(int x, int y, int width, int height, SDL_Texture* texture)
{
    bool pressed = false;
    if (mouseDown && mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height)
    {
        pressed = true;
    }
    
    SDL_Rect fillRect = { x, y, width, height };
    SDL_SetRenderDrawColor(gRenderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    if (pressed)
    {
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    }
    else
    {
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    SDL_RenderDrawLine(gRenderer, x, y, x+width, y);
    SDL_RenderDrawLine(gRenderer, x, y, x, y+height);
    if (pressed)
    {
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    else
    {
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    }
    SDL_RenderDrawLine(gRenderer, x, y+height, x+width, y+height);
    SDL_RenderDrawLine(gRenderer, x+width, y, x+width, y+height);
    
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };
    SDL_RenderCopy(gRenderer, texture, NULL, &dstrect);
    
    return pressed;
}

void handleKey(SDL_Keycode keycode)
{
    printf("%d\n", keycode);
    switch (keycode)
    {
        case SDLK_ESCAPE:
            if (currentState == GS_RUN)
            {
                currentState = GS_EXIT;
            }
            else if (currentState == GS_EXIT)
            {
                currentState = GS_RUN;
            }
            break;
    }
}

void handleMouse(unsigned int type)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    //printf("%d %d\n", x, y);
    mouseX = x;
    mouseY = y;
    switch(type)
    {
        case SDL_MOUSEBUTTONDOWN:
            mouseDown = true;
            if (lastTicks - mouseClick < DOUBLE_CLICK)
            {
                //printf("double click\n");
                mouseDouble = true;
            }
            mouseClick = lastTicks;
            //printf("down\n");
            break;
        case SDL_MOUSEBUTTONUP:
            mouseDown = false;
            mouseDouble = false;
            //printf("up\n");
            break;
    }
}

void generateText()
{
    SDL_Color colour = { 0x00, 0x00, 0x00 };
    
    buttonOKSurface = TTF_RenderText_Solid(gFont, "OK", colour);
    buttonOKTexture = SDL_CreateTextureFromSurface(gRenderer, buttonOKSurface);
    
    buttonCancelSurface = TTF_RenderText_Solid(gFont, "Cancel", colour);
    buttonCancelTexture = SDL_CreateTextureFromSurface(gRenderer, buttonCancelSurface);
    
    textExitSurface = TTF_RenderText_Solid(gFont, "Exit", colour);
    textExitTexture = SDL_CreateTextureFromSurface(gRenderer, textExitSurface);
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
            
            generateText();
            
            while(currentState != GS_QUIT)
            {
                lastTicks = SDL_GetTicks();
                
//                SDL_SetWindowTitle(gWindow, );
                
                while(SDL_PollEvent(&e) != 0)
                {
                    switch (e.type)
                    {
                        case SDL_QUIT:
                            currentState = GS_QUIT;
                            break;
                        case SDL_KEYDOWN:
                            handleKey(e.key.keysym.sym);
                            break;
                        case SDL_MOUSEBUTTONDOWN:
                            handleMouse(e.type);
                            break;
                        case SDL_MOUSEBUTTONUP:
                            handleMouse(e.type);
                            break;
                    }
                }
                
                drawDesktop();
                
                if (currentState != GS_START)
                {
                    drawIcons();
                }
                
                if (currentState == GS_EXIT)
                {
                    drawExitPopup();
                }
                
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    
    close();
    
    return 0;
}