#!/usr/bin/env ruby
# 
# Filename: emacs_ac_break.rb
# Created: 2018-02-13 22:37:24 +0100
# Author: Felix Nared
# 

DEF_TAG = 'EMACS_AC_BREAK_H'

open('../emacs_ac_break.h', 'w') { |file|
  file.write "\#ifndef #{DEF_TAG}\n\#define #{DEF_TAG}\n\n"
  ('a'..'z').each { |c| file.write "\#define #{c}Break\n" }
  ('a'..'z').each { |c| file.write "\#define #{c}_Break\n" }
  ('A'..'Z').each { |c| file.write "#define #{c}BREAK\n" }
  ('A'..'Z').each { |c| file.write "#define #{c}_BREAK\n" }
  file.write "\n\n\#endif /* #{DEF_TAG} */\n"
}
