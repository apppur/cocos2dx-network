#include "CR4.h"

void CR4::Init()
{
	for (auto i = 0; i < 8; i++)
	{
		m_key[i] = 'a' + i;
	}

	for (auto i = 0; i < 256; i++)
	{
		m_sbox[i] = i;
	}

	int j = 0;
	for (auto i = 0; i < 256; i++)
	{
		j = (j + m_sbox[i] + m_key[i % 8]) % 256;
		char c = m_sbox[i];
		m_sbox[i] = m_sbox[j];
		m_sbox[j] = m_sbox[i];
	}
}

void CR4::Crypt(char* buff, unsigned int sz)
{
	int i = 0;
	int j = 0;

	for (auto k = 0; k < sz; k++)
	{
		i = (i + 1) % 256;
		j = (j + m_sbox[i]) % 256;
		char c = m_sbox[i];
		m_sbox[i] = m_sbox[j];
		m_sbox[j] = m_sbox[i];
		char v = *(buff + k);
		*(buff + k) = v ^ m_sbox[(m_sbox[i] + m_sbox[j]) % 256];
	}
}