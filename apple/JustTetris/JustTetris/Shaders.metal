//
//  Shaders.metal
//  JustTetris
//
//  Created by Felix Naredi on 3/9/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

#include <metal_stdlib>
#include "shader_types.h"

using namespace metal;


struct RasterizerData
{
  float4 clipSpacePosition [[ position ]];
  float4 color;

  RasterizerData(float4 _clipSpacePosition, float4 _color) :
    clipSpacePosition(_clipSpacePosition),
    color(_color)
  { }

  RasterizerData(float2 _clipSpacePosition, float4 _color) :
    RasterizerData(float4(_clipSpacePosition, 0, 1), _color)
  { }
};


vertex RasterizerData
vertexShader(uint vertexID [[vertex_id]],
             constant js_shader_vertex* verticies [[ buffer(JS_GRAPHIC_RESOURCE_VERTICIES) ]],
             constant float4x4& matrix [[ buffer(JS_GRAPHIC_RESOURCE_SHAPE_MATRIX) ]])
{
  const auto v = verticies[vertexID];
  return RasterizerData(float4(v.position, 0, 1) * matrix, v.color);
}

fragment float4
fragmentShader(RasterizerData in [[stage_in]])
{
  return in.color;
}
