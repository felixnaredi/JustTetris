#!/usr/bin/env ruby
#
# Filename: score.rb
# Created: 2018-03-13 17:29:37
# Author: Felix Nared
#
# Find a neat and fair score generating algorithm.
# -------------------------------------------------------------

def stride(b, e, step, &block)
	return unless b < e
	yield b
	stride(b + step, e, step, &block)
end

def linear_score(rows, mul)
	rows * mul
end

def flat_adder_score(rows, add)
	return 1 unless rows > 1
	(rows * flat_adder_score(rows - 1, add)) / (rows - 1) + add
end

def linear_adder_score(rows, add)
	return 1 unless rows > 1
	(rows * linear_adder_score(rows - 1, add)) / (rows - 1) + add * rows
end
