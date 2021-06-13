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
#include "notices.h"
#include "text.h"
#include "terminal.h"

Button buttonOK;
Button buttonCancel;
Button buttonLArrow;
Button buttonRArrow;

Popup popupShutdown;
Popup popupTerminal;
Popup popupHelp;
Popup popupManage;

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

PopupType draggedPopup = P_NONE;
int mouseX = -1;
int mouseY = -1;
int mouseDragX = 0;
int mouseDragY = 0;
bool mouseDown1 = false;
unsigned int mouseClick1 = -1;
bool mouseDown3 = false;
unsigned int mouseClick3 = -1;

int numIcons = 4;
int iconSize = 60;
int iconXOffset = 30;
int iconYOffset = 60;

bool init();

bool loadMedia();

void close();

void drawBoot();

void drawDesktop();

void drawBootBuffer();

void drawIcons();

void drawShutdownPopup();

void drawShutdownFadeIn();

void drawShutdownFadeOut();

void handleKey();

void handleKeyPopup(PopupType popup, SDL_Keycode keycode);

void handleMouse(unsigned int type, int button);

void generateText();

void terminalBufferUpdate(SDL_Keycode keycode);

void terminalBufferGenerate();

void terminalBufferProcess();

void bootBufferUpdate(std::string line);

void bootBufferGenerate();

void boot();

void drawContextMenu();

PopupType popupClicked(int x, int y);

PopupType popupDragged(int x, int y);

Notices notices;

Popups popups;

unsigned int startTime = 0;

bool alphaOverride = false;
char desktopAlpha;

unsigned int shutdownFadeTicks = 0;
char shutdownFadeMaxAlpha = 128;

SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

Text textOK;
Text textCancel;

Text textLArrow;
Text textRArrow;

Text textShutdown;
Text textTerminal;
Text textHelp;
Text textManage;

Terminal terminal;
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
        int x = iconXOffset;
        int y = iconYOffset+(i*(iconSize+30));
        int w = iconSize;
        int h = iconSize;
        SDL_Rect outlineRect = { x, y, w, h };
        
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

void drawShutdownPopup()
{
    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, shutdownFadeMaxAlpha);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    if (popupShutdown.draw(gRenderer, currentTicks))
    {
        bool pressed = buttonOK.update(mouseDown1, mouseX, mouseY);
        buttonOK.draw(gRenderer);
        textOK.x = buttonOK.x+5;
        textOK.y = buttonOK.y+5;
        textOK.draw(gRenderer);
        if (pressed)
        {
            currentState = GS_QUIT;
        }
        
        pressed = buttonCancel.update(mouseDown1, mouseX, mouseY);
        buttonCancel.draw(gRenderer);
        textCancel.x = buttonCancel.x+5;
        textCancel.y = buttonCancel.y+5;
        textCancel.draw(gRenderer);
        if (pressed)
        {
            currentState = GS_SHUTDOWN_FADE_OUT;
            shutdownFadeTicks = currentTicks;
        }
    }
    
    textShutdown.x = popupShutdown.x+5;
    textShutdown.y = popupShutdown.y+5;
    textShutdown.draw(gRenderer);
}

void drawShutdownFadeIn()
{
    int fillHeight = SCREEN_HEIGHT;
    if (currentTicks - shutdownFadeTicks < SHUTDOWN_FADE_IN)
    {
        if (currentTicks - shutdownFadeTicks > 0)
        {
            fillHeight = SCREEN_HEIGHT / (SHUTDOWN_FADE_IN / (currentTicks - shutdownFadeTicks));
        }
        else
        {
            fillHeight = 0;
        }
    }
    
    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH, fillHeight };
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, shutdownFadeMaxAlpha);
    SDL_RenderFillRect(gRenderer, &fillRect);
    
    if (currentTicks - shutdownFadeTicks > SHUTDOWN_FADE_IN)
    {
        currentState = GS_SHUTDOWN;
        popupShutdown.openedTicks = currentTicks;
    }
}

void drawShutdownFadeOut()
{
    int fillHeight = SCREEN_HEIGHT;
    if (currentTicks - shutdownFadeTicks < SHUTDOWN_FADE_OUT)
    {
        if (currentTicks - shutdownFadeTicks > 0)
        {
            fillHeight = SCREEN_HEIGHT / (SHUTDOWN_FADE_OUT / (currentTicks - shutdownFadeTicks));
        }
        else
        {
            fillHeight = 0;
        }
    }
    
    SDL_Rect fillRect = { 0, fillHeight, SCREEN_WIDTH, SCREEN_HEIGHT - fillHeight };
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, shutdownFadeMaxAlpha);
    SDL_RenderFillRect(gRenderer, &fillRect);

    if (currentTicks - shutdownFadeTicks > SHUTDOWN_FADE_OUT)
    {
        currentState = GS_RUN;
    }
}

bool showContext = false;
int contextX;
int contextY;

