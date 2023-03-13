#pragma once
#include "GameHandler.h"
#include "SDL.h"

class Clock;
struct windowDimensions {
	int width;
	int height;
};

class Interface
{
private:
	Clock* m_p_lastDisplayedFPS;
	windowDimensions m_windowDimensions;
	GameHandler *m_p_GameHandler;
	SDL_Window *m_p_window;
	SDL_Renderer* m_p_renderer;
	SDL_Event m_inputQueue;
	double m_pixel_per_pixel;
	Interface();
public:
	static Interface& getInstance();
	~Interface();
	void waitForInput(Uint32 ticksToWait);
	inline SDL_Window* getWindow() { return m_p_window; }
	inline SDL_Event* getInputQueue() {	return &m_inputQueue; }
	void calculatePixelPerPixel();
	void checkWindowProportions();
	void startGame();
	void displayFPS(double deltaTime, GameHandlerType appType);
	inline double getPixelPerPixel() { return m_pixel_per_pixel; }
	Interface(const Interface& obj) = delete;
};

