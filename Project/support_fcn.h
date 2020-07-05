#include "stdafx.h"

#include <vector>


std::string _hex_to_string(const std::string& input);

//std::string string_to_hex(const std::string& input);
std::string _string_to_hex(const std::string& value);

void _sleep(long delay_ms);

std::string _trim_all(std::string& s);

std::string _trim(std::string& s,const std::string& drop);

void _tokenize ( std::vector<std::string> & theStringVector,  const  std::string  & theString, const  std::string  & theDelimiter);
