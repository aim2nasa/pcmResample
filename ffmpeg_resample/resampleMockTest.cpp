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

	for(int i=0;i<3;i++){	//������ ����
		inpBuffer[0]=input_stream_id;		//�Է½�Ʈ���� ���� ���о��̵�� 0�� ����
		inpBuffer[1]=input_channels;		//���׷���
		CMockResample::writeSampleFreq(inpBuffer+2,nInpSampFreq);

		inpBuffer[4]=CMockResample::getSampleSize(CMockResample::readSampleFreq(inpBuffer+2),inpBuffer[1]);	
		inpBuffer[5]=i; //������ ��ȣ

		CMockResample::pcmStrInfo pi = CMockResample::parsePcmStrInfo(inpBuffer);
		EXPECT_EQ(pi.id,input_stream_id);
		EXPECT_EQ(pi.ch,input_channels);
		EXPECT_EQ(pi.sf,nInpSampFreq);
		EXPECT_EQ(pi.ns,CMockResample::getSampleSize(48000,2));
		EXPECT_EQ(pi.no,i);

		nRtn = pRsp->resample(outBuffer,inpBuffer,CMockResample::getSampleSize(nInpSampFreq,input_channels));
		EXPECT_EQ(nRtn,CMockResample::getSampleSize(nOutSampFreq,output_channels));

		CMockResample::pcmStrInfo po = CMockResample::parsePcmStrInfo(outBuffer);
	
		EXPECT_EQ(po.id,pi.id+1);	//��� ��Ʈ���� ���̵�� �Էº��� �ϳ� ����
		EXPECT_EQ(po.ch,output_channels);
		EXPECT_EQ(po.sf,nOutSampFreq);
		EXPECT_EQ(po.ns,nRtn);		//���ð��� ��
		EXPECT_EQ(po.no,pi.no);		//�Է� ��Ʈ���� ������ ��ȣ�� ��½�Ʈ���� �����ӹ�ȣ�� ����
	}
	pRsp->close();
	delete pRsp;
}

TEST(pcm_resample_mock_test, resampling)
{
	mockRspTest(0,2,2,16000,48000);
}