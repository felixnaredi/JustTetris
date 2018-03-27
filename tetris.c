//
// Filename: tetris.c
// Created: 2018-02-13 20:57:53 +0100
// Author: Felix Nared
//


#include <stdbool.h>
#include <stdlib.h>

#include "build.h"
#include "tetris.h"
#include "vector.h"

#ifdef JS_USING_EMACS

#include "emacs_ac_break.h"
#endif /* JS_USING_EMACS */


#define JS_BLOCK_FILLED    0x00000001
#define JS_BLOCK_FORMATION 0xE0000000

typedef struct
{
	jsVec2i offset;
	const jsBlock blocks[JS_SHAPE_BLOCK_AMOUNT];
} __jsShapeData;

#define JS_SHAPE_DATA(id, x0, y0, x1, y1, x2, y2, x3, y3) \
	{ {id | JS_BLOCK_FILLED, x0, y0}, {id | JS_BLOCK_FILLED, x1, y1}, \
	  {id | JS_BLOCK_FILLED, x2, y2}, {id | JS_BLOCK_FILLED, x3, y3} }

static const __jsShapeData shape_data[] = {
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationO, 1, 2, 2, 2, 1, 3, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationI, 2, 0, 2, 1, 2, 2, 2, 3) },
	{ {3, 17}, JS_SHAPE_DATA(jsShapeFormationI, 0, 2, 1, 2, 2, 2, 3, 2) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationS, 2, 1, 1, 2, 2, 2, 1, 3) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationS, 1, 2, 2, 2, 2, 3, 3, 3) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationZ, 1, 1, 1, 2, 2, 2, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationZ, 2, 2, 3, 2, 1, 3, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationL, 1, 1, 2, 1, 1, 2, 1, 3) },
	{ {4, 17}, JS_SHAPE_DATA(jsShapeFormationL, 0, 1, 0, 2, 1, 2, 2, 2) },
	{ {4, 16}, JS_SHAPE_DATA(jsShapeFormationL, 1, 1, 1, 2, 0, 3, 1, 3) },
	{ {4, 16}, JS_SHAPE_DATA(jsShapeFormationL, 0, 2, 1, 2, 2, 2, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationJ, 1, 1, 2, 1, 2, 2, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationJ, 1, 2, 2, 2, 3, 2, 1, 3) },
	{ {2, 16}, JS_SHAPE_DATA(jsShapeFormationJ, 2, 1, 2, 2, 2, 3, 3, 3) },
	{ {3, 17}, JS_SHAPE_DATA(jsShapeFormationJ, 3, 1, 1, 2, 2, 2, 3, 2) },
	{ {3, 16}, JS_SHAPE_DATA(jsShapeFormationT, 1, 1, 1, 2, 2, 2, 1, 3) },
	{ {4, 17}, JS_SHAPE_DATA(jsShapeFormationT, 1, 1, 0, 2, 1, 2, 2, 2) },
	{ {4, 16}, JS_SHAPE_DATA(jsShapeFormationT, 1, 1, 0, 2, 1, 2, 1, 3) },
	{ {4, 16}, JS_SHAPE_DATA(jsShapeFormationT, 0, 2, 1, 2, 2, 2, 1, 3) },
};

typedef struct
{
	int min;
	int max;
} __jsIndexRange;

static const __jsIndexRange shape_index_ranges[] = {
	{0, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 10}, {11, 14}, {15, 18},
};

#define JS_SHAPE_INDEX_RANGES_LENGTH \
	(sizeof(shape_index_ranges) / sizeof(shape_index_ranges[0]))

static int __js_timer(int level)
{
	return 120 / level;
}

/// Returns an empty block.
static jsBlock __js_empty_block()
{
	return (jsBlock) {0, -1, -1};
}

