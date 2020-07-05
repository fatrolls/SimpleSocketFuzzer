#include "support_fcn.h"
#include <ios>
#include <iomanip>


void _tokenize ( std::vector<std::string> & theStringVector,  const  std::string  & theString, const  std::string  & theDelimiter)
{
	if ( theDelimiter.size()==0 )
		throw std::invalid_argument("tokenize: invalid delimiter string\n");

	size_t  start = 0, end = 0;

	while ( end != std::string::npos)
	{
		end = theString.find( theDelimiter, start);

		//handle the situation when the delimiter was the last one
		//it continue in this case AAA<delim>   (delimiters was last)
		//it go on in this case AAA<delim>BBB  
		if( (end==std::string::npos && start==theString.size()) ||
			(end==std::string::npos && start==0) )
			continue;

		// If at end, use length=maxLength.  Else use length=end-start.
		theStringVector.push_back( theString.substr( start,
			(end == std::string::npos) ? std::string::npos : end - start));

		// If at end, use start=maxSize.  Else use start=end+delimiter.
		start = (   ( end > (std::string::npos - theDelimiter.size()) )
			?  std::string::npos  :  end + theDelimiter.size());
	}
	//limit situation no delimiters found then the vector is filled with a single element which is a copy
	//of the input string (a more tolerant solution for the function callers).
	if(theStringVector.size()==0)
		theStringVector.push_back(theString);
}

std::string _trim(std::string& s,const std::string& drop) {
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}

std::string _trim_all(std::string& s) {
	//erase the mixture of tabs and spaces which usually is between the command and the comment, to really trim the string
	char last;
	std::string r;
	do { 
		r=_trim(s, " "); //spaces
		s=_trim(r, "\t"); //tabs
		if(r.size()>=1)
			last=r.at(r.size()-1);
		else
			break; //handle the case where the string is only made of tabs and spaces and nothing is left after trimming it
	}
	while(last==' ' || last=='\t');
	return r;
}

//convert hex string into text representation
std::string _string_to_hex(const std::string& input) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const char c = input[i];
		//output.push_back(lut[c >> 4]);
		output.push_back(lut[(c & 240)>>4]); //second byte 
		output.push_back(lut[c & 15]); //first byte
	}
	return output;
}

//convert string hex textual into bytes
std::string _hex_to_string(const std::string& input) {
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	if (len & 1) throw std::invalid_argument("odd length");

	std::string output;
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2)
	{
		char a = ::toupper(input[i]);
		if(!isxdigit(a))
			throw std::invalid_argument("not a hex digit (1)");
		
		const char* p = std::lower_bound(lut, lut + 16, a);
		if (*p != a) throw std::invalid_argument("not a hex digit (2)");

		char b = input[i + 1];
		const char* q = std::lower_bound(lut, lut + 16, b);
		if (*q != b) throw std::invalid_argument("not a hex digit (3)");

		output.push_back(((p - lut) << 4) | (q - lut));
	}
	return output;
}

//my own c++ compatible sleep function
void _sleep(long delay_ms) {
	clock_t delay=delay_ms/1000*CLOCKS_PER_SEC;
	clock_t start=clock();
	while (clock() - start < delay) ;
}
