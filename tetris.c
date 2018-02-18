// 
// Filename: tetris.c
// Created: 2018-02-13 20:57:53 +0100
// Author: Felix Nared
//


#include <stdbool.h>

#include "tetris.h"
#include "build.h"

#include "emacs_ac_break.h"

#define JS_BLOCK_FILLED    0x00000001
#define JS_BLOCK_TIME      0x1FFFFF00
#define JS_BLOCK_FORMATION 0xE0000000

static bool __drawEachFrame = false;
static int __countdown = 0;
static int __fullCountdown = 0;
static int __score = 0;
static jsShape __nextShape;

#define JS_SHAPE_VERTICIES(id, x0, y0, x1, y1, x2, y2, x3, y3) \
	{id | JS_BLOCK_FILLED, x0, y0, id | JS_BLOCK_FILLED, x1, y1, \
	 id | JS_BLOCK_FILLED, x2, y2, id | JS_BLOCK_FILLED, x3, y3}

static const jsShape __shape_verticies[] = {
	JS_SHAPE_VERTICIES(jsShapeFormationO, 1, 2, 2, 2, 1, 3, 2, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationI, 1, 0, 1, 1, 1, 2, 1, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationI, 0, 2, 1, 2, 2, 2, 3, 2),
	JS_SHAPE_VERTICIES(jsShapeFormationS, 2, 1, 1, 2, 2, 2, 1, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationS, 1, 2, 2, 2, 2, 3, 3, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationZ, 1, 1, 1, 2, 2, 2, 2, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationZ, 2, 2, 3, 2, 1, 3, 2, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationL, 1, 1, 2, 1, 1, 2, 1, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationL, 0, 1, 0, 2, 1, 2, 2, 2),
	JS_SHAPE_VERTICIES(jsShapeFormationL, 1, 1, 1, 2, 0, 3, 1, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationL, 0, 2, 1, 2, 2, 2, 2, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationJ, 1, 1, 2, 1, 2, 2, 2, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationJ, 1, 2, 2, 2, 3, 2, 1, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationJ, 2, 1, 2, 2, 2, 3, 3, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationJ, 3, 1, 1, 2, 2, 2, 3, 2),
	JS_SHAPE_VERTICIES(jsShapeFormationT, 1, 1, 1, 2, 2, 2, 1, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationT, 1, 1, 0, 2, 1, 2, 2, 2),
	JS_SHAPE_VERTICIES(jsShapeFormationT, 1, 1, 0, 2, 1, 2, 1, 3),
	JS_SHAPE_VERTICIES(jsShapeFormationT, 0, 2, 1, 2, 2, 2, 1, 3),
};

typedef struct
{
	int min;
	int max;
} __jsRange;

static const __jsRange __rot_ranges[] = {
	{0, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 10}, {11, 14}, {15, 18}, 
};

#define JS_SHAPE_FORMATION_AMOUNT (sizeof(__rot_ranges) / sizeof(__rot_ranges[0]))

/// Resets the countdown timer.
static void __js_reset_countdown()
{	
	__countdown = __fullCountdown;
}

/// Returns an empty block.
static jsBlock __js_empty_block()
{
	return (jsBlock) {0, -1, -1};
}

/// Returns an empty row.
static jsRow __js_empty_row()
{
	jsRow row;
	int i;

	for(i = 0; i < JS_BOARD_COLUMN_AMOUNT; i++)
		row.blocks[i] = __js_empty_block();

	return row;
}

/// Returns an empty board.
static jsBoard __js_empty_board()
{
	jsBoard board;
	int i;

	for(i = 0; i < JS_BOARD_ROW_AMOUNT; i++)
		board.rows[i] = __js_empty_row();

	return board;
}

/// Returns an empty board.
jsBoard js_empty_board()
{
	jsBoard board;
	int i;

	for(i = 0; i < JS_BOARD_ROW_AMOUNT; i++)
		board.rows[i] = __js_empty_row();

	return board;
}

/// Returns true if the block is empty.
static bool __js_block_is_empty(jsBlock block)
{
	return !(block.status & JS_BLOCK_FILLED);
}


/// Returns true if the block is empty.
bool js_block_is_empty(jsBlock block)
{
	return !(block.status & JS_BLOCK_FILLED);
}

/// Returns true if every block in row is non empty.
static bool __js_row_is_full(const jsRow *row)
{
	int i;
	const jsBlock *blocks = row->blocks;

	for(i = 0; i < JS_BOARD_COLUMN_AMOUNT; i++) {
		if(__js_block_is_empty(blocks[i]))
			return false;
	}

	return true;
}

/// Returns true if at least one of the blocks in shape are at the
/// same position as a non empty one in board.
static bool __js_overlapp(const jsBoard *board, const jsShape *shape, jsVec2i offset)
{
	int i;
	const jsBlock *blocks = shape->blocks;
	
	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsVec2i blockPos = js_vec2i_add(blocks[i].position, shape->offset);
		jsVec2i pos = js_vec2i_add(blockPos, offset);

		if(!__js_block_is_empty(board->pos[pos.y][pos.x]))
			return true;
	}

	return false;
}

/// Clears all full rows in board. By clearing a row, all rows above
/// the cleared row will be lowered one step, the top row will be
/// set to an empty row.
///
/// Returns the amount of rows cleared.
static int __js_clear_rows(jsBoard *board)
{
	int i, count = 0;
	jsRow *rows = board->rows;

	for(i = 0; i < JS_BOARD_ROW_AMOUNT; i++) {
		int j;

		if(!__js_row_is_full(&board->rows[i]))
			continue;		

		for(j = i; j < JS_BOARD_ROW_AMOUNT - 1; j++)
			rows[j] = rows[j + 1];

		rows[JS_BOARD_ROW_AMOUNT - 1] = __js_empty_row();		
		count++;
	}

	return count;
}
