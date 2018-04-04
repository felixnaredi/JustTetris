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

	return (jsShape){
		.blocks = data->blocks,
		.index = index,
		.offset = data->offset
	};
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

/// Sets the position of the blocks in row 'offset' amount in y dimension.
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
	return pos.x < 0 || pos.y < 0 ||
		pos.x >= JS_BOARD_COLUMN_AMOUNT || pos.y >= JS_BOARD_ROW_AMOUNT;
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

	return __js_merge(board,
	                  &(jsShape){
	                  	.blocks = shape->blocks,
	                  	.index = shape->index,
	                  	.offset = js_vec2i_add(offset, shape->offset)
	                  });
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
float js_clear_rows_score(const jsClearRowsResult *result)
{
  switch (result->count) {
    case 1: return 1.0;
    case 2: return 3.0;
    case 3: return 6.0;
    case 4: return 10.0;
    default: return 0;
  }
}

/// Returns the result of the translation.
jsTranslationResult
js_translate_result(const jsShape *shape, const jsBoard *board, jsVec2i vector)
{
	if(js_vec2i_equal(vector, (jsVec2i){0, 0}))
		return (jsTranslationResult){
			.status = jsMoveStatusMute,
			.offset = vector,
			.new_position = shape->offset
		};

	if(!__js_overlapp(board, shape, vector))
		return (jsTranslationResult){
			.status = jsMoveStatusSuccess,
		     	.offset = vector,
		    	.new_position = js_vec2i_add(shape->offset, vector)
		};

	// Game over accures when a shape overlapps with its original offset.
	if(js_vec2i_equal(shape->offset, shape_data[shape->index].offset))
		return (jsTranslationResult){
			.status = jsMoveStatusGameOver,
			.offset = vector,
			.new_position = shape->offset
		};

	return (jsTranslationResult){
		.status = vector.y < 0 ? jsMoveStatusMerge : jsMoveStatusFailure,
		.offset = vector,
		.new_position = shape->offset
	};
}

float js_translate_score(const jsTranslationResult *result)
{
  return (float)(result->offset.y * -1) * (1.0/8.0);
}

/// Returns the translated shape.
jsShape js_translate_shape(const jsShape *shape, const jsTranslationResult *result)
{
	if(result->status == jsMoveStatusSuccess)
		return (jsShape){
			.blocks = shape->blocks,
			.index = shape->index,
			.offset = result->new_position
		};
	return *shape;
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

/// Returns the result of the translation.
jsRotationResult
js_rotate_result(const jsShape *shape, const jsBoard *board, jsRotation direction)
{
	int index = shape->index;
	int new_index = __js_rotate_shape_index(index, direction);

	if(direction == jsRotationNone)
		return (jsRotationResult){
			.status = jsMoveStatusMute,
			.old_shape_index = index,
			.new_shape_index = index
		};

	switch(__js_overlapp(board,
	                     &(jsShape){
	                     	.blocks = __js_blocks_at(new_index),
	                     	.index = new_index,
	                     	.offset = shape->offset
	                     },
	                     (jsVec2i){0, 0}))
	{
	case jsBlockPositionStatusValid:
		return (jsRotationResult){
			.status = jsMoveStatusSuccess,
			.old_shape_index = index,
			.new_shape_index = new_index
		};

	default:
		return (jsRotationResult){
			.status = jsMoveStatusFailure,
			.old_shape_index = index,
			.new_shape_index = index
		};
	}
}

/// Returns the rotated shape
jsShape js_rotate_shape(const jsShape *shape, const jsRotationResult *result)
{
	if(result->status == jsMoveStatusSuccess)
		return (jsShape){
			.blocks = __js_blocks_at(result->new_shape_index),
			.index = result->new_shape_index,
			.offset = shape->offset
		};

	return *shape;
}
