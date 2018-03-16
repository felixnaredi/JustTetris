//
// Filename: tetris_types.h
// Created: 2018-02-09 16:41:11 +0100
// Author: Felix Nared
//

#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#ifdef JS_USING_EMACS

#include "emacs_ac_break.h"
#endif /* JS_USING_EMACS */

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

jsBlock js_empty_block(void);
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

#define JS_SHAPE_ROW_AMOUNT 4
#define JS_SHAPE_COLUMN_AMOUNT 4
#define JS_SHAPE_BLOCK_AMOUNT 4

typedef struct
{
	jsVec2i offset;
	int index;
	const jsBlock *blocks;
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

jsTetrisState *js_alloc_tetris_state(void);
void js_dealloc_tetris_state(jsTetrisState *state);
void js_init_tetris_state(jsTetrisState *state);

/// When a tetris states values are modified or checked by some functions the
/// event will be stored in the status of the state.
#define JS_STATE_CHANGED_SHAPE_OFFSET 0x00000001
#define JS_STATE_CHANGED_SHAPE_INDEX  0x00000002
#define JS_STATE_CHANGED_NEXT_SHAPE   0x00000004
#define JS_STATE_CHANGED_BOARD        0x00000008
#define JS_STATE_CHANGED_ROWS         0x00000010
#define JS_STATE_CHANGED_LEVEL        0x00000020
#define JS_STATE_CHANGED_SCORE        0x00000040
#define JS_STATE_MOVE_SUCCEDED        0x00000080
#define JS_STATE_RESET_COUNTDOWN      0x00000100
#define JS_STATE_GAME_OVER            0x00000200

#define JS_STATE_CLEAR                0x00000000

void js_clear_state_status(jsTetrisState *state);
bool js_state_is_modified(const jsTetrisState *state);

void js_move_shape(jsTetrisState *state, jsVec2i offset);
void js_force_shape(jsTetrisState *state, jsVec2i offset);

typedef enum {
	jsRotationClockwise        =  1,
	jsRotationCounterClockwise = -1,
} jsRotation;

void js_rotate_shape(jsTetrisState *state, jsRotation rot);


#endif /* TETRIS_H */
