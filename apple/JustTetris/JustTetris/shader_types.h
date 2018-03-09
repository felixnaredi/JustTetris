//
//  shader_types.h
//  JustTetris
//
//  Created by Felix Naredi on 3/9/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

#ifndef shader_types_h
#define shader_types_h


#include <simd/simd.h>


#define JS_BUFFER_VERTICIES 0


typedef struct
{
  vector_float2 position;
  vector_float4 color;
} js_shader_vertex;


#endif /* shader_types_h */
