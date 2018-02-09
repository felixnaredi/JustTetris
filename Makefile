# 
# Filename: Makefile
# Created: 2018-02-01 20:12:03 +0100
# Author: Felix Naredi
#

CC=clang++
OPENGL_LIBS=-lGL -lGLEW -lGLU
GLFW_LIBS=-lglfw
C++17=-std=c++17

gl: glpp/gl.cc
	$(CC) -c $^ $(C++17)

Renderer_objs = \
	gl.o

Renderer: Renderer.cc glpp/Buffer.hpp
	$(CC) -c $< $(C++17)

Window_objs = \
	gl.o \
	Renderer.o

Window: Window.cc Renderer.cc
	$(CC) -c $< $(C++17)

main_objs = \
	gl.o \
	Renderer.o \
	Window.o

main: main.cc $(main_objs)
	$(CC) $^ -o $@.o -std=c++17 $(OPENGL_LIBS) $(GLFW_LIBS)

clean:
	rm *.o *~

