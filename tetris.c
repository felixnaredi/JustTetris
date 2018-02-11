// 
// Filename: tetris.c
// Created: 2018-02-09 21:23:44 +0100
// Author: Felix Nared
//

#include <stdbool.h>

#include "tetris_types.h"

#include "build.h"


#define JS_BLOCK_COLOR_FILLED 0x00000001;
#define JS_BLOCK_COLOR_RED    0x0000FF00;
#define JS_BLOCK_COLOR_GREEN  0x00FF0000;
#define JS_BLOCK_COLOR_BLUE   0xFF000000;

#define JS_BLOCK_TIME      0x1FFFFFFF;
#define JS_BLOCK_FORMATION 0xE0000000;

#define JS_BLOCK_FORMATION_OFFSET 29

#define JS_SHAPE_ROW_AMOUNT 4
#define JS_SHAPE_COLUMN_AMOUNT 4


int js_block_is_empty(jsBlock block)
{
	return !(block.color & JS_BLOCK_COLOR_FILLED);
}

int js_block_color(jsBlock block)
{
#ifdef JS_BUILD_SAFE_GLOBAL

	if(js_block_is_empty(block))
		JS_DEBUG_PRINT(js_block_color, "empty block", __FILE__, __LINE__);
#endif // JS_BUILD_SAFE_GLOBAL
	
	jsVec3f color_vector;
	int color = block.color;
	
	color_vector.r = (float) ((color & JS_BLOCK_COLOR_RED) >> 0x0008);
	color_vector.g = (float) ((color & JS_BLOCK_COLOR_GREEN) >> 0x0010);
	color_vector.b = (float) ((color & JS_BLOCK_COLOR_BLUE) >> 0x0018);

	return color_vector;
}

unsigned js_row_positions(const jsRow *row, jsVec2i *des)
{
	JS_DEBUG_NULLPTR(row, js_row_positions, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(row->blocks, js_row_positions, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(des, js_row_positions, __FILE__, __LINE__);

	int i, count = 0;
	jsBlock *blocks = row->blocks;

	for(i = 0; i < len; i++) {
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
	jsBlock *blocks = row->blocks;

	for(i = 0; i < JS_BOARD_COLUMN_AMOUNT; i++) {
		if(js_block_is_empty(blocks[i]))
			return FALSE;
	}

	return TRUE;
}

unsigned js_board_indicies(const jsBoard *board, unsigned *des)
{
	JS_DEBUG_NULLPTR(board, js_board_indicies, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(board->rows, js_board_indicies, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(board->rows->blocks, js_board_indicies, __FILE__, __LINE__);
	
	unsigned i, count = 0;
	jsBlock *blocks = board->rows->blocks;

	for(i = 0; i < JS_BOARD_BLOCK_AMOUNT; i++) {
		if(!(js_block_is_empty(blocks[i]))) {
			des[count] = i;
			count++;
		}
	}

	return count;
}
			

void js_set_shape(const jsShape *shape, jsShapeFormation form, jsVec2i offset)
{
	JS_DEBUG_NULLPTR(shape, js_set_shape, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(shape->blocks, js_set_shape, __FILE__, __LINE__);

	int i;
	jsBlock *blocks = (jsBlock *) shape->blocks;	
	
	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++)
		blocks[i] |= form;
	
	shape->offset = offset;
}

jsShapeFormation js_get_shape_formation(const jsShape *shape)
{
	JS_DEBUG_NULLPTR(shape, js_get_shape_formation, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(shape->blocks, js_get_shape_formation, __FILE__, __LINE__);
	
#ifdef JS_BUILD_SAFE_GLOBAL
	int i;
	jsBlock *blocks = shape->blocks
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
	jsBlock *blocks = shape->blocks;

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++)
		des[i] = blocks[i] + shape->offset;

	return JS_SHAPE_BLOCK_AMOUNT;
}
