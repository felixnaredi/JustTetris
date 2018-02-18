// 
// Filename: tetris_types.h
// Created: 2018-02-09 16:41:11 +0100
// Author: Felix Nared
//

#ifndef TETRIS_TYPES_H
#define TETRIS_TYPES_H

#include <stdbool.h>

#include "emacs_ac_break.h"

//
// Frame Cycle:
//
//   cycle
//   |
//   game over
//   |       |
//  (false) (true)
//   |          |
//   |          o----------------------------o
//   |                                       |
//   input buffer                            |
//   |          |                            |
//  (empty)    (input)                       |
//   |              |                        |
//   countdown      |                        |
//   |       |      |                        |
//  (false) (true)--o                        |
//   |              |                        |
//   |              try move shape           |
//   |              |            |           |
//   |             (false)      (true)       o-------o
//   |              |   |           |                |
//   o--------(ignore) (freeze)     reset countdown  |
//   |                  |           |                |
//   |                  |           move shape       |
//   |                  |                    |       |
//   |                  o--------------------|-------o
//   |                  |                    |
//   |                  add to board         |
//   |                  |                    |
//   |                  clear rows           |
//   should redraw      |                    |
//   |           |      next shape           |
//  (no)        (yes)   |                    |
//   |           |      set game over        |
//   |           |      |                    |
//   |           o------o--------------------o
//   |           |
//   |      redraw
//   |      |
//   o------o
//   |
//   next cycle
//

#define js_max(a, b) (a > b ? a : b)
#define js_min(a, b) (a < b ? a : b)
#define js_abs(a) (a < 0 ? -a : a)

typedef struct
{
	int x;
	int y;
} jsVec2i;

typedef struct
{
	float x;
	float y;
} jsVec2f;

typedef struct
{
	int x;
	int y;
	int z;
} jsVec3i;

typedef struct
{
	float x;
	float y;
	float z;	
} jsVec3f;

int js_vec2i_equal(jsVec2i a, jsVec2i b);
int js_vec2f_equal(jsVec2f a, jsVec2f b);
int js_vec3i_equal(jsVec3i a, jsVec3i b);
int js_vec3f_equal(jsVec3f a, jsVec3f b);

jsVec2i js_vec2i_add(jsVec2i a, jsVec2i b);
jsVec2f js_vec2f_add(jsVec2f a, jsVec2f b);
jsVec3i js_vec3i_add(jsVec3i a, jsVec3i b);
jsVec3f js_vec3f_add(jsVec3f a, jsVec3f b);


typedef struct
{
	float r;
	float g;
	float b;
} jsColor;


typedef struct
{
	int status;
	jsVec2i position;
} jsBlock;

bool js_block_is_empty(jsBlock block);

#define JS_BOARD_ROW_AMOUNT 20
#define JS_BOARD_COLUMN_AMOUNT 10
#define JS_BOARD_BLOCK_AMOUNT JS_BOARD_ROW_AMOUNT * JS_BOARD_COLUMN_AMOUNT

typedef struct
{
	jsBlock blocks[JS_BOARD_COLUMN_AMOUNT];
} jsRow;


typedef union
{
	jsBlock pos[JS_BOARD_ROW_AMOUNT][JS_BOARD_COLUMN_AMOUNT];
	jsBlock blocks[JS_BOARD_ROW_AMOUNT * JS_BOARD_COLUMN_AMOUNT];
	jsRow rows[JS_BOARD_ROW_AMOUNT];
} jsBoard;

jsBoard js_empty_board();

#define JS_SHAPE_BLOCK_AMOUNT 4

typedef struct
{
	jsVec2i offset;
	jsBlock blocks[JS_SHAPE_BLOCK_AMOUNT];
} jsShape;

typedef enum {
	jsShapeFormationO = 0x20000000,
	jsShapeFormationI = 0x40000000,
	jsShapeFormationS = 0x60000000,
	jsShapeFormationZ = 0x80000000,
	jsShapeFormationL = 0xA0000000,
	jsShapeFormationJ = 0xC0000000,
	jsShapeFormationT = 0xE0000000,
} jsShapeFormation;


#endif /* TETRIS_TYPES_H */
