#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

#include "computer.h"
#include "entity.h"
#include "button.h"
#include "popups.h"
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
bool mouseDown1 = false;
unsigned int mouseClick1 = -1;
bool mouseDown3 = false;
unsigned int mouseClick3 = -1;

bool init();

bool loadMedia();

void close();

void drawBoot();

void drawDesktop();

void drawBootBuffer();

void drawIcons();

void drawShutdownPopup();

void drawTermPopup();

void handleKey();

void handleMouse(unsigned int type, int button);

void generateText();

void terminalBufferUpdate(SDL_Keycode keycode);

void terminalBufferGenerate();

void terminalBufferProcess();

void bootBufferUpdate(std::string line);

void bootBufferGenerate();

void boot();

void drawContextMenu();

Popups popups;

unsigned int startTime = 0;

bool alphaOverride = false;
char desktopAlpha;

SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

Text textOK;
Text textCancel;

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
GameState currentState = GS_BOOT;

Popups popupOrder[P_NUMBER_OF_POPUPS];

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

void drawBoot()
{
    int numBoot = 3;
    int bootTime = currentTicks-startTime-(BOOT_FLASH);
    int bootToDraw =  bootTime / (BOOT_TEXT / numBoot);
    
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
}

void drawDesktop()
{
    desktopAlpha = 0xFF;
    
    if (currentState == GS_BOOT)
    {
        if (currentTicks - startTime < BOOT_FLASH + BOOT_TEXT)
        {
            desktopAlpha = 0x00;
            
            if (currentTicks-startTime < BOOT_FLASH)
            {
                // white flash
                SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(gRenderer, &fillRect);
            }
            else
            {
                // boot text
            }
        }
        else
        {
            // desktop alpha fade in
            SDL_RenderClear(gRenderer);
            desktopAlpha = (256.0 / DESKTOP_FADE_IN) * (currentTicks - startTime - (BOOT_FLASH + BOOT_TEXT));
            
            if (currentTicks - startTime > DESKTOP_FADE_IN + BOOT_FLASH + BOOT_TEXT)
            {
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
        if (currentTicks > BOOT_FLASH + BOOT_TEXT + DESKTOP_FADE_IN + DESKTOP_ICON_IN)
        {
            currentState = GS_RUN;
        }
        else
        {
            int iconTime = currentTicks-(BOOT_FLASH + BOOT_TEXT + DESKTOP_FADE_IN);
            iconsToDraw =  iconTime / (DESKTOP_ICON_IN / numIcons);
        }
    }
    
    // todo: this needs rewriting
    for (int i = 0; i < iconsToDraw; i++)
    {
        int x = 30;
        int y = 60+(i*(iconSize+30));
        int w = iconSize;
        int h = iconSize;
        SDL_Rect outlineRect = { x, y, w, h };
        
        if (currentState == GS_RUN)
        {
            if (mouseDown1 && mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h)
            {
                iconSelected = i;
                
                switch(iconSelected)
                {
                    case 0:
                        popups.push(P_TERMINAL);
                        break;
                    case 1:
                        popups.push(P_HELP);
                        break;
                    case 2:
                        popups.push(P_MANAGE);
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
            if (currentTicks - mouseClick1 > ICON_SELECT_TIMEOUT)
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
    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x80);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    if (popupShutdown.draw(gRenderer, currentTicks))
    {
        bool newState = buttonOK.draw(gRenderer, mouseDown1, mouseX, mouseY);
        textOK.x = buttonOK.x+5;
        textOK.y = buttonOK.y+5;
        textOK.draw(gRenderer);
        if (newState == false && buttonOKClicked == true)
        {
            currentState = GS_QUIT;
        }
        buttonOKClicked = newState;
        
        newState = buttonCancel.draw(gRenderer, mouseDown1, mouseX, mouseY);
        textCancel.x = buttonCancel.x+5;
        textCancel.y = buttonCancel.y+5;
        textCancel.draw(gRenderer);
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

int contextX;
int contextY;

void drawContextMenu()
{
    int timeToShow = 500;
    int numItems = 4;
    int itemWidth = 80;
    int itemHeight = 30;
    
    SDL_Rect fillRect = { contextX, contextY, 15, itemHeight*numItems };
    SDL_SetRenderDrawColor(gRenderer, 0x0A, 0x8C, 0x61, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    int itemTime = currentTicks - mouseClick3;
    int itemsToDraw =  itemTime / (timeToShow / numItems);
    if (itemsToDraw > numItems)
    {
        itemsToDraw = numItems;
    }
    int mX, mY;
    SDL_GetMouseState(&mX, &mY);
    for (int i = 0; i < itemsToDraw; i++)
    {
        int x = contextX + 15;
        int y = contextY + (i*itemHeight);
        SDL_Rect rect = { x, y, itemWidth, itemHeight+1 };
        if (mX > x && mX < x + itemWidth && mY > y && mY < y + itemHeight)
        {
            SDL_SetRenderDrawColor(gRenderer, 0x03, 0x46, 0x25, 0xFF);
            SDL_RenderFillRect(gRenderer, &rect);
        }
        SDL_SetRenderDrawColor(gRenderer, 0x03, 0x36, 0x25, 0xFF);
        SDL_RenderDrawRect(gRenderer, &rect);
    }
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
    // todo: send key to top popup?
    switch (keycode)
    {
        case SDLK_ESCAPE:
            if (currentState == GS_RUN)
            {
                currentState = GS_SHUTDOWN;
            }
            else if (currentState == GS_SHUTDOWN)
            {
                currentState = GS_RUN;
            }
            break;
        case SDLK_F1:
            alphaOverride = !alphaOverride;
            break;
        default:
//            if (currentState == GS_TERM)
//            {
//                terminalBufferUpdate(keycode);
//            }
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

void handleMouse(unsigned int type, int button)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    //printf("%d %d\n", x, y);
    mouseX = x;
    mouseY = y;
    switch(type)
    {
        case SDL_MOUSEBUTTONDOWN:
            //printf("%d\n", button);
            if (button == 1)
            {
                mouseDown1 = true;
                mouseClick1 = currentTicks;
            }
            if (button == 3)
            {
                mouseDown3 = true;
                mouseClick3 = currentTicks;
                
//                currentState = GS_CONTEXT;
                contextX = mouseX;
                contextY = mouseY;
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

void generateText()
{
    SDL_Color black = { 0x00, 0x00, 0x00 };
    
    textOK.generate(gRenderer, gFont, black, "OK");
    
    textCancel.generate(gRenderer, gFont, black, "Cancel");
    
    textShutdown.generate(gRenderer, gFont, black, "Shutdown");
    
    textTerminal.generate(gRenderer, gFont, black, "Terminal");
    
    textHelp.generate(gRenderer, gFont, black, "Help");
    
    textManage.generate(gRenderer, gFont, black, "Manage");
}

void boot()
{
    bootBuffer.clear();
    bootLines = 0;
    startTime = currentTicks; // SDL_GetTicks();
    currentState = GS_BOOT;
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
                
                drawBoot();
                
                drawDesktop();
                
                // icons drawn on top of desktop
                if (currentState != GS_BOOT)
                {
                    drawIcons();
                }
                
//                if (currentState == GS_CONTEXT)
//                {
//                    drawContextMenu();
//                }
                
                for (int i = P_NUMBER_OF_POPUPS; i > -1; i--)
                {
                    switch (popups.order[i])
                    {
                        case P_TERMINAL:
                            drawTermPopup();
                            break;
                        case P_MANAGE:
                            drawManagePopup();
                            break;
                        case P_HELP:
                            drawHelpPopup();
                            break;
                    }
                }
                
                // shutdown drawn last, on top of everything else
                if (currentState == GS_SHUTDOWN)
                {
                    drawShutdownPopup();
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