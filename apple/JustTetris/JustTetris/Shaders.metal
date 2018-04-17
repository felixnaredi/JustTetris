//
//  Shaders.metal
//  JustTetris
//
//  Created by Felix Naredi on 3/9/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

#include <metal_stdlib>

using namespace metal;


struct RasterizerData
{
  float4 position [[ position ]];
  float4 color;

  RasterizerData(float4 _position, float4 _color) :
    position(_position),
    color(_color)
  { }

};


vertex RasterizerData
vertexShader(constant float2*  positions     [[ buffer(0) ]],
             constant int2&    grid          [[ buffer(1) ]],
             constant uint8_t* colorIndicies [[ buffer(2) ]],
             constant float3*  colors        [[ buffer(3) ]],
             uint              vertexID      [[ vertex_id ]],
             uint              instanceID    [[ instance_id ]])
{
  int2 gp{
    int(instanceID) % grid.x,
    int(instanceID) / grid.x,
  };
  float2 vp(positions[vertexID]);
  float2x2 smx{
    float2(1.0 / grid.x, 0.0),
    float2(0.0, 1.0 / grid.y),
  };
  float2 t(gp.x * 2.0 + 1.0, gp.y * 2.0 + 1.0);
  float2 p(((vp + t) * smx) + float2(-1, -1));
  
  return RasterizerData{
    float4(p, 0, 1),
    float4(colors[colorIndicies[instanceID]], 1)
  };
}

fragment float4
fragmentShader(RasterizerData in [[stage_in]])
{
  return in.color;
}
