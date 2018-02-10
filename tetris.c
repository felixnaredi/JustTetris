// 
// Filename: tetris.c
// Created: 2018-02-09 21:23:44 +0100
// Author: Felix Nared
//

#include "tetris_types.h"

#include "build.h"


#define JS_BLOCK_COLOR_FILLED 0x00000001;
#define JS_BLOCK_COLOR_RED    0x00000FF0;
#define JS_BLOCK_COLOR_GREEN  0x000FF000;
#define JS_BLOCK_COLOR_BLUE   0x0FF00000;

#define JS_BLOCK_TIME      0x1FFFFFFF;
#define JS_BLOCK_FORMATION 0xE0000000;

#define JS_BLOCK_FORMATION_OFFSET 29

#define JS_SHAPE_LENGTH 16


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

static unsigned
__js_row_positions(const jsBoardRow *row, unsigned y, jsVec2i *des, unsigned len)
{
	JS_DEBUG_NULLPTR(row, __js_row_positions, __FILE__, __LINE__);
	JS_DEBUG_NULLPTR(des, __js_row_positions, __FILE__, __LINE__);

	int i, count = 0;
	jsBlock *blocks = row->blocks;

	for(i = 0; i < len; i++) {
		jsBlock block = blocks[i];
		
		if(!js_block_is_empty(blocks)) {
			des[count].x = i;
			des[count].y = y;
			
			count++;
		}
	}

	return count;
}

unsigned js_brow_positions(const jsBoardRow *row, unsigned y, jsVec2i *des)
{
	return __js_row_positions(row, y, des, JS_BOARD_COLUMN_AMOUNT);
}

unsigned js_srow_positions(const jsShapeRow *row, unsigned y, jsVec2i *des)
{
	return __js_row_positions(row, y, des, JS_SHAPE_COLUMN_AMOUNT);
}

void js_set_shape(const jsShape *shape, jsShapeFormation form, jsVec2i offset)
{
	JS_DEBUG_NULLPTR(shape, js_get_shape_formation, __FILE__, __LINE__);

	int i;
	jsBlock *blocks = (jsBlock *) shape->rows->blocks;	
	
	for(i = 0; i < JS_SHAPE_LENGTH; i++) {		
		jsBlock block = blocks[i];

		if(!js_block_is_empty(block))
			block |= form;
	}
	
	shape->offset = offset;
}

jsShapeFormation js_get_shape_formation(const jsShape *shape)
{
	JS_DEBUG_NULLPTR(shape, js_get_shape_formation, __FILE__, __LINE__);	
	
#ifdef JS_BUILD_SAFE_GLOBAL
	int i;
	jsBlock *blocks = shape->rows->blocks
	int form = blocks[0].status & JS_BLOCK_FORMATION;
	
	for(i = 0; i < JS_SHAPE_LENGTH; i++) {
		if(blocks[0].status & JS_BLOCK_FORMATION != form)
			JS_DEBUG_PRINT(js_get_shape_formation,
				       "blocks in shape have different formations",
				       __FILE__, __LINE__);
	}
#endif /* JS_BUILD_SAFE_GLOBAL */
	
	return blocks[0].status & JS_BLOCK_FORMATION;
}	



