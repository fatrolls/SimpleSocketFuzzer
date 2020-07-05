#ifndef C_FUZZ_CLASS_H
#define C_FUZZ_CLASS_H

#include <vector>
#include <string>
#include <algorithm>


#include "stdafx.h"
#include "cMutable.h"
#include "Socket.h" 

using namespace std;

//A special delimiter must be used with the Hex strings, because I use getline to separate tokens thus if the delim is part of the
//hex string actually the real hex string is truncated before its real end.
const string special_hex_delim="<HEX_DELIM>";

class cFuzz{

	vector< cMutable > _Mutables;
	vector< string > _Constants;

	vector< bool > _is_a_Mutable; //if true is a mutable, if false is a constant

	unsigned int _current_mutant; //counter used to keep trakc of the permutation done
	unsigned int _current_constant; //counter used to keep track of the constant done
	unsigned int _total_packets_to_send; //number of packets to send, the minimum is the number of permutation, plus eventually the number of repeats asked 

	void GetNextPermutationIndex();
	
public:
	cFuzz();

	void Cleanup();
	void Repeat( unsigned int nTimes );

	void AddMutable( string Mutes, string Delim );
	void AddConstant( string Constant );

	double GetNumberOfPermutations();
	string GetNextPermutation();

};

#endif