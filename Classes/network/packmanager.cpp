#include <string.h>
#include "basecore.h"
#include "packmanager.h"
#include "ringbuffer.h"
#include "messagefactory.h"

PackManager::PackManager(SocketServer* socketserver)
{
	m_socketserver = socketserver;
	memset(m_readbuffer, 0, sizeof(m_readbuffer));
	memset(m_writebuffer, 0, sizeof(m_writebuffer));
}

PackManager::~PackManager()
{
}

void PackManager::Dispatch()
{
	if (m_socketserver == nullptr)
	{
		fprintf(stderr, "socketserver ptr is null.");
		return;
	}

	RingBuffer ringbuffer = m_socketserver->GetRecvBuffer();
	if (ringbuffer.IsComplete())
	{
		long sz = ringbuffer.GetSize();
		if (sz != ringbuffer.Read(m_readbuffer, sz))
		{
			fprintf(stderr, "ringbuffer read data error.");
			return;
		}

		unsigned int version = PVERSION(m_readbuffer);
		unsigned int opcode = POPCODE(m_readbuffer);

		//google::protobuf::Message* pMessage = MessageFactory::CreateMessage(opcode);
		//pMessage->SerializeToArray(m_readbuffer, sz - HEADSIZE);
	}
}

void PackManager::Send(unsigned int opcode, google::protobuf::Message* pMessage)
{
	if (pMessage == nullptr)
	{
		fprintf(stderr, "message ptr is null.");
		return;
	}
	int sz = pMessage->ByteSize();

	// package size
	unsigned char s1 = BIT32_EXTRACT(sz+6, 8, 8);
	unsigned char s2 = BIT32_EXTRACT(sz+6, 0, 8);
	memset(m_writebuffer, s1, sizeof(unsigned char));
	memset(m_writebuffer+1, s2, sizeof(unsigned char));
	// protocol version
	unsigned char v1 = BIT32_EXTRACT(1, 8, 8);
	unsigned char v2 = BIT32_EXTRACT(1, 0, 8);
	memset(m_writebuffer+2, v1, sizeof(unsigned char));
	memset(m_writebuffer+3, v2, sizeof(unsigned char));
	// opcode
	unsigned char o1 = BIT32_EXTRACT(opcode, 24, 8);
	unsigned char o2 = BIT32_EXTRACT(opcode, 16, 8);
	unsigned char o3 = BIT32_EXTRACT(opcode, 8, 8);
	unsigned char o4 = BIT32_EXTRACT(opcode, 0, 8);
	memset(m_writebuffer + 4, o1, sizeof(unsigned char));
	memset(m_writebuffer + 5, o2, sizeof(unsigned char));
	memset(m_writebuffer + 6, o3, sizeof(unsigned char));
	memset(m_writebuffer + 7, o4, sizeof(unsigned char));

	pMessage->SerializeToArray(m_writebuffer+8, sz);

	//m_socketserver->GetSendBuffer().Write(m_writebuffer, sz+8);
	m_socketserver->Send(m_writebuffer, sz+8);
}