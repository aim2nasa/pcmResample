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

class CResample {
public:
	CResample();
	virtual ~CResample();

	int pcm_resample_init(int in_rate, int out_rate);
	int pcm_resample_close();
	int pcmFileResample(size_t bytes_read, char* p_in_buffer, char* p_out_buffer, int out_buffer_size);
	
private:
	int avcodec_link();
	void avcodec_unlink();

private:
	// Dynamic Linking
	HMODULE m_hDll;
	struct AVResampleContext* m_audio_cntx;
	int m_samples_consumed;

	struct AVResampleContext* (*test_av_resample_init)(int, int, int, int
		, int, double);
	int (*test_av_resample)(struct AVResampleContext *, short *, short *, int *, int, int, int);
	void (*test_av_resample_close)(struct AVResampleContext *);
	void (*test_avcodec_register_all)(void);
	void *(*test_av_malloc)(size_t);
	void (*test_av_freep)(void *);
};

#endif
