#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

#define STARTUP_FLASH 125
#define STARTUP_BLANK 600
#define STARTUP_FADE 900
#define STARTUP_ICON 1000
#define DOUBLE_CLICK 250
#define ICON_DELAY 125
#define POPUP_DELAY 250

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

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

void drawTermPopup();

bool drawPopup(int x, int y, int width, int height, SDL_Texture* texture, unsigned int ticks);

bool drawButton(int x, int y, int width, int height, SDL_Texture* texture);

void handleKey();

void handleMouse(unsigned int type);

void generateText();

void renderText(int x, int y, SDL_Texture* texture);

SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

SDL_Surface* buttonOKSurface = NULL;
SDL_Texture* buttonOKTexture = NULL;

SDL_Surface* buttonCancelSurface = NULL;
SDL_Texture* buttonCancelTexture = NULL;

unsigned int popupExit = 0;
SDL_Surface* textExitSurface = NULL;
SDL_Texture* textExitTexture = NULL;

unsigned int popupTerm = 0;
SDL_Surface* textTermSurface = NULL;
SDL_Texture* textTermTexture = NULL;

SDL_Surface* textBoot0Surface = NULL;
SDL_Texture* textBoot0Texture = NULL;

SDL_Surface* textBoot1Surface = NULL;
SDL_Texture* textBoot1Texture = NULL;

SDL_Surface* textBoot2Surface = NULL;
SDL_Texture* textBoot2Texture = NULL;

enum GameState
{
    GS_START,
    GS_DESKTOP,
    GS_RUN,
    GS_EXIT,
    GS_TERM,
    GS_QUIT
};

unsigned int currentTicks = 0;
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
    
    SDL_DestroyTexture(textTermTexture);
    SDL_FreeSurface(textTermSurface);
    
    SDL_DestroyTexture(textBoot0Texture);
    SDL_FreeSurface(textBoot0Surface);
    
    SDL_DestroyTexture(textBoot1Texture);
    SDL_FreeSurface(textBoot1Surface);
    
    SDL_DestroyTexture(textBoot2Texture);
    SDL_FreeSurface(textBoot2Surface);
    
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
    SDL_RenderClear(gRenderer);
    
    char alpha = 0xFF;
    if (currentState == GS_START)
    {
        if (currentTicks < STARTUP_FLASH + STARTUP_BLANK)
        {
            alpha = 0x00;
            
            int numBoot = 3;
            int bootTime = currentTicks-(STARTUP_FLASH);
            int bootToDraw =  bootTime / (STARTUP_BLANK / numBoot);
            switch (bootToDraw)
            {
                case 2:
                    renderText(10, 50, textBoot2Texture);
                case 1:
                    renderText(10, 30, textBoot1Texture);
                case 0:
                    renderText(10, 10, textBoot0Texture);
                    break;
            }
            
            if (currentTicks < STARTUP_FLASH)
            {
                SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(gRenderer, &fillRect);
            }
        }
        else
        {
            alpha = (256.0 / STARTUP_FADE) * (currentTicks - (STARTUP_FLASH + STARTUP_BLANK));
    
            if (currentTicks > STARTUP_FADE + STARTUP_FLASH + STARTUP_BLANK)
            {
                //printf("desktop\n");
                currentState = GS_DESKTOP;
            }
        }
    }

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
        if (currentTicks > STARTUP_FLASH + STARTUP_BLANK + STARTUP_FADE + STARTUP_ICON)
        {
            //printf("run\n");
            currentState = GS_RUN;
        }
        else
        {
            int iconTime = currentTicks-(STARTUP_FLASH + STARTUP_BLANK + STARTUP_FADE);
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
                
                switch(iconSelected)
                {
                    case 0:
                        currentState = GS_TERM;
                        popupTerm = currentTicks;
                        break;
                    case 3:
                        currentState = GS_EXIT;
                        popupExit = currentTicks;
                        break;
                }
            }
        }
        
        SDL_SetRenderDrawColor(gRenderer, 0x03, 0x36, 0x25, 0xFF);
        if (iconSelected == i)
        {
            SDL_RenderFillRect(gRenderer, &outlineRect);
            if (currentTicks - mouseClick > ICON_DELAY)
            {
                iconSelected = -1;
            }
        }
        else
        {
            SDL_RenderDrawRect(gRenderer, &outlineRect);
        }
        
        if (i == 0)
        {
            renderText(x, y+h+5, textTermTexture);
        }
        if (i == 3)
        {
            renderText(x, y+h+5, textExitTexture);
        }
    }
}

