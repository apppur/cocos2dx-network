#ifndef _SOCKET_MANAGER_H
#define _SOCKET_MANAGER_H
#include "socketserver.h"

class SocketManager
{
public:
	SocketManager();
	~SocketManager();

public:
	void Run(SocketServer* server);
};

#endif;