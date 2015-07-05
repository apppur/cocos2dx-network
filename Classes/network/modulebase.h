#ifndef _MODULE_BASE_H
#define _MODULE_BASE_H
#include "google/protobuf/message.h"

class ModuleBase
{
public:
	virtual void OnMessage(unsigned int opcode, const const google::protobuf::Message* msg) = 0;
protected:
	virtual ~ModuleBase() {}

private:
	char m_modulename[16];
};

#endif