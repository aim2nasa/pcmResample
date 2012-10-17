
/*
* 1. Compile FFmpeg without "--enable-gpl" and without "--enable-nonfree".
* 2. Use dynamic linking (on windows, this means linking to dlls) for linking with FFmpeg libraries.
* 3. Distribute the source code of FFmpeg, no matter if you modified it or not.
* 4. Make sure the source code corresponds exactly to the library binaries you are distributing.
* 5. Run the command "git diff > changes.diff" in the root directory of the FFmpeg source code to create a file with only the changes.
* 6. Explain how you compiled FFmpeg, for example the configure line, in a text file added to the root directory of the source code.
* 7. Use tarball or a zip file for distributing the source code.
* 8. Host the FFmpeg source code on the same webserver as the binary you are distributing.
* 9. Add "This software uses code of <a href=http://ffmpeg.org>FFmpeg</a> licensed under the <a href=http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>LGPLv2.1</a> and its source can be downloaded <a href=link_to_your_sources>here</a>" to every page in your website where there is a download link to your application.
* 10. Mention "This software uses libraries from the FFmpeg project under the LGPLv2.1" in your program "about box".
* 11. Mention in your EULA that your program uses FFmpeg under the LGPLv2.1.
* 12. If your EULA claims ownership over the code, you have to explicitly mention that you do not own FFmpeg, and where the relevant owners can be found.
* 13. Remove any prohibition of reverse engineering from your EULA.
* 14. Apply the same changes to all translations of your EULA.
* 15. Do not misspell FFmpeg (two capitals F and lowercase "mpeg").
* 16. Do not rename FFmpeg dlls to some obfuscated name, but adding a suffix or prefix is fine (renaming "avcodec.dll" to "MyProgDec.dll" is not fine, but to "avcodec-MyProg.dll" is).
* 17. Go through all the items again for any LGPL external library you compiled into FFmpeg (for example LAME).
* 18. Make sure your program is not using any GPL libraries (notably libx264).
*/

#pragma once
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
#include <stdlib.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include <windows.h>
#include <math.h>
#include <time.h>
#include "gettime.h"
#include <assert.h>

#define LENGTH_MS 500		// how many milliseconds of speech to store
#define RATE 44000		// the sampling rate
#define CHANNELS 2		// 1 = mono 2 = stereo
#define ANGLE_STEP_PER_SEC 15.0	// degrees per second

#define DEG2RAD( angle ) (((angle)/180.0)*3.14159265)

/* Globals */
FILE* infile = 0;
FILE* outfile = 0;
char* filename = 0;
int rate_skew = 0; // Num samples to skew
long file_len = 0;
struct AVResampleContext* audio_cntx = 0;
double angle = 0.0;
double prev_time = 0.0;

/* this buffer holds the digitized audio */
char in_buffer[(LENGTH_MS*RATE*16*CHANNELS)/8000];
char out_buffer[ sizeof( in_buffer ) / 2/* * 4*/];

int dllLinkTest();
int audioResampleTest();
int calc_skew();

int main(int argc, char *argv[])
{
	if(argc <= 2)
	{
		printf("Argument Failed!\n");
		return -1;
	}

	filename = argv[1];
	rate_skew = atoi(argv[2]);

	int ret = dllLinkTest();
	if(ret!=0)
	{
		printf("dllLinkTest Failed!\n");
	}
	else
	{
		ret = audioResampleTest();
		if(ret!=0)
		{
			printf("audioResampleTest Failed!\n");
		}
	}


	return ret;
};

int dllLinkTest()
{
	// Dynamic Linking
	HMODULE hDll = NULL;
	ReSampleContext* (*test_av_audio_resample_init)(int, int, int, int
		, enum AVSampleFormat, enum AVSampleFormat, int, int, int, double) = NULL;
	void (*test_audio_resample_close)(ReSampleContext *) = NULL;

	hDll = LoadLibrary(TEXT("../bin/avcodec.dll"));

	assert(hDll && "[ERR] Load Library Failed!!");

	test_av_audio_resample_init = ( ReSampleContext* (*)(int, int, int, int
		, enum AVSampleFormat, enum AVSampleFormat, int, int, int, double) ) GetProcAddress(hDll, "av_audio_resample_init");
	test_audio_resample_close = ( void (*)(ReSampleContext *) ) GetProcAddress(hDll, "audio_resample_close");

	// av_audio_resample_init, audio_resample_close
	ReSampleContext* resampleCtx = test_av_audio_resample_init(2, 2,
		48000, 24000,
		AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16,
		16, 10, 1, 0.8);
	assert(resampleCtx && "[ERR] Audio Resample Init Failed!!");

	test_audio_resample_close(resampleCtx);

	printf("ffmpeg test OK!\n");

	FreeLibrary(hDll);
	return 0;
}

