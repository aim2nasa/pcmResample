#ifndef __CRESAMPLE_H__
#define __CRESAMPLE_H__

#pragma once
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <windows.h>

#define LENGTH_MS 500		// how many milliseconds of speech to store
#define RATE 48000		// the sampling rate
#define CHANNELS 2		// 1 = mono 2 = stereo

#define RSP_OK					0
#define RSP_LOAD_LIBRARY_FAIL	-1
#define RSP_AUDIO_CTX_INIT_FAIL	-2
#define RSP_LIBRARY_NOT_LOADED	-3
#define RSP_FREELIBRARY_FAIL	-4

class __declspec(dllexport) CResample {
public:
	CResample();
	virtual ~CResample();

	int init(int in_rate, int out_rate);
	int close();
	int resample(size_t bytes_read, char* p_in_buffer, char* p_out_buffer, int out_buffer_size);
	
protected:
	int avcodec_link();
	int avcodec_unlink();

protected:
	// Dynamic Linking
	HMODULE						m_hDll;
	struct AVResampleContext*	m_audio_cntx;
	int							m_samples_consumed;

	struct AVResampleContext* (*test_av_resample_init)(int, int, int, int, int, double);
	int (*test_av_resample)(struct AVResampleContext *, short *, short *, int *, int, int, int);
	void (*test_av_resample_close)(struct AVResampleContext *);
	void (*test_avcodec_register_all)(void);
	void *(*test_av_malloc)(size_t);
	void (*test_av_freep)(void *);
};

#endif
