#include "gmock/gmock.h"
#include "../constants.cpp"
#include "../hamming.cpp"
#include <vector>

int parity_bits = 3;
TEST(hamming, data_bit_length)
{
  Hamming ham(parity_bits);
  ASSERT_EQ(4, ham.data_bit_length);
}

TEST(hammimg, building_generator_matrix)
{
  ArrayXXb correct_gm(4,7);
  correct_gm << 0,1,1,1,0,0,0,
                1,0,1,0,1,0,0,
                1,1,0,0,0,1,0,
                1,1,1,0,0,0,1;

  Hamming ham(parity_bits);
  ASSERT_TRUE( (correct_gm == ham.generator_matrix).all() );
}

TEST(hamming, building_parity_check_matrix)
{
  ArrayXXb correct_pcm(3,7);
  correct_pcm << 1,0,0,0,1,1,1,
                 0,1,0,1,0,1,1,
                 0,0,1,1,1,0,1;

  Hamming ham(parity_bits);
  ASSERT_TRUE( (correct_pcm == ham.parity_check_matrix).all() );
}

TEST(hamming, encode)
{
  Hamming ham(parity_bits);
  ArrayXXb correct_encoded(1,7);
  vector<bool> input;
  input.push_back(1);
  input.push_back(0);
  input.push_back(1);
  input.push_back(0);
  vector<bool> output;
  ham.encode(input, output);
  ASSERT_THAT(output, testing::ElementsAre(1,0,1,1,0,1,0));
}

TEST(hamming, decode_with_no_errors)
{
  Hamming ham(parity_bits);
  ArrayXXb correct_decoded(1,4);
  vector<bool> input;
  input.push_back(1);
  input.push_back(0);
  input.push_back(1);
  input.push_back(1);
  input.push_back(0);
  input.push_back(1);
  input.push_back(0);
  vector<bool> output;
  ham.decode(input, output);
  ASSERT_THAT(output, testing::ElementsAre(1,0,1,0));
}
