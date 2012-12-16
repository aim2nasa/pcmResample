#include "CResample.h"
#include <Windows.h>
#include <assert.h>
#include <stdio.h>

#pragma once
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include "libavcodec/avcodec.h"

CResample::CResample()
:m_hDll(NULL),m_pCtx(NULL),
m_fp_av_audio_resample_init(NULL),m_fp_audio_resample(NULL),m_fp_audio_resample_close(NULL)
{

}

CResample::~CResample()
{
	
}

int CResample::init(int output_channels, int input_channels,int output_rate, int input_rate,
					enum AVSampleFormat sample_fmt_out,
					enum AVSampleFormat sample_fmt_in,
					int filter_length, int log2_phase_count,
					int linear, double cutoff)
{
	assert(output_channels==1||output_channels==2);
	assert(input_channels==1||input_channels==2);
	assert(output_rate>0&&input_rate>0);

	int nRtn = avcodec_link();
	if(nRtn!=RSP_OK) return nRtn;

	assert(m_fp_av_audio_resample_init);
	m_pCtx = m_fp_av_audio_resample_init(output_channels,input_channels,output_rate,input_rate,sample_fmt_out,sample_fmt_in,
		filter_length,log2_phase_count,linear,cutoff);
	if(m_pCtx==NULL) return RSP_AUDIO_CTX_INIT_FAIL;
	return RSP_OK;
}

int CResample::resample(short *output, short *input, int nb_samples)
{
	assert(m_pCtx);
	assert(output);
	assert(input);
	assert(nb_samples>0);
	return m_fp_audio_resample(m_pCtx,output,input,nb_samples);
}

int CResample::close()
{
	m_fp_audio_resample_close(m_pCtx);
	return avcodec_unlink();
}

int CResample::avcodec_link()
{
	m_hDll = reinterpret_cast<void*>(LoadLibrary(TEXT("avcodec.dll")));
	if(m_hDll==NULL)
	{
		m_hDll = reinterpret_cast<void*>(LoadLibraryEx(TEXT("avcodec.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH));
		if(m_hDll==NULL)
		{
			return RSP_LOAD_LIBRARY_FAIL;
		}
	}

	assert(m_hDll);
	HMODULE hDll = reinterpret_cast<HMODULE>(m_hDll);

	m_fp_av_audio_resample_init = ( struct ReSampleContext* (*)(int,int,int,int,enum AVSampleFormat,enum AVSampleFormat,int,int,int,double) ) GetProcAddress(hDll, "av_audio_resample_init");
	m_fp_audio_resample = ( int (*)(struct ReSampleContext*,short*,short*,int) ) GetProcAddress(hDll, "audio_resample");
	m_fp_audio_resample_close = ( void (*)(struct ReSampleContext *) ) GetProcAddress(hDll, "audio_resample_close");
	return RSP_OK;
}

int CResample::avcodec_unlink()
{
	if(m_hDll!=NULL) { 
		HMODULE hDll = reinterpret_cast<HMODULE>(m_hDll);
		if(FreeLibrary(hDll))
			return RSP_OK;
		else
			return RSP_FREELIBRARY_FAIL;
	}
	return RSP_LIBRARY_NOT_LOADED;
}
