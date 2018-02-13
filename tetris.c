// 
// Filename: tetris.c
// Created: 2018-02-09 21:23:44 +0100
// Author: Felix Nared
//

#include <stdbool.h>

#include "tetris.h"
#include "build.h"


#define JS_BLOCK_COLOR_FILLED 0x00000001
#define JS_BLOCK_TIME         0x1FFFFF00
#define JS_BLOCK_FORMATION    0xE0000000

#define JS_BLOCK_FORMATION_OFFSET 29

#define JS_SHAPE_ROW_AMOUNT 4
#define JS_SHAPE_COLUMN_AMOUNT 4


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
static bool __js_row_full(const jsRow *row)
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
static bool __js_overlapp(const jsShape *shape, jsVec2i offset, const jsBoard *board)
{
	int i;
	const jsBlock *blocks = shape->blocks;
	
	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsVec2i pos = js_vec2i_add(blocks[i].position, offset);

		if(!__js_block_is_empty(board->rows[pos.y].blocks[pos.x]))
			return true;
	}

	return false;
}
		

	
	
