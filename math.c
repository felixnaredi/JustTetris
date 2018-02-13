// 
// Filename: math.c
// Created: 2018-02-09 21:18:04 +0100
// Author: Felix Nared
//

#include "tetris.h"


int js_vec2i_equal(jsVec2i a, jsVec2i b)
{
	return a.x == b.x && a.y == b.y;
}

int js_vec2f_equal(jsVec2f a, jsVec2f b)
{
	return a.x == b.x && a.y == b.y;
}

int js_vec3i_equal(jsVec3i a, jsVec3i b)
{
	return a.x == b.x && a.y == b.y;
}

int js_vec3f_equal(jsVec3f a, jsVec3f b)
{
	return a.x == b.x && a.y == b.y;
}

jsVec2i js_vec2i_add(jsVec2i a, jsVec2i b)
{
	return (jsVec2i) {a.x + b.x, a.y + b.y};
}

jsVec2f js_vec2f_add(jsVec2f a, jsVec2f b)
{
	return (jsVec2f) {a.x + b.x, a.y + b.y};
}

jsVec3i js_vec3i_add(jsVec3i a, jsVec3i b)
{
	return (jsVec3i) {a.x + b.x, a.y + b.y};
}

jsVec3f js_vec3f_add(jsVec3f a, jsVec3f b)
{
	return (jsVec3f) {a.x + b.x, a.y + b.y};
}
