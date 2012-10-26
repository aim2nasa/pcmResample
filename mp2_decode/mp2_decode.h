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

#pragma once

DLL_EXPORT int mp2Decode_Init();
DLL_EXPORT void mp2Decode_Cleanup(mpg123_handle* m=NULL);

#ifdef __cplusplus
}
#endif

#endif /* _MP2_DECODE_H_ */