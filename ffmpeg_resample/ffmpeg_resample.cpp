#include <stdio.h>
#include <assert.h>
#include "CResample.h"

/* Globals */
FILE* infile = 0;
FILE* outfile = 0;
long file_len = 0;

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Argument Failed!\n");
		return -1;
	}
	char* filename = 0;
	CResample *res = new CResample;
	double angle = 0.0;

	size_t bytes_read = 0;
	size_t total_bytes_read = 0;
	/* this buffer holds the digitized audio */
	char in_buffer[(LENGTH_MS*RATE*16*CHANNELS)/8000];
	//char out_buffer[ sizeof( in_buffer ) / 3 ];	// 16KHz
	//char out_buffer[ sizeof( in_buffer ) / 2 ];	// 24KHz
	char out_buffer[ sizeof( in_buffer ) * 10 / 15 ]; // 32KHz
	int out_rate = 32000;
	int out_buffer_size = sizeof( out_buffer );
	filename = argv[1];

	res->pcm_resample_init(48000, out_rate);

	// Open audio file.
	errno_t err = fopen_s(&infile, filename, "rb");
	if(err!=0)
	{
		printf("Source File Open Error!!\n");
		return -2;
	}
	fseek( infile, 0, SEEK_END );
	file_len = ftell( infile );
	fseek( infile, 0, SEEK_SET );

	err = fopen_s(&outfile, "dest.pcm", "wb");
	if(err!=0)
	{
		printf("Dest File Open Error!!\n");
		return -3;
	}

	// Loop through the file until all data is read.
	for( ; !feof( infile ) && !ferror( infile ); )
	{
		// Read the next chunk of data from the file.
		bytes_read = fread( in_buffer, 1, sizeof( in_buffer ), infile );
		assert( !ferror( infile ) && "Error reading audio file!" );
		total_bytes_read += bytes_read;
		res->pcmFileResample(bytes_read, in_buffer, out_buffer, out_buffer_size);

		fwrite (out_buffer , 1 , sizeof(out_buffer) , outfile );

		// Display progress text.
		printf( "\rProgress: %2.1f%% Complete, Skew: %2.1f%%, Angle: %2.1f Degrees       ", 
			((float)total_bytes_read/(float)file_len)*100.0,
			((float)(out_rate)/(float)RATE)*100.0,
			angle );
		fflush( stdout );
	}

	res->pcm_resample_close();



	return 0;
};