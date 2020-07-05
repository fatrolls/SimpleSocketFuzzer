#include "Socket.h"

#include "support_fcn.h"

#pragma comment(lib, "ws2_32.lib")

const int MSG_HEADER_LEN = 6;

void StartWSA(){
	WSADATA wsaData;
	WSAStartup(0x202, &wsaData);
	return;
}

void StopWSA(){
	WSACleanup();
}

mySocket::mySocket(int pNumber, int sockstream)
{
    portNumber = pNumber;
    blocking = 1;
	_verbose=Disabled;

	__try {
		if (sockstream!=SOCK_STREAM && sockstream!=SOCK_DGRAM)
			throw "Error: socket() failed, invalid sockstream";

		if ( (socketId=socket(AF_INET, sockstream,0)) == -1)
        {
			throw "Error: socket() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: mySocket()";
	}

    /* 
	   set the initial address of client that shall be communicated with to 
	   any address as long as they are using the same port number. 
	   The clientAddr structure is used in the future for storing the actual
	   address of client applications with which communication is going
	   to start
	*/
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(portNumber);
}
    
void mySocket::setDebug(int debugToggle)
{
	__try 
	{
		if ( setsockopt(socketId,SOL_SOCKET,SO_DEBUG,(char *)&debugToggle,sizeof(debugToggle)) == -1 )
		{
			throw "Error: setsockopt() failed!";
        }
	}
    __except(1)
	{
			throw "Exception: setDebug()";
	}
}

void mySocket::setReuseAddr(int reuseToggle)
{
	__try 
	{
		if ( setsockopt(socketId,SOL_SOCKET,SO_REUSEADDR,(char *)&reuseToggle,sizeof(reuseToggle)) == -1 )
		{
			throw "Error: setsockopt() failed!";
        }
	}
    __except(1)
	{
			throw "Exception: setReuseAddr()";
	}
} 

void mySocket::setKeepAlive(int aliveToggle)
{
	__try 
	{
		if ( setsockopt(socketId,SOL_SOCKET,SO_KEEPALIVE,(char *)&aliveToggle,sizeof(aliveToggle)) == -1 )
		{
			throw "Error: setsockopt() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: setKeepAlive()";
	}
} 

void mySocket::setLingerSeconds(int seconds)
{
	struct linger lingerOption;
	
	if ( seconds > 0 )
	{
		lingerOption.l_linger = seconds;
		lingerOption.l_onoff = 1;
	}
	else lingerOption.l_onoff = 0;
	 
	__try 
	{
		if ( setsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,sizeof(struct linger)) == -1 )
		{
			throw "Error: setsockopt() failed!";
        }
	}
    __except(1)
	{
			throw "Exception: setLingerSeconds()";
	}
}

void mySocket::setLingerOnOff(bool lingerOn)
{
	struct linger lingerOption;

	if ( lingerOn ) lingerOption.l_onoff = 1;
	else lingerOption.l_onoff = 0;

	__try 
	{
		if ( setsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,sizeof(struct linger)) == -1 )
		{
			throw "Error: setsockopt() failed!";
        }
	}
    __except(1)
	{
			throw "Exception: setLingerOnOff()!";
	}
}

void mySocket::setSendBufSize(int sendBufSize)
{
	__try 
	{
		if ( setsockopt(socketId,SOL_SOCKET,SO_SNDBUF,(char *)&sendBufSize,sizeof(sendBufSize)) == -1 )
		{
			throw "Error: setSendBufSize() failed!";
        }
	}
    __except(1)
	{
			throw "Exception: setSendBufSize()";
	}
} 

void mySocket::setReceiveBufSize(int receiveBufSize)
{
	__try 
	{
		if ( setsockopt(socketId,SOL_SOCKET,SO_RCVBUF,(char *)&receiveBufSize,sizeof(receiveBufSize)) == -1 )
		{
			throw "Error: setsockopt() failed!";
        }
	}
    __except(1)
	{
			throw "Exception: setReceiveBufSize()";
	}
}

void mySocket::setSocketBlocking(int blockingToggle)
{

	blockingToggle =! blockingToggle;

	__try 
	{
		if (ioctlsocket(socketId,FIONBIO,(unsigned long *)&blockingToggle) == -1)
			{
				throw "Error: ioctlsocket() failed!";
			}
	}
    __except(1)
	{
			throw "Exception: setSocketBlocking()";
	}
}

