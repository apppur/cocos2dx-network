#include <thread>
#include "socketmanager.h"

SocketManager::SocketManager()
{
}

SocketManager::~SocketManager()
{
}

void SocketManager::Run(SocketServer* server)
{
	//std::thread socket_thread(&SocketServer::Run());
	//socket_thread.join();
	return;
}