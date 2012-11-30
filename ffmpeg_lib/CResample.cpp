#include "CResample.h"
#include <assert.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

CResample::CResample()
:m_hDll(NULL),m_audio_cntx(NULL),m_samples_consumed(0),
test_av_resample_init(NULL),test_av_resample(NULL),test_av_resample_close(NULL),
test_avcodec_register_all(NULL),test_av_malloc(NULL),test_av_freep(NULL)
{

}

CResample::~CResample()
{
	
}

int CResample::init(int in_rate, int out_rate)
{
	assert(in_rate>0&&out_rate>0);

	// DLL Link
	int nRtn = avcodec_link();
	if(nRtn!=RSP_OK) return nRtn;

	// Initialize ffmpeg resampling.
	test_avcodec_register_all();
	m_audio_cntx = test_av_resample_init( out_rate,		// out rate
		in_rate,										// in rate
		16,												// filter length
		10,												// phase count
		1,												// linear FIR filter
		1.0 );											// cutoff frequency
	if(m_audio_cntx==NULL) return RSP_AUDIO_CTX_INIT_FAIL;
	return RSP_OK;
}

int CResample::close()
{
	// DLL Unlink
	return avcodec_unlink();
}

int CResample::resample(size_t bytes_read, char* p_in_buffer, char* p_out_buffer, int out_buffer_size)
{
	assert(m_audio_cntx);
	assert(p_in_buffer);
	assert(p_out_buffer);
	assert(out_buffer_size>0);

	return test_av_resample( m_audio_cntx,
							(short*)p_out_buffer,								// dst
							(short*)p_in_buffer,								// src
							&m_samples_consumed,								// consumed
							bytes_read / 2,										// src_size
							out_buffer_size / 2,								// dst_size
							0 );												// update_ctx
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
	test_av_resample_init = ( struct AVResampleContext* (*)(int, int, int, int, int, double) ) GetProcAddress(m_hDll, "av_resample_init");
	test_av_resample = ( int (*)(struct AVResampleContext *, short *, short *, int *, int, int, int) ) GetProcAddress(m_hDll, "av_resample");
	test_av_resample_close = ( void (*)(struct AVResampleContext *) ) GetProcAddress(m_hDll, "av_resample_close");
	test_avcodec_register_all = ( void (*)(void) ) GetProcAddress(m_hDll, "avcodec_register_all");
	test_av_malloc = (void* (*)(size_t)) GetProcAddress(m_hDll, "av_malloc");
	test_av_freep = (void (*)(void *)) GetProcAddress(m_hDll, "av_freep");
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
