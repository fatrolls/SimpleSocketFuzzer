
#include "cMutable.h"
#include "cFuzzClass.h"
#include "Socket.h"

#include <string>
#include <iostream>

using namespace std ;

cFuzz::cFuzz() {
	_current_mutant = 0;
	_current_constant = 0;
	_total_packets_to_send = 0;
}

void cFuzz::GetNextPermutationIndex()
{
	for( unsigned int i = 0; i < _Mutables.size(); i++ ) {
		unsigned int s = _Mutables.at( i ).GetCurrentPermutationIndex() + 1;
		if( s == _Mutables.at( i ).GetNumberOfPermutations() )	{
			_Mutables.at( i ).Reset();
			continue;
		}
		else {
			_Mutables.at( i ).GetNextPermutation();
			break;
		}
	}
}

void cFuzz::Cleanup()
{
	_current_mutant = 0;
	_current_constant = 0;
	_total_packets_to_send = 0;

	_Mutables.clear();
	_Constants.clear();
	_is_a_Mutable.clear();
}

void cFuzz::Repeat( unsigned int nTimes )
{
	_total_packets_to_send += nTimes ;
}

void cFuzz::AddMutable( string Mutes, string Delim )
{
	size_t found=Mutes.find("HEX");
	if(found!=string::npos && found==0) {
		//convert to upper the whole string
		std::transform(Mutes.begin(), Mutes.end(), Mutes.begin(), ::toupper);

		Mutes=Mutes.substr(found+3);
		Mutes=_trim(Mutes, " ");
		if(Delim.size()>1 && Delim!=special_hex_delim)
			throw invalid_argument("Delim must be a single char");

		try { 
			vector<string> tokens;
			vector<string> results;
			//istringstream iss(Mutes);
			//while(getline(iss, token, (Delim.size()?(*Delim.c_str()):'\n') )) { //access to the first character of the Delim string, which must be single char
			//	results.push_back(token);
			//}
			_tokenize(tokens,Mutes, Delim);
			for(auto it=tokens.begin(); it!=tokens.end(); it++)
				results.push_back(*it); //must use *it instead of it->data() to preserve 00 bytes in hex strings

			Mutes.clear();
			
			for ( auto Hex : results ) //new c++ ranged loops
				Mutes+=_hex_to_string(Hex)+Delim;
			//erase last delim put by above foreach
			size_t found=Mutes.rfind(Delim);
			if(found!=string::npos)
				Mutes.erase(found);
		}
		catch ( exception &e ) {
			cerr << "Caught " << e.what( ) << endl;
			cerr << "Type " << typeid( e ).name( ) << endl;
		};

	}
	
	cMutable c( Mutes, Delim );

	_Mutables.push_back( c );
	_is_a_Mutable.push_back( true );
}

void cFuzz::AddConstant( string Constant )
{
	size_t found=Constant.find("HEX");
	if(found!=string::npos && found==0) {
		//convert to upper the whole string
		std::transform(Constant.begin(), Constant.end(), Constant.begin(), ::toupper);
		Constant=Constant.substr(found+3);
		Constant=_trim(Constant, " ");
		
		for ( auto Ch : Constant ) //new c++ ranged loops
			if ( !((Ch>='0' && Ch<='9') || (Ch>='A' && Ch<='F')) )
				throw invalid_argument("Constant must be an hex string without spaces and only valid hex char");
		Constant=_hex_to_string(Constant);
	}

	_Constants.push_back( Constant );
	_is_a_Mutable.push_back( false );
}

double cFuzz::GetNumberOfPermutations()
{
	double Result = 0;
	vector< double > Numbers;

	for( unsigned int i = 0; i < _Mutables.size(); i++ ) {
		Numbers.push_back( _Mutables.at( i ).GetNumberOfPermutations() );
	}

	Result = Numbers.at( 0 );

	for( unsigned int i = 1; i < Numbers.size(); i++ ) {
		Result *= Numbers.at( i );
	}

	return Result + _total_packets_to_send;
}

string cFuzz::GetNextPermutation()
{
	string Result = "";
	_current_mutant = 0;
	_current_constant = 0;

	for( unsigned int i = 0; i < _is_a_Mutable.size(); i++ )
	{
		if( _is_a_Mutable.at( i ) )
		{
			Result += _Mutables.at( _current_mutant++ ).GetCurrentPermutation();
		} 
		else
		{
			Result += _Constants.at( _current_constant++ );
		}
	}

	GetNextPermutationIndex();

	return Result;
}

