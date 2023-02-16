#include "Interface.h"
#include "Resources.h"
#include "GameHandler.h"
#include "Clock.h"
#include "SoundHandler.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

Interface::Interface()
{
    if (TTF_Init() < 0) {
        std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
    }
    SDL_Init(SDL_INIT_EVERYTHING);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    }

    m_p_lastDisplayedFPS = new Clock(1000);

    m_p_window = SDL_CreateWindow("Medieval Debugger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, STARTUP_SCREEN_WIDTH, STARTUP_SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    m_p_renderer = SDL_CreateRenderer(m_p_window, -1, 0);
    m_windowDimensions.width = 800;
    m_windowDimensions.height = 640;

    calculatePixelPerPixel();
}

Interface& Interface::getInstance()
{
    static Interface instance;
    return instance;
}

Interface::~Interface()
{
    SDL_DestroyWindow(m_p_window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

void Interface::waitForInput(Uint32 ticksToWait)
{
    ticksToWait += SDL_GetTicks();
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_USEREVENT, SDL_LASTEVENT);
    while (SDL_PollEvent(&m_inputQueue) >= 0)
    { 
        switch (m_inputQueue.type)
        {
        case SDL_QUIT:
            exit(1);
            break;

        case SDL_KEYDOWN:
            if (SDL_GetTicks() > ticksToWait)
                return;
        }
            
    }
}

void Interface::calculatePixelPerPixel()
{
    checkWindowProportions();
    m_pixel_per_pixel = double(m_windowDimensions.height) / 640.0; // The window should always show 640 pixels on the y-axis
    SDL_RenderSetScale(m_p_renderer, m_pixel_per_pixel, m_pixel_per_pixel);
}

void Interface::checkWindowProportions()
{
    int newWidth, newHeight;
    SDL_GetWindowSize(m_p_window, &newWidth, &newHeight);

    if (m_windowDimensions.width == newWidth && m_windowDimensions.height == newHeight)
        return;
    
    if (m_windowDimensions.height != newHeight) {
        m_windowDimensions.height = newHeight;
        m_windowDimensions.width = newHeight * 1.25;
        SDL_SetWindowSize(m_p_window, m_windowDimensions.width, m_windowDimensions.height);
        return;
    }

    if (m_windowDimensions.width != newWidth) {
        m_windowDimensions.width = newWidth;
        m_windowDimensions.height = newWidth / 1.25;
        SDL_SetWindowSize(m_p_window, m_windowDimensions.width, m_windowDimensions.height);
        return;
    }
}

void Interface::startGame()
{
    SoundHandler::getInstance();    //This will create the SoundHandler
    m_p_GameHandler = new GameHandler(m_p_renderer);
    while (true) {
        m_p_GameHandler->initWorld();
        m_p_GameHandler->resetWorld();
    }
}

void Interface::displayFPS(double deltaTime)
{
    if (m_p_lastDisplayedFPS->checkClockState()) {
        int fps = round(1000.0f / deltaTime);
        std::string windowTitle = "Medieval Debugger FPS:" + std::to_string(fps);
        SDL_SetWindowTitle(m_p_window, windowTitle.c_str());
    }
}


