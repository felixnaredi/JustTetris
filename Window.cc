// 
// Filename: Window.cc
// Created: 2018-01-30 16:02:00 +0100
// Author: Felix Naredi
//

#include <iostream>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glpp/gl.hpp"
#include "Window.hpp"

namespace jutt
{   
   Window::KeyCode Window::moveUp = GLFW_KEY_W;
   Window::KeyCode Window::moveDown = GLFW_KEY_S;
   Window::KeyCode Window::moveLeft = GLFW_KEY_A;
   Window::KeyCode Window::moveRight = GLFW_KEY_D;
   Window::KeyCode Window::quitKey = GLFW_KEY_Q;

   enum KeyAction
   {
      down = 0,
      up = 1
   };

   void close(GLFWwindow *window)
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }

   void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
   {
      std::cout << "Key input: {"
		<< "key: " << key
		<< ", scancode: " << scancode
		<< ", action: " << action
		<< ", mods: " << mods
		<< "}" << std::endl;	

      if(key == Window::quitKey && mods == GLFW_MOD_CONTROL)
      {
	 close(window);
	 return;
      }

      switch(action)
      {
	 case KeyAction::down:
	    
	    if(key == Window::moveUp)
	    {
	       std::cout << "move up\n";		
	       return;
	    }
	    
	    if(key == Window::moveDown)
	    {
	       std::cout << "moveDown\n";
	       return;
	    }
	    
	    if(key == Window::moveLeft)
	    {
	       std::cout << "move left\n";
	       return;
	    }
	    
	    if(key == Window::moveRight)
	    {
	       std::cout << "move right\n";
	       return;
	    }
	    
	 default:
	    break;
      }
   }

   Window::Window()
   {
      std::cout << "Window::Window()\n";

      if(!glfwInit())
      {
	 throw std::runtime_error("Failed to init GLFW");
      }

      window = glfwCreateWindow(500, 500, "Tetris", NULL, NULL);
    
      glfwMakeContextCurrent(window);
      glfwSetKeyCallback(window, keyCallback);

      if(glewInit())
      {
	 throw std::runtime_error("Failed to init GLEW");
      }

      std::cout << "GLEW init\n";
	
      glClearColor(0.35, 0.50, 0.65, 1.0);
   }

   void Window::run()
   {
      while(!glfwWindowShouldClose(window))
      {
	 renderer->draw();
	 glfwWaitEvents();
	 glfwSwapBuffers(window);
      }
   }

   void Window::close()
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }  
  
}



















