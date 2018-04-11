//
// Filename: ruleset.h
// Created: 2018-04-09 19:44:51 +0200
// Author: Felix Nared
//

#ifndef RULESET_H
#define RULESET_H

#include <stddef.h>
#include "tetris.h"

typedef struct
{
	size_t time;
	size_t force_down_time;
	int force_down_duration;
	bool force_down_did_trigger;
} jsTimer;

typedef struct
{
	const char label[64];
	float (*score_for_clear)(jsResult);
	float (*score_for_translation)(jsResult);
	jsTimer (*increment_timer)(jsTimer);
  jsTimer (*timer_for_result)(jsTimer, jsResult);
	int (*timer_force_down_for_level)(float level);
	float (*level_increment_for_clear)(float level, jsResult);
	float (*level_score_multiplier)(float level);
} jsRuleset;

jsRuleset js_standard_ruleset(void);


#endif /* RULESET_H */
