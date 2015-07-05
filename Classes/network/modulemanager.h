#ifndef _MODULE_MANAGER_H
#define _MODULE_MANAGER_H
#include <map>
#include "google/protobuf/message.h"

class ModuleBase;

class ModuleManager
{
public:
	ModuleManager();
	~ModuleManager();

	// register module
	void RegisterModule(unsigned int id, ModuleBase* base);
	
	// dispatch message
	void DispatchMessage(unsigned int opcode, const google::protobuf::Message* msg);
private:
	std::map<unsigned int, ModuleBase*> m_modules;
};

#endif