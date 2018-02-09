// 
// Filename: SplitOfDifferent.cc
// Created: 2018-01-30 22:43:50 +0100
// Author: Felix Naredi
// 

#include <string>
#include <iostream>


struct A
{
    int value;
    A(int v)
    {
	value = v;
    }
	    
};

struct B
{
    int value;
    B(int v)
    {
	value = v;
    }
};

std::string f(A a)
{
    return std::to_string(a.value) + ": of class A";
}

std::string f(B b)
{
    return std::to_string(b.value) + ": of class B";
}

int main(int argc, char *argv[])
{
    std::cout << f(A{1}) << "\n";
    std::cout << f(B{2}) << "\n";
    
    return 0;
}
