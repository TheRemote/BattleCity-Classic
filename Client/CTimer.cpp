#include "CTimer.h"

CTime::CTime()
{
	timeBeginPeriod(1);
}

__int64 CTime::GetTime()
{
	return timeGetTime();
}