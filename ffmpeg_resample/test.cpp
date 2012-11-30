#include <gtest/gtest.h>
#include "CResample.h"

#define REF_PCM_FRAME_SIZE	(1152*2)	//48khz, short형(2바이트) 기준

void resampTest(int nOutSampFreq,int nInpSampFreq,const char* pInpFIle,const char* pOutFile);

TEST(pcm_resample_test, From48khz_To_48khz)
{
	resampTest(48000,48000,"dump_48khz.pcm","resampled_48KHz.pcm");
}

TEST(pcm_resample_test, From48khz_To_16khz)
{
	resampTest(16000,48000,"dump_48khz.pcm","resampled_16KHz.pcm");
}

TEST(pcm_resample_test, From48khz_To_32khz)
{
	resampTest(32000,48000,"dump_48khz.pcm","resampled_32KHz.pcm");
}

TEST(pcm_resample_test, From48khz_To_24khz)
{
	resampTest(24000,48000,"dump_48khz.pcm","resampled_24KHz.pcm");
}

void resampTest(int nOutSampFreq,int nInpSampFreq,const char* pInpFIle,const char* pOutFile)
{
	CResample *pRsp = new CResample();

	int nRtn = pRsp->init(nOutSampFreq,nInpSampFreq,16,10,1,0.0);
	EXPECT_EQ(nRtn,RSP_OK);

	FILE* inpFile,*outFile;
	errno_t err;

	err = fopen_s(&inpFile,pInpFIle,"rb");
	EXPECT_EQ(err, 0);

	err = fopen_s(&outFile,pOutFile,"wb");
	EXPECT_EQ(err, 0);

	int nConsumed = 0;
	size_t nSize = 0;
	short inpBuffer[REF_PCM_FRAME_SIZE];
	short outBuffer[REF_PCM_FRAME_SIZE];
	while(!feof(inpFile)){
		nSize = fread(inpBuffer,sizeof(short),REF_PCM_FRAME_SIZE,inpFile);
		if(nSize!=REF_PCM_FRAME_SIZE) 
			break;

		nRtn = pRsp->resample(outBuffer,inpBuffer,&nConsumed,nSize,REF_PCM_FRAME_SIZE);
		EXPECT_GT(nRtn,0);
		EXPECT_EQ(nSize,nConsumed);
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
