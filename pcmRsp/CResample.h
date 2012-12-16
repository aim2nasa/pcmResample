#ifndef __CRESAMPLE_H__
#define __CRESAMPLE_H__

#define RSP_OK					0
#define RSP_LOAD_LIBRARY_FAIL	-1
#define RSP_AUDIO_CTX_INIT_FAIL	-2
#define RSP_LIBRARY_NOT_LOADED	-3
#define RSP_FREELIBRARY_FAIL	-4

class __declspec(dllexport) CResample {
public:
	CResample();
	virtual ~CResample();

	int init(int output_channels, int input_channels,int output_rate, int input_rate,
		enum AVSampleFormat sample_fmt_out,
		enum AVSampleFormat sample_fmt_in,
		int filter_length, int log2_phase_count,
		int linear, double cutoff);
	int resample(short *output, short *input, int nb_samples);
	int close();
	
protected:
	int avcodec_link();
	int avcodec_unlink();

protected:
	void*						m_hDll;
	struct ReSampleContext*		m_pCtx;

	ReSampleContext* (*m_fp_av_audio_resample_init)(int,int,int,int,enum AVSampleFormat,enum AVSampleFormat,int,int,int,double);
	int (*m_fp_audio_resample)(ReSampleContext*,short*,short*,int);
	void (*m_fp_audio_resample_close)(ReSampleContext*);
};

#endif