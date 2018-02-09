// 
// Filename: tetris.c
// Created: 2018-02-09 21:23:44 +0100
// Author: Felix Nared
//

static int current_id = 0;

int js_genid()
{
	current_id++;
	return current_id;
}

int js_block_is_empty(jsBlock block)
{
	return !(block.color & JS_COLORMAP_FILLED);
}

	
