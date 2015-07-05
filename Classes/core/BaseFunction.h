#ifndef _BASE_FUNCTION_H_
#define _BASE_FUNCTION_H_

#include "string"

using namespace std;

unsigned long GetMillisecondNow();
void AddMillisecondTime(float nDelta);
time_t GetMicrosecondeNow();

unsigned int GetMonthDays(unsigned int year, unsigned int month);
#endif