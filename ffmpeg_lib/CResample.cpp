
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

#include "CResample.h"
#include <assert.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

CResample::CResample()
{
	m_hDll = NULL;
	m_audio_cntx = 0;
	m_samples_consumed = 0;
}

CResample::~CResample()
{
	
}

int CResample::pcm_resample_init(int in_rate, int out_rate)
{
	assert( in_rate == 48000 && "Doesn't support input rate!" );
	assert( out_rate == 32000 || out_rate == 24000 || out_rate == 16000 && "Doesn't support output rate!" );

	// DLL Link
	avcodec_link();

	// Initialize ffmpeg resampling.
	test_avcodec_register_all();

	m_audio_cntx = test_av_resample_init( out_rate,		// out rate
		in_rate,										// in rate
		16,												// filter length
		10,												// phase count
		1,												// linear FIR filter
		1.0 );											// cutoff frequency
	assert( m_audio_cntx && "Failed to create resampling context!" );

	// 따로 분리
	//char out_buffer[ sizeof( in_buffer ) / 3 ];		// 16KHz
	//char out_buffer[ sizeof( in_buffer ) / 2 ];		// 24KHz
	//char out_buffer[ sizeof( in_buffer ) * 10 / 15 ]; // 32KHz

	return 0;
}

int CResample::pcm_resample_close()
{
	// DLL Unlink
	avcodec_unlink();

	return 0;
}

int CResample::pcmFileResample(size_t bytes_read, char* p_in_buffer, char* p_out_buffer, int out_buffer_size)
{
	int samples_output = test_av_resample( m_audio_cntx,
		(short*)p_out_buffer,								// dst
		(short*)p_in_buffer,								// src
		&m_samples_consumed,								// consumed
		bytes_read / 2,										// src_size
		out_buffer_size / 2,								// dst_size
		0 );												// update_ctx
	assert( samples_output > 0 && "Error calling av_resample()!" );

	return samples_output;
}

int CResample::avcodec_link()
{
	m_hDll = LoadLibrary(TEXT("../bin/avcodec.dll"));
	assert(m_hDll && "Load 'avcodec.dll' library Failed!!");

	test_av_resample_init = ( struct AVResampleContext* (*)(int, int, int, int
		, int, double) ) GetProcAddress(m_hDll, "av_resample_init");
	test_av_resample = ( int (*)(struct AVResampleContext *, short *, short *, int *, int, int, int) ) GetProcAddress(m_hDll, "av_resample");
	test_av_resample_close = ( void (*)(struct AVResampleContext *) ) GetProcAddress(m_hDll, "av_resample_close");
	test_avcodec_register_all = ( void (*)(void) ) GetProcAddress(m_hDll, "avcodec_register_all");
	test_av_malloc = (void* (*)(size_t)) GetProcAddress(m_hDll, "av_malloc");
	test_av_freep = (void (*)(void *)) GetProcAddress(m_hDll, "av_freep");

	return 0;
}

void CResample::avcodec_unlink()
{
	if(m_hDll != NULL)
		FreeLibrary(m_hDll);
}
