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

#define JS_ROW_CLEAR_MAX 4

typedef struct
{
	bool mute_action;
	bool user_action;
	bool game_over;
	bool successfull;

	struct
	{
		jsVec2i offset;
		jsVec2i new_position;
	} translation;

	struct
	{
		int old_shape_index;
		int new_shape_index;
	} rotation;

	struct
	{
		bool should_merge;
		int rows_cleared;
		int indicies[JS_ROW_CLEAR_MAX];
	} merge;
} jsResult;

jsShape js_result_old_shape(jsResult result);
jsShape js_result_new_shape(jsResult result);

jsResult js_translate_shape(jsShape *shape, jsBoard *board, jsVec2i offset,
                            bool user_action);

typedef enum {
	jsRotateClockwise        =  1,
	jsRotateCounterClockwise = -1,
} jsRotate;

jsResult js_rotate_shape(jsShape *shape, jsBoard *board, jsRotate direction,
                         bool user_action);

#endif /* TETRIS_H */
