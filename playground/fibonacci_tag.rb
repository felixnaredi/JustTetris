#!/usr/bin/env ruby
#
# Filename: fibonacci_tag.rb
# Created: 2018-03-28 18:38:44 +0200
# Author: Felix Nared
#
# --------------------------------------------------------------
# A fun little program showcasing functional style programing
# in ruby.

def fib(n)
	return 1 unless n > 1
	(fib (n - 2)) + (fib (n - 1))
end

def fprod(x)
	(0..x).map { |n| fib n }.inject(1) { |s, e| s * e }
end

def logo(n, s = 'Fibonacci')
	length = (fprod n).to_s.length
	ratio = (fib (n - 1)) / (fib (n - 2)).to_f
	"#{(Array.new (length * ratio).to_i - length, ' ').join}#{s}"
end

def tag(size)
	(0...(fib size)).map { |n| "// #{fprod n}#{(n + 1) == (fib (size - 1)) ? (logo n) : ''}" }.join "\n"
end

(4..8).each { |n| puts tag n }
