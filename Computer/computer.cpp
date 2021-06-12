#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

#include "computer.h"
#include "entity.h"
#include "button.h"
#include "popup.h"
#include "icon.h"
#include "text.h"

Button buttonOK;
Button buttonCancel;

Popup popupShutdown;
Popup popupTerminal;
Popup popupHelp;
Popup popupManage;

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

void drawBootBuffer();

void drawIcons();

void drawShutdownPopup();

void drawTermPopup();

void handleKey();

void handleMouse(unsigned int type);

void generateText();

void terminalBufferUpdate(SDL_Keycode keycode);

void terminalBufferGenerate();

void terminalBufferProcess();

void bootBufferUpdate(std::string line);

void bootBufferGenerate();

void boot();

unsigned int startTime = 0;

bool alphaOverride = false;
char desktopAlpha;

SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

SDL_Surface* buttonOKSurface = NULL;
SDL_Texture* buttonOKTexture = NULL;

SDL_Surface* buttonCancelSurface = NULL;
SDL_Texture* buttonCancelTexture = NULL;

Text textShutdown;
Text textTerminal;
Text textHelp;
Text textManage;

Text textTerminalBuffer;
char termPrompt = '>';
int termLines = 0;
std::string termBuffer = "";

Text textBootBuffer;
int bootLines = 0;
std::string bootBuffer = "";

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
        
        gWindow = SDL_CreateWindow( "Computer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
    
    desktopAlpha = 0xFF;
    
    int numBoot = 3;
    int bootTime = currentTicks-startTime-(STARTUP_FLASH);
    int bootToDraw =  bootTime / (STARTUP_BLANK / numBoot);
    
    if (bootToDraw >= 0 && bootLines < 1)
    {
        bootBufferUpdate("System Disk OS Version 0.1");
    }
    if (bootToDraw >= 1 && bootLines < 2)
    {
        bootBufferUpdate("(c) company name 19xx");
    }
    if (bootToDraw >= 2 && bootLines < 3)
    {
        bootBufferUpdate("Starting...");
    }
    drawBootBuffer();
    
    if (currentState == GS_START)
    {
        if (currentTicks - startTime < STARTUP_FLASH + STARTUP_BLANK)
        {
            desktopAlpha = 0x00;
            
            if (currentTicks-startTime < STARTUP_FLASH)
            {
                SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(gRenderer, &fillRect);
            }
        }
        else
        {
            SDL_RenderClear(gRenderer);
            desktopAlpha = (256.0 / STARTUP_FADE) * (currentTicks - startTime - (STARTUP_FLASH + STARTUP_BLANK));
    
            if (currentTicks - startTime > STARTUP_FADE + STARTUP_FLASH + STARTUP_BLANK)
            {
                //printf("desktop\n");
                currentState = GS_DESKTOP;
            }
        }
    }

    if (alphaOverride)
    {
        desktopAlpha = 0x00;
    }

    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, 30 };
    SDL_SetRenderDrawColor(gRenderer, 0x0A, 0x8C, 0x61, desktopAlpha);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    fillRect = { 0, 30, SCREEN_WIDTH, SCREEN_HEIGHT-30 };
    SDL_SetRenderDrawColor(gRenderer, 0x03, 0xFC, 0xA9, desktopAlpha);
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
                        termBuffer.clear();
                        termLines = 0;
                        termBuffer.push_back(termPrompt);
                        terminalBufferGenerate();
                        popupTerminal.openedTicks = currentTicks;
                        break;
                    case 1:
                        currentState = GS_HELP;
                        popupHelp.openedTicks = currentTicks;
                        break;
                    case 2:
                        currentState = GS_MANAGE;
                        popupManage.openedTicks = currentTicks;
                        break;
                    case 3:
                        currentState = GS_SHUTDOWN;
                        popupShutdown.openedTicks = currentTicks;
                        break;
                }
            }
        }
        
        SDL_SetRenderDrawColor(gRenderer, 0x03, 0x36, 0x25, desktopAlpha);
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
            textTerminal.x = x;
            textTerminal.y = y+h+5;
            textTerminal.draw(gRenderer);
        }
        if (i == 1)
        {
            textHelp.x = x;
            textHelp.y = y+h+5;
            textHelp.draw(gRenderer);
        }
        if (i == 2)
        {
            textManage.x = x;
            textManage.y = y+h+5;
            textManage.draw(gRenderer);
        }
        if (i == 3)
        {
            textShutdown.x = x;
            textShutdown.y = y+h+5;
            textShutdown.draw(gRenderer);
        }
    }
}

