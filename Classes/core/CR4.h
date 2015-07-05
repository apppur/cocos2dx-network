#ifndef _CR4_H
#define _CR4_H

class CR4
{
public:
	CR4() {}
	~CR4() {}

	void Init();

	void Crypt(char* buff, unsigned int sz);
	//void Dump();
private:
	unsigned char m_sbox[256];
	unsigned char m_key[8];
};

#endif