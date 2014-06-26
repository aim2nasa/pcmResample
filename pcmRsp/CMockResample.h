#ifndef __CRESAMPLE_H__
#define __CRESAMPLE_H__

#include "IResample.h"

#define REF_PCM_FRAME_SIZE				(1152*2)	//48khz, short��(2����Ʈ) ����
#define MONO_SAMPLES_PER_SAMPLE_FREQ	24
#define STEREO_SAMPLES_PER_SAMPLE_FREQ	48

//stream format definition for automatic testing, data unit (short:2byte little endian)
//<id>   xxxx pcm��Ʈ���� �����ϴ� ������, resample��� ��Ʈ���� �Է½�Ʈ���� ������+1�� �����ڸ� ������
//<ch>   xxxx ä�μ� ex) 0100 : 1 0200: 2
//<sf>   xxxx xxxx ���ø� ���ļ� ex)80 bb :48000 , 4����Ʈ�� ����
//<ns>   xxxx ���ð���
//<no>	 xxxx ����
//{unit}=<type><ch><sf><ns><no> ��6���� short������ ����,i.e. 12����Ʈ

class __declspec(dllexport) CMockResample : public IResample{
public:
	CMockResample():m_nInpChannels(0),m_nInpRate(0),m_nOutChannels(0),m_nOutRate(0){}
	virtual ~CMockResample(){}

	int init(int output_channels, int input_channels,int output_rate, int input_rate,
		enum AVSampleFormat sample_fmt_out,enum AVSampleFormat sample_fmt_in,
		int filter_length, int log2_phase_count,int linear, double cutoff);
	int resample(short *output, short *input, int nb_samples);
	int close();

	struct pcmStrInfo{
		int id;
		int ch;
		int sf;
		int ns;
		int no;
	};

	static int getSampleSize(int nSampleFreq,int nChannels);
	static pcmStrInfo parsePcmStrInfo(short *pPcmStr);
	static void writeSampleFreq(short *pStr,int nSf);
	static int readSampleFreq(short *pStr);

protected:
	int m_nInpChannels;
	int m_nInpRate;
	int m_nOutChannels;
	int m_nOutRate;
};

#endif