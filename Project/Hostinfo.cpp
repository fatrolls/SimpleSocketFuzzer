#include "HostInfo.h"

myHostInfo::myHostInfo()
{

#ifdef WINDOWS_XP
	
	char sName[HOST_NAME_LENGTH+1];
	memset(sName,0,sizeof(sName));
	gethostname(sName,HOST_NAME_LENGTH);

	try 
	{
		hostPtr = gethostbyname(sName);
	}
	catch( ... )
	{
		exit(1);
	}
	
#endif

}

myHostInfo::myHostInfo(string& hostName,hostType type)
{
	try 
	{
		if (type == NAME)
		{
			// Retrieve host by name
			hostPtr = gethostbyname(hostName.c_str());
        }
		else if (type == ADDRESS)
		{
			// Retrieve host by address
		    unsigned long netAddr = inet_addr(hostName.c_str());
			if (netAddr == -1)
			{
				throw "Error: Could not get host ip address!";
			}

	        hostPtr = gethostbyaddr((char *)&netAddr, sizeof(netAddr), AF_INET);
			if (hostPtr == NULL)
			{
				throw "Error: Could not get host ip address!";
			}
        }
		else
		{
			throw "Error: Could not get host ip address!";
		}
    } catch( ... ){
		throw "Exception: cpp exception!";
	}

}