// 
// Filename: tetris.c
// Created: 2018-02-13 20:57:53 +0100
// Author: Felix Nared
//


#include <stdbool.h>

#include "tetris.h"
#include "build.h"

#include "emacs_ac_break.h"

#define JS_BLOCK_COLOR_FILLED 0x00000001
#define JS_BLOCK_TIME         0x1FFFFF00
#define JS_BLOCK_FORMATION    0xE0000000

#define JS_SHAPE_ROW_AMOUNT 4
#define JS_SHAPE_COLUMN_AMOUNT 4

static bool __drawEachFrame = false;
static int __countdown = 0;
static int __fullCountdown = 0;
static int __score = 0;
static jsShape __nextShape;


/// static void
/// __js_reset_countdown
///
/// Resets the countdown timer.
static void __js_reset_countdown()
{	
	__countdown = __fullCountdown;
}

/// static jsBlock
/// __js_empty_block
///
/// Returns an empty block.
static jsBlock __js_empty_block()
{
	return (jsBlock) {0, -1, -1};
}

/// static jsRow
/// __js_empty_row
///
/// Returns an empty row.
static jsRow __js_empty_row()
{
	jsRow row;
	int i;

	for(i = 0; i < JS_BOARD_COLUMN_AMOUNT; i++)
		row.blocks[i] = __js_empty_block();

	return row;
}

/// static jsBoard
/// __js_empty_board
///
/// Returns an empty board.
static jsBoard __js_empty_board()
{
	jsBoard board;
	int i;

	for(i = 0; i < JS_BOARD_ROW_AMOUNT; i++)
		board.rows[i] = __js_empty_row();

	return board;
}

/// static bool
/// __js_block_is_empty
///
/// Returns true if the block is empty.
static bool __js_block_is_empty(const jsBlock block)
{
	return !(block.status & JS_BLOCK_COLOR_FILLED);
}

/// static bool
/// __js_row_full
///
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

/// static bool
/// __js_overlapp
///
/// Returns true if at least one of the blocks in shape are at the
/// same position as a non empty one in board.
static bool __js_overlapp(const jsBoard *board, const jsShape *shape, jsVec2i offset)
{
	int i;
	const jsBlock *blocks = shape->blocks;
	
	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsVec2i blockPos = js_vec2i_add(blocks[i].position, shape->offset);
		jsVec2i pos = js_vec2i_add(blockPos, offset);

		if(!__js_block_is_empty(board->rows[pos.y].blocks[pos.x]))
			return true;
	}

	return false;
}

/// static int
/// __js_clear_rows
///
/// Clears all full rows in board. By clearing a row, all rows above
/// the cleared row will be lowered one step, the top row will be
/// set to an empty row.
///     Returns the amount of rows cleared.
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

	
	
