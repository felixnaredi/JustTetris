
//
// Filename: tetris.c
// Created: 2018-02-13 20:57:53 +0100
// Author: Felix Nared
//


#include <stdbool.h>
#include <stdlib.h>

#include "tetris.h"
#include "build.h"

#include "emacs_ac_break.h"


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
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_O, 1, 2, 2, 2, 1, 3, 2, 3) },
	{ {4, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_I, 1, 0, 1, 1, 1, 2, 1, 3) },
	{ {3, 17}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_I, 0, 2, 1, 2, 2, 2, 3, 2) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_S, 2, 1, 1, 2, 2, 2, 1, 3) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_S, 1, 2, 2, 2, 2, 3, 3, 3) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_Z, 1, 1, 1, 2, 2, 2, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_Z, 2, 2, 3, 2, 1, 3, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_L, 1, 1, 2, 1, 1, 2, 1, 3) },
	{ {4, 17}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_L, 0, 1, 0, 2, 1, 2, 2, 2) },
	{ {4, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_L, 1, 1, 1, 2, 0, 3, 1, 3) },
	{ {4, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_L, 0, 2, 1, 2, 2, 2, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_J, 1, 1, 2, 1, 2, 2, 2, 3) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_J, 1, 2, 2, 2, 3, 2, 1, 3) },
	{ {2, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_J, 2, 1, 2, 2, 2, 3, 3, 3) },
	{ {3, 17}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_J, 3, 1, 1, 2, 2, 2, 3, 2) },
	{ {3, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_T, 1, 1, 1, 2, 2, 2, 1, 3) },
	{ {4, 17}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_T, 1, 1, 0, 2, 1, 2, 2, 2) },
	{ {4, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_T, 1, 1, 0, 2, 1, 2, 1, 3) },
	{ {4, 16}, JS_SHAPE_DATA(JS_SHAPE_FORMATION_T, 0, 2, 1, 2, 2, 2, 1, 3) },
};

typedef struct
{
	int min;
	int max;
} __jsRange;

static const __jsRange rot_ranges[] = {
	{0, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 10}, {11, 14}, {15, 18},
};

#define JS_SHAPE_FORMATION_AMOUNT (sizeof(rot_ranges) / sizeof(rot_ranges[0]))

static int __js_timer(int level)
{
	return 120 / level;
}

