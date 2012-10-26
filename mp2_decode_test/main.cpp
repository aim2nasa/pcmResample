#include <iostream>
#include <gtest/gtest.h>
#include "../mp2_decode/mp2_decode.h"

#define INBUFF  16384
#define OUTBUFF 32768 

#define COMPARE_BUF_SIZE	255

unsigned char mp2buffer[INBUFF];
unsigned char pcmBuffer[OUTBUFF];
size_t size = 0;

FILE *inpStream,*outStream;
errno_t err;

int filecomp(char *src_name, char *dst_name)
{
	size_t src_bytes_read=0;
	size_t dst_bytes_read=0;

	unsigned int idx=0;
	FILE *src_file =0;
	FILE *dst_file =0;

	char *src_buffer = (char*) malloc (COMPARE_BUF_SIZE);
	char *dst_buffer = (char*) malloc (COMPARE_BUF_SIZE);

	long srcSize=0;
	long dstSize=0;

	errno_t err = fopen_s(&src_file, src_name, "rb");
	EXPECT_EQ(err, 0);

	// obtain file size:
	fseek (src_file , 0 , SEEK_END);
	srcSize = ftell (src_file);
	rewind (src_file);

	err = fopen_s(&dst_file, dst_name, "rb");
	EXPECT_EQ(err, 0);

	// obtain file size:
	fseek (dst_file , 0 , SEEK_END);
	dstSize = ftell (dst_file);
	rewind (dst_file);

	EXPECT_EQ(srcSize, dstSize);

	printf("Compare Start!\n");

	// Loop through the file until all data is read.
	for( ; !feof( dst_file ) && !ferror( dst_file ); )
	{
		src_bytes_read = fread( src_buffer, sizeof(char), COMPARE_BUF_SIZE, src_file );
		dst_bytes_read = fread( dst_buffer, sizeof(char), COMPARE_BUF_SIZE, dst_file );
		EXPECT_EQ(src_bytes_read, dst_bytes_read);

		for (idx=0;idx<COMPARE_BUF_SIZE;idx++)
		{
			if((char)src_buffer[idx] != (char)dst_buffer[idx])
			{
				printf("|%d|(%02x:%02x)\n", idx, src_buffer[idx], dst_buffer[idx]);
			}
		}
		idx=0;
	}
	printf("Compare Done!\n");

	fclose( src_file );
	fclose( dst_file );

	free(src_buffer);
	free(dst_buffer);

	return 0;
}

TEST(mp2_decoder_test, mp2_decoder_48KHz){
	mpg123_handle *m = mp2Decode_Init();
	if(m == NULL) EXPECT_EQ(0,1);

	if( (err = fopen_s( &inpStream,"../contents/dab_48khz_jointstereo_96_subCh(1).mp2","rb" )) !=0 ) {
		mp2Decode_Cleanup();
	}
	ASSERT_EQ(err, 0);
	if( (err = fopen_s( &outStream,"dab_48khz_jointstereo_96_subCh(1).pcm","wb" )) !=0 ) {
	}
	ASSERT_EQ(err, 0);

	MP2_HEADER header;
	int nFrameCount =0;

	while( !feof( inpStream ) ) {
		size_t nRead = fread(mp2buffer,sizeof(unsigned char),4,inpStream);	//read header only
		if(nRead&&nRead!=4) continue;	//EOF
		parseMp2Header(&header,mp2buffer);

		EXPECT_EQ(header.sync, 4095);
		EXPECT_EQ(header.id, 1);
		EXPECT_EQ(header.layer, 2);
		EXPECT_EQ(header.protect, 0);
		EXPECT_EQ(header.bitrateIdx, 6);
		EXPECT_EQ(header.sampling, 1);
		EXPECT_EQ(header.padding, 0);
		EXPECT_EQ(header.priv, 0);
		EXPECT_EQ(header.mode_ext, 0);
		EXPECT_EQ(header.copyright, 0);
		EXPECT_EQ(header.orignal, 1);
		EXPECT_EQ(header.emphasis, 0);

		int nSrcBufferSize = getBufSize(header.bitrateIdx, header.id);
		//int nSrcBufferSize = 144*getBitrate(header.id,header.bitrateIdx)/((header.id)?48000:24000);
		nRead = fread(mp2buffer+4,sizeof(unsigned char),nSrcBufferSize-4,inpStream);	//read rest of the frame
		if(nRead&&nRead!=(nSrcBufferSize-4)) continue;	//EOF

		size_t nWrite = 0;
		int ret = mp2_decode(m,mp2buffer,nSrcBufferSize,pcmBuffer,OUTBUFF,&size);
		EXPECT_NE(ret, MPG123_ERR);

		nWrite = fwrite(pcmBuffer,sizeof(unsigned char),size,outStream);
		EXPECT_EQ(nWrite, size);

		fprintf(stderr, "[%04i", ++nFrameCount);
		fprintf(stderr, "]\r");
		fflush(stderr);
	}

	fclose(inpStream);
	fclose(outStream);

	filecomp("../contents/dab_48khz_jointstereo_96_subCh(1)_comp.pcm", "dab_48khz_jointstereo_96_subCh(1).pcm");
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
