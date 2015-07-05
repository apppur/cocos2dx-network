#ifndef _MESSAGE_FACTORY_H
#define _MESSAGE_FACTORY_H
#include "singleton.h"
#include "google/protobuf/message.h"
#include "message/coremessage.pb.h"
#include "message/rolemessage.pb.h"
#include "message/loginmessage.pb.h"

#define NEW_MSG(msgname)		return new(m_messagebuffer) msgname;	// replace new
#define MAX_MESSAGE_BUFF_LEN	(1024)									// max message buffer

class MessageFactory : public Singleton < MessageFactory >
{
public:
	MessageFactory();
	~MessageFactory();

	google::protobuf::Message* CreateMessage(unsigned int opcode);

public:
	enum MODULE_OFFSET
	{
		LOGIN_OFFSET = 0x00000000,
		ROLE_OFFSET = LOGIN_OFFSET + LOGIN_MODULE_END & 0x0000ffff,
		FIGHT_OFFSET = ROLE_OFFSET + ROLE_MODULE_END & 0x0000ffff,
		MESSAGE_NUM = ROLE_OFFSET + ROLE_MODULE_END & 0x0000ffff
	};

	unsigned int GetModuleOffset(unsigned int moduleid);
	int GetFuncIndex(unsigned int opcode);

private:
	typedef google::protobuf::Message* (MessageFactory::*FuncMsgInstance)();
	void RegisterFunc(unsigned int opcode, FuncMsgInstance pfunc);

	// login module message
	google::protobuf::Message* GetLoginInfoRequest() { NEW_MSG(CMessageLoginInfoRequest) }
	google::protobuf::Message* GetLoginInfoResponse() { NEW_MSG(CMessageLoginInfoResponse) }
	google::protobuf::Message* GetLoginServerRequest() { NEW_MSG(CMessageLoginServerRequest) }
	google::protobuf::Message* GetLoginServerResponse() { NEW_MSG(CMessageLoginServerResponse) }
	// role module message
	google::protobuf::Message* GetRoleLoginRequest() { NEW_MSG(CMessageRoleLoginRequest) }
	google::protobuf::Message* GetRoleLoginResponse() { NEW_MSG(CMessageRoleLoginResponse) }
	google::protobuf::Message* GetRolePropertyRequest() { NEW_MSG(CMessageRolePropertyRequest) }
	google::protobuf::Message* GetRolePropertyResponse() { NEW_MSG(CMessageRolePropertyResponse) }
	google::protobuf::Message* GetRoleCreateRequest() { NEW_MSG(CMessageRoleCreateRequest) }
	google::protobuf::Message* GetRoleCreateResponse() { NEW_MSG(CMessageRoleCreateResponse) }

private:
	static char m_messagebuffer[MAX_MESSAGE_BUFF_LEN];
	static FuncMsgInstance m_msgfuncs[MESSAGE_NUM];
	static unsigned int m_moduleoffset[MODULE_END];
};

#endif