/// Global wrapper of '__js_empty_block'.
jsBlock js_empty_block()
{
  return __js_empty_block();
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

/// Global wrapper for '__js_empty_board'
jsBoard js_empty_board()
{
	return __js_empty_board();
}

/// Returns the blocks for the shape at a given index.
static const jsBlock *__js_blocks_at(int index)
{
	return shape_data[index].blocks;
}

jsShapeFormation js_block_formation(jsBlock block)
{
  return block.status & JS_BLOCK_FORMATION;
}

/// Makes a shape from the given index, corresponding to the data in
/// 'shape_data'.
///
/// Returns the shape for the index.
static jsShape __js_make_shape(int index)
{
	const __jsShapeData *data = &shape_data[index];

	return (jsShape){data->blocks, index, data->offset};
}

/// Generates a random index of a shape pointing to the first shape of a form.
///
/// Returns a index to be used in 'shape_data'.
static int __js_gen_shape_index()
{
	return shape_index_ranges[rand() % JS_SHAPE_INDEX_RANGES_LENGTH].min;
}

/// Returns a random shape from the first of each formation.
jsShape js_rand_shape()
{
	return __js_make_shape(__js_gen_shape_index());
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

/// Returns true if every block in row is not empty.
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

jsClearRowsResult js_clear_rows_result(const jsBoard *board)
{
	int i, *indicies, count = 0;
	jsClearRowsResult result;

	indicies = result.indicies;

	for(i = 0; i < JS_BOARD_ROW_AMOUNT; i++) {
		if(__js_row_is_full(&board->rows[i])) {
			indicies[count] = i;
			count++;
		}
	}

	result.count = count;

	return result;
}

/// Moves the blocks at row 'offset' amount in y dimension.
static void __js_move_row(jsRow *row, int offset)
{
	int x;

	for(x = 0; x < JS_BOARD_COLUMN_AMOUNT; x++)
		row->blocks[x].position.y += offset;
}

/// Returns true if rows were cleared.
bool js_clear_board_rows(jsBoard *board, const jsClearRowsResult *result)
{
	int i;
	jsRow *rows = board->rows;

	if(result->count == 0)
		return false;

	// Reversed iteration of indicies.
	for(i = result->count - 1; i > -1; i--) {
		int j;

		for(j = result->indicies[i]; j < JS_BOARD_ROW_AMOUNT - 1; j++) {
			rows[j] = rows[j + 1];
			__js_move_row(&rows[j], -1);
		}
	}

	rows[JS_BOARD_ROW_AMOUNT - 1] = __js_empty_row();

	return true;
}

static bool __js_outside_board(jsVec2i pos)
{
	return pos.x < 0 || pos.y < 0 || pos.x >= JS_BOARD_COLUMN_AMOUNT ||
		pos.y >= JS_BOARD_ROW_AMOUNT;
}

typedef enum {
	jsBlockPositionStatusValid     = 0,
	jsBlockPositionStatusOverlapp,
	jsBlockPositionStatusOutside,
} __jsBlockPositionStatus;

/// Returns true if at least one of the blocks in shape are at the
/// same position as a non empty block in board or if at least one
/// block in shape are outside the bounds of board.
static __jsBlockPositionStatus
__js_overlapp(const jsBoard *board, const jsShape *shape, jsVec2i offset)
{
	int i;
	const jsBlock *blocks = shape->blocks;

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsVec2i pos = js_vec2i_add(
			js_vec2i_add(blocks[i].position, shape->offset), offset);

		if(__js_outside_board(pos))
			return jsBlockPositionStatusOutside;

		if(!__js_block_is_empty(board->pos[pos.y][pos.x]))
			return jsBlockPositionStatusOverlapp;
	}

	return jsBlockPositionStatusValid;
}

/// Adds each block from shape, that is inside board, to board.
///
/// Returns the amount of blocks that are outside board.
static int __js_merge(jsBoard *board, const jsShape *shape)
{
	int i, count = 0;
	const jsBlock *blocks = shape->blocks;

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsBlock block = blocks[i];
		jsVec2i pos = js_vec2i_add(block.position, shape->offset);

		if(__js_outside_board(pos)) {
			count++;
			continue;
		}

		block.position = pos;
		board->pos[pos.y][pos.x] = block;
	}

	return count;
}

