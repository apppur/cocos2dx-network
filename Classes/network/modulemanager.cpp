#include "basecore.h"
#include "modulebase.h"
#include "modulemanager.h"

ModuleManager::ModuleManager()
{
	m_modules.clear();
}

ModuleManager::~ModuleManager()
{
}

void ModuleManager::RegisterModule(unsigned int id, ModuleBase* base)
{
	std::pair<std::map<unsigned int, ModuleBase*>::iterator, bool> result;
	result = m_modules.insert(std::pair<unsigned int, ModuleBase*>(id, base));
	if (result.second == false)
	{
		fprintf(stderr, "register module failed.");
	}
	else
	{
		printf("register module success.");
	}
}

void ModuleManager::DispatchMessage(unsigned int opcode, const google::protobuf::Message* msg)
{
	unsigned int moduleid = BIT32_EXTRACT(opcode, 8, 8);
	
	std::map<unsigned int, ModuleBase*>::iterator iter = m_modules.find(moduleid);
	if (iter != m_modules.end())
	{
		ModuleBase* pModule = iter->second;
		if (pModule != nullptr)
		{
			pModule->OnMessage(opcode, msg);
		}
		fprintf(stderr, "module %u pointer is nullptr.");
	}
	else
	{
		fprintf(stderr, "don't find module, module id: %u.", moduleid);
	}
}