int audioResampleTest()
{
	// Dynamic Linking
	HMODULE hDll = NULL;
	struct AVResampleContext* (*test_av_resample_init)(int, int, int, int
		, int, double) = NULL;
	int (*test_av_resample)(struct AVResampleContext *, short *, short *, int *, int, int, int) = NULL;
	void (*test_av_resample_close)(struct AVResampleContext *)=NULL;
	void (*test_avcodec_register_all)(void)=NULL;

	hDll = LoadLibrary(TEXT("../bin/avcodec.dll"));

	if(hDll == NULL)
	{
		printf("[ERR] Load Library Failed!!\n");
		return -1;
	}

	test_av_resample_init = ( struct AVResampleContext* (*)(int, int, int, int
		, int, double) ) GetProcAddress(hDll, "av_resample_init");
	test_av_resample = ( int (*)(struct AVResampleContext *, short *, short *, int *, int, int, int) ) GetProcAddress(hDll, "av_resample");
	test_av_resample_close = ( void (*)(struct AVResampleContext *) ) GetProcAddress(hDll, "av_resample_close");
	test_avcodec_register_all = ( void (*)(void) ) GetProcAddress(hDll, "avcodec_register_all");

	// Initialize ffmpeg resampling.
	test_avcodec_register_all();

	size_t bytes_read = 0;
	size_t total_bytes_read = 0;

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

		// Resample audio.
		int out_rate = 16000/*RATE + calc_skew()*/;
		audio_cntx = test_av_resample_init( out_rate,		// out rate
			RATE,											// in rate
			16,												// filter length
			10,												// phase count
			1,												// linear FIR filter
			0.8 );											// cutoff frequency
		assert( audio_cntx && "Failed to create resampling context!" );

		int samples_consumed = 0;
		int samples_output = test_av_resample( audio_cntx,
			(short*)out_buffer,
			(short*)in_buffer,
			&samples_consumed,
			bytes_read / 2,
			sizeof( out_buffer ) / 2, // in samples
			0 );
		assert( samples_output > 0 && "Error calling av_resample()!" );

		test_av_resample_close( audio_cntx );

		fwrite (out_buffer , 1 , sizeof(out_buffer) , outfile );
		// Play the processed samples.
// 		pa_simple_write( pulse_conn,
// 			out_buffer,
// 			samples_output * 2,
// 			0 );

		// Display progress text.
		printf( "\rProgress: %2.1f%% Complete, Skew: %2.1f%%, Angle: %2.1f Degrees       ", 
			((float)total_bytes_read/(float)file_len)*100.0,
			((float)(out_rate)/(float)RATE)*100.0,
			angle );
		fflush( stdout );
	}
	printf( "\nDone!\n" );

	if(infile!=NULL)
		fclose(infile);

	if(outfile!=NULL)
		fclose(outfile);

	FreeLibrary(hDll);
	return 0;
}


int calc_skew()
{
	// Calculate how much time has elapsed sinse the last call.
	struct timeval now_tv;
	gettimeofday( &now_tv, 0 );
	double now = (double)now_tv.tv_sec + ((double)now_tv.tv_usec / 1000000.0);
	double delta = 0.0;
	if( prev_time )
		delta = now - prev_time;
	prev_time = now;

	// Calculate how much to increase the angle by.
	angle += ANGLE_STEP_PER_SEC * delta; // 'delta' is normalized to seconds so this works.
	if( angle >= 360.0 )
		angle = angle - 360.0;

	return( rate_skew * sin( DEG2RAD(angle) ) );
}