void drawContextMenu()
{
    int timeToShow = 500;
    int numItems = 4;
    int itemWidth = 80;
    int itemHeight = 30;
    
    SDL_Rect fillRect = { contextX, contextY, 15, (itemHeight*numItems)+1 };
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
        SDL_Rect rect = { x, y, itemWidth, itemHeight + 1 };
        if (mX > x && mX < x + itemWidth && mY > y && mY < y + itemHeight)
        {
            SDL_SetRenderDrawColor(gRenderer, 0x03, 0x46, 0x25, 0xFF);
            SDL_RenderFillRect(gRenderer, &rect);
        }
        else
        {
            SDL_SetRenderDrawColor(gRenderer, 0x03, 0xFC, 0xA9, 0xFF);
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
        SDL_Rect fillRect = { popupHelp.x+5, popupHelp.y+30+5, popupHelp.w-(5*2), popupHelp.h-(buttonLArrow.h+10)-30-(5*2) };
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(gRenderer, &fillRect);
        
        buttonLArrow.x = popupHelp.x + 10;
        buttonLArrow.y = popupHelp.y + popupHelp.h - (10 + buttonLArrow.h);
        
        buttonRArrow.x = popupHelp.x + popupHelp.w - (10 + buttonRArrow.w);
        buttonRArrow.y = popupHelp.y + popupHelp.h - (10 + buttonRArrow.h);
        
        bool pressed = buttonLArrow.update(mouseDown1, mouseX, mouseY);
        buttonLArrow.draw(gRenderer);
        textLArrow.x = buttonLArrow.x+5;
        textLArrow.y = buttonLArrow.y+5;
        textLArrow.draw(gRenderer);
        if (pressed)
        {
            //
        }
        
        pressed = buttonRArrow.update(mouseDown1, mouseX, mouseY);
        buttonRArrow.draw(gRenderer);
        textRArrow.x = buttonRArrow.x+5;
        textRArrow.y = buttonRArrow.y+5;
        textRArrow.draw(gRenderer);
        if (pressed)
        {
            //
        }
    }
    
    textHelp.x = popupHelp.x+5;
    textHelp.y = popupHelp.y+5;
    textHelp.draw(gRenderer);
}

void handleKey(SDL_Keycode keycode)
{
    PopupType p = popups.peek();
    
    switch (keycode)
    {
        case SDLK_F1:
            alphaOverride = !alphaOverride;
            break;
        case SDLK_ESCAPE:
            if (showContext)
            {
                showContext = false;
            }
            else
            {
                if (p == P_NONE)
                {
                    if (currentState == GS_RUN)
                    {
                        currentState = GS_SHUTDOWN_FADE_IN;
                        shutdownFadeTicks = currentTicks;
                    }
                    else if (currentState == GS_SHUTDOWN)
                    {
                        currentState = GS_SHUTDOWN_FADE_OUT;
                        shutdownFadeTicks = currentTicks;
                    }
                }
                else
                {
                    popups.remove(p);
                }
            } 
            break;
        default:
            if (p != P_NONE)
            {
               handleKeyPopup(p, keycode);
            }
            break;
    }
}

void handleKeyPopup(PopupType popup, SDL_Keycode keycode)
{
    switch (popup)
    {
        case P_TERMINAL:
            terminalBufferUpdate(keycode);
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
        // just assume terminal is active
        popups.remove(P_TERMINAL);
    }
    else if (command.compare("help") == 0)
    {
        termBuffer.append(" quit\r");
        termLines+=1;
    }
    else if (command.compare("notice") == 0)
    {
        termBuffer.append("ok\r");
        termLines+=1;
        notices.add();
    }
    else if (command.compare("reboot") == 0)
    {
        popups.clear();
        boot();
    }
    else
    {
        termBuffer.append("error\r");
        termLines+=1;
    }
}

PopupType popupClicked(int x, int y)
{
    for (int i = 0; i < P_NUMBER_OF_POPUPS; i++)
    {
        switch (popups.order[i])
        {
            case P_HELP:
                if (popupHelp.isClicked(x, y))
                {
                    return P_HELP;
                }
                break;
            case P_MANAGE:
                if (popupManage.isClicked(x, y))
                {
                    return P_MANAGE;
                }
                break;
            case P_TERMINAL:
                if (popupTerminal.isClicked(x, y))
                {
                    return P_TERMINAL;
                }
                break;
        }
    }
    
    return P_NONE;
}

PopupType popupDragged(int x, int y)
{
    for (int i = 0; i < P_NUMBER_OF_POPUPS; i++)
    {
        switch (popups.order[i])
        {
            case P_HELP:
                if (popupHelp.isDragged(x, y))
                {
                    return P_HELP;
                }
                break;
            case P_MANAGE:
                if (popupManage.isDragged(x, y))
                {
                    return P_MANAGE;
                }
                break;
            case P_TERMINAL:
                if (popupTerminal.isDragged(x, y))
                {
                    return P_TERMINAL;
                }
                break;
        }
    }
    
    return P_NONE;
}

