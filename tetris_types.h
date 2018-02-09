// 
// Filename: tetris_types.h
// Created: 2018-02-09 16:41:11 +0100
// Author: Felix Nared
//

#ifndef TETRIS_TYPES_H
#define TETRIS_TYPES_H


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
	float r;
	float g;
	float b;
} jsColor;


typedef struct
{
	int color;
	int id;
} jsBlock;

#define JS_COLORMAP_FILLED 0x0001;
#define JS_COLORMAP_RED    0x00F0;
#define JS_COLORMAP_GREEN  0x0F00;
#define JS_COLORMAP_BLUE   0xF000;

int js_genid();
int js_block_is_empty(jsBlock block);
jsColor js_block_color(jsBlock block);

#define JS_BOARD_ROW_AMOUNT 20
#define JS_BOARD_COLUMN_AMOUNT 10
#define JS_SHAPE_ROW_AMOUNT 4
#define JS_SHAPE_COLUMN_AMOUNT 4

typedef struct
{
	jsBlock *blocks;
} jsRow;


typedef struct
{
	jsBlock blocks[JS_BOARD_COLUMN_AMOUNT];
} jsBoardRow;

typedef struct
{
	jsBlock blocks[JS_SHAPE_COLUMN_AMOUNT];
} jsShapeRow;

unsigned js_brow_positions(jsBoardRow *row, unsigned y, jsVec2i *des);
unsigned js_srow_positions(jsShapeRow *row, unsigned y, jsVec2i *des);
int js_row_full(jsBoardRow *row);
void js_row_clear(jsBoardRow *row);


typedef struct
{
	jsBoardRow rows[JS_BOARD_ROW_AMOUNT];
} jsBoard;

typedef struct
{
	jsVec2i offset;
	jsShapeRow rows[JS_SHAPE_ROW_AMOUNT];
} jsShape;

unsigned js_board_positions(jsBoard *board, jsVec2i *des);
unsigned js_shape_positions(jsShape *shape, jsVec2i *des);


#endif /* TETRIS_TYPES_H */
