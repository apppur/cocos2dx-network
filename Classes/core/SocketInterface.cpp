#include "../network/basecore.h"
#include "../network/messagefactory.h"
#include "../core/BaseFunction.h"
#include "SocketInterface.h"

SocketInterface::SocketInterface()
{
	m_msgno = 0;
	memset(m_readbuffer, 0, sizeof(m_readbuffer));
	memset(m_writebuffer, 0, sizeof(m_writebuffer));
	memset(m_WaitMsg.buff, 0, sizeof(m_WaitMsg.buff));
	m_waitId = 0;
}

bool SocketInterface::Initialize(const char* host, unsigned int port, std::string unique, std::string macaddress)
{
	/*
	if (m_socketserver.Connect(host, port))
	{
		m_unique = unique;
		m_macaddress = macaddress;
		CCLOG("connect server:[%s:%u] success.", host, port);
	}
	else
	{
		CCLOG("connect server:[%s:%u] failed.", host, port);
		return false;
	}
	*/
	m_ip = host;
	m_port = port;
	MessageFactory* pMessageFactory = MessageFactory::getInstance();

	// register callback events
	// RegisterEvent(opcode, func);

	m_socketserver.Initialize(host, port);
	m_socketserver.Run();
	return true;
}

bool SocketInterface::RegisterEvent(unsigned int opcode, const std::function<int(unsigned int, google::protobuf::Message*)>& func)
{
	/*
	std::pair<std::map<unsigned int, std::function<int(google::protobuf::Message*)>>, bool> result;
	result = m_events.insert(std::pair<unsigned int, std::function<int(google::protobuf::Message*)>>(opcode, fun));
	if (result.second == false)
	{
		CCLOG("register event:[%u] failed.", opcode);
		return false;
	}
	else
	{
		CCLOG("register event:[%u] success.", opcode);
		return true;
	}
	*/

	int index = MessageFactory::getInstance()->GetFuncIndex(opcode);
	if (index < 0 || index >= MessageFactory::MESSAGE_NUM)
	{
		//CCLOG("register event:[%u] failed.", opcode);
		return false;
	}
	else
	{
		m_events[index] = func;
		//CCLOG("register event:[%u] success.", opcode);
		return true;
	}
}

void SocketInterface::Update()
{
	RingBuffer& ringbuffer = m_socketserver.GetRecvBuffer();
	if (ringbuffer.IsComplete() && ringbuffer.GetSize() > 0)
	{
		long sz = ringbuffer.GetSize();
		if (sz == 0)
		{
			return;
		}
		if ((sz+2) != ringbuffer.Read(m_readbuffer, sz+2))
		{
			fprintf(stderr, "ringbuffer read data error.");
			return;
		}

		unsigned int version = PVERSION(m_readbuffer);
		unsigned int opcode = POPCODE(m_readbuffer);
		//opcode = 1;

		google::protobuf::Message* pMessage = MessageFactory::getInstance()->CreateMessage(opcode);
		if (pMessage == nullptr)
		{
			//CCLOG("illegal opcode:[%u].", opcode);
			return;
		}
		pMessage->ParseFromArray(m_readbuffer + 8, sz - 6);
		/*
		if (opcode == 1)
		{
			CMessageLoginInfoResponse* roleResponse = (CMessageLoginInfoResponse*)pMessage;
			std::string ver = roleResponse->version();
			std::string name = roleResponse->name();
			std::string ip = roleResponse->ip();
			unsigned int port = roleResponse->port();
			unsigned int serial = roleResponse->serial();
			CCLOG("Login Server Info: version = %s, name = %s, ip = %s, port = %u, serial = %u", ver.c_str(), name.c_str(), ip.c_str(), port, serial);
		}
		else if (opcode == 3)
		{
			CMessageLoginServerResponse* serverResponse = (CMessageLoginServerResponse*)pMessage;
			for (int i = 0; i < serverResponse->lists_size(); i++)
			{
				const CMessageLoginServerResponse_ServerList& roleResponse = serverResponse->lists(i);
				std::string ver = "aass";
				std::string name = roleResponse.name();
				std::string ip = roleResponse.ip();
				unsigned int port = roleResponse.port();
				unsigned int serial = roleResponse.serial();
				CCLOG("Login Server Info: version = %s, name = %s, ip = %s, port = %u, serial = %u", ver.c_str(), name.c_str(), ip.c_str(), port, serial);
			}
		}
		*/
		int index = MessageFactory::getInstance()->GetFuncIndex(opcode);
		if (index < 0 || index >= MessageFactory::MESSAGE_NUM)
		{
			//CCLOG("update event:[%u] failed.", opcode);
			return;
		}
		else
		{
			if (m_WaitMsg.state != 0 && m_WaitMsg.recvOp == opcode)
			{
				if (m_WaitMsg.waitTime != 0)
				{
					//通知正在连接状态结束
					m_waitCallBackList[m_waitId].waitCallBack(1, m_WaitMsg.sendOp, m_WaitMsg.recvOp);
					//size_t size = m_waitCallBackList.size();
					//if (size > 0)
					//{
					//	if (size > 1)
					//		CCLOG("m_waitCallBackList num > 1");
					//}
				}
				m_WaitMsg.state = 0;
				m_WaitMsg.sendOp = 0;
				m_WaitMsg.recvOp = 0;
				m_WaitMsg.delay = 0;
				m_WaitMsg.sendTime = 0;
				m_WaitMsg.waitTime = 0;
				m_WaitMsg.length = 0;
			}
			if (m_events[index])
			{
				m_events[index](opcode, pMessage);
			}
			else
			{
				//Property::getInstance()->OnMessage(opcode, pMessage);
			}
			//CCLOG("register event:[%u] success.", opcode);
			return;
		}

		/*
		std::map<unsigned int, std::function<int(google::protobuf::Message*)>>::const_iterator iter = m_events.find(opcode);
		if (iter != m_events.end())
		{
			iter->second(pMessage);
		}
		else
		{
			CCLOG("illegal opcode:[%u].", opcode);
		}
		*/
	}
	if (m_WaitMsg.state != 0)
	{
		if (m_WaitMsg.sendTime != 0 && GetMillisecondNow() - m_WaitMsg.sendTime > m_WaitMsg.delay)
		{
			m_WaitMsg.sendTime = 0;
			m_WaitMsg.waitTime = GetMillisecondNow();
			//通知正在连接中
			m_waitCallBackList[m_waitId].waitCallBack(2, m_WaitMsg.sendOp, m_WaitMsg.recvOp);
			//size_t size = m_waitCallBackList.size();
			//if (size > 0)
			//{
			//	if (size > 1)
			//		CCLOG("m_waitCallBackList num > 1");
			//}
		}
		else if (m_WaitMsg.waitTime != 0 && GetMillisecondNow() - m_WaitMsg.waitTime > WAIT_TIME_OUT)
		{
			m_WaitMsg.waitTime = 0;
			//通知连接失败
			m_waitCallBackList[m_waitId].waitCallBack(3, m_WaitMsg.sendOp, m_WaitMsg.recvOp);
			//size_t size = m_waitCallBackList.size();
			//if (size > 0)
			//{
			//	if (size > 1)
			//		CCLOG("m_waitCallBackList num > 1");
			//}
		}
	}
}

