#include <gtest/gtest.h>
#include "CResample.h"

#pragma once
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <libavutil/samplefmt.h>

#define REF_PCM_FRAME_SIZE		(1152*2)	//48khz, short형(2바이트) 기준
#define MONO_SAMPLES_PER_SAMPLE_FREQ	24
#define STEREO_SAMPLES_PER_SAMPLE_FREQ	48

//===================================================================
//Table.1 SampleFreq와 sample size의 관계
//
//SampleFreq(KHz)	Sample size(short형 기준), 바이트=sample sizex2
//48				2304개
//32				1536개
//24				1152개
//16				768개
//
//1KHz당 샘플수 = 48개
//===================================================================

void resampTest(int output_channels,int input_channels,int nOutSampFreq,int nInpSampFreq,const char* pInpFIle,const char* pOutFile);

//Resample from Stereo to Stereo
TEST(pcm_resample_test, From48khz_Stereo_To_16khz_Stereo)
{
	resampTest(2,2,16000,48000,"dump_48khz_Stereo.pcm","resampled_16KHz_stereo.pcm");
}

TEST(pcm_resample_test, From48khz_Stereo_To_24khz_Stereo)
{
	resampTest(2,2,24000,48000,"dump_48khz_Stereo.pcm","resampled_24KHz_stereo.pcm");
}

TEST(pcm_resample_test, From48khz_Stereo_To_32khz_Stereo)
{
	resampTest(2,2,32000,48000,"dump_48khz_Stereo.pcm","resampled_32KHz_stereo.pcm");
}

TEST(pcm_resample_test, From48khz_Stereo_To_48khz_Stereo)
{
	resampTest(2,2,48000,48000,"dump_48khz_Stereo.pcm","resampled_48KHz_stereo.pcm");
}

//Resample from Stereo to Mono
TEST(pcm_resample_test, From48khz_Stereo_To_16khz_Mono)
{
	resampTest(1,2,16000,48000,"dump_48khz_Stereo.pcm","resampled_16KHz_mono.pcm");
}

TEST(pcm_resample_test, From48khz_Stereo_To_24khz_Mono)
{
	resampTest(1,2,24000,48000,"dump_48khz_Stereo.pcm","resampled_24KHz_mono.pcm");
}

TEST(pcm_resample_test, From48khz_Stereo_To_32khz_Mono)
{
	resampTest(1,2,32000,48000,"dump_48khz_Stereo.pcm","resampled_32KHz_mono.pcm");
}

TEST(pcm_resample_test, From48khz_Stereo_To_48khz_Mono)
{
	resampTest(1,2,48000,48000,"dump_48khz_Stereo.pcm","resampled_48KHz_mono.pcm");
}

int getSampleSize(int nSampleFreq,int nChannels)
{
	int nSamples = 0;
	if(nChannels==1) nSamples = MONO_SAMPLES_PER_SAMPLE_FREQ*nSampleFreq/1000;
	if(nChannels==2) nSamples = STEREO_SAMPLES_PER_SAMPLE_FREQ*nSampleFreq/1000;
	return nSamples;
}

void resampTest(int output_channels,int input_channels,int nOutSampFreq,int nInpSampFreq,const char* pInpFIle,const char* pOutFile)
{
	assert(output_channels==1||output_channels==2);

	CResample *pRsp = new CResample();

	int nRtn = pRsp->init(output_channels,input_channels,nOutSampFreq,nInpSampFreq,AV_SAMPLE_FMT_S16,AV_SAMPLE_FMT_S16,16,10,1,0.0);
	EXPECT_EQ(nRtn,RSP_OK);

	FILE* inpFile,*outFile;
	errno_t err;

	err = fopen_s(&inpFile,pInpFIle,"rb");
	EXPECT_EQ(err, 0);

	err = fopen_s(&outFile,pOutFile,"wb");
	EXPECT_EQ(err, 0);

	int nSamples = 0;
	if(output_channels==1) nSamples = MONO_SAMPLES_PER_SAMPLE_FREQ*nInpSampFreq/1000;
	if(output_channels==2) nSamples = STEREO_SAMPLES_PER_SAMPLE_FREQ*nInpSampFreq/1000;

	size_t nSize = 0;
	short inpBuffer[REF_PCM_FRAME_SIZE*2];
	short outBuffer[REF_PCM_FRAME_SIZE*2];
	while(!feof(inpFile)){
		nSize = fread(inpBuffer,sizeof(short),REF_PCM_FRAME_SIZE,inpFile);
		if(nSize!=REF_PCM_FRAME_SIZE) 
			break;

		nRtn = pRsp->resample(outBuffer,inpBuffer,nSamples);
		EXPECT_GT(nRtn,0);
		EXPECT_EQ(nRtn,getSampleSize(nOutSampFreq,output_channels));
		nSize = fwrite(outBuffer,sizeof(short),nRtn,outFile);
		EXPECT_EQ(nSize,nRtn);
	}
	pRsp->close();

	_fcloseall();
	delete pRsp;
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
