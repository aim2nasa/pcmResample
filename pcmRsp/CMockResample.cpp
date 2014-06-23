#include "CMockResample.h"
#include <assert.h>

#pragma once
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <libavutil/samplefmt.h>

int CMockResample::init(int output_channels, int input_channels,int output_rate, int input_rate,
						enum AVSampleFormat sample_fmt_out,enum AVSampleFormat sample_fmt_in,
						int filter_length, int log2_phase_count,int linear, double cutoff)
{
	m_nInpChannels = input_channels;
	m_nInpRate = input_rate;
	m_nOutChannels = output_channels;
	m_nOutRate = output_rate;
	return RSP_OK;
}

int CMockResample::resample(short *output, short *input, int nb_samples)
{
	assert(nb_samples==CMockResample::getSampleSize(m_nInpRate,m_nInpChannels));
	return CMockResample::getSampleSize(m_nOutRate,m_nOutChannels);
}

int CMockResample::close()
{
	return RSP_OK;
}

int CMockResample::getSampleSize(int nSampleFreq,int nChannels)
{
	assert(nChannels==1||nChannels==2);
	int nSamples = 0;
	if(nChannels==1) nSamples = MONO_SAMPLES_PER_SAMPLE_FREQ*nSampleFreq/1000;
	if(nChannels==2) nSamples = STEREO_SAMPLES_PER_SAMPLE_FREQ*nSampleFreq/1000;
	return nSamples;
}

CMockResample::pcmStrInfo CMockResample::parsePcmStrInfo(short *pPcmStr)
{
	pcmStrInfo info;
	info.id = pPcmStr[0];
	info.ch = pPcmStr[1];
	info.sf = readSampleFreq(pPcmStr+2);
	info.ns = pPcmStr[4];
	info.no = pPcmStr[5];
	return info;
}

void CMockResample::writeSampleFreq(short *pStr,int nSf)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(pStr);
	p[0] = (nSf&0xff);
	p[1] = (nSf&0xff00)>>8;
	p[2] = (nSf&0xff0000)>>16;
	p[3] = (nSf&0xff000000)>>24;
}

int CMockResample::readSampleFreq(short *pStr)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(pStr);
	return (((int)p[3])<<24)|(((int)p[2])<<16)|(((int)p[1])<<8)|(int)p[0];
}