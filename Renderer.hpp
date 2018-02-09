// 
// Filename: Renderer.hpp
// Created: 2018-01-31 02:39:18 +0100
// Author: Felix Naredi
// 

#ifndef JUTT_RENDERER_HPP
#define JUTT_RENDERER_HPP

#include <vector>
#include <iostream>

#include <glm/vec3.hpp>

#include "glpp/gl.hpp"
#include "glpp/Buffer.hpp"

namespace jutt
{   
    struct Vertex
    {
	glm::vec3 position;
	glm::vec3 color;

	Vertex(glm::vec3 position, glm::vec3 color);
	
	template <typename T>
	Vertex(T x, T y, T z, T r, T g, T b);
    };

    template <typename CharT>
    std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> &stream, const glm::vec3 vec);

    template <typename CharT>
    std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> &stream, const Vertex vertex);
    
    class Renderer
    {
	typedef gl::Buffer<gl::buffer::Type::array, gl::buffer::Usage::staticDraw
			  > VertexBuffer;

	typedef gl::Buffer<gl::buffer::Type::elementArray, gl::buffer::Usage::staticDraw
			  > IndexBuffer;
	
	const static std::vector<Vertex> verticies;
	const static std::vector<gl::UInt> indicies;

	gl::program::Id program;
	std::vector<gl::shader::Id> shaders;      

	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;

	gl::vertex_array::Id vertexArray;

	template <gl::shader::Type Type>
	void attachShader(const char* path);
	
	void linkProgram();
	
    public:      

	Renderer();
	void draw();
    };
}

#endif /* JUTT_RENDERER_HPP */











