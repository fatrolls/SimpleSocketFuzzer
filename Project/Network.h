#ifndef NETWORK_H
#define NETWORK_H

#include <string>

using namespace std;

extern void Init();
extern void Cleanup();

extern bool IsTargetStillALive( char* IpAddress, unsigned int port );
extern bool IsTargetStillALive( string Address, unsigned int port );

extern int DoRound( cFuzz &c, string Address, int Port, int socksteam, long delay_ms, _verbose_mode verbose );
extern int DoRound( cFuzz &c, char* Address, int Port, int sockstream, long delay_ms, _verbose_mode verbose );

#define TARGET_UP 1
#define TARGET_DOWN 2

#endif