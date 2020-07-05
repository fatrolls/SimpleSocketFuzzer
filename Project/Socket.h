#ifndef _mySocket
#define _mySocket

#include "Hostinfo.h"
#include "support_fcn.h"

#include <iostream>

#include <winsock.h>

extern void StartWSA();
extern void StopWSA();

enum _verbose_mode {Disabled, Normal, Hexadecimal};

// so far we only consider the TCP socket, UDP will be added in later release
//const int MAX_RECV_LEN = 1048576;
const int MAX_RECV_LEN = 8096;
const int MAX_MSG_LEN = 1024;
const int PORTNUM = 1200;

class mySocket
{

protected:

	/*
	   only used when the socket is used for client communication
	   once this is done, the next necessary call is setSocketId(int)
	*/
	mySocket() {}
	void setSocketId(int socketFd) { socketId = socketFd; }

protected:

    int portNumber;        // Socket port number    
    int socketId;          // Socket file descriptor

    int blocking;          // Blocking flag
    int bindFlag;          // Binding flag

    struct sockaddr_in clientAddr;    // Address of the client that sent data

	_verbose_mode _verbose; //set to true for detailed log

public:

    mySocket(int pNumber, int socksteam=SOCK_STREAM);   // given a port number and a socket type (SOCK_STREAM or SOCK_DGRAM), create a socket

    virtual ~mySocket()
    {
        closesocket(socketId);
    }

	void set_verbose(_verbose_mode ver) {
		_verbose=ver;
	}

	//virtual methods MUST be overwritten by derived classes
	virtual int sends(string&) {return 0;}
	virtual char getc() {return '0';}	
	virtual void bindSocket() {}
	virtual mySocket* acceptClient(string&) {return NULL;}
	virtual void listenToClient(int numPorts = 5) {}
	virtual void connectToServer(string&,hostType) {}

public:

	// socket options : ON/OFF

    void setDebug(int);
    void setReuseAddr(int);
    void setKeepAlive(int);
    void setLingerOnOff(bool);
	void setLingerSeconds(int);
    void setSocketBlocking(int);

    // size of the send and receive buffer

    void setSendBufSize(int);
    void setReceiveBufSize(int);

    // retrieve socket option settings

    int  getDebug();
    int  getReuseAddr();
    int  getKeepAlive();
    int  getSendBufSize();
    int  getReceiveBufSize();
    int  getSocketBlocking() { return blocking; }
	int  getLingerSeconds();
    bool getLingerOnOff();
	
    // returns the socket file descriptor
    int getSocketId() { return socketId; }

	// returns the port number
	int getPortNumber() { return portNumber; }

private:

};

class myTcpSocket : public mySocket
{
public:

	/* 
	   Constructor. used for creating instances dedicated to client 
	   communication:

	   when accept() is successful, a socketId is generated and returned
	   this socket id is then used to build a new socket using the following
	   constructor, therefore, the next necessary call should be setSocketId()
	   using this newly generated socket fd
	*/
	myTcpSocket() {};
	~myTcpSocket() {};

	// Constructor.  Used to create a new TCP socket given a port
	myTcpSocket(int pNumber) : mySocket(pNumber, SOCK_STREAM) { };

	/*
		sends a string to the server
	*/
	int sends(string&);

	/*
	   receives a character from the stream
	*/
	char getc();

	/*
	   receive messages and stores the message in a buffer
	*/
	int gets(string&);
	int gets(string& message, int n);

	/*
	   Binds the socket to an address and port number
	   a server call
	*/
	void bindSocket();

	/*
	   accepts a connecting client.  The address of the connected client 
	   is stored in the parameter
	   a server call
	*/
	myTcpSocket* acceptClient(string&);

	// Listens to connecting clients, a server call
	void listenToClient(int numPorts = 5);

	// connect to the server, a client call
	virtual void connectToServer(string&,hostType);

};

class myUdpSocket : public mySocket
{
public:

	/* 
	   Constructor. used for creating instances dedicated to client 
	   communication:

	   when accept() is successful, a socketId is generated and returned
	   this socket id is then used to build a new socket using the following
	   constructor, therefore, the next necessary call should be setSocketId()
	   using this newly generated socket fd
	*/
	myUdpSocket() {};
	~myUdpSocket() {};

	// Constructor.  Used to create a new TCP socket given a port
	myUdpSocket(int pNumber) : mySocket(pNumber, SOCK_DGRAM) { };

	/*
		sends a string to the server
	*/
	int sends(string&);

	/*
	   receives a character from the stream
	*/
	char getc();

	/*
	   receive messages and stores the message in a buffer
	*/
	int gets(string&);
	int gets(string& message, int n);

	/*
	   Binds the socket to an address and port number
	   a server call
	*/
	void bindSocket();

	/*
	   accepts a connecting client.  The address of the connected client 
	   is stored in the parameter
	   a server call
	*/
	myUdpSocket* acceptClient(string&);

	// Listens to connecting clients, a server call
	void listenToClient(int numPorts = 5);

	// connect to the server, a client call
	virtual void connectToServer(string&,hostType);

};

#endif