bool buttonOKClicked = false;
bool buttonCancelClicked = false;

void drawShutdownPopup()
{
    if (popupShutdown.draw(gRenderer, currentTicks))
    {
        bool newState = buttonOK.draw(gRenderer, buttonOKTexture, mouseDown, mouseX, mouseY);
        if (newState == false && buttonOKClicked == true)
        {
            currentState = GS_QUIT;
        }
        buttonOKClicked = newState;
        
        newState = buttonCancel.draw(gRenderer, buttonCancelTexture, mouseDown, mouseX, mouseY);
        if (newState == false && buttonCancelClicked == true)
        {
            currentState = GS_RUN;
        }
        buttonCancelClicked = newState;
    }
    
    textShutdown.x = popupShutdown.x+5;
    textShutdown.y = popupShutdown.y+5;
    textShutdown.draw(gRenderer);
}

void drawBootBuffer()
{
    textBootBuffer.x = 10;
    textBootBuffer.y = 10;
    textBootBuffer.draw(gRenderer);
}

void drawTermPopup()
{
    if (popupTerminal.draw(gRenderer, currentTicks))
    {
        SDL_Rect fillRect = { popupTerminal.x+5, popupTerminal.y+30+5, popupTerminal.w-(5*2), popupTerminal.h-30-(5*2) };
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(gRenderer, &fillRect);
        
        textTerminalBuffer.x = popupTerminal.x+5;
        textTerminalBuffer.y = popupTerminal.y+30+5;
        textTerminalBuffer.draw(gRenderer);
    }
    
    textTerminal.x = popupTerminal.x+5;
    textTerminal.y = popupTerminal.y+5;
    textTerminal.draw(gRenderer);
}

void drawManagePopup()
{
    if (popupManage.draw(gRenderer, currentTicks))
    {
        
    }
    
    textManage.x = popupManage.x+5;
    textManage.y = popupManage.y+5;
    textManage.draw(gRenderer);
}

void drawHelpPopup()
{
    if (popupHelp.draw(gRenderer, currentTicks))
    {
        
    }
    
    textHelp.x = popupHelp.x+5;
    textHelp.y = popupHelp.y+5;
    textHelp.draw(gRenderer);
}

void handleKey(SDL_Keycode keycode)
{
    //printf("%d\n", keycode);
    switch (keycode)
    {
        case SDLK_ESCAPE:
            if (currentState == GS_RUN)
            {
                currentState = GS_SHUTDOWN;
            }
            else if (currentState == GS_SHUTDOWN || currentState == GS_TERM || currentState == GS_HELP || currentState == GS_MANAGE)
            {
                currentState = GS_RUN;
            }
            break;
        case SDLK_F1:
            alphaOverride = !alphaOverride;
            break;
        default:
            if (currentState == GS_TERM)
            {
                terminalBufferUpdate(keycode);
            }
            break;
    }
}

void bootBufferUpdate(std::string line)
{
    bootBuffer.append(line);
    bootBuffer.append("\r");
    bootLines += 1;
    bootBufferGenerate();
}

void bootBufferGenerate()
{
    while (bootLines > 20)
    {
        std::size_t loc = bootBuffer.find('\r');
        bootBuffer = bootBuffer.substr(loc+1);
        bootLines-=1;
    }
    
    SDL_Color grey = { 0xC0, 0xC0, 0xC0 };
    textBootBuffer.generateWrapped(gRenderer, gFont, grey, bootBuffer.c_str(), SCREEN_WIDTH);
}

