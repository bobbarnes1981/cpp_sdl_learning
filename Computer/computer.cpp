#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <cmath>

#include "computer.h"
#include "context.h"
#include "entity.h"
#include "button.h"
#include "popups.h"
#include "popup.h"
#include "icon.h"
#include "icons.h"
#include "notice.h"
#include "notices.h"
#include "text.h"
#include "terminal.h"
#include "timedevent.h"

Context context;

Button buttonOK;
Button buttonCancel;
Button buttonLArrow;
Button buttonRArrow;

Popup popupShutdown;

Mix_Chunk* soundStartup = NULL;
Mix_Chunk* soundShutdown = NULL;
Mix_Chunk* soundNotice = NULL;
Mix_Chunk* soundError = NULL;
Mix_Chunk* soundPopupRemove = NULL;
Mix_Chunk* soundDesktop = NULL;

TimedEvent eventServerError;

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

PopupType draggedPopup = P_NONE;
int mouseX = -1;
int mouseY = -1;
int mouseDragX = 0;
int mouseDragY = 0;
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

void addNotice(std::string text, NoticeType type);

Notices notices;
Icons icons;
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

    soundStartup = Mix_LoadWAV("sound/startup.wav" );
    if (soundStartup == NULL )
    {
        printf("Failed to load startup sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    soundShutdown = Mix_LoadWAV("sound/shutdown.wav" );
    if (soundShutdown == NULL )
    {
        printf("Failed to load shutdown sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    soundNotice = Mix_LoadWAV("sound/notice.wav" );
    if (soundNotice == NULL )
    {
        printf("Failed to load notice sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    soundError = Mix_LoadWAV("sound/error.wav" );
    if (soundError == NULL )
    {
        printf("Failed to load error sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }
    
    soundPopupRemove = Mix_LoadWAV("sound/popup_remove.wav" );
    if (soundPopupRemove == NULL )
    {
        printf("Failed to load popup_remove sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }
    
    soundDesktop = Mix_LoadWAV("sound/desktop.wav" );
    if (soundDesktop == NULL )
    {
        printf("Failed to load desktop sound effect! SDL_mixer Error: %s\n", Mix_GetError());
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
    
    Mix_FreeChunk(soundStartup);
    soundStartup = NULL;
    
    Mix_FreeChunk(soundShutdown);
    soundShutdown = NULL;
    
    Mix_FreeChunk(soundNotice);
    soundShutdown = NULL;
    
    Mix_FreeChunk(soundError);
    soundError = NULL;
    
    Mix_FreeChunk(soundPopupRemove);
    soundPopupRemove = NULL;
    
    Mix_FreeChunk(soundDesktop);
    soundDesktop = NULL;
    
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

bool startupPlayed = false;

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
                if (startupPlayed == false)
                {
                    Mix_PlayChannel(-1, soundStartup, 0);
                    startupPlayed = true;
                }
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

void drawBrowserPopup()
{
    if (popups.popups[P_BROWSER].draw(gRenderer, currentTicks))
    {
        
    }
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

void drawBootBuffer()
{
    textBootBuffer.x = 10;
    textBootBuffer.y = 10;
    textBootBuffer.draw(gRenderer);
}

void drawManagePopup()
{
    if (popups.popups[P_MANAGE].draw(gRenderer, currentTicks))
    {
        
    }
}

void drawHelpPopup()
{
    if (popups.popups[P_HELP].draw(gRenderer, currentTicks))
    {
        SDL_Rect fillRect = { popups.popups[P_HELP].x+5, popups.popups[P_HELP].y+30+5, popups.popups[P_HELP].w-(5*2), popups.popups[P_HELP].h-(buttonLArrow.h+10)-30-(5*2) };
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(gRenderer, &fillRect);
        
        buttonLArrow.x = popups.popups[P_HELP].x + 10;
        buttonLArrow.y = popups.popups[P_HELP].y + popups.popups[P_HELP].h - (10 + buttonLArrow.h);
        
        buttonRArrow.x = popups.popups[P_HELP].x + popups.popups[P_HELP].w - (10 + buttonRArrow.w);
        buttonRArrow.y = popups.popups[P_HELP].y + popups.popups[P_HELP].h - (10 + buttonRArrow.h);
        
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
            if (context.show)
            {
                context.show = false;
            }
            else
            {
                if (currentState == GS_RUN)
                {
                    if (p == P_NONE)
                    {
                        currentState = GS_SHUTDOWN_FADE_IN;
                        shutdownFadeTicks = currentTicks;
                    }
                    else
                    {
                        Mix_PlayChannel(-1, soundPopupRemove, 0);
                        popups.remove(p);
                    }
                }
                else if (currentState == GS_SHUTDOWN)
                {
                    currentState = GS_SHUTDOWN_FADE_OUT;
                    shutdownFadeTicks = currentTicks;
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
    textTerminalBuffer.generateWrapped(gRenderer, gFont, green, termBuffer.c_str(), popups.popups[P_TERMINAL].w-(5*2));
}

enum ServerErrors
{
    SE_NONE = 0,
    SE_DB_MISSING = 1,
    SE_DB_FAULT = 2,
    SE_DB_OVERLOAD = 4,
    SE_WEB_MISSING = 8,
    SE_WEB_FAULT = 16,
    SE_WEB_OVERLOAD = 32,
    SE_DNS_MISSING = 64,
    SE_DNS_FAULT = 128,
    SE_DNS_OVERLOAD = 256
};
int serverError = SE_NONE;

void terminalBufferProcess()
{
    std::size_t loc = termBuffer.rfind('\r', termBuffer.length()-2);
    std::string command = termBuffer.substr(loc+2, termBuffer.length()-(loc+2+1));
    if (command.compare("quit") == 0)
    {
        // just assume terminal is active
        Mix_PlayChannel(-1, soundPopupRemove, 0);
        popups.remove(P_TERMINAL);
    }
    else if (command.compare("help") == 0)
    {
        termBuffer.append(" server\r");
        termBuffer.append(" db\r");
        termBuffer.append(" web\r");
        termBuffer.append(" dns\r");
        termBuffer.append(" quit\r");
        termLines+=5;
    }
    else if (command.compare("notice") == 0)
    {
        termBuffer.append("ok\r");
        termLines+=1;
        addNotice("testing", NT_INFO);
    }
    else if (command.compare("reboot") == 0)
    {
        popups.clear();
        boot();
    }
    else if (command.compare("server") == 0)
    {
        termBuffer.append(" check\r");
        termBuffer.append(" reset\r");
        termBuffer.append(" stop\r");
        termBuffer.append(" start\r");
        termLines+=4;
    }
    else if (command.compare("server check") == 0)
    {
        termBuffer.append(std::to_string(serverError));
        termBuffer.append("\r");
        termLines+=1;
    }
    else if (command.compare("db") == 0)
    {
        termBuffer.append(" check\r");
        termBuffer.append(" reset\r");
        termBuffer.append(" stop\r");
        termBuffer.append(" start\r");
        termBuffer.append(" restore\r");
        termLines+=5;
    }
    else if (command.compare("db reset") == 0)
    {
        termBuffer.append("ok\r");
        if (serverError == SE_DB_FAULT)
        {
            serverError = SE_NONE;
        }
        termLines+=1;
    }
    else
    {
        termBuffer.append("error\r");
        termLines+=1;
        Mix_PlayChannel(-1, soundError, 0);
    }
}

void addNotice(std::string text, NoticeType type)
{
    Mix_PlayChannel(-1, soundNotice, 0);
    notices.add(gRenderer, gFont, text, type);
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
                
                if (currentState == GS_RUN)
                {
                    int selectedContext = context.clicked(mouseX, mouseY);
                    context.show = false;
                    if (selectedContext != -1)
                    {
                        // context clicked
                        printf("%d\n", selectedContext);
                    }
                    else
                    {
                        // context not clicked
                        int selectedNotice =  notices.clicked(mouseX, mouseY);
                        //printf("%d\n", selectedNotice);
                        if (selectedNotice != -1)
                        {
                            // notice clicked
                            notices.remove(gRenderer, gFont, selectedNotice);
                        }
                        else
                        {
                            // notice not clicked
                            PopupType selectedPopup = popups.clicked(mouseX, mouseY);
                            if (selectedPopup != P_NONE)
                            {
                                // popup clicked
                                draggedPopup = popups.dragged(mouseX, mouseY);
                                //printf("%d\n", selectedPopup);
                                if (selectedPopup != popups.peek())
                                {
                                    popups.select(selectedPopup);
                                }
                            }
                            else
                            {
                                // no popup clicked
                                for (int i = 0; i < I_NUMBER_OF_ICONS; i++)
                                {
                                    int iconX = icons.xOffset;
                                    int iconY = icons.yOffset+(i*(ICON_SIZE+30));
                                    int iconW = ICON_SIZE;
                                    int iconH = ICON_SIZE;
                                    if (mouseDown1 && mouseX > iconX && mouseX < iconX + iconW && mouseY > iconY && mouseY < iconY + iconH)
                                    {
                                        icons.selected = i;
                                        break;
                                    }
                                }
                                if (icons.selected != -1)
                                {
                                    // icon clicked
                                    switch(icons.selected)
                                    {
                                        case 0:
                                            if (!popups.exists(P_TERMINAL))
                                            {
                                                popups.push(P_TERMINAL);
                                                popups.popups[P_TERMINAL].openedTicks = currentTicks;
                                                
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
                                                popups.popups[P_HELP].openedTicks = currentTicks;
                                            }
                                            break;
                                        case 2:
                                            if (!popups.exists(P_MANAGE))
                                            {
                                                popups.push(P_MANAGE);
                                                popups.popups[P_MANAGE].openedTicks = currentTicks;
                                            }
                                            break;
                                        case 3:
                                            if (!popups.exists(P_BROWSER))
                                            {
                                                popups.push(P_BROWSER);
                                                popups.popups[P_BROWSER].openedTicks = currentTicks;
                                            }
                                            break;
                                        case 4:
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
                    }
                }
            }
            if (button == 3)
            {
                mouseDown3 = true;
                mouseClick3 = currentTicks;
                
                if (currentState == GS_RUN)
                {
                    //TODO: ensure we haven't clicked an icon or popup infront of desktop
                    context.show = true;
                    context.x = mouseX;
                    context.y = mouseY;
                }
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
                    popups.drag(draggedPopup, mouseX - mouseDragX, mouseY - mouseDragY, currentTicks);
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
    
    popups.popups[P_BROWSER].generate(gRenderer, gFont, "Browser");
    
    popups.popups[P_TERMINAL].generate(gRenderer, gFont, "Terminal");
    
    popups.popups[P_HELP].generate(gRenderer, gFont, "Help");
    
    popups.popups[P_MANAGE].generate(gRenderer, gFont, "Manage");
}

void boot()
{
    startupPlayed = false;
    bootBuffer.clear();
    bootLines = 0;
    startTime = currentTicks; // SDL_GetTicks();
    currentState = GS_BOOT;
}

int main(int argc, char* args[])
{
    popups.popups[P_TERMINAL].w = 500;
    popups.popups[P_TERMINAL].h = 300;
    popups.popups[P_TERMINAL].x = (SCREEN_WIDTH/2)-popups.popups[P_TERMINAL].w/2;
    popups.popups[P_TERMINAL].y = (SCREEN_HEIGHT/2)-popups.popups[P_TERMINAL].h/2;
    popups.popups[P_TERMINAL].openedTicks = 0;
    
    popups.popups[P_HELP].w = 250;
    popups.popups[P_HELP].h = 400;
    popups.popups[P_HELP].x = SCREEN_WIDTH-(popups.popups[P_HELP].w+30);
    popups.popups[P_HELP].y = 60;
    popups.popups[P_HELP].openedTicks = 0;
    
    popups.popups[P_MANAGE].w = 500;
    popups.popups[P_MANAGE].h = 400;
    popups.popups[P_MANAGE].x = (SCREEN_WIDTH/2)-popups.popups[P_MANAGE].w/2;
    popups.popups[P_MANAGE].y = (SCREEN_HEIGHT/2)-popups.popups[P_MANAGE].h/2;
    popups.popups[P_MANAGE].openedTicks = 0;
    
    popups.popups[P_BROWSER].w = 600;
    popups.popups[P_BROWSER].h = 300;
    popups.popups[P_BROWSER].x = (SCREEN_WIDTH/2)-popups.popups[P_BROWSER].w/2;
    popups.popups[P_BROWSER].y = (SCREEN_HEIGHT/2)-popups.popups[P_BROWSER].h/2;
    popups.popups[P_BROWSER].openedTicks = 0;
    
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
    
    eventServerError.start = currentTicks;
    eventServerError.delay = 5000;

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
            icons.icons[I_TERMINAL].generate(gRenderer, gFont, "[Terminal]");
            icons.icons[I_HELP].generate(gRenderer, gFont, "[Help]");
            icons.icons[I_MANAGE].generate(gRenderer, gFont, "[Manage]");
            icons.icons[I_BROWSER].generate(gRenderer, gFont, "[Browser]");
            icons.icons[I_SHUTDOWN].generate(gRenderer, gFont, "[Shutdown]");
            
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
                
                // timed events
                if (!eventServerError.triggered && eventServerError.delay > currentTicks - eventServerError.delay)
                {
                    eventServerError.triggered = true;
                    serverError = SE_DB_FAULT; // DATABASE FAULT
                    addNotice("server error reported", NT_CRITICAL);
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
                            popups.fakeDrag(draggedPopup, mouseX - mouseDragX, mouseY - mouseDragY);
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
                    unsigned int iconsToDraw = I_NUMBER_OF_ICONS;
                    if (currentState == GS_DESKTOP)
                    {
                        if (currentTicks > BOOT_FLASH + BOOT_TEXT + DESKTOP_FADE_IN + DESKTOP_ICON_IN)
                        {
                            Mix_PlayChannel(-1, soundDesktop, 0);
                            currentState = GS_RUN;
                        }
                        else
                        {
                            int iconTime = currentTicks-(BOOT_FLASH + BOOT_TEXT + DESKTOP_FADE_IN);
                            iconsToDraw =  iconTime / (DESKTOP_ICON_IN / I_NUMBER_OF_ICONS);
                        }
                    }
                    
                    icons.draw(gRenderer, iconsToDraw, desktopAlpha, currentTicks - mouseClick1);
                }
                
                // this is a hack
                if (desktopAlpha != 0x00 )
                {
                    for (int i = P_NUMBER_OF_POPUPS; i > -1; i--)
                    {
                        switch (popups.order[i])
                        {
                            case P_TERMINAL:
                                terminal.draw(gRenderer, popups.popups[P_TERMINAL], textTerminalBuffer, currentTicks);
                                break;
                            case P_MANAGE:
                                drawManagePopup();
                                break;
                            case P_HELP:
                                drawHelpPopup();
                                break;
                            case P_BROWSER:
                                drawBrowserPopup();
                                break;
                        }
                    }
                    
                    if (currentState != GS_BOOT)
                    {
                        notices.x = SCREEN_WIDTH;
                        notices.y = SCREEN_HEIGHT;
                        notices.draw(gRenderer);
                    }
                    
                    if (context.show)
                    {
                        context.draw(gRenderer, currentTicks - mouseClick3);
                    }
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
    
    Mix_PlayChannel(-1, soundShutdown, 0);
    while (Mix_Playing(-1) != 0)
    {
        SDL_Delay(200);
    }

    close();
    
    return 0;
}