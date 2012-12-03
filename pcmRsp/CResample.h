#ifndef __CRESAMPLE_H__
#define __CRESAMPLE_H__

#pragma once
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#define RSP_OK					0
#define RSP_LOAD_LIBRARY_FAIL	-1
#define RSP_AUDIO_CTX_INIT_FAIL	-2
#define RSP_LIBRARY_NOT_LOADED	-3
#define RSP_FREELIBRARY_FAIL	-4

class __declspec(dllexport) CResample {
public:
	CResample();
	virtual ~CResample();

	int init(int out_rate, int in_rate, int filter_length, int log2_phase_count, int linear, double cutoff);
	int close();
	int resample(short *dst, short *src, int *consumed, int src_size, int dst_size, int update_ctx=0);
	
protected:
	int avcodec_link();
	int avcodec_unlink();

protected:
	void*						m_hDll;
	struct AVResampleContext*	m_audio_cntx;

	struct AVResampleContext* (*fp_av_resample_init)(int, int, int, int, int, double);
	int (*fp_av_resample)(struct AVResampleContext *, short *, short *, int *, int, int, int);
	void (*fp_av_resample_close)(struct AVResampleContext *);
};

#endif