void terminalBufferUpdate(SDL_Keycode keycode)
{
    //printf("%d\n", keycode);
    
    switch(keycode)
    {
        case '\r':
            termBuffer.push_back(keycode);
            terminalBufferProcess();
            termBuffer.push_back(termPrompt);
            termLines += 1;
            break;
        case '\b':
            if (termBuffer.length() == 2 || termBuffer.length() > 2 && termBuffer[termBuffer.length()-2] != '\r')
            {
                termBuffer.pop_back();
            }
            break;
        default:
            termBuffer.push_back(keycode);
            break;
    }
    
    terminalBufferGenerate();
}

void terminalBufferGenerate()
{
    while (termLines > 13)
    {
        std::size_t loc = termBuffer.find('\r');
        termBuffer = termBuffer.substr(loc+1);
        termLines-=1;
    }
    
    SDL_Color green = { 0x00, 0xFF, 0x00 };
    textTerminalBuffer.generateWrapped(gRenderer, gFont, green, termBuffer.c_str(), popupTerminal.w-(5*2));
}

void terminalBufferProcess()
{
    std::size_t loc = termBuffer.rfind('\r', termBuffer.length()-2);
    std::string command = termBuffer.substr(loc+2, termBuffer.length()-(loc+2+1));
    if (command.compare("quit") == 0)
    {
        currentState = GS_RUN;
    }
    else if (command.compare("help") == 0)
    {
        termBuffer.append(" quit\r");
        termLines+=1;
    }
    else if (command.compare("reboot") == 0)
    {
        boot();
    }
    else
    {
        termBuffer.append("error\r");
        termLines+=1;
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
    
    textShutdown.generate(gRenderer, gFont, black, "Shutdown");
    
    textTerminal.generate(gRenderer, gFont, black, "Terminal");
    
    textHelp.generate(gRenderer, gFont, black, "Help");
    
    textManage.generate(gRenderer, gFont, black, "Manage");
}

void boot()
{
    bootBufferUpdate("Rebooting");
    bootBuffer.clear();
    bootLines = 0;
    startTime = currentTicks; // SDL_GetTicks();
    currentState = GS_START;
}

int main(int argc, char* args[])
{
    popupTerminal.w = 500;
    popupTerminal.h = 300;
    popupTerminal.x = (SCREEN_WIDTH/2)-popupTerminal.w/2;
    popupTerminal.y = (SCREEN_HEIGHT/2)-popupTerminal.h/2;
    popupTerminal.openedTicks = 0;
    
    popupHelp.w = 250;
    popupHelp.h = 400;
    popupHelp.x = (SCREEN_WIDTH/4*3)-popupHelp.w/2;
    popupHelp.y = (SCREEN_HEIGHT/2)-popupHelp.h/2;
    popupHelp.openedTicks = 0;
    
    popupManage.w = 500;
    popupManage.h = 400;
    popupManage.x = (SCREEN_WIDTH/2)-popupManage.w/2;
    popupManage.y = (SCREEN_HEIGHT/2)-popupManage.h/2;
    popupManage.openedTicks = 0;
    
    popupShutdown.w = 150;
    popupShutdown.h = 100;
    popupShutdown.x = (SCREEN_WIDTH/2)-popupShutdown.w/2;
    popupShutdown.y = (SCREEN_HEIGHT/2)-popupShutdown.h/2;
    popupShutdown.openedTicks = 0;
    
    buttonOK.w = 60;
    buttonOK.h = 30;
    buttonOK.x = popupShutdown.x + 10;
    buttonOK.y = popupShutdown.y + 60;
    
    buttonCancel.w = 60;
    buttonCancel.h = 30;
    buttonCancel.x = popupShutdown.x + 80;
    buttonCancel.y = popupShutdown.y + 60;

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
            
            boot();
            
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
                
                if (currentState == GS_SHUTDOWN)
                {
                    drawShutdownPopup();
                }
                
                if (currentState == GS_TERM)
                {
                    drawTermPopup();
                }
                
                if (currentState == GS_HELP)
                {
                    drawHelpPopup();
                }
                
                if (currentState == GS_MANAGE)
                {
                    drawManagePopup();
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