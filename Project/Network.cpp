#include <vector>
#include <string>
#include <memory>

#include "cMutable.h"
#include "cFuzzClass.h"
#include "Socket.h"
#include "Network.h"

extern enum _verbose_mode;

void DoRound( cFuzz &c, shared_ptr<mySocket> s, long delay_ms, _verbose_mode verbose );

void Init()
{
	StartWSA();
}

void Cleanup()
{
	StopWSA();
}

bool IsTargetStillALive( char* IpAddress, unsigned int port )
{
	try
	{
		myTcpSocket s( port );
		s.connectToServer( (string)IpAddress, ADDRESS );
		s.~myTcpSocket();
		return true;
	} 
	catch( ... )
	{
		return false;
	}
}

bool IsTargetStillALive( string Address, unsigned int port )
{
	try
	{
		myTcpSocket s( port );
		s.connectToServer( Address, NAME );
		s.~myTcpSocket();
		return true;
	} 
	catch( ... )
	{
		return false;
	}
}

int DoRound( cFuzz &c, string Address, int Port, int socksteam, long delay_ms, _verbose_mode verbose )
{
	if(socksteam==SOCK_STREAM) {
		auto s= make_shared<myTcpSocket>( Port );
		s->set_verbose(verbose);
		s->connectToServer( Address, NAME );
		DoRound( c, s, delay_ms, verbose );
	}
	else { //UDP
 		auto s=make_shared<myUdpSocket>( Port );
 		s->set_verbose(verbose);
		s->connectToServer( Address, NAME );
 		DoRound( c, s, delay_ms, verbose );
	}
	
	if( IsTargetStillALive( Address, Port ) )
		return TARGET_UP;
	else 
		return TARGET_DOWN;
}

int DoRound( cFuzz &c, char* Address, int Port, int sockstream, long delay_ms, _verbose_mode verbose )
{
	if(Address!=NULL) {
		string address(Address);
		return DoRound(c, address, Port, sockstream, delay_ms, verbose);
	}
	throw invalid_argument ("Address string is NULL");
}

void DoRound( cFuzz &c, shared_ptr<mySocket> s, long delay_ms, _verbose_mode verbose ) {
	for( unsigned int i = 0; i < c.GetNumberOfPermutations(); i++ )	{
		s->sends( c.GetNextPermutation() );
		if(delay_ms!=0) {
			if(verbose)
				cout << " [sleep " << delay_ms << " ms]";
			_sleep(delay_ms);
		}
		if(verbose)
			cout << endl;
	}

	if(verbose)
		cout << "\t\tPermutations done: " << c.GetNumberOfPermutations() << endl;
}
