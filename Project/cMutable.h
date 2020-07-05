
#ifndef C_MUTABLE_H
#define C_MUTABLE_H

#include <vector>
#include <string>

#include "support_fcn.h"

using namespace std;

class cMutable{

	vector< string > _mutables_vector; //vector of mutables.
	vector< string > _out_string_vec; //hold the list of strings ready to go out

	unsigned int _current_permutation;

	string GetRandomString( unsigned int Size, bool isHex=false );
	
	string decode_special_codes(string *token_read) ;

public:
	cMutable( string Mutes, string Delim );
	void Reset();

	string GetNextPermutation();
	string GetCurrentPermutation();

	bool GetNextPermutationSentence( string &s, string delim );

	double GetNumberOfPermutations();
	double GetNumberOfPermuationSentences();
	unsigned int GetCurrentPermutationIndex();
};

#endif