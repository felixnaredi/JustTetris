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
//   |          +----------------------------+
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
//   |             (false)      (true)       +-------+
//   |              |   |           |                |
//   o--------(ignore) (freeze)     reset countdown  |
//   |                  |           |                |
//   |                  |           move shape       |
//   |                  |                    |       |
//   |                  o--------------------|-------+
//   |                  |                    |
//   |                  add to board         |
//   |                  |                    |
//   |                  clear rows           |
//   should redraw      |                    |
//   |           |      next shape           |
//  (no)        (yes)   |                    |
//   |           |      set game over        |
//   |           |      |                    |
//   |           o------o--------------------+
//   |           |
//   |      redraw
//   |      |
//   o------+
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


#define JS_SHAPE_BLOCK_AMOUNT 4

typedef struct
{
	jsVec2i offset;
	jsBlock blocks[JS_SHAPE_BLOCK_AMOUNT];
} jsShape;

typedef enum {
	JS_SHAPE_FORMATION_O = 0x20000000,
	JS_SHAPE_FORMATION_I = 0x40000000,
	JS_SHAPE_FORMATION_S = 0x60000000,
	JS_SHAPE_FORMATION_Z = 0x80000000,
	JS_SHAPE_FORMATION_L = 0xA0000000,
	JS_SHAPE_FORMATION_J = 0xC0000000,
	JS_SHAPE_FORMATION_T = 0xE0000000,
} jsShapeFormation;


typedef struct
{
	int status;
	jsBoard *board;
	jsShape *shape;
	jsShape *next_shape;
	int rows;
	int level;
	double score;
	int countdown;
	int timer;
} jsTetrisState;

jsTetrisState *js_alloc_tetris_state();
void js_dealloc_tetris_state(jsTetrisState *state);
void js_init_tetris_state(jsTetrisState *state);

#define JS_STATE_SHAPE_CHANGE      0x00000001
#define JS_STATE_BOARD_CHANGE      0x00000002
#define JS_STATE_NEXT_SHAPE_CHANGE 0x00000004
#define JS_STATE_ROWS_CHANGE       0x00000008
#define JS_STATE_LEVEL_CHANGE      0x00000010
#define JS_STATE_SCORE_CHANGE      0x00000020
#define JS_STATE_RESET_COUNTDOWN   0x00000040
#define JS_STATE_GAME_OVER         0x00000080
#define JS_STATE_DIRTY             0x00000100

#define JS_STATE_CHANGE \
	(JS_STATE_SHAPE_CHANGE | JS_STATE_BOARD_CHANGE | JS_STATE_NEXT_SHAPE_CHANGE \
	 JS_STATE_ROWS_CHANGE | JS_STATE_LEVEL_CHANGE | JS_STATE_SCORE_CHANGE)

void js_move_shape(jsTetrisState *state, jsVec2i offset);
void js_force_shape(jsTetrisState *state, jsVec2i offset);
// void js_rotate_shape(jsTetrisState *state, int rot);


#endif /* TETRIS_TYPES_H */

