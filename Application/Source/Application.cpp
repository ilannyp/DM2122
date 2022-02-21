#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>
#include "Scene_LV2.h" //-for test
#include "SP_Start.h"
#include "SP.h"
#include "Scene_EndScreen.h"
#include "SP_3.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame
unsigned Application::m_width;
unsigned Application::m_height;
Player Application::yourself;



void resize_callback(GLFWwindow* window, int w, int h)
{
	Application::m_width = w;
	Application::m_height = h;
	glViewport(0, 0, w, h);
}
bool Application::IsMousePressed(unsigned short key) //0 - Left, 1 - Right, 2 - Middle
{
	return glfwGetMouseButton(m_window, key) != 0;
}
void Application::GetCursorPos(double* xpos, double* ypos)
{
	glfwGetCursorPos(m_window, xpos, ypos);
}
int Application::GetWindowWidth()
{
	return m_width;
}
int Application::GetWindowHeight()
{
	return m_height;
}

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

bool Application::IsKeyPressed(unsigned short key)
{
	return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}



void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	m_width = 800;
	m_height = 600;
	m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	glfwSetWindowSizeCallback(m_window, resize_callback);


	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized	
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}
}

void Application::Run()
{
	//Main Loop
	SP cab;
	Scene* scene1 = new SP_Start();
	Scene* scene2 = new SP();
	Scene* scene3 = new Scene_EndScreen();
	Scene* scene4 = new Scene_LV2();
	Scene* scene5 = new SP_3();


	Scene* scene = scene1;
	scene1->Init();
	std::cout << cab.cabTP() << std::endl;

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !IsKeyPressed(VK_ESCAPE))
	{
		if (IsKeyPressed(VK_RETURN))
		{
			yourself.set_currency(1);
			yourself.set_alive();
			yourself.set_in_cab(false);
			yourself.set_first_scammed(false);
			yourself.set_coin1_enabled(false);
			yourself.set_coin2_enabled(false);
			yourself.set_coin3_enabled(false);
			yourself.set_coin1_obtained(false);
			yourself.set_coin2_obtained(false);
			yourself.set_coin3_obtained(false);
			yourself.set_currency_added_from_C1(false);
			yourself.set_currency_added_from_C2(false);
			yourself.set_currency_added_from_C3(false);
			scene = scene2;
			scene->Init();
		}
		if (yourself.get_coin1_enabled())
		{

			//std::cout << "asd" << std::endl;
		}
		//std::cout << "asd" << std::endl;
		if (IsKeyPressed(VK_F3))
		{
			scene = scene3;
			scene->Init();
			die_screen_once = true;
		}
		if (yourself.get_in_cab() == true)
		{
			scene = scene4;
			scene->Init();
			yourself.set_in_cab(false);
		}

		if (yourself.get_life_status() == false)
		{
			scene = scene3;
			scene->Init();
			die_screen_once = true;
		}
		if (cab.cabTP() <= 10)
		{
			std::cout << "inside \n";
			if (IsKeyPressed('E'))
			{
				std::cout << "to level 2 \n";
				scene = scene4;
				scene->Init();
			}
		}
		if (IsKeyPressed(VK_F4))
		{
			scene = scene5;
			scene->Init();
		}


















		//debug
		if (IsKeyPressed(VK_F9))


		{
			scene = scene4;
			scene->Init();
		}





		scene->Update(m_timer.getElapsedTime());
		scene->Render();
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
		m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.



	} //Check if the ESC key had been pressed or if the window had been closed
	scene1->Exit();
	scene2->Exit();
	scene3->Exit();
	scene4->Exit();
	scene5->Exit();
	delete scene1;
	delete scene2;
	delete scene3;
	delete scene4;
	delete scene5;

}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
