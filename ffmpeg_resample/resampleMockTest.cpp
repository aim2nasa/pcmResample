#include <gtest/gtest.h>
#include "CMockResample.h"

void mockRspTest(int input_stream_id,int output_channels,int input_channels,int nOutSampFreq,int nInpSampFreq)
{
	assert(output_channels==1||output_channels==2);
	IResample *pRsp = new CMockResample();

	int nRtn = pRsp->init(output_channels,input_channels,nOutSampFreq,nInpSampFreq,(AVSampleFormat)0,(AVSampleFormat)0,0,0,0,0.0);
	EXPECT_EQ(nRtn,RSP_OK);

	short inpBuffer[REF_PCM_FRAME_SIZE*2];
	short outBuffer[REF_PCM_FRAME_SIZE*2];

	for(int i=0;i<3;i++){	//프레임 갯수
		inpBuffer[0]=input_stream_id;		//입력스트림에 대한 구분아이디로 0을 지정
		inpBuffer[1]=input_channels;		//스테레오
		CMockResample::writeSampleFreq(inpBuffer+2,nInpSampFreq);

		inpBuffer[4]=CMockResample::getSampleSize(CMockResample::readSampleFreq(inpBuffer+2),inpBuffer[1]);	
		inpBuffer[5]=i; //프레임 번호

		CMockResample::pcmStrInfo pi = CMockResample::parsePcmStrInfo(inpBuffer);
		EXPECT_EQ(pi.id,input_stream_id);
		EXPECT_EQ(pi.ch,input_channels);
		EXPECT_EQ(pi.sf,nInpSampFreq);
		EXPECT_EQ(pi.ns,CMockResample::getSampleSize(48000,2));
		EXPECT_EQ(pi.no,i);

		nRtn = pRsp->resample(outBuffer,inpBuffer,CMockResample::getSampleSize(nInpSampFreq,input_channels));
		EXPECT_EQ(nRtn,CMockResample::getSampleSize(nOutSampFreq,output_channels));

		CMockResample::pcmStrInfo po = CMockResample::parsePcmStrInfo(outBuffer);
	
		EXPECT_EQ(po.id,pi.id+1);	//출력 스트림의 아이디는 입력보다 하나 높다
		EXPECT_EQ(po.ch,output_channels);
		EXPECT_EQ(po.sf,nOutSampFreq);
		EXPECT_EQ(po.ns,nRtn);		//샘플갯수 비교
		EXPECT_EQ(po.no,pi.no);		//입력 스트림의 프레임 번호와 출력스트림의 프레임번호는 같다
	}
	pRsp->close();
	delete pRsp;
}

TEST(pcm_resample_mock_test, resampling)
{
	mockRspTest(0,2,2,16000,48000);
}