void popupDrag(PopupType popup, int x, int y)
{
    //printf("%d,%d\n", x, y);
    switch (popup)
    {
        case P_HELP:
            popupHelp.drag(x, y);
            popupHelp.openedTicks = currentTicks;
            break;
        case P_MANAGE:
            popupManage.drag(x, y);
            popupManage.openedTicks = currentTicks;
            break;
        case P_TERMINAL:
            popupTerminal.drag(x, y);
            popupTerminal.openedTicks = currentTicks;
            break;
    }
}

void popupFakeDrag(PopupType popup, int x, int y)
{
    switch (popup)
    {
        case P_HELP:
            popupHelp.fakeDrag(x, y);
            break;
        case P_MANAGE:
            popupManage.fakeDrag(x, y);
            break;
        case P_TERMINAL:
            popupTerminal.fakeDrag(x, y);
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
                
                // TODO: just always hide it for now, fix this later
                showContext = false;
                
                //TODO: check for notice clicked, if so don't check for popup clicked/dragged
                PopupType selectedPopup = popupClicked(mouseX, mouseY);
                if (selectedPopup != P_NONE)
                {
                    // popup clicked
                    draggedPopup = popupDragged(mouseX, mouseY);
                    //printf("%d\n", selectedPopup);
                    if (selectedPopup != popups.peek())
                    {
                        popups.select(selectedPopup);
                    }
                }
                else
                {
                    // no popup clicked
                    for (int i = 0; i < numIcons; i++)
                    {
                        int iconX = iconXOffset;
                        int iconY = iconYOffset+(i*(iconSize+30));
                        int iconW = iconSize;
                        int iconH = iconSize;
                        if (mouseDown1 && mouseX > iconX && mouseX < iconX + iconW && mouseY > iconY && mouseY < iconY + iconH)
                        {
                            iconSelected = i;
                            break;
                        }
                    }
                    if (iconSelected != -1)
                    {
                        // icon clicked
                        switch(iconSelected)
                        {
                            case 0:
                                if (!popups.exists(P_TERMINAL))
                                {
                                    popups.push(P_TERMINAL);
                                    popupTerminal.openedTicks = currentTicks;
                                    
                                    termBuffer.clear();
                                    termLines = 0;
                                    termBuffer.push_back(termPrompt);
                                    terminalBufferGenerate();
                                }
                                break;
                            case 1:
                                if (!popups.exists(P_HELP))
                                {
                                    popups.push(P_HELP);
                                    popupHelp.openedTicks = currentTicks;
                                }
                                break;
                            case 2:
                                if (!popups.exists(P_MANAGE))
                                {
                                    popups.push(P_MANAGE);
                                    popupManage.openedTicks = currentTicks;
                                }
                                break;
                            case 3:
                                currentState = GS_SHUTDOWN_FADE_IN;
                                shutdownFadeTicks = currentTicks;
                                break;
                        }
                    }
                    else
                    {
                        // no icon clicked
                    }
                }
            }
            if (button == 3)
            {
                mouseDown3 = true;
                mouseClick3 = currentTicks;
                
                //TODO: ensure we haven't clicked an icon or popup infront
                showContext = true;
                contextX = mouseX;
                contextY = mouseY;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (button == 1)
            {
                mouseDown1 = false;
                
                //printf("%d,%d\n", mouseDragX, mouseDragY);
                if (mouseDragX != 0 || mouseDragY != 0)
                {
                    //printf("%d,%d\n", mouseX, mouseDragX);
                    popupDrag(draggedPopup, mouseX - mouseDragX, mouseY - mouseDragY);
                    mouseDragX = 0;
                    mouseDragY = 0;
                }
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
    
    textLArrow.generate(gRenderer, gFont, black, "<");
    
    textRArrow.generate(gRenderer, gFont, black, ">");
    
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
    
    buttonLArrow.w = 30;
    buttonLArrow.h = 30;
    
    buttonRArrow.w = 30;
    buttonRArrow.h = 30;

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
                
                // do this code here for now
                if (currentState == GS_RUN)
                {
                    if (mouseDown1)
                    {
                        int mX, mY;
                        SDL_GetMouseState(&mX, &mY);
                        if (mX != mouseX || mY != mouseY)
                        {
                            //printf("%d,%d\n", mX, mY);
                            mouseDragX = mX;
                            mouseDragY = mY;
                            popupFakeDrag(draggedPopup, mouseX - mouseDragX, mouseY - mouseDragY);
                        }
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
                
                for (int i = P_NUMBER_OF_POPUPS; i > -1; i--)
                {
                    switch (popups.order[i])
                    {
                        case P_TERMINAL:
                            terminal.draw(gRenderer, popupTerminal, textTerminal, textTerminalBuffer, currentTicks);
                            break;
                        case P_MANAGE:
                            drawManagePopup();
                            break;
                        case P_HELP:
                            drawHelpPopup();
                            break;
                    }
                }
                
                notices.draw(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
                
                if (showContext)
                {
                    drawContextMenu();
                }
                
                // shutdown drawn last, on top of everything else
                if (currentState == GS_SHUTDOWN_FADE_IN)
                {
                    drawShutdownFadeIn();
                }
                if (currentState == GS_SHUTDOWN_FADE_OUT)
                {
                    drawShutdownFadeOut();
                }
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