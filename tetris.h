// 
// Filename: tetris_types.h
// Created: 2018-02-09 16:41:11 +0100
// Author: Felix Nared
//

#ifndef TETRIS_TYPES_H
#define TETRIS_TYPES_H

#include <stdbool.h>

// -------------------------------------------------------------
// Break for my auto complete in emacs
//
#define J_BREAK
typedef int jBreak;
typedef int iBreak;
typedef int uBreak;
typedef int cBreak;
typedef int bBreak;
typedef int rBreak;
// -------------------------------------------------------------

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
//   move trigger   |                        |
//   |          |   |                        |
//  (false) (true)--|                        |
//   |              |                        |
//   |              try move shape           |
//   |              |            |           |
//   |             (false)      (true)       |
//   |              |   |        |           |
//   |--------(ignore) (freeze)  move shape  |
//   |                  |                 |  |
//   |                  add to board      o--o
//   should redraw      |                 |
//   |           |      clear rows        |
//  (no)        (yes)   |                 |
//   |           |      next shape        |
//   |           |      |                 |
//   |      redraw------o-----------------o
//   |      |
//   o------o
//      |
//      next cycle
//


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


#define JS_BOARD_ROW_AMOUNT 20
#define JS_BOARD_COLUMN_AMOUNT 10
#define JS_BOARD_BLOCK_AMOUNT JS_BOARD_ROW_AMOUNT * JS_BOARD_COLUMN_AMOUNT

typedef struct
{
	jsBlock blocks[JS_BOARD_COLUMN_AMOUNT];
} jsRow;

typedef struct
{
	jsRow rows[JS_BOARD_ROW_AMOUNT];
} jsBoard;


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
