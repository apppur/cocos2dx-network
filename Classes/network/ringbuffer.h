#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H
#define BUFFER_SIZE 8192*8	// the size must be the power of two for optimization. set 16 for dump test.
#define BUFFER_MASK (BUFFER_SIZE - 1)

class RingBuffer
{
public:
	RingBuffer();
	~RingBuffer();

	void Initialize();
	long int Write(const char* buff, unsigned long int len);
	long int Read(char* buff, unsigned long int len);

	inline bool Empty() { return m_head == m_tail; }
	inline bool Full() { return ((m_tail + 1) & BUFFER_SIZE) == m_head; }
	inline int Available() { return (m_tail >= m_head ? (m_tail - m_head) : (m_tail - m_head + BUFFER_SIZE)); }
	inline int Space() { return BUFFER_SIZE - Available() - 1; }

	long int GetSize();				// return package head size.
	bool IsComplete();				// is package complete?
	// for test
	void Dump();
private:
	char m_buffer[BUFFER_SIZE];
	unsigned int m_head;
	unsigned int m_tail;
};

#endif