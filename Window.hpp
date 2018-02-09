// 
// Filename: Window.hpp
// Created: 2018-01-30 15:56:25 +0100
// Author: Felix Naredi
//


#ifndef JUTT_WINDOW_HPP
#define JUTT_WINDOW_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.hpp"

namespace jutt
{
    class Window
    {
	typedef int KeyCode;
    
	GLFWwindow *window;
    
    public:

	static KeyCode moveDown;
	static KeyCode moveUp;
	static KeyCode moveLeft;
	static KeyCode moveRight;
	static KeyCode quitKey;

	Renderer* renderer = nullptr;
    
	Window();

	void run();
	void close();
    };  
}


#endif /* JUTT_WINDOW_HPP */