int mySocket::getDebug()
{
    int myOption;
    int myOptionLen = sizeof(myOption);

	__try 
	{
		if ( getsockopt(socketId,SOL_SOCKET,SO_DEBUG,(char *)&myOption,&myOptionLen) == -1 )
		{
			throw "Error: getDebug() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: getDebug()";
	}
    
    return myOption;
}

int mySocket::getReuseAddr()
{
    int myOption;        
    int myOptionLen = sizeof(myOption);

	__try 
	{
		if ( getsockopt(socketId,SOL_SOCKET,SO_REUSEADDR,(char *)&myOption,&myOptionLen) == -1 )
		{
			throw "Error: getsockopt() failed!";
        }
	}
    __except(1)
	{
			throw "Exception: getReuseAddr()";
	}

	return myOption;
}

int mySocket::getKeepAlive()
{
    int myOption;        
    int myOptionLen = sizeof(myOption);

	__try 
	{
		if ( getsockopt(socketId,SOL_SOCKET,SO_KEEPALIVE,(char *)&myOption,&myOptionLen) == -1 )
		{
			throw "Error: getsockopt() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: getKeepAlive()";
	}
    return myOption;    
}

int mySocket::getLingerSeconds()
{
	struct linger lingerOption;
	int myOptionLen = sizeof(struct linger);

	__try 
	{
		if ( getsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,&myOptionLen) == -1 )
		{
			throw "Error: getLingerSeconds() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: getLingerSeconds()";
	}

	return lingerOption.l_linger;
}

bool mySocket::getLingerOnOff()
{
	struct linger lingerOption;
	int myOptionLen = sizeof(struct linger);

	__try 
	{
		if ( getsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,&myOptionLen) == -1 )
		{
			throw "Error: getsockopt() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: getLingerOnOff()";
	}

	if ( lingerOption.l_onoff == 1 ) return true;
	else return false;
}

int mySocket::getSendBufSize()
{
    int sendBuf;
    int myOptionLen = sizeof(sendBuf);

	__try 
	{
		if ( getsockopt(socketId,SOL_SOCKET,SO_SNDBUF,(char *)&sendBuf,&myOptionLen) == -1 )
		{
			throw "Error: getsockopt() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: getSendBufSize()!";
	}
    return sendBuf;
}    

int mySocket::getReceiveBufSize()
{
    int rcvBuf;
    int myOptionLen = sizeof(rcvBuf);

	__try 
	{
		if ( getsockopt(socketId,SOL_SOCKET,SO_RCVBUF,(char *)&rcvBuf,&myOptionLen) == -1 )
		{
			throw "Error: getsockopt() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: getReceiveBufSize()!";
	}
    return rcvBuf;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void myTcpSocket::bindSocket()
{
	__try
	{
		if (bind(socketId,(struct sockaddr *)&clientAddr,sizeof(struct sockaddr_in))==-1)
		{
		 throw "Error: Bind() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: bindSocket()!";
	}
}

void myTcpSocket::connectToServer(string& serverNameOrAddr,hostType hType)
{ 
	/* 
	   when this method is called, a client socket has been built already,
	   so we have the socketId and portNumber ready.

       a myHostInfo instance is created, no matter how the server's name is 
	   given (such as www.yuchen.net) or the server's address is given (such
	   as 169.56.32.35), we can use this myHostInfo instance to get the 
	   IP address of the server
	*/

	myHostInfo serverInfo(serverNameOrAddr,hType);
	
    // Store the IP address and socket port number	
	struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverInfo.getHostIPAddress());
    serverAddress.sin_port = htons(portNumber);

    // Connect to the given address
		if (connect(socketId,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) == -1)
		{
			throw "Error: Connect failed!";
        }

}

myTcpSocket* myTcpSocket::acceptClient(string& clientHost)
{
	int newSocket;   // the new socket file descriptor returned by the accept systme call

    // the length of the client's address
    int clientAddressLen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientAddress;    // Address of the client that sent data

    // Accepts a new client connection and stores its socket file descriptor
		if ((newSocket = accept(socketId, (struct sockaddr *)&clientAddress,&clientAddressLen)) == -1)
		{
			throw "Error: Accept failed!";
        }

    
	// Get the host name given the address
    char *sAddress = inet_ntoa((struct in_addr)clientAddress.sin_addr);
	myHostInfo clientInfo(string(sAddress),ADDRESS);
	char* hostName = clientInfo.getHostName();
    clientHost += string(hostName);
	
    // Create and return the new myTcpSocket object
    myTcpSocket* retSocket = new myTcpSocket();
	retSocket->setSocketId(newSocket);
    return retSocket;
}

void myTcpSocket::listenToClient(int totalNumPorts)
{
	__try 
	{
		if (listen(socketId,totalNumPorts) == -1)
		{
			throw "Error: Listen failed!";
        }
	}
    __except(1)
	{
		throw "Exception: listenToClient()";
	}
}       

int myTcpSocket::sends(string& message)
{
	int numBytes;  // the number of bytes sent

	// Sends the message to the connected host

	if(_verbose==Normal)
		std::cout << "\t\tSocket send: " << message;
 	if(_verbose==Hexadecimal)
 		std::cout << "\t\tSocket send: " << _string_to_hex(message);

	try 
	{
		if (numBytes = send(socketId, (char*)message.c_str(), message.size(), 0) == -1)
		{
			return -1;
        }
	}
    catch(...)
	{
		throw "Exception: sends()!";
	}

	return numBytes;
}

char myTcpSocket::getc(){
	char c = 0;
	if( recv( socketId, &c, 1, 0 ) == SOCKET_ERROR  )
		return -1;
return c;
}

int myTcpSocket::gets(string& message)
{
	char c = 0;
	do{
		c = getc();
		message += c;
		if(_verbose==Normal)
			cout << c;
		if(_verbose==Hexadecimal)
			cout << std::hex << c ;

		if( c == -1 )
			return -1;

	} while( c != '\n' );
	if(_verbose!=Disabled)
		cout << endl;
	
return 1;
}

int myTcpSocket::gets(string& message, int n /*specified number of bytes*/)
{
	char c = 0;
	int i=0;
	do{
		c = getc();
		message += c;
		if(_verbose==Normal)
			cout << c;
		if(_verbose==Hexadecimal)
			cout << std::hex << c ;
		i++;

	} while( i<n );
	if(_verbose!=Disabled)
		cout << endl;

	return 1;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void myUdpSocket::bindSocket()
{
	__try
	{
		if (bind(socketId,(struct sockaddr *)&clientAddr,sizeof(struct sockaddr_in))==-1)
		{
		 throw "Error: Bind() failed!";
        }
	}
    __except(1)
	{
		throw "Exception: bindSocket()!";
	}
}

void myUdpSocket::connectToServer(string& serverNameOrAddr,hostType hType)
{ 
	/* 
	   when this method is called, a client socket has been built already,
	   so we have the socketId and portNumber ready.

       a myHostInfo instance is created, no matter how the server's name is 
	   given (such as www.yuchen.net) or the server's address is given (such
	   as 169.56.32.35), we can use this myHostInfo instance to get the 
	   IP address of the server
	*/

	myHostInfo serverInfo(serverNameOrAddr,hType);
	
    // Store the IP address and socket port number	
	struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverInfo.getHostIPAddress());
    serverAddress.sin_port = htons(portNumber);

    // Connect to the given address
	if (connect(socketId,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) == -1)
	{
		throw "Error: Connect failed!";
       }

}

myUdpSocket* myUdpSocket::acceptClient(string& clientHost) {
	throw "myUdpSocket: acceptClient() not supported with UDP";
	return NULL;
}

void myUdpSocket::listenToClient(int totalNumPorts) {
	throw "myUdpSocket: listenToClient() not supported with UDP";
}       

int myUdpSocket::sends(string& message)
{
	int numBytes;  // the number of bytes sent

	// Sends the message to the connected host

	if(_verbose)
		std::cout << "\t\tSocket send: " << message;

	__try 
	{
		if (numBytes = send(socketId, (char*)message.c_str(), message.size(), 0) == -1)
		{
			return -1;
        }
	}
    __except( 1 )
	{
		throw "Exception: sends()!";
	}

	return numBytes;
}

char myUdpSocket::getc(){
	char c = 0;
	if( recv( socketId, &c, 1, 0 ) == SOCKET_ERROR  )
		return -1;
return c;
}

int myUdpSocket::gets(string& message)
{
	char c = 0;
	do{
		c = getc();
		message += c;

		if( c == -1 )
			return -1;

	} while( c != '\n' );
return 1;
}

int myUdpSocket::gets(string& message, int n /*specified number of bytes*/)
{
	char c = 0;
	int i=0;
	do{
		c = getc();
		message += c;
		i++;

	} while( i<n );
	return 1;
}

