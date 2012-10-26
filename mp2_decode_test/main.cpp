#include <iostream>
#include <gtest/gtest.h>
#include "../mp2_decode/mp2_decode.h"

TEST(mp2_decoder_test, mp2_decoder_48KHz){
	EXPECT_EQ(0, mp2Decode_Init());
	mp2Decode_Cleanup();
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