long SocketInterface::Send(unsigned int opcode, google::protobuf::Message* pMessage)
{
	if (pMessage == nullptr)
	{
		//CCLOG("message ptr is null.");
		return -1;
	}

	int sz = pMessage->ByteSize();

	// package size
	unsigned char s1 = BIT32_EXTRACT(sz + 6, 8, 8);
	unsigned char s2 = BIT32_EXTRACT(sz + 6, 0, 8);
	memset(m_writebuffer, s1, sizeof(unsigned char));
	memset(m_writebuffer + 1, s2, sizeof(unsigned char));
	// protocol version
	unsigned char v1 = BIT32_EXTRACT(1, 8, 8);
	unsigned char v2 = BIT32_EXTRACT(1, 0, 8);
	memset(m_writebuffer + 2, v1, sizeof(unsigned char));
	memset(m_writebuffer + 3, v2, sizeof(unsigned char));
	// opcode
	unsigned char o1 = BIT32_EXTRACT(opcode, 24, 8);
	unsigned char o2 = BIT32_EXTRACT(opcode, 16, 8);
	unsigned char o3 = BIT32_EXTRACT(opcode, 8, 8);
	unsigned char o4 = BIT32_EXTRACT(opcode, 0, 8);
	memset(m_writebuffer + 4, o1, sizeof(unsigned char));
	memset(m_writebuffer + 5, o2, sizeof(unsigned char));
	memset(m_writebuffer + 6, o3, sizeof(unsigned char));
	memset(m_writebuffer + 7, o4, sizeof(unsigned char));

	pMessage->SerializeToArray(m_writebuffer + 8, sz);

	//m_socketserver->GetSendBuffer().Write(m_writebuffer, sz+8);
	
	int res = m_socketserver.Send(m_writebuffer, sz + 8);
	if (res < 0)
	{
		int i = 0;
		do
		{
			m_socketserver.Close();
			if (m_socketserver.Connect(m_ip.c_str(), m_port))
			{
				m_socketserver.Send(m_writebuffer, sz + 8);
				break;
			} 
			else
			{
				i++;
			}
		} while (i < 3);
	}
}

