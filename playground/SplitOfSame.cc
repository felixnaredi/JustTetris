// 
// Filename: SplitOfSame.cc
// Created: 2018-01-30 22:24:52 +0100
// Author: Felix Naredi
// 

#include <string>
#include <iostream>
#include <sstream>

#define STRONG_TYPEDEF(type, name)			\
    class name						\
    {							\
	type value;					\
      public:						\
	name() : value(static_cast<type>(0)) { };	\
	name(type v) : value(v) { }			\
	operator type&() { return value; }		\
	operator type() const { return value; }		\
    };							\
  

STRONG_TYPEDEF(int, A);

std::string f(A a)
{
    return std::to_string(a) + ": of class A";
}

STRONG_TYPEDEF(int, B);

std::string f(B b)
{
    return std::to_string(b) + ": of class B";
}


int main(int argc, char *argv[])
{
    A a = 1;
    std::cout << f(a) << "\n";

    B b = 2;
    std::cout << f(b) << "\n";
    
    return 0;
}