/// Resets the countdown timer.
static void __js_reset_countdown(jsTetrisState *state)
{
	state->countdown = state->timer;
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

/// Makes a shape from the given index, corresponding to the data in
/// 'shape_data'.
///
/// Returns the shape for the index.
static jsShape __js_make_shape(int index)
{
	const __jsShapeData *shapeData = &shape_data[index];

	return (jsShape) {shapeData->offset, index, shapeData->blocks};
}

/// Generates a random index of a shape pointing to the first shape of a form.
///
/// Returns a index to be used in 'shape_data'.
static int __js_gen_shape_index()
{
	return rot_ranges[rand() % JS_SHAPE_FORMATION_AMOUNT].min;
}

/// Allocates the size of type to var. If allocation fails it jumps to
/// the label err. Therefor must functions using JS_ALLOC provide an
/// err label.
#define JS_ALLOC(var, type, err) \
	var = malloc(sizeof(type)); \
	if(var == NULL) goto err

/// Frees var as long as it isn't NULL.
#define JS_DEALLOC(var) if(var != NULL) free(var)

/// Allocates a jsTetrisState.
///
/// Returns pointer to the allocated structure or NULL if failure.
jsTetrisState *js_alloc_tetris_state()
{
	jsTetrisState *state;

	JS_ALLOC(state, jsTetrisState, err);
	JS_ALLOC(state->board, jsBoard, err);
	JS_ALLOC(state->shape, jsShape, err);
	JS_ALLOC(state->next_shape, jsShape, err);

	return state;
err:
	JS_DEBUG_PUTS(js_alloc_tetris_state, "failed alloc");
	js_dealloc_tetris_state(state);

	return NULL;
}

/// Deallocates a jsTetrisState.
void js_dealloc_tetris_state(jsTetrisState *state)
{
	JS_DEALLOC(state->next_shape);
	JS_DEALLOC(state->shape);
	JS_DEALLOC(state->board);
	JS_DEALLOC(state);
}

/// Initailizes a jsTetrisState as it is at a new game.
void js_init_tetris_state(jsTetrisState *state)
{
	state->status = 0;
	*state->board = __js_empty_board();
	*state->shape = __js_make_shape(__js_gen_shape_index());
	*state->next_shape = __js_make_shape(__js_gen_shape_index());
	state->rows = 0;
	state->level = 1;
	state->score = 0;
	state->timer = __js_timer(1);
	__js_reset_countdown(state);
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

/// Returns true if at least one of the blocks in shape are at the
/// same position as a non empty block in board or if at least one
/// block in shape are outside the bounds of board.
static bool
__js_overlapp(const jsBoard *board, const jsShape *shape, jsVec2i offset)
{
	int i;
	const jsBlock *blocks = shape->blocks;

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsVec2i pos = js_vec2i_add(
			js_vec2i_add(blocks[i].position, shape->offset), offset);

		if(__js_outside_board(pos))
			return true;

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

	for(i = 0; i < JS_BOARD_ROW_AMOUNT;) {
		int j;

		if(!__js_row_is_full(&board->rows[i])) {
			i++;
			continue;
		}

		for(j = i; j < JS_BOARD_ROW_AMOUNT - 1; j++)
			rows[j] = rows[j + 1];

		rows[JS_BOARD_ROW_AMOUNT - 1] = __js_empty_row();
		count++;
	}

	return count;
}

/// Adds each block from shape, that is inside board, to board.
//
/// Returns the amount of blocks that are outside board.
static int __js_merge(jsBoard *board, const jsShape *shape)
{
	int i, count = 0;
	const jsBlock *blocks = shape->blocks;

	for(i = 0; i < JS_SHAPE_BLOCK_AMOUNT; i++) {
		jsBlock block = blocks[i];
		jsVec2i pos = js_vec2i_add(block.position, shape->offset);

		if(pos.y >= JS_BOARD_ROW_AMOUNT || pos.x >= JS_BOARD_COLUMN_AMOUNT) {
			count++;
			continue;
		}

		board->pos[pos.y][pos.x] = block;
	}

	return count;
}

/// Set score to less than 0 if a successfull move shouldn't give any score.
///
/// Returns the status affected by the move.
static int __js_move(jsTetrisState *state, jsVec2i offset, bool freeze, double score)
{
	jsBoard *board = state->board;
	jsShape *shape = state->shape, *next_shape = state->next_shape;
	int rows, new_rows, old_level, new_level, status = 0;
	bool overlapp = __js_overlapp(board, shape, offset);

	if(!overlapp) {
		__js_reset_countdown(state);
		shape->offset = js_vec2i_add(shape->offset, offset);

		status |= JS_STATE_SHAPE_CHANGE | JS_STATE_RESET_COUNTDOWN;

		if(score < 0)
			return status;

		// state->score += score * __js_level_multiplier(level);
		status |= JS_STATE_SCORE_CHANGE;

		return status;
	}

	if(!freeze)
		return 0;

	__js_merge(board, shape);

	*shape = *next_shape;
	*next_shape = __js_make_shape(__js_gen_shape_index());

	status |= JS_STATE_SHAPE_CHANGE | JS_STATE_BOARD_CHANGE | JS_STATE_NEXT_SHAPE_CHANGE;

	rows = __js_clear_rows(board);

	// if(__js_game_over(board, shape))
	// 	status |= JS_STATE_GAME_OVER;

	if(rows == 0)
		return status;

	new_rows = state->rows + rows;

	old_level = state->level;
	// new_level = __js_get_level(new_rows);

	state->rows = new_rows;
	state->level = new_level;
	// state->score += __js_score_rows(rows, new_level);

	status |= JS_STATE_ROWS_CHANGE | JS_STATE_SCORE_CHANGE;
	status |= new_level == old_level ? 0 : JS_STATE_LEVEL_CHANGE;

	return status;
}

#ifdef JS_DEBUG

/// Returns true if one or more non nullable pointers of state is
/// NULL.
static bool __js_debug_nonnull_state(const jsTetrisState *state)
{
	JS_DEBUG_NULLPTR(__js_debug_nonnull_state, state);
	JS_DEBUG_NULLPTR(__js_debug_nonnull_state, state->board);
	JS_DEBUG_NULLPTR(__js_debug_nonnull_state, state->shape);
	JS_DEBUG_NULLPTR(__js_debug_nonnull_state, state->next_shape);

	return state == NULL || state->board == NULL || state->shape == NULL ||
				 state->next_shape == NULL;
}

#endif /* JS_DEBUG */

/// Moves the shape on the board of state. If the offsets y value is
/// negative the move will generate score on success and 'freeze' the
/// shape on overlap.
void js_move_shape(jsTetrisState *state, jsVec2i offset)
{
#ifdef JS_DEBUG

	if(__js_debug_nonnull_state(state)) {
		JS_DEBUG_PUTS(js_move_shape, "state may not include NULL");
		return;
	}
#endif /* JS_DEBUG */

	bool freeze = false;
	double score = -1.0;

	if(offset.y < 0) {
		freeze = true;
		score = 0.3;
	}

	state->status |= __js_move(state, offset, freeze, score);
}

/// Moves the shape on the board of state with force. A forced move is
/// never worth any score and if it overlaps it will always 'freeze'
/// the shape.
void js_force_shape(jsTetrisState *state, jsVec2i offset)
{
#ifdef JS_DEBUG

	if(__js_debug_nonnull_state(state)) {
		JS_DEBUG_PUTS(js_force_shape, "state may not include NULL");
		return;
	}
#endif /* JS_DEBUG */

	state->status |= __js_move(state, offset, true, -1);
}

/// Finds the form of the current index. Then checks if the incremented index is
/// inside the bounds of the form, loops the index if so.
///
/// Returns next index given the rotation.
static int __js_incr_shape_index(int index, jsRotation rot)
{
	__jsRange form;
	int newIndex = index + (int) rot, i = -1;

	do {
		i++;
		form = rot_ranges[i];
	} while(index < form.min || index > form.max);

	if(newIndex > form.max)
		return form.min;

	if(newIndex < form.min)
		return form.max;

	return newIndex;
}

/// Rotates the shape on the board of the state. Sets state status on success.
void js_rotate_shape(jsTetrisState *state, jsRotation rot)
{
	#ifdef JS_DEBUG

		if(__js_debug_nonnull_state(state)) {
			JS_DEBUG_PUTS(js_rotate_shape, "state may not include NULL");
			return;
		}
	#endif /* JS_DEBUG */

	jsShape *shape = state->shape;
	const jsBlock *oldBlocks = shape->blocks;
	int result, newIndex = __js_incr_shape_index(shape->index, rot);

	shape->blocks = __js_make_shape(newIndex).blocks;

	result = __js_move(state, (jsVec2i) {0, 0}, false, -1);

	if(!result) {
		shape->blocks = oldBlocks;
		return;
	}

	state->status |= result;
	shape->index = newIndex;
}
