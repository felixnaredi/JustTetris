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


bool js_block_is_empty(const jsBlock block)
{
	return !(block.status & JS_BLOCK_COLOR_FILLED);
}


unsigned js_row_positions(const jsRow *row, jsVec2i *des)
{
	JS_DEBUG_NULLPTR(row, js_row_positions, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(row->blocks, js_row_positions, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(des, js_row_positions, __FILE__, __LINE__);

	int i, count = 0;
	const jsBlock *blocks = row->blocks;

	for(i = 0; i < JS_BOARD_COLUMN_AMOUNT; i++) {
		jsBlock block = blocks[i];
		
		if(!js_block_is_empty(block)) {
			des[count] = block.position;			
			count++;
		}
	}

	return count;
}

bool js_row_full(const jsRow *row)
{
	JS_DEBUG_NULLPTR(row, js_row_full, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(row->blocks, js_row_full, __FILE__, __LINE__);
	
	int i;
	const jsBlock *blocks = row->blocks;

	for(i = 0; i < JS_BOARD_COLUMN_AMOUNT; i++) {
		if(js_block_is_empty(blocks[i]))
			return false;
	}

	return true;
}

unsigned js_board_indicies(const jsBoard *board, unsigned *des)
{
	JS_DEBUG_NULLPTR(board, js_board_indicies, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(board->rows, js_board_indicies, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(board->rows->blocks, js_board_indicies, __FILE__, __LINE__);
	
	unsigned i, count = 0;
	const jsBlock *blocks = board->rows->blocks;

	for(i = 0; i < JS_BOARD_BLOCK_AMOUNT; i++) {
		if(!(js_block_is_empty(blocks[i]))) {
			des[count] = i;
			count++;
		}
	}

	return count;
}
			

void js_set_shape(jsShape *shape, jsShapeFormation form, jsVec2i offset)
{
	JS_DEBUG_NULLPTR(shape, js_set_shape, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(shape->blocks, js_set_shape, __FILE__, __LINE__);

	int i;
	jsBlock *blocks = (jsBlock *) shape->blocks;	
	
	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++)
		blocks[i].status |= form;
	
	shape->offset = offset;
}

jsShapeFormation js_get_shape_formation(const jsShape *shape)
{
	JS_DEBUG_NULLPTR(shape, js_get_shape_formation, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(shape->blocks, js_get_shape_formation, __FILE__, __LINE__);
	
#ifdef JS_BUILD_SAFE_GLOBAL
	int i;
	jsBlock *blocks = shape->blocks;
	int form = blocks[0].status & JS_BLOCK_FORMATION;
	
	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		if(blocks[0].status & JS_BLOCK_FORMATION != form)
			JS_DEBUG_PRINT(js_get_shape_formation,
				       "blocks in shape have different formations",
				       __FILE__, __LINE__);
	}
#endif /* JS_BUILD_SAFE_GLOBAL */
	
	return shape->blocks[0].status & JS_BLOCK_FORMATION;
}	

unsigned js_shape_positions(const jsShape *shape, jsVec2i *des)
{
	JS_DEBUG_NULLPTR(shape, js_shape_positions, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(shape->blocks, js_shape_positions, __FILE__, __LINE__);

	int i;
	const jsBlock *blocks = shape->blocks;

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++)
	  des[i] = js_vec2i_add(blocks[i].position, shape->offset);

	return JS_SHAPE_BLOCK_AMOUNT;
}

static void __js_shape_positions(const jsShape *shape, jsVec2i *des)
{
	int i;
	const jsBlock *blocks = shape->blocks;

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++)
		des[i] = js_vec2i_add(blocks[i].position, shape->offset);
}

static bool __js_overlapp(const jsShape *shape, jsVec2i offset, const jsBoard *board)
{
	jsVec2i positions[JS_SHAPE_BLOCK_AMOUNT];
	int i;
	
	__js_shape_positions(shape, positions);

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsVec2i pos = js_vec2i_add(positions[i], offset);

		if(!js_block_is_empty(board->rows[pos.y].blocks[pos.x]))
			return true;
	}

	return false;
}
		

	
	
