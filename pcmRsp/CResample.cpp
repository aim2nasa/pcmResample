#include "CResample.h"
#include <assert.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"

CResample::CResample()
:m_hDll(NULL),m_audio_cntx(NULL),
fp_av_resample_init(NULL),fp_av_resample(NULL),fp_av_resample_close(NULL)
{

}

CResample::~CResample()
{
	
}

int CResample::init(int out_rate, int in_rate, int filter_length, int log2_phase_count, int linear, double cutoff)
{
	assert(in_rate>0&&out_rate>0);

	int nRtn = avcodec_link();
	if(nRtn!=RSP_OK) return nRtn;

	m_audio_cntx = fp_av_resample_init(out_rate,in_rate,filter_length,log2_phase_count,linear,cutoff);
	if(m_audio_cntx==NULL) return RSP_AUDIO_CTX_INIT_FAIL;
	return RSP_OK;
}

int CResample::close()
{
	fp_av_resample_close(m_audio_cntx);
	return avcodec_unlink();
}

int CResample::resample(short *dst, short *src, int *consumed, int src_size, int dst_size, int update_ctx)
{
	assert(m_audio_cntx);
	assert(dst&&src);
	assert(src_size>0&&dst_size>0);
	return fp_av_resample(m_audio_cntx,dst,src,consumed,src_size,dst_size,update_ctx);
}

int CResample::avcodec_link()
{
	m_hDll = LoadLibrary(TEXT("avcodec.dll"));
	if(m_hDll==NULL)
	{
		m_hDll = LoadLibraryEx(TEXT("avcodec.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if(m_hDll==NULL)
		{
			return RSP_LOAD_LIBRARY_FAIL;
		}
	}

	assert(m_hDll);
	fp_av_resample_init = ( struct AVResampleContext* (*)(int, int, int, int, int, double) ) GetProcAddress(m_hDll, "av_resample_init");
	fp_av_resample = ( int (*)(struct AVResampleContext *, short *, short *, int *, int, int, int) ) GetProcAddress(m_hDll, "av_resample");
	fp_av_resample_close = ( void (*)(struct AVResampleContext *) ) GetProcAddress(m_hDll, "av_resample_close");
	return RSP_OK;
}

int CResample::avcodec_unlink()
{
	if(m_hDll!=NULL) { 
		if(FreeLibrary(m_hDll))
			return RSP_OK;
		else
			return RSP_FREELIBRARY_FAIL;
	}
	return RSP_LIBRARY_NOT_LOADED;
}
