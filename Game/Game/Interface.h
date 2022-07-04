#pragma once
#include "SDL.h"
class GameHandler;

class Interface
{
private:
	GameHandler *m_p_GameHandler_;
	SDL_Window *m_p_window_;
	SDL_Renderer* m_p_renderer_;
	SDL_Event m_inputQueue_;
	double m_pixel_per_pixel_;
public:
	Interface();
	~Interface();
	void waitForInput(Uint32 ticksToWait);
	inline SDL_Window* getWindow() { return m_p_window_; }
	inline SDL_Event* getInputQueue() {	return &m_inputQueue_; }
	void getPixelPerPixel();
};

