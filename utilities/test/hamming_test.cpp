#include "gmock/gmock.h"
#include "../constants.cpp"
#include "../hamming.cpp"
#include <vector>

/* Hamming Code with 3 parity bits */

TEST(hamming, data_bit_length_with_3_parity_bits)
{
  Hamming ham(3);
  ASSERT_EQ(4, ham.data_bit_length);
}

TEST(hamming, data_bit_length_with_4_parity_bits)
{
  Hamming ham(4);
  ASSERT_EQ(11, ham.data_bit_length);
}

TEST(hammimg, building_generator_matrix_with_3_parity_bits)
{
  ArrayXXb correct_gm(4,7);
  correct_gm << 0,1,1,1,0,0,0,
                1,0,1,0,1,0,0,
                1,1,0,0,0,1,0,
                1,1,1,0,0,0,1;

  Hamming ham(3);
  ASSERT_TRUE( (correct_gm == ham.generator_matrix).all() );
}

TEST(hammimg, building_generator_matrix_with_4_parity_bits)
{
  ArrayXXb correct_gm(11,15);
  correct_gm << 0,1,1,1,
                1,0,1,1,
                1,1,0,1,
                1,1,1,0,


  Hamming ham(4);
  ASSERT_TRUE( (correct_gm == ham.generator_matrix).all() );
}

TEST(hamming, building_parity_check_matrix_with_3_parity_bits)
{
  ArrayXXb correct_pcm(3,7);
  correct_pcm << 1,0,0,0,1,1,1,
                 0,1,0,1,0,1,1,
                 0,0,1,1,1,0,1;

  Hamming ham(3);
  ASSERT_TRUE( (correct_pcm == ham.parity_check_matrix).all() );
}

TEST(hamming, encode_with_3_parity_bits)
{
  Hamming ham(3);
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

// TEST(hamming, decode_with_no_errors)
// {
  // Hamming ham(3);
  // ArrayXXb correct_decoded(1,4);
  // vector<bool> input;
  // input.push_back(1);
  // input.push_back(0);
  // input.push_back(1);
  // input.push_back(1);
  // input.push_back(0);
  // input.push_back(1);
  // input.push_back(0);
  // vector<bool> output;
  // ham.decode(input, output);
  // ASSERT_THAT(output, testing::ElementsAre(1,0,1,0));
// }

/* Hamming Code with 4 parity bits */



