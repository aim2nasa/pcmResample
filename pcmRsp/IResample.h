#ifndef __IRESAMPLE_H__
#define __IRESAMPLE_H__

#define RSP_OK					0
#define RSP_LOAD_LIBRARY_FAIL	-1
#define RSP_AUDIO_CTX_INIT_FAIL	-2
#define RSP_LIBRARY_NOT_LOADED	-3
#define RSP_FREELIBRARY_FAIL	-4

class __declspec(dllexport) IResample {
public:
	virtual ~IResample(){}

	virtual int init(int output_channels, int input_channels,int output_rate, int input_rate,
					 enum AVSampleFormat sample_fmt_out,enum AVSampleFormat sample_fmt_in,
					 int filter_length, int log2_phase_count,int linear, double cutoff)=0;
	virtual int resample(short *output, short *input, int nb_samples)=0;
	virtual int close()=0;
};

#endif