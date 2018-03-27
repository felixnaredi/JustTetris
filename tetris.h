//
// Filename: tetris_types.h
// Created: 2018-02-09 16:41:11 +0100
// Author: Felix Nared
//

#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#include "vector.h"

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

jsBoard js_empty_board(void);


#define JS_SHAPE_ROW_AMOUNT 4
#define JS_SHAPE_COLUMN_AMOUNT 4
#define JS_SHAPE_BLOCK_AMOUNT 4

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
	const jsBlock *blocks;
	int index;
	jsVec2i offset;
} jsShape;

jsShape js_rand_shape(void);
jsShapeFormation js_block_formation(jsBlock block);


typedef enum {
	jsMoveStatusMute,
	jsMoveStatusFailure,
	jsMoveStatusSuccess,
	jsMoveStatusScore,
	jsMoveStatusMerge,
} jsMoveStatus;

typedef struct
{
	jsMoveStatus status;
	jsVec2i offset;
	jsVec2i new_position;
} jsTranslationResult;

jsTranslationResult js_translate_result(const jsShape *shape, const jsBoard *board, jsVec2i vector);

jsShape js_translate_shape(const jsShape *shape, const jsTranslationResult *result);
int js_merge(jsBoard *board, const jsShape *shape);
float js_translate_score(const jsTranslationResult *result);

typedef enum {
	jsRotationNone             =  0,
	jsRotationClockwise        =  1,
	jsRotationCounterClockwise = -1,
} jsRotation;

typedef struct
{
	jsMoveStatus status;
	int old_shape_index;
	int new_shape_index;
} jsRotationResult;

jsRotationResult js_rotate_result(const jsShape *shape, const jsBoard *board, jsRotation direction);
jsShape js_rotate_shape(const jsShape *shape, const jsRotationResult *result);

#define JS_ROW_CLEAR_MAX 4

typedef struct
{
	int count;
	int indicies[JS_ROW_CLEAR_MAX];
} jsClearRowsResult;

jsClearRowsResult js_clear_rows_result(const jsBoard *board);

bool js_clear_board_rows(jsBoard *board, const jsClearRowsResult *result);


#endif /* TETRIS_H */
