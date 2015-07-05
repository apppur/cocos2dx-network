#include "basecore.h"
#include "messagefactory.h"

char MessageFactory::m_messagebuffer[MAX_MESSAGE_BUFF_LEN] = { 0 };
MessageFactory::FuncMsgInstance MessageFactory::m_msgfuncs[MESSAGE_NUM] = {0};
unsigned int MessageFactory::m_moduleoffset[MODULE_END] = {0};

MessageFactory::MessageFactory()
{
	m_moduleoffset[MODULE_ROLE]		= ROLE_OFFSET;

	// login register function.
	RegisterFunc(ID_C2S_LOGIN_INFO_REQUEST, &MessageFactory::GetLoginInfoRequest);
	RegisterFunc(ID_S2C_LOGIN_INFO_RESPONSE, &MessageFactory::GetLoginInfoResponse);
	RegisterFunc(ID_C2S_LOGIN_SERVER_REQUEST, &MessageFactory::GetLoginServerRequest);
	RegisterFunc(ID_S2C_LOGIN_SERVER_RESPONSE, &MessageFactory::GetLoginServerResponse);

	// role register functions.
	RegisterFunc(ID_C2S_ROLE_LOGIN_REQUEST, &MessageFactory::GetRoleLoginRequest);
	RegisterFunc(ID_S2C_ROLE_LOGIN_RESPONSE, &MessageFactory::GetRoleLoginResponse);
	RegisterFunc(ID_C2S_ROLE_CREATE_REQUEST, &MessageFactory::GetRoleCreateRequest);
	RegisterFunc(ID_S2C_ROLE_CREATE_RESPONSE, &MessageFactory::GetRoleCreateResponse);
	RegisterFunc(ID_C2S_ROLE_PROPERTY_REQUEST, &MessageFactory::GetRolePropertyRequest);
	RegisterFunc(ID_S2C_ROLE_PROPERTY_RESPONSE, &MessageFactory::GetRolePropertyResponse);

}

MessageFactory::~MessageFactory()
{
}
void MessageFactory::RegisterFunc(unsigned int opcode, FuncMsgInstance pfunc)
{
	int index = GetFuncIndex(opcode);
	if (index < 0 || index >= MESSAGE_NUM)
	{
		fprintf(stderr, "illage function index: %d.", index);
		return;
	}

	m_msgfuncs[index] = pfunc;

	return;
}

google::protobuf::Message* MessageFactory::CreateMessage(unsigned int opcode)
{
	int index = GetFuncIndex(opcode);
	if (index < 0 || index >= MESSAGE_NUM)
	{
		fprintf(stderr, "illage function index: %d.", index);
		return nullptr;
	}

	FuncMsgInstance pfunc = m_msgfuncs[index];
	if (pfunc == nullptr)
	{
		return nullptr;
	}
	else
	{
		return (this->*pfunc)();
	}
}

int MessageFactory::GetFuncIndex(unsigned int opcode)
{
	unsigned int moduleid = BIT32_EXTRACT(opcode, 16, 8);
	if (moduleid >= MODULE_END || moduleid < 0)
	{
		fprintf(stderr, "illage module id: %u.", moduleid);
		return -1;
	}

	return m_moduleoffset[moduleid] + (opcode & 0x0000ffff);
}

unsigned int MessageFactory::GetModuleOffset(unsigned int moduleid)
{
	if (moduleid >= MODULE_END || moduleid < 0)
	{
		fprintf(stderr, "illage module id: %u.", moduleid);
		return -1;
	}

	return m_moduleoffset[moduleid];
}