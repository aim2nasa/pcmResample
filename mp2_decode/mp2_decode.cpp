#include <iostream>
#include <assert.h>
#include <mpg123.h>
#include "mp2_decode.h"

using namespace std;

static const int bitrateTable[2][15]={
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160},		//for 24KHz
	{0,32,48,56,64,80,96,112,128,160,192,224,256,320,384}	//for 48KHz
};

mpg123_handle *mp2Decode_Init()
{
	if(mpg123_init()!=MPG123_OK) { cout<<"mpg123_init() failed"; mp2Decode_Cleanup(); return NULL; }

	int ret=0;
	mpg123_handle *m = mpg123_new(NULL,&ret);
	if(m==NULL) {
		cout<<"mpg123_new() failed"<<endl;
		mp2Decode_Cleanup();
		return NULL;
	}
	cout<<"Current decoder:"<<mpg123_current_decoder(m)<<endl;

	if(mpg123_open_feed(m)!=MPG123_OK) {
		cout<<"mpg123_open_feed() failed"<<endl;
		mp2Decode_Cleanup(m);
		return NULL;
	}

	return m;
}

void mp2Decode_Cleanup(mpg123_handle* m)
{
	if(m) mpg123_delete(m);
	mpg123_exit();
}

void parseMp2Header(MP2_HEADER* pHeader,unsigned char* pBuffer)
{
	pHeader->sync		= (((unsigned short)pBuffer[0]&0xFF)<<4) | (((unsigned short)pBuffer[1]&0xF0)>>4);
	pHeader->id			= (pBuffer[1]&0x08)>>3;
	pHeader->layer		= (pBuffer[1]&0x06)>>1;
	pHeader->protect	= (pBuffer[1]&0x01);
	pHeader->bitrateIdx	= (pBuffer[2]&0xF0)>>4;
	pHeader->sampling	= (pBuffer[2]&0x0C)>>2;
	pHeader->padding	= (pBuffer[2]&0x02)>>1;
	pHeader->priv		= (pBuffer[2]&0x01);
	pHeader->mode		= (pBuffer[3]&0xC0)>>6;
	pHeader->mode_ext	= (pBuffer[3]&0x30)>>4;
	pHeader->copyright	= (pBuffer[3]&0x08)>>3;
	pHeader->orignal	= (pBuffer[3]&0x04)>>2;
	pHeader->emphasis	= (pBuffer[3]&0x03);
}

int getBitrate(int nId,int nBitrateIdx)
{
	assert(nId==0 || nId==1);
	assert(nBitrateIdx>0 && nBitrateIdx<15);
	return 1000*bitrateTable[nId][nBitrateIdx];	//kbps unit
}

int getBufSize(int nBitrateIdx, int nId)
{
	assert(nId==0 || nId==1);
	
	int nSrcBufferSize = 144*getBitrate(nId,nBitrateIdx)/((nId)?48000:24000);

	return nSrcBufferSize;
}

int mp2_decode(mpg123_handle *m, const unsigned char *mp2buffer, size_t nSrcBufferSize, unsigned char *pcmBuffer, size_t outmemsize, size_t *size)
{
	int ret = mpg123_decode(m,mp2buffer,nSrcBufferSize,pcmBuffer,outmemsize,size);
	if(ret == MPG123_NEW_FORMAT)
	{
		long rate;
		int channels, enc;
		mpg123_getformat(m, &rate, &channels, &enc);
		fprintf(stderr, "New format: %li Hz, %i channels, encoding value %i\n", rate, channels, enc);
	}
	
	while(ret != MPG123_ERR && ret != MPG123_NEED_MORE)
	{ /* Get all decoded audio that is available now before feeding more input. */
		ret = mp2_decode(m, NULL, 0, pcmBuffer, outmemsize, size);
	}

	if(ret == MPG123_ERR){ fprintf(stderr, "some error: %s", mpg123_strerror(m)); }

	return ret;
}
