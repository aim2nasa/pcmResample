/*
* mp2_decode.h
*
* Created: 2012-10-25 ¿ÀÈÄ 17:29:10
*  Author: Jerome
*/ 


#ifndef _MP2_DECODE_H_
#define _MP2_DECODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mpg123.h"

#ifndef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif

#pragma pack(push, 1)
	typedef struct{
		unsigned short sync:12;
		unsigned short id:1;
		unsigned short layer:2;
		unsigned short protect:1;
		unsigned short bitrateIdx:4;
		unsigned short sampling:2;
		unsigned short padding:1;
		unsigned short priv:1;
		unsigned short mode:2;
		unsigned short mode_ext:2;
		unsigned short copyright:1;
		unsigned short orignal:1;
		unsigned short emphasis:2;
	}MP2_HEADER;
#pragma pack(pop)

#pragma once

DLL_EXPORT mpg123_handle *mp2Decode_Init();
DLL_EXPORT void mp2Decode_Cleanup(mpg123_handle* m=NULL);
DLL_EXPORT void parseMp2Header(MP2_HEADER* pHeader,unsigned char* pBuffer);
DLL_EXPORT int getBitrate(int nId,int nBitrateIdx);
DLL_EXPORT int getBufSize(int nBitrateIdx, int nId);
DLL_EXPORT int mp2_decode(mpg123_handle *m, const unsigned char *mp2buffer, size_t nSrcBufferSize, unsigned char *pcmBuffer, size_t outmemsize, size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* _MP2_DECODE_H_ */