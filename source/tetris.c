//
// Filename: tetris.c
// Created: 2018-02-13 20:57:53 +0100
// Author: Felix Nared
//


#include <stdbool.h>
#include <stdlib.h>

#include "debug.h"
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
	{ \
		{id | JS_BLOCK_FILLED, x0, y0}, {id | JS_BLOCK_FILLED, x1, y1}, \
	  	{id | JS_BLOCK_FILLED, x2, y2}, {id | JS_BLOCK_FILLED, x3, y3} \
	}

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

#define JS_SHAPE_INDEX_RANGES_LENGTH					\
	(sizeof(shape_index_ranges) / sizeof(shape_index_ranges[0]))

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

jsShape js_result_old_shape(jsResult result)
{
	return (jsShape){
		.blocks = shape_data[result.rotation.old_shape_index].blocks,

		.offset = result.successfull ?
			js_vec2i_subtract(
				result.translation.new_position,
				result.translation.offset
			) :
			result.translation.new_position,

		.index = result.rotation.old_shape_index,
	};
}

jsShape js_result_new_shape(jsResult result)
{
	return (jsShape){
		.blocks = shape_data[result.rotation.new_shape_index].blocks,
		.offset = result.translation.new_position,
		.index = result.rotation.new_shape_index,
	};
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

void js_clear_rows(jsBoard *board, const int *indicies, int count)
{
	int i;
	jsRow *rows = board->rows;

	if(count == 0)
		return;

	for(i = count - 1; i > -1; i--) {
		int j;

		for(j = indicies[i]; j < JS_BOARD_ROW_AMOUNT - 1; j++) {
			int x;

			rows[j] = rows[j + 1];
			for(x = 0; x < JS_BOARD_COLUMN_AMOUNT; x++)
				rows[j].blocks[x].position.y -= 1;
		}
	}

	rows[JS_BOARD_ROW_AMOUNT - 1] = __js_empty_row();
}

typedef struct
{
	int count;
	int indicies[JS_ROW_CLEAR_MAX];
} __jsClearResult;

__jsClearResult __js_clear_result(const jsBoard *board)
{
	int i, count = 0;
	int indicies[JS_ROW_CLEAR_MAX] = {-1, -1, -1, -1};

	for(i = 0; i < JS_BOARD_ROW_AMOUNT; i++) {
		if(__js_row_is_full(&board->rows[i])) {
			indicies[count] = i;
			count++;
		}
	}

	return (__jsClearResult){
		.count = count,
			.indicies = {indicies[0], indicies[1], indicies[2], indicies[3]},
        };
}

jsResult __js_translation_result(const jsShape *shape,
                                 const jsBoard *board,
                                 jsVec2i offset,
                                 bool user_action)
{
	if(js_vec2i_equal((jsVec2i){0, 0}, offset))
		return (jsResult){
			.mute_action = true,
				.user_action = user_action,
				.game_over = false,
				.successfull = true,
				.did_merge = false,
				.translation = {
				.offset = offset,
				.new_position = shape->offset,
			},
				.rotation = {
				.old_shape_index = shape->index,
				.new_shape_index = shape->index,
			},
				.merge = {
				.rows_cleared = 0,
				.indicies = {-1, -1, -1, -1},
			},
	        };

	switch (__js_overlapp(board, shape, offset)) {
	case jsBlockPositionStatusValid:
		return (jsResult){
			.mute_action = false,
				.user_action = user_action,
				.game_over = false,
				.successfull = true,
				.did_merge = false,
				.translation = {
				.offset = offset,
				.new_position = js_vec2i_add(shape->offset, offset),
			},
				.rotation = {
				.old_shape_index = shape->index,
				.new_shape_index = shape->index,
			},
				.merge = {
				.rows_cleared = 0,
				.indicies = {-1, -1, -1, -1},
			},
	        };
	case jsBlockPositionStatusOutside:
	case jsBlockPositionStatusOverlapp:
		return (jsResult){
			.mute_action = false,
				.user_action = user_action,
				.game_over = js_vec2i_equal(
					shape->offset,
					shape_data[shape->index].offset
					),
				.successfull = false,
				.did_merge = offset.y < 0,
				.translation = {
				.offset = offset,
				.new_position = shape->offset,
			},
				.rotation = {
				.old_shape_index = shape->index,
				.new_shape_index = shape->index,
			},
				.merge = {
				.rows_cleared = 0,
				.indicies = {-1, -1, -1, -1},
			},

	        };
	}
}

jsResult js_translate_shape(jsShape *shape, jsBoard *board, jsVec2i offset, bool user_action)
{
	__jsClearResult clear_result;
        int i;
      	jsResult result = __js_translation_result(shape, board, offset, user_action);

      	if(result.successfull) {
      		shape->offset = result.translation.new_position;
      		return result;
      	}

      	if(!result.did_merge)
      		return result;

      	__js_merge(board, shape);
      	clear_result = __js_clear_result(board);

        result.merge.rows_cleared = clear_result.count;
        for(i = 0; i < clear_result.count; i++)
		result.merge.indicies[i] = clear_result.indicies[i];

      	return result;
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
static int __js_rotate_shape_index(int index, jsRotate direction)
{
	__jsIndexRange range = __js_index_range_for_shape_index(index);
	int new_index = index + (int) direction;

	if(new_index > range.max)
		return range.min;

	if(new_index < range.min)
		return range.max;

	return new_index;
}

static jsResult __js_rotation_result(const jsShape *shape,
                                     const jsBoard *board,
                                     jsRotate direction,
                                     bool user_action)
{
	int index = __js_rotate_shape_index(shape->index, direction);
	jsShape new_shape = {
		.blocks = shape_data[index].blocks,
		.index = index,
		.offset = shape->offset,
	};

	switch(__js_overlapp(board, &new_shape, (jsVec2i){0, 0})) {
	case jsBlockPositionStatusValid:
		return (jsResult){
			.mute_action = false,
				.user_action = user_action,
				.game_over = false,
				.successfull = true,
				.did_merge = false,
				.translation = {
				.offset = (jsVec2i){0, 0},
				.new_position = new_shape.offset,
			},
				.rotation = {
				.old_shape_index = shape->index,
				.new_shape_index = new_shape.index,
			},
				.merge = {
				.rows_cleared = 0,
				.indicies = {-1, -1, -1, -1},
			},
		};
	default:
		return (jsResult){
			.mute_action = false,
				.user_action = user_action,
				.game_over = false,
				.successfull = false,
				.did_merge = false,
				.translation = {
				.offset = (jsVec2i){0, 0},
				.new_position = shape->offset,
			},
				.rotation = {
				.old_shape_index = shape->index,
				.new_shape_index = shape->index,
			},
				.merge = {
				.rows_cleared = 0,
				.indicies = {-1, -1, -1, -1},
			},
	        };
	}
}

jsResult js_rotate_shape(jsShape *shape,
                         jsBoard *board,
                         jsRotate direction,
                         bool user_action)
{
	int index;
	jsResult result = __js_rotation_result(shape, board, direction, user_action);

	index = result.rotation.new_shape_index;
	shape->index = index;
	shape->blocks = shape_data[index].blocks;

	return result;
}