void renderText(int x, int y, SDL_Texture* texture)
{
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };
    SDL_RenderCopy(gRenderer, texture, NULL, &dstrect);
}

bool buttonOK = false;
bool buttonCancel = false;

void drawExitPopup()
{
    if (drawPopup(60, 60, 150, 100, textExitTexture, popupExit))
    {
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
}

void drawTermPopup()
{
    if (drawPopup(60, 60, 300, 200, textTermTexture, popupTerm))
    {
        SDL_Rect fillRect = { 60+5, 60+30+5, 300-(5*2), 200-30-(5*2) };
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(gRenderer, &fillRect);
    }
}

bool drawPopup(int x, int y, int width, int height, SDL_Texture* texture, unsigned int ticks)
{
    SDL_Rect fillRect = { x, y, width, 30 };
    SDL_SetRenderDrawColor(gRenderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    renderText(x+5, y+5, texture);
    
    if (currentTicks - ticks > POPUP_DELAY)
    {
        fillRect = { x, y+30, width, height-30 };
        SDL_SetRenderDrawColor(gRenderer, 0x03, 0xFC, 0xA9, 0xFF);
        SDL_RenderFillRect(gRenderer, &fillRect);
        
        SDL_Rect outlineRect = { x, y, width, height };
        SDL_SetRenderDrawColor(gRenderer, 0x03, 0x36, 0x25, 0xFF);
        SDL_RenderDrawRect(gRenderer, &outlineRect);
        
        return true;
    }
    
    return false;
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
    
    renderText(x+5, y+5, texture);
    
    return pressed;
}

void handleKey(SDL_Keycode keycode)
{
    //printf("%d\n", keycode);
    switch (keycode)
    {
        case SDLK_ESCAPE:
            if (currentState == GS_RUN)
            {
                currentState = GS_EXIT;
            }
            else if (currentState == GS_EXIT || currentState == GS_TERM)
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
            if (currentTicks - mouseClick < DOUBLE_CLICK)
            {
                //printf("double click\n");
                mouseDouble = true;
            }
            mouseClick = currentTicks;
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
    SDL_Color black = { 0x00, 0x00, 0x00 };
    
    buttonOKSurface = TTF_RenderText_Solid(gFont, "OK", black);
    buttonOKTexture = SDL_CreateTextureFromSurface(gRenderer, buttonOKSurface);
    
    buttonCancelSurface = TTF_RenderText_Solid(gFont, "Cancel", black);
    buttonCancelTexture = SDL_CreateTextureFromSurface(gRenderer, buttonCancelSurface);
    
    textExitSurface = TTF_RenderText_Solid(gFont, "Exit", black);
    textExitTexture = SDL_CreateTextureFromSurface(gRenderer, textExitSurface);
    
    textTermSurface = TTF_RenderText_Solid(gFont, "Term", black);
    textTermTexture = SDL_CreateTextureFromSurface(gRenderer, textTermSurface);
    
    SDL_Color grey = { 0xC0, 0xC0, 0xC0 };
    textBoot0Surface = TTF_RenderText_Solid(gFont, "System Disk OS Version 0.1", grey);
    textBoot0Texture = SDL_CreateTextureFromSurface(gRenderer, textBoot0Surface);
    textBoot1Surface = TTF_RenderText_Solid(gFont, "(c) company name 19xx", grey);
    textBoot1Texture = SDL_CreateTextureFromSurface(gRenderer, textBoot1Surface);
    textBoot2Surface = TTF_RenderText_Solid(gFont, "Startup...", grey);
    textBoot2Texture = SDL_CreateTextureFromSurface(gRenderer, textBoot2Surface);
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
                currentTicks = SDL_GetTicks();
                
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
                
                if (currentState == GS_TERM)
                {
                    drawTermPopup();
                }
                
                SDL_RenderPresent(gRenderer);
                
                unsigned int ticks = currentTicks - lastTicks;
                if (ticks < SCREEN_TICKS_PER_FRAME)
                {
                    SDL_Delay(SCREEN_TICKS_PER_FRAME - ticks);
                }
            }
        }
    }
    
    close();
    
    return 0;
}