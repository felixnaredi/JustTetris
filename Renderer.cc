// 
// Filename: Renderer.cc
// Created: 2018-01-31 13:23:19 +0100
// Author: Felix Naredi
// 

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include "Renderer.hpp"

#include "glpp/gl.hpp"
#include "glpp/Buffer.hpp"
#include "glpp/Shader.hpp"

namespace jutt
{    
   Vertex::Vertex(glm::vec3 p, glm::vec3 c)
   {
      position = p;
      color = c;	
   }

   template <typename T>
   Vertex::Vertex(T x, T y, T z, T r, T g, T b)
   {
      position = glm::vec3(x, y, z);
      color = glm::vec3(r, g, b);
   }

    

   template <typename CharT>
   std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> &stream, const glm::vec3 vec)
   {
      return stream << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
   };

   template <typename CharT>
   std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> &stream, const Vertex vertex)
   {
      return stream << "Vertex: {position: " << vertex.position << ", color: " << vertex.color << "}";
   }

   const std::vector<Vertex> Renderer::verticies =
   {
      // position           color
      {{ 0.0,  0.5, 0.0},  {1.0, 0.0, 0.0}},
      {{-0.5, -0.5, 0.0},  {0.0, 1.0, 0.0}},
      {{ 0.5, -0.5, 0.0},  {0.0, 0.0, 1.0}},
   };

   const std::vector<gl::UInt> Renderer::indicies =
   {
      0, 1, 2
   };    

   template <gl::shader::Type type>
   void Renderer::attachShader(const char* path)
   {
      std::ifstream file(path);
      gl::Shader<type> shader(file);

      file.close();

      gl::attach(program, shader);
      shaders.push_back(shader);
   }

   void Renderer::linkProgram()
   {      
      gl::link(program);

      while(!shaders.empty())
      {
	 auto shader = std::move(shaders.back());
	 shaders.pop_back();

	 gl::free(shader);
      }
   }

   void setPointer(gl::program::Id program, const gl::Char* name, gl::Size size, gl::Offset offset)
   {
      auto location = gl::vertex_attribute::Location {program, name};

      gl::setPointer(location, size, gl::DataType::Float, false, sizeof(Vertex), offset);
      gl::enable(location);
   }

   Renderer::Renderer()
   {
      attachShader<gl::shader::Type::vertex>("shaders/vertex.glsl");
      attachShader<gl::shader::Type::fragment>("shaders/fragment.glsl");
      linkProgram();

      vertexBuffer.load(Renderer::verticies);
      indexBuffer.load(Renderer::indicies);	

      gl::bind(vertexArray);

      setPointer(program, "position", 3, 0);
      setPointer(program, "color", 3, sizeof(glm::vec3));
   }
    
   void Renderer::draw()
   {
      gl::clear(gl::BufferBit::color);
      gl::use(program);

      gl::bind(vertexArray);
      indexBuffer.bind();
	
      gl::drawElements(gl::Primitive::triangles, 3, gl::DataType::UnsignedInt, 0);
   }
}
