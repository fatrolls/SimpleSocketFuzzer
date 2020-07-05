#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

#include <time.h>

#include "cMutable.h"

using namespace std;

cMutable::cMutable( string Mutes, string Delim = " " )
{
	vector<string> tokens;

    _tokenize(tokens,Mutes, Delim);
    for(auto it=tokens.begin(); it!=tokens.end(); it++)
		_mutables_vector.push_back(*it); //must use *it instead of it->data() to preserve 00 bytes in hex strings
	
	Reset();	
}

string cMutable::GetRandomString( unsigned int Size, bool isHex )
{
	string str;

	str.resize( Size );
	
	srand( static_cast<unsigned int>(time( NULL )) ^ rand() );

	for( unsigned int i = 0; i < Size; i++) {
		str[i] = rand() % 255;
		str[i] = str[i] >> 2;
		str[i] ^= ( rand() >> 3 );
		str[i] = str[i] << 3;
	}

	return str;
}

void cMutable::Reset()
{
	sort( _mutables_vector.begin(), _mutables_vector.end() );
	_out_string_vec = _mutables_vector;
	_current_permutation = 0;
}

string cMutable::GetNextPermutation() {
	if( ++_current_permutation == _mutables_vector.size()  )
		Reset();

	return GetCurrentPermutation();
}

string cMutable::GetCurrentPermutation() {	
	string ret=decode_special_codes( &_mutables_vector.at( _current_permutation ) );
	return ret;
}

string cMutable::decode_special_codes(string *token_read)  {

	//RND and RNDS are a special tags because its result changes at runtime (random) while all the others
	//are not modified once interpreted
	// Codes MUST be parsed from the longest to the shortest one, to avoid substring matches in other codes (es RND found into RNDH)..
	if( token_read->substr( 0, 4 ) == "RNDH" ) {
		unsigned int Size = atoi( token_read->substr( 4 ).c_str() );
		return GetRandomString( Size, true );
	}

	if( token_read->substr( 0, 3 ) == "RND" ) {
		unsigned int Size = atoi( token_read->substr( 3 ).c_str() );
		return GetRandomString( Size );
	}

	//format RPT10.A repeat 10 times the char A
	if( _mutables_vector.at( _current_permutation ).substr( 0, 3 ) == "RPT" ) {
		string out_sequence;
		unsigned int repetitions = atoi(token_read->substr(3, token_read->find_first_of(".")).c_str() );
		string sequence_str= token_read->erase (0, token_read->find_first_of(".")+1 );
		for(unsigned int i=0;i<repetitions;i++) {
			out_sequence += sequence_str;
		}
		*token_read=out_sequence;
	}

	//find escaped characters and convert to real characters
	//needed to handle the patterns like RPT10.\00\65
	size_t position=0;
	if(token_read->find("\\x")!=string::npos) {
		while((position=token_read->find("\\x"))!=string::npos)
			token_read->erase(position,2);	
		*token_read=_hex_to_string(*token_read);
	}
	
	return *token_read;
}

bool cMutable::GetNextPermutationSentence( string &s, string delim )
{
	if( next_permutation( _out_string_vec.begin(), _out_string_vec.end() ) )
	{
		for( unsigned int i = 0; i < _out_string_vec.size(); i++ )
		{
			s += _out_string_vec.at( i );

			if( delim.size() )
				s += delim;
		}
	return true;
	}
return false;
}

double cMutable::GetNumberOfPermutations()
{
	return _mutables_vector.size();
}

double cMutable::GetNumberOfPermuationSentences()
{
	double Result = 1;

	for( unsigned int i = 1; i < _out_string_vec.size(); i++)
	{
		Result *= i;
	}
return Result;
}

unsigned int cMutable::GetCurrentPermutationIndex()
{
	return _current_permutation;
}