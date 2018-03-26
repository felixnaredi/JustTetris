//
// Filename: EventTracker.cc
// Created: 2018-03-15 00:34:24 +0100
// Author: Felix Naredi
//
// BUILD:
//	clang++ EventTracker.cc -o EventTracker.o --std=c++14

#include <vector>
#include <iostream>

int main(int argc, char const *argv[])
{
	const std::vector<int> v = {1, 2, 3};

	std::copy(begin(v), end(v) - 1, std::ostream_iterator<int>(std::cout, ", "));
	std::cout << v.back() << '\n';

	return 0;
}
