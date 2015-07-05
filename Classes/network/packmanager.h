#ifndef _PACK_MANAGER_H
#define _PACK_MANAGER_H
#include "socketserver.h"
#include "google/protobuf/message.h"

class PackManager
{
public:
	PackManager(SocketServer* socketserver);
	~PackManager();

	void Dispatch();
	long int Pack();
	long int Unpack();
	void Send(unsigned int opcode, google::protobuf::Message* pMessage);

private:
	SocketServer* m_socketserver;
	char m_readbuffer[1024];
	char m_writebuffer[1024];
};

#endif