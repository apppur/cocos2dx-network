#include <stdio.h>
#include <string.h>
#include "basecore.h"
#include "ringbuffer.h"

RingBuffer::RingBuffer()
{
	Initialize();
}

RingBuffer::~RingBuffer()
{
}

void RingBuffer::Initialize()
{
	memset(m_buffer, 0, sizeof(m_buffer));
	m_head = m_tail = 0;
}

long int RingBuffer::Write(const char* buff, unsigned long int len)
{
	if (Space() < len)
	{
		printf("ring buffer space is not enough. please waiting.\n");
		return 0;
	}

	unsigned long int rest = BUFFER_SIZE - m_tail;
	if (rest >= len)
	{
		memcpy(m_buffer+m_tail, buff, len);
	}
	else
	{
		memcpy(m_buffer+m_tail, buff, rest);
		memcpy(m_buffer, buff+rest, len-rest);
	}

	m_tail = (m_tail + len) & BUFFER_MASK;

	return len;
}

long int RingBuffer::Read(char* buff, unsigned long int len)
{
	if (Available() < len)
	{
		printf("ring buffer is empty. no available data. please waiting.\n");
		return 0;
	}

	unsigned long int rest = BUFFER_SIZE - m_head;
	if (rest >= len)
	{
		memcpy(buff, m_buffer+m_head, len);
	}
	else
	{
		memcpy(buff, m_buffer+m_head, rest);
		memcpy(buff+rest, m_buffer, len-rest);
	}

	m_head = (m_head + len) & BUFFER_MASK;

	return len;
}

void RingBuffer::Dump()
{
	char buffer[1024] = {0};
	long size;
	if ((size = Read(buffer, 1024)) == 0)
	{
		printf("please waiting\n");
	}
	char array[20] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', \
					  'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'}; 

	Write(array, 6);
	Write(array, 10);
	memset(buffer, 0, sizeof(buffer));
	size = Read(buffer, 4);
	for (auto i = 0; i < size; i++)
	{
		printf("%c", buffer[i]);
	}
	printf("\n");
	Write(array, 10);
	memset(buffer, 0, sizeof(buffer));
	size = Read(buffer, 12);
	for (auto i = 0; i < size; i++)
	{
		printf("%c", buffer[i]);
	}
	printf("\n");
	memset(buffer, 0, sizeof(buffer));
	Read(buffer, 1);
	Write(array, 15);
	memset(buffer, 0, sizeof(buffer));
	size = Read(buffer, 15);
	for (auto i = 0; i < size; i++)
	{
		printf("%c", buffer[i]);
	}
	printf("\n");

	return;
}

long int RingBuffer::GetSize()
{
	long int size = 0;
	char buff[2] = {0};
	if (Available() < 2)
	{
		return 0;
	}
	else
	{
		size = ((unsigned char)m_buffer[m_head] << 8) | ((unsigned char)m_buffer[(m_head + 1) & BUFFER_MASK]);
		return size;
	}
}

bool RingBuffer::IsComplete()
{
	return (Available() >= GetSize());
}