long SocketInterface::SendCache(unsigned int opcode, google::protobuf::Message* pMessage)
{
	if (pMessage == nullptr)
	{
		//CCLOG("message ptr is null.");
		return -1;
	}

	int sz = pMessage->ByteSize();

	if (sz + 8 > 1024)
	{
		//CCLOG("message is too long opcode = %d,size = %d",opcode,sz);
		return -2;
	}

	// package size
	unsigned char s1 = BIT32_EXTRACT(sz + 6, 8, 8);
	unsigned char s2 = BIT32_EXTRACT(sz + 6, 0, 8);
	memset(m_writebuffer, s1, sizeof(unsigned char));
	memset(m_writebuffer + 1, s2, sizeof(unsigned char));
	// protocol version
	unsigned char v1 = BIT32_EXTRACT(1, 8, 8);
	unsigned char v2 = BIT32_EXTRACT(1, 0, 8);
	memset(m_writebuffer + 2, v1, sizeof(unsigned char));
	memset(m_writebuffer + 3, v2, sizeof(unsigned char));
	// opcode
	unsigned char o1 = BIT32_EXTRACT(opcode, 24, 8);
	unsigned char o2 = BIT32_EXTRACT(opcode, 16, 8);
	unsigned char o3 = BIT32_EXTRACT(opcode, 8, 8);
	unsigned char o4 = BIT32_EXTRACT(opcode, 0, 8);
	memset(m_writebuffer + 4, o1, sizeof(unsigned char));
	memset(m_writebuffer + 5, o2, sizeof(unsigned char));
	memset(m_writebuffer + 6, o3, sizeof(unsigned char));
	memset(m_writebuffer + 7, o4, sizeof(unsigned char));

	pMessage->SerializeToArray(m_writebuffer + 8, sz);

	long res = m_socketserver.GetSendBuffer().Write(m_writebuffer, sz + 8);

	m_writebufferLength = sz + 8;
	return res;
}

long SocketInterface::Recv(char* buffer, long int sz)
{
	long rvsz = m_socketserver.Recv(buffer, sz);
	if (rvsz == 0)
	{
		int i = 0;
		do
		{
			m_socketserver.Close();
			if (m_socketserver.Connect(m_ip.c_str(), m_port))
			{
				break;
			}
			else
			{
				i++;
			}
		} while (i < 3);

		return 0;
	}
	return rvsz;
}

void SocketInterface::Run()
{
	//std::thread socket_thread(&SocketServer::RecvCacheMsg, this);
	//socket_thread.detach();
}


void SocketInterface::UnRegisterEvent(unsigned int opcode)
{
	int index = MessageFactory::getInstance()->GetFuncIndex(opcode);
	if (index < 0 || index >= MessageFactory::MESSAGE_NUM)
	{
		//CCLOG("update event:[%u] failed.", opcode);
		return;
	}
	else
	{
		if (m_events[index])
		{
			m_events[index] = nullptr;
		}
		//CCLOG("register event:[%u] success.", opcode);
		return;
	}
}

void SocketInterface::Close()
{
	//m_socketserver.Close();
	m_socketserver.SetCloseCmd(1);
}

bool SocketInterface::Connect(const char* host, unsigned int port)
{
	m_socketserver.Initialize(host, port);
	m_socketserver.SetSocketCmd(1);
	return true;
}

bool SocketInterface::Connect()
{
	m_socketserver.SetSocketCmd(1);
	return true;
}

/*
void SocketInterface::RegisterWaitEvent(BaseScene* scene, const std::function<int(int, unsigned int, unsigned int)>& func)
{
	if (!scene)
		return;
	int exit = 0;
	for (auto& iter : m_waitCallBackList)
	{
		if (iter.second.scene == scene)
		{
			exit = iter.first;
			break;
		}
	}
	if (exit == 0)
	{
		m_waitId++;
		WaitCallBack info;
		info.scene = scene;
		info.waitCallBack = func;
		m_waitCallBackList[m_waitId] = info;
	}
}


void SocketInterface::UnRegisterWaitEvent(BaseScene* scene)
{
	int exit = 0;
	for (auto& iter : m_waitCallBackList)
	{
		if (iter.second.scene == scene)
		{
			exit = iter.first;
			break;
		}
	}
	if (exit != 0)
	{
		m_waitCallBackList.erase(exit);
	}
}
*/

int SocketInterface::SendMsg(unsigned int opcode, google::protobuf::Message* pMessage, unsigned int recvOp, int delay)
{
	if (recvOp == 0)
	{
		if (SendCache(opcode, pMessage) < 0)
			return -1;
	}
	else
	{
		if (m_WaitMsg.state != 0)
			return 1;//不能同时发送堵塞消息
		if (SendCache(opcode, pMessage) < 0)
			return 2;
		m_WaitMsg.state = 1;
		m_WaitMsg.sendOp = opcode;
		m_WaitMsg.recvOp = recvOp;
		m_WaitMsg.delay = delay;
		m_WaitMsg.sendTime = GetMillisecondNow();
		m_WaitMsg.waitTime = 0;
		memcpy(m_WaitMsg.buff, m_writebuffer, m_writebufferLength);
		m_WaitMsg.length = m_writebufferLength;
	}
	return 0;
}
int SocketInterface::ReSendMsg()
{
	if (m_WaitMsg.state == 0)
		return -1;
	long res = m_socketserver.GetSendBuffer().Write(m_WaitMsg.buff, m_WaitMsg.length);
	m_WaitMsg.sendTime = 0;
	m_WaitMsg.waitTime = GetMillisecondNow();
	return 0;
}
void SocketInterface::ResetSendMsg()
{
	m_WaitMsg.state = 0;
	m_WaitMsg.sendOp = 0;
	m_WaitMsg.recvOp = 0;
	m_WaitMsg.delay = 0;
	m_WaitMsg.sendTime = 0;
	m_WaitMsg.waitTime = 0;
	m_WaitMsg.length = 0;
}
