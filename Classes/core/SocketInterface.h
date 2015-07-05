#ifndef _SOCKET_INTERFACE_H
#define _SOCKET_INTERFACE_H
#include <map>
#include "Singleton.h"
//#include "cocos2d.h"
#include "../network/socketserver.h"
#include "../network/ringbuffer.h"
#include "../network/messagefactory.h"
#include "../network/protobuf/google/protobuf/message.h"

#define CCPACKHEAD(msg)	\
	msg.mutable_head()->set_msgno(SocketInterface::getInstance()->GetMsgno()); \
	msg.mutable_head()->set_token(SocketInterface::getInstance()->GetUnique()); \
	msg.mutable_head()->set_mac(SocketInterface::getInstance()->GetMacAddrees())

class SocketInterface : public Singleton<SocketInterface>
{
public:
	SocketInterface();

public:
	bool Initialize(const char* host, unsigned int port, std::string unique, std::string macaddress);
	bool RegisterEvent(unsigned int opcode, const std::function<int(unsigned int, google::protobuf::Message*)>& func);
	void UnRegisterEvent(unsigned int opcode);
	void Update();
	long Send(unsigned int opcode, google::protobuf::Message* pMessage);
	long SendCache(unsigned int opcode, google::protobuf::Message* pMessage);

	//void RegisterWaitEvent(BaseScene* scene, const std::function<int(int, unsigned int, unsigned int)>& func);
	//void UnRegisterWaitEvent(BaseScene* scene);
	int SendMsg(unsigned int opcode, google::protobuf::Message* pMessage, unsigned int recvOp = 0, int delay = 1000);
	int ReSendMsg();
	void ResetSendMsg();
	void Close();

	long Recv(char* buffer, long int sz);

	void Run();

	unsigned int GetMsgno() { return m_msgno; }
	void SetMsgno(unsigned int no) { m_msgno = no; }

	std::string GetUnique() { return m_unique; }
	void SetUnique(std::string unique) { m_unique = unique; }

	std::string GetMacAddrees() { return m_macaddress; }
	void SetMacAddress(std::string address) { m_macaddress = address; }

	void SetConnectCallback(std::function<int(ConnInfo)>& func) { m_socketserver.SetConnectCallback(func); }

	bool Connect(const char* host, unsigned int port);
	bool Connect();

private:
	SocketServer m_socketserver;
	std::string m_unique;				// platform unique identification
	std::string m_macaddress;			// device MAC address
	unsigned int m_msgno;

	std::string m_ip;
	unsigned int m_port;

	char m_readbuffer[1024*16];
	char m_writebuffer[1024*16];
	int m_writebufferLength;

	//std::map<unsigned int, std::function<int(google::protobuf::Message*)>> m_events;

	std::function<int(unsigned int, google::protobuf::Message*)> m_events[MessageFactory::MESSAGE_NUM];

	struct WaitCallBack
	{
		//BaseScene* scene;
		std::function<int(int, unsigned int, unsigned int)> waitCallBack;
	};
	std::map<int,WaitCallBack> m_waitCallBackList;
	int m_waitId;
	const int WAIT_TIME_OUT = 5000;
	struct WaitMessage
	{
		WaitMessage() :state(0), sendOp(0), recvOp(0), sendTime(0), waitTime(0), delay(0), length(0) {}
		int state;
		unsigned int sendOp;
		unsigned int recvOp;
		unsigned long sendTime;
		unsigned long waitTime;
		int delay;
		char buff[1024];
		int length;
	};
	WaitMessage m_WaitMsg;
};

#endif