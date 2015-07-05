#include "BaseFunction.h"
#include <Windows.h>

float fNowTick = 0;

unsigned long GetMillisecondNow()
{
	return (unsigned long)fNowTick;
}

void AddMillisecondTime(float nDelta)
{
	fNowTick += nDelta * 1000;
}

unsigned int GetMonthDays(unsigned int year, unsigned int month)
{
	bool leap;
	if (year % 4 != 0)
		leap = false;
	else if (year % 100 != 0)
		leap = true;
	else if (year % 400 != 0)
		leap = false;
	else
		leap = true;
	unsigned int days = 0;
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
	{
		days = 31;
	}
	break;
	case 2:
	{
		days = leap ? 29 : 28;
	}
	break;
	default:
	{
		days = 30;
	}
	}
	return days;
}

time_t GetMicrosecondeNow()
{
	struct timeval val;
#ifdef WIN32
	LARGE_INTEGER liTime, liFreq;
	QueryPerformanceFrequency(&liFreq);
	QueryPerformanceCounter( &liTime );
	val.tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
	val.tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val.tv_sec * 1000000.0);
#else
	gettimeofday((struct timeval *)&val, NULL);
#endif
	return (val.tv_sec*1000000.0 + val.tv_usec);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#else
bool IConvConvert(const char *from_charset, const char *to_charset, const char *inbuf, int inlen, char *outbuf, int outlen)
{
	iconv_t cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return false;

	const char **pin = &inbuf;
	char **pout = &outbuf;
	memset(outbuf, 0, outlen);
#ifdef WIN32
	size_t ret = iconv(cd, pin, (size_t *)&inlen, pout, (size_t *)&outlen);
#else
	size_t ret = iconv(cd, const_cast<char**>(pin), (size_t *)&inlen, pout, (size_t *)&outlen);
#endif
	if (ret == -1)
		CCLOG("error = %s", strerror(errno));
	iconv_close(cd);

	return ret == (size_t)(-1) ? false : true;
}
#endif


