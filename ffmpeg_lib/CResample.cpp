#include <Windows.h>
#include "CResample.h"
#include <assert.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

CResample::CResample()
{
	m_hDll = NULL;
	m_audio_cntx = 0;
	m_samples_consumed = 0;
}

CResample::~CResample()
{
	
}

int CResample::pcm_resample_init(int in_rate, int out_rate)
{
	assert( in_rate == 48000 && "Doesn't support input rate!" );
	assert( out_rate == 32000 || out_rate == 24000 || out_rate == 16000 && "Doesn't support output rate!" );

	// DLL Link
	avcodec_link();

	// Initialize ffmpeg resampling.
	test_avcodec_register_all();

	m_audio_cntx = test_av_resample_init( out_rate,		// out rate
		in_rate,										// in rate
		16,												// filter length
		10,												// phase count
		1,												// linear FIR filter
		1.0 );											// cutoff frequency
	assert( m_audio_cntx && "Failed to create resampling context!" );

	// 따로 분리
	//char out_buffer[ sizeof( in_buffer ) / 3 ];		// 16KHz
	//char out_buffer[ sizeof( in_buffer ) / 2 ];		// 24KHz
	//char out_buffer[ sizeof( in_buffer ) * 10 / 15 ]; // 32KHz

	return 0;
}

int CResample::pcm_resample_close()
{
	// DLL Unlink
	avcodec_unlink();

	return 0;
}

int CResample::pcmFileResample(size_t bytes_read, char* p_in_buffer, char* p_out_buffer, int out_buffer_size)
{
	int samples_output = test_av_resample( m_audio_cntx,
		(short*)p_out_buffer,								// dst
		(short*)p_in_buffer,								// src
		&m_samples_consumed,								// consumed
		bytes_read / 2,										// src_size
		out_buffer_size / 2,								// dst_size
		0 );												// update_ctx
	assert( samples_output > 0 && "Error calling av_resample()!" );

	return samples_output;
}

int CResample::avcodec_link()
{
	m_hDll = LoadLibrary(TEXT("avcodec.dll"));
	if(m_hDll==NULL)
	{
		m_hDll = LoadLibraryEx(TEXT("avcodec.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if(m_hDll==NULL)
		{
			printf("GetLastErrorCode(%d)\n", GetLastError());
		}
	}
	assert(m_hDll && "Load 'avcodec.dll' library Failed!!");

	test_av_resample_init = ( struct AVResampleContext* (*)(int, int, int, int
		, int, double) ) GetProcAddress(m_hDll, "av_resample_init");
	test_av_resample = ( int (*)(struct AVResampleContext *, short *, short *, int *, int, int, int) ) GetProcAddress(m_hDll, "av_resample");
	test_av_resample_close = ( void (*)(struct AVResampleContext *) ) GetProcAddress(m_hDll, "av_resample_close");
	test_avcodec_register_all = ( void (*)(void) ) GetProcAddress(m_hDll, "avcodec_register_all");
	test_av_malloc = (void* (*)(size_t)) GetProcAddress(m_hDll, "av_malloc");
	test_av_freep = (void (*)(void *)) GetProcAddress(m_hDll, "av_freep");

	return 0;
}

void CResample::avcodec_unlink()
{
	if(m_hDll != NULL)
		FreeLibrary(m_hDll);
}
