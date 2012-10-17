#include < time.h >
#include "gettime.h"
#include <windows.h>
#include <iostream>
#include <stdio.h>
// gettimeofday in windows
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		// system time�� ���ϱ�
		GetSystemTimeAsFileTime(&ft);

		// unsigned 64 bit�� �����
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;
		// 100nano�� 1micro�� ��ȯ�ϱ�    tmpres /= 10;
		// epoch time���� ��ȯ�ϱ�
		tmpres -= DELTA_EPOCH_IN_MICROSECS;    

		// sec�� micorsec���� ���߱�
		tv->tv_sec = (tmpres / 1000000UL);
		tv->tv_usec = (tmpres % 1000000UL);
	}

	// timezone ó��
	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
