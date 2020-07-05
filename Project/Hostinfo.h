#ifndef _myHostInfo
#define _myHostInfo

#include <string>

using namespace std;

// this version is for both Windows and UNIX, the following line
// specifies that this is for WINDOWS

#include <winsock.h>
#include <stdio.h>
    
enum hostType {NAME, ADDRESS};
const int HOST_NAME_LENGTH = 64;

class myHostInfo
{

private:

	#ifdef UNIX
		char searchHostDB;     // search the host database flag
	#endif

	struct hostent *hostPtr;    // Entry within the host address database

public:

    // Default constructor
    myHostInfo();

    // Retrieves the host entry based on the host name or address
    myHostInfo(string& hostName, hostType type);
 
    // Destructor.  Closes the host entry database.
    ~myHostInfo()
    {
		#ifdef UNIX
			endhostent();
		#endif
    }

    // Retrieves the hosts IP address
    char* getHostIPAddress() 
    {
        struct in_addr *addr_ptr;
		// the first address in the list of host addresses
        addr_ptr = (struct in_addr *)*hostPtr->h_addr_list;
		// changed the address format to the Internet address in standard dot notation
        return inet_ntoa(*addr_ptr);
    }    
    
    // Retrieves the hosts name
    char* getHostName()
    {
        return hostPtr->h_name;
    }

};

#endif