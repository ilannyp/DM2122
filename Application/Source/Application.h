#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
#include "Player.h"
class Application
{
public:
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double* xpos, double* ypos);
	static int GetWindowWidth();
	static int GetWindowHeight();
	static unsigned m_width;
	static unsigned m_height;
	Application();
	~Application();
	void variables();
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static Player yourself;
	bool die_screen_once = false;

private:

	//Declare a window object
	StopWatch m_timer;
};

#endif