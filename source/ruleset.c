//
// Filename: ruleset.h
// Created: 2018-04-09 19:45:39 +0200
// Author: Felix Nared
//

#include "ruleset.h"
#include "debug.h"


static float __js_standard_score_for_clear(jsResult result)
{
  switch (result.merge.rows_cleared) {
    case 1: return 1.0;
    case 2: return 3.0;
    case 3: return 6.0;
    case 4: return 10.0;
    default: return 0;
  }
}

static float __js_standard_score_for_translation(jsResult result)
{
	return result.translation.offset.y < 0 ?
		(float)-result.translation.offset.y * 0.1 :
		0.0;
}

static float
__js_standard_level_increment_for_clear(float level, jsResult result)
{
	switch (result.merge.rows_cleared) {
	case 1: return 0.10;
	case 2: return 0.30;
	case 3: return 0.60;
	case 4: return 1.00;
	default: return 0;
	}
}

static float __js_standard_level_score_multiplier(float level)
{
	return (float)((int)level + 1) / 16.0;
}

jsRuleset js_standard_ruleset()
{
	return (jsRuleset){
		.label = "Standard",
		.score_for_clear = __js_standard_score_for_clear,
		.score_for_translation = __js_standard_score_for_translation,
		.level_increment_for_clear = __js_standard_level_increment_for_clear,
		.level_score_multiplier = __js_standard_level_score_multiplier,
	};
}
