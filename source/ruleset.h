//
// Filename: ruleset.h
// Created: 2018-04-09 19:44:51 +0200
// Author: Felix Nared
//

#ifndef RULESET_H
#define RULESET_H


#include "tetris.h"

typedef struct
{
	int reset_duration;
	int duration;
} jsTimer;

typedef struct
{
	const char label[64];
	float (*score_for_clear)(jsResult);
	float (*score_for_translation)(jsResult);
	jsTimer (*increment_timer)(jsTimer);
	jsTimer (*timer_for_level)(float level);
	float (*level_increment_for_clear)(float level, jsResult);
	float (*level_score_multiplier)(float level);
} jsRuleset;

jsRuleset js_standard_ruleset(void);


#endif /* RULESET_H */
