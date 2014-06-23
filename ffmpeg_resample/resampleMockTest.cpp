#include <gtest/gtest.h>
#include "CMockResample.h"

void mockRspTest(int output_channels,int input_channels,int nOutSampFreq,int nInpSampFreq)
{
	assert(output_channels==1||output_channels==2);
	IResample *pRsp = new CMockResample();

	int nRtn = pRsp->init(output_channels,input_channels,nOutSampFreq,nInpSampFreq,(AVSampleFormat)0,(AVSampleFormat)0,0,0,0,0.0);
	EXPECT_EQ(nRtn,RSP_OK);

	short inpBuffer[REF_PCM_FRAME_SIZE*2];
	short outBuffer[REF_PCM_FRAME_SIZE*2];

	for(int i=0;i<100;i++){	//프레임 갯수
		inpBuffer[0]=0;		//입력스트림에 대한 구분아이디로 0을 지정
		inpBuffer[1]=2;		//스테레오
		CMockResample::writeSampleFreq(inpBuffer+2,48000);

		inpBuffer[4]=CMockResample::getSampleSize(CMockResample::readSampleFreq(inpBuffer+2),inpBuffer[1]);	
		inpBuffer[5]=i;

		CMockResample::pcmStrInfo pi = CMockResample::parsePcmStrInfo(inpBuffer);
		EXPECT_EQ(pi.id,0);
		EXPECT_EQ(pi.ch,2);
		EXPECT_EQ(pi.sf,48000);
		EXPECT_EQ(pi.ns,CMockResample::getSampleSize(48000,2));
		EXPECT_EQ(pi.no,i);

		nRtn = pRsp->resample(outBuffer,inpBuffer,CMockResample::getSampleSize(nInpSampFreq,input_channels));
		EXPECT_EQ(nRtn,CMockResample::getSampleSize(nOutSampFreq,output_channels));
	}
	pRsp->close();
	delete pRsp;
}

TEST(pcm_resample_mock_test, first)
{
	mockRspTest(2,2,16000,48000);
}