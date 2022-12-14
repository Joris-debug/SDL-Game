#include "Interface.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "Resources.h"
#include "GameHandler.h"

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

    m_p_window_ = SDL_CreateWindow("Part Time Knight", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, STARTUP_SCREEN_WIDTH, STARTUP_SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    m_p_renderer_ = SDL_CreateRenderer(m_p_window_, -1, 0);
    //SDL_SetWindowFullscreen(m_p_window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
    getPixelPerPixel();
}

Interface& Interface::getInstance()
{
    static Interface instance;
    return instance;
}

Interface::~Interface()
{
    SDL_DestroyWindow(m_p_window_);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

void Interface::waitForInput(Uint32 ticksToWait)
{
    ticksToWait += SDL_GetTicks();
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_USEREVENT, SDL_LASTEVENT);
    while (SDL_PollEvent(&m_inputQueue_) >= 0)
    { 
        switch (m_inputQueue_.type)
        {
        case SDL_QUIT:
            exit(1);
            break;

        case SDL_KEYDOWN:
        case SDL_MOUSEBUTTONDOWN:
            if (SDL_GetTicks() > ticksToWait)
                return;
        }
            
    }
}

void Interface::getPixelPerPixel()
{
    int width;
    int height;
    SDL_GetWindowSize(m_p_window_, &width, &height);
    m_pixel_per_pixel_ = double(height) / 640.0; // The window should always show 640 pixels on the y-axis
    SDL_RenderSetScale(m_p_renderer_, m_pixel_per_pixel_, m_pixel_per_pixel_);
}

void Interface::startGame()
{
    m_p_GameHandler_ = new GameHandler(this, m_p_renderer_);
    m_p_GameHandler_->initLevel1();
}


