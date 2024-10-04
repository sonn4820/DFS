#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string>	Strings; 

Strings SplitStringOnDelimiter( std::string const& originalString, char delimiterToSplitOn = ',', bool removeEmpty = false);
int SplitStringOnDelimiter(Strings& out_splitStrings, std::string const& originalString, std::string const& delimiterToSplitOn, bool removeEmpty = false);
Strings SplitStringOnDelimiter(std::string const& originalString, std::string const& delimiterToSplitOn, bool removeEmpty = false);

//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... );
const std::string Stringf( int maxLength, char const* format, ... );