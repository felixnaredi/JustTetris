// 
// Filename: StaticVertexAttribute.cc
// Created: 2018-02-01 18:18:55 +0100
// Author: Felix Naredi
// 

#include <string>
#include <iostream>
#include <sstream>
#include <utility>

namespace playground
{
    std::string addedString(const std::string& str)
    {
	std::stringstream ss;
	ss << "addedString(" << str << ")";
	return ss.str();
    }

    template <typename T>
    struct fixed_string_wrapper
    {
	template <typename CharT>
	operator const std::basic_string<CharT>() const
	{ return std::basic_string<CharT>(T::value); }

	operator const char*() const { return (char *)T::value; }
    };

    struct fixed_string { constexpr static char value[] = "Fixed"; };
    typedef fixed_string_wrapper<fixed_string> FixedString;

}

using namespace playground;

int main(int argc, char *argv[])
{
    FixedString fs;
    std::cout << (const char*)fs << "\n";

    const char* ptr = fs;

    std::cout << ptr << "\n";
    std::cout << (const char*)fs << "\n";

    std::string s = fs;
    s[0] = 'f';

    std::cout << s << "\n";
    std::cout << fs << "\n";
    
    const auto str = std::string("es") + std::string("presso");
    std::cout << str << "\n";
    std::cout << addedString(str) << "\n";
    return 0;
}
