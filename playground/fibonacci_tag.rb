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
	"#{(Array.new (fprod n).to_s(16).length, ' ').join}#{s}"
end

def tag(size)
	(0...(fib size)).map { |n|
		"// #{(fprod n).to_s 16}#{n == (fib (size - 1)) ? (logo (n - 1)) : ''}"
	}.join "\n"
end

(4..8).each { |n| puts tag n }
