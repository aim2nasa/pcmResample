#include <stdio.h>
#include <assert.h>
#include "CResample.h"

#include <gtest/gtest.h>

#define COMPARE_BUF_SIZE	255

/* Globals */
FILE* infile = 0;
FILE* outfile = 0;

int i=0, j=0;
long file_len = 0;
char* filename = "dump.pcm";
double angle = 0.0;
int ret;
int samples_output=0;
size_t bytes_read = 0;
size_t total_bytes_read = 0;
char in_buffer[(LENGTH_MS*RATE*16*CHANNELS)/8000];
int filecomp(char *src_name, char *dst_name);

TEST(pcm_resample_test, init_and_close){
	
	CResample *res = new CResample;

	ret = res->pcm_resample_init(48000, 32000);

	EXPECT_EQ(ret,0);

	res->pcm_resample_close();

	ret = res->pcm_resample_init(48000, 24000);

	EXPECT_EQ(ret,0);

	res->pcm_resample_close();

	ret = res->pcm_resample_init(48000, 16000);

	EXPECT_EQ(ret,0);

	res->pcm_resample_close();

	delete res;
}

TEST(pcm_resample_test, pcm_resample_16KHz){

	CResample *res = new CResample;
	
	char out_buffer[ sizeof( in_buffer ) / 3 ];	// 16KHz
	int out_buffer_size = sizeof( out_buffer );
	int out_rate = 16000;
	j=0;

	// Open audio file.
	errno_t err = fopen_s(&infile, filename, "rb");
	EXPECT_EQ(err, 0);
	
	fseek( infile, 0, SEEK_END );
	file_len = ftell( infile );
	fseek( infile, 0, SEEK_SET );

	err = fopen_s(&outfile, "dump_dest_16KHz.pcm", "wb");
	EXPECT_EQ(err, 0);

	ret = res->pcm_resample_init(48000, out_rate);
	EXPECT_EQ(ret,0);

	// Loop through the file until all data is read.
	for( ; !feof( infile ) && !ferror( infile ); )
	{
		// Read the next chunk of data from the file.
		bytes_read = fread( in_buffer, 1, sizeof( in_buffer ), infile );
		assert( !ferror( infile ) && "Error reading audio file!" );
		total_bytes_read += bytes_read;
		samples_output = res->pcmFileResample(bytes_read, in_buffer, out_buffer, out_buffer_size);

		fwrite (out_buffer , 1 , sizeof(out_buffer) , outfile );

		// Display progress text.
		printf( "\r48KHz->16KHz Progress: %2.1f%% Complete, Skew: %2.1f%%, Angle: %2.1f Degrees       ", 
			((float)total_bytes_read/(float)file_len)*100.0,
			((float)(out_rate)/(float)RATE)*100.0,
			angle );
		fflush( stdout );
	}

	fclose( infile );
	fclose( outfile );

	res->pcm_resample_close();
	printf( "\nDone!\n" );

	filecomp("dump-resampled_16KHz.pcm", "dump_dest_16KHz.pcm");

	delete res;
}

TEST(pcm_resample_test, pcm_resample_24KHz){

	CResample *res = new CResample;

	char out_buffer[ sizeof( in_buffer ) / 2 ];	// 24KHz
	int out_buffer_size = sizeof( out_buffer );
	int out_rate = 24000;
	j=0;
	// Open audio file.
	errno_t err = fopen_s(&infile, filename, "rb");
	EXPECT_EQ(err, 0);
	fseek( infile, 0, SEEK_END );
	file_len = ftell( infile );
	fseek( infile, 0, SEEK_SET );

	err = fopen_s(&outfile, "dump_dest_24KHz.pcm", "wb");
	EXPECT_EQ(err, 0);

	ret = res->pcm_resample_init(48000, out_rate);
	EXPECT_EQ(ret,0);

	// Loop through the file until all data is read.
	for( ; !feof( infile ) && !ferror( infile ); )
	{
		// Read the next chunk of data from the file.
		bytes_read = fread( in_buffer, 1, sizeof( in_buffer ), infile );
		assert( !ferror( infile ) && "Error reading audio file!" );
		total_bytes_read += bytes_read;
		samples_output = res->pcmFileResample(bytes_read, in_buffer, out_buffer, out_buffer_size);

		fwrite (out_buffer , 1 , sizeof(out_buffer) , outfile );

		// Display progress text.
		printf( "\r48KHz->24KHz Progress: %2.1f%% Complete, Skew: %2.1f%%, Angle: %2.1f Degrees       ", 
			((float)total_bytes_read/(float)file_len)*100.0,
			((float)(out_rate)/(float)RATE)*100.0,
			angle );
		fflush( stdout );
	}

	fclose( infile );
	fclose( outfile );

	res->pcm_resample_close();
	printf( "\nDone!\n" );

	filecomp("dump-resampled_24KHz.pcm", "dump_dest_24KHz.pcm");

	delete res;
}

TEST(pcm_resample_test, pcm_resample_32KHz){

	CResample *res = new CResample;

	char out_buffer[ sizeof( in_buffer ) * 10 / 15 ]; // 32KHz
	int out_buffer_size = sizeof( out_buffer );
	int out_rate = 32000;
	j=0;

	// Open audio file.
	errno_t err = fopen_s(&infile, filename, "rb");
	EXPECT_EQ(err, 0);
	fseek( infile, 0, SEEK_END );
	file_len = ftell( infile );
	fseek( infile, 0, SEEK_SET );

	err = fopen_s(&outfile, "dump_dest_32KHz.pcm", "wb");
	EXPECT_EQ(err, 0);

// 	err = fopen_s(&samplefile, "dump-resampled_32KHz.pcm", "rb");
// 	EXPECT_EQ(err, 0);

	ret = res->pcm_resample_init(48000, out_rate);
	EXPECT_EQ(ret,0);

	// Loop through the file until all data is read.
	for( ; !feof( infile ) && !ferror( infile ); )
	{
		// Read the next chunk of data from the file.
		bytes_read = fread( in_buffer, 1, sizeof( in_buffer ), infile );
		assert( !ferror( infile ) && "Error reading audio file!" );
		total_bytes_read += bytes_read;
		samples_output = res->pcmFileResample(bytes_read, in_buffer, out_buffer, out_buffer_size);

		fwrite (out_buffer , 1 , sizeof(out_buffer) , outfile );

		// Display progress text.
		printf( "\r48KHz->32KHz Progress: %2.1f%% Complete, Skew: %2.1f%%, Angle: %2.1f Degrees       ", 
			((float)total_bytes_read/(float)file_len)*100.0,
			((float)(out_rate)/(float)RATE)*100.0,
			angle );
		fflush( stdout );
	}

	fclose( infile );
	fclose( outfile );
//	fclose( samplefile );

	res->pcm_resample_close();
	printf( "\nDone!\n" );

	filecomp("dump-resampled_32KHz.pcm", "dump_dest_32KHz.pcm");

	delete res;
}

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

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

	return 0;
};