/// Merges shape and board. Moves shape upwards until it doesn't overlapp with
/// the board.
int js_merge(jsBoard *board, const jsShape *shape)
{
	jsVec2i offset = {0, 0};

	while(__js_overlapp(board, shape, offset) == jsBlockPositionStatusOverlapp)
		offset.y += 1;

	return __js_merge(board, shape);
}

/// Returns the level for the given amount of rows.
static int __js_get_level(int rows)
{
	return rows / 8 + 1;
}

/// Returns a score multiplier for the given level.
static float __js_level_multiplier(int level)
{
	return (float)level / 8.0 + 1.0;
}

/// Returns the score clearing the given amount of rows is worth for the current
/// level.
static float __js_score_rows(int rows, int level) {
	float multiplier = __js_level_multiplier(level);

	switch (rows) {
	case 1: return 1.0 * multiplier;
	case 2: return 3.0 * multiplier;
	case 3: return 6.0 * multiplier;
	case 4: return 10.0 * multiplier;
	default: return 0;
	}
}

///
jsTranslationResult js_translate_result(const jsShape *shape, const jsBoard *board, jsVec2i vector)
{
	if(js_vec2i_equal(vector, (jsVec2i){0, 0}))
		return (jsTranslationResult){jsMoveStatusMute, vector, shape->offset};

	if(!__js_overlapp(board, shape, vector)) {
		jsVec2i new_position = js_vec2i_add(shape->offset, vector);

		if(vector.y < 0)
			return (jsTranslationResult){jsMoveStatusScore, vector, new_position};

		return (jsTranslationResult){jsMoveStatusSuccess, vector, new_position};
	}

	if(vector.y < 0)
		return (jsTranslationResult){jsMoveStatusMerge, vector, shape->offset};

	return (jsTranslationResult){jsMoveStatusFailure, vector, shape->offset};
}

///
jsShape js_translate_shape(const jsShape *shape, const jsTranslationResult *result)
{
	return (jsShape){shape->blocks, shape->index, result->new_position};
}

/// Returns the index range for the formation that the shape at index belongs
/// to.
static __jsIndexRange __js_index_range_for_shape_index(int index)
{
	int i;

	for(i = 0; i < JS_SHAPE_INDEX_RANGES_LENGTH; i++) {
		__jsIndexRange range = shape_index_ranges[i];

		if(index <= range.max)
			return shape_index_ranges[i];
	}

	// Invalid index.
	return (__jsIndexRange){-1, -1};
}

/// Returns next index given the rotation.
static int __js_rotate_shape_index(int index, jsRotation direction)
{
	__jsIndexRange range = __js_index_range_for_shape_index(index);
	int new_index = index + (int) direction;

	if(new_index > range.max)
		return range.min;

	if(new_index < range.min)
		return range.max;

	return new_index;
}

jsRotationResult js_rotate_result(const jsShape *shape, const jsBoard *board, jsRotation direction)
{
	int new_index, index = shape->index;
	jsShape new_shape;
	__jsBlockPositionStatus position_status;

	if(direction == jsRotationNone)
		return (jsRotationResult){jsMoveStatusMute, index, index};

	new_index = __js_rotate_shape_index(index, direction);

	new_shape.blocks = __js_blocks_at(new_index);
	new_shape.index = new_index;
	new_shape.offset = shape->offset;

	position_status = __js_overlapp(board, &new_shape, (jsVec2i){0, 0});

	if(position_status != jsBlockPositionStatusValid)
		return (jsRotationResult){jsMoveStatusFailure, index, index};

	return (jsRotationResult){jsMoveStatusSuccess, index, new_index};
}

jsShape js_rotate_shape(const jsShape *shape, const jsRotationResult *result)
{
	return (jsShape){
		__js_blocks_at(result->new_shape_index),
		result->new_shape_index,
		shape->offset
	};
}
