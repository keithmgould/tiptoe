#include "gmock/gmock.h"
#include "../src/constants.cpp"
#include "../src/hamming.cpp"
#include <vector>

/* Hamming Code with 3 parity bits */

TEST(hamming, building_parity_check_matrix_with_3_parity_bits)
{
  ArrayXXb correct_pcm(3,7);
  correct_pcm << 1,0,0,1,1,0,1,
                 0,1,0,1,0,1,1,
                 0,0,1,0,1,1,1;

  Hamming ham(3);
  ASSERT_TRUE( (correct_pcm == ham.parity_check_matrix).all() );
}

TEST(hamming, building_parity_check_matrix_with_4_parity_bits)
{
  ArrayXXb correct_pcm(4,15);
  correct_pcm << 1,0,0,0,1,0,1,1,1,0,1,0,1,0,1,
                 0,1,0,0,0,1,1,1,0,1,1,0,0,1,1,
                 0,0,1,0,1,1,1,0,0,0,0,1,1,1,1,
                 0,0,0,1,0,0,0,0,1,1,1,1,1,1,1;

  Hamming ham(4);
  ASSERT_TRUE( (correct_pcm == ham.parity_check_matrix).all() );
}

TEST(hammimg, building_generator_matrix_with_3_parity_bits)
{
  ArrayXXb correct_gm(4,7);
  correct_gm << 1,1,0,1,0,0,0,
                1,0,1,0,1,0,0,
                0,1,1,0,0,1,0,
                1,1,1,0,0,0,1;

  Hamming ham(3);
  ASSERT_TRUE( (correct_gm == ham.generator_matrix).all() );
}


TEST(hammimg, building_generator_matrix_with_4_parity_bits)
{
  ArrayXXb correct_gm(11,15);

  correct_gm << 1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,
                0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,
                1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,
                1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,
                1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,
                0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,
                1,1,0,1,0,0,0,0,0,0,1,0,0,0,0,
                0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,
                1,0,1,1,0,0,0,0,0,0,0,0,1,0,0,
                0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,
                1,1,1,1,0,0,0,0,0,0,0,0,0,0,1;

  Hamming ham(4);
  ASSERT_TRUE( (correct_gm == ham.generator_matrix).all() );
}

TEST(hamming, encode_with_3_parity_bits)
{
  Hamming ham(3);
  vector<bool> input;
  input.push_back(1);
  input.push_back(0);
  input.push_back(1);
  input.push_back(0);
  vector<bool> output;
  ham.encode(input, output);
  ASSERT_THAT(output, testing::ElementsAre(1,0,1,1,0,1,0));
}

TEST(hamming, encode_with_4_parity_bits)
{
  Hamming ham(4);
  // the input
  bool input_bool[11] = { 1,0,1,0,1,0,1,0,1,0,1 };
  vector<bool> input;
  input.insert(input.begin(), input_bool, input_bool+11);

  // the correct encoding
  bool correct_bool[15] = { 0,1,0,0,1,0,1,0,1,0,1,0,1,0,1 };
  vector<bool> correct;
  correct.insert(correct.begin(), correct_bool, correct_bool+15);

  // the tested encoding
  vector<bool> output;
  ham.encode(input, output);

  //judgement day
  ASSERT_THAT(output, testing::ContainerEq(correct));
}

TEST(hamming, decode_with_no_errors_with_3_parity_bits)
{
  Hamming ham(3);
  bool input_bool[7] = { 1,0,1,1,0,1,0 };
  vector<bool> input;
  input.insert(input.begin(), input_bool, input_bool+7);
  vector<bool> output;
  ham.decode(input, output);
  ASSERT_THAT(output, testing::ElementsAre(1,0,1,0));
}

TEST(hamming, decode_with_no_errors_with_4_parity_bits)
{
  Hamming ham(4);

  bool input_bool[15] = { 0,1,0,0,1,0,1,0,1,0,1,0,1,0,1 };
  vector<bool> input;
  input.insert(input.begin(), input_bool, input_bool+15);

  bool correct_bool[11] = { 1,0,1,0,1,0,1,0,1,0,1 };
  vector<bool> correct;
  correct.insert(correct.begin(), correct_bool, correct_bool+11);

  vector<bool> output;
  ham.decode(input, output);
  ASSERT_THAT(output, testing::ContainerEq(correct));
}

TEST(hamming, decode_with_1_error_with_3_parity_bits)
{
  Hamming ham(3);
  ArrayXXb correct_decoded(1,4);
  bool input_bool[11] = { 1,0,1,1,1,1,0 };
  vector<bool> input;
  input.insert(input.begin(), input_bool, input_bool+7);
  vector<bool> output;
  ham.decode(input, output);
  ASSERT_THAT(output, testing::ElementsAre(1,0,1,0));
}

TEST(hamming, decode_with_1_error_with_4_parity_bits)
{
  Hamming ham(4);

  bool input_bool[15] = { 0,1,0,0,1,0,1,0,1,0,1,1,1,0,1 };
  vector<bool> input;
  input.insert(input.begin(), input_bool, input_bool+15);

  bool correct_bool[11] = { 1,0,1,0,1,0,1,0,1,0,1 };
  vector<bool> correct;
  correct.insert(correct.begin(), correct_bool, correct_bool+11);

  vector<bool> output;
  ham.decode(input, output);
  ASSERT_THAT(output, testing::ContainerEq(correct));
}
