#include <iostream>
#include <assert.h>
#include <mpg123.h>
#include "mp2_decode.h"

using namespace std;

int mp2Decode_Init()
{
	if(mpg123_init()!=MPG123_OK) { cout<<"mpg123_init() failed"; mp2Decode_Cleanup(); return -1; }

	int ret=0;
	mpg123_handle *m = mpg123_new(NULL,&ret);
	if(m==NULL) {
		cout<<"mpg123_new() failed"<<endl;
		mp2Decode_Cleanup();
		return -1;
	}
	cout<<"Current decoder:"<<mpg123_current_decoder(m)<<endl;

	if(mpg123_open_feed(m)!=MPG123_OK) {
		cout<<"mpg123_open_feed() failed"<<endl;
		mp2Decode_Cleanup(m);
		return -1;
	}

	return 0;
}

void mp2Decode_Cleanup(mpg123_handle* m)
{
	if(m) mpg123_delete(m);
	mpg123_exit();
}
