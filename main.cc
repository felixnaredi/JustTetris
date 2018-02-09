// 
// Filename: main.cc
// Created: 2018-01-30 16:16:48 +0100
// Author: Felix Naredi
// 

#include <iostream>

#include "Renderer.hpp"
#include "Window.hpp"

int main(int argc, char *argv[])
{   
   jutt::Window window;
   jutt::Renderer renderer;

   window.renderer = &renderer;
   std::cout << "renderer set to window\n";

   window.run();
    
   return 0;
}










