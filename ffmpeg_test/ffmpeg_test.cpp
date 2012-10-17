
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
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include <windows.h>
#include <assert.h>

int dllLinkTest();

int main(int argc, char *argv[])
{
	int ret = dllLinkTest();
	if(ret!=0)
	{
		printf("dllLinkTest Failed!\n");
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
