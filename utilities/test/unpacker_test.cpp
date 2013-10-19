#include "gmock/gmock.h"
#include "../src/printer.cpp"
#include "../include/constants.h"
#include "../src/unpacker.cpp"
#include <vector>

/*
raw data length 48: 110100111010100101011101001011011011110010010100
transcoded data length 90: 101001001110100101001101011010010011011001100110101000110011010010101010101010101010101010
raw parity bits 1100010
transcoded parity of length 11: 10100010110
packet of length 106: 1111010100010110101001001110100101001101011010010011011001100110101000110011010010101010101010101010101010
*/

TEST(unpacker, basic)
{
  bool packetArray[101] = {1,0,1,0,0,0,1,0,1,1,0,1,0,1,0,0,1,0,0,1,1,1,0,1,0,0,1,0,1,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,1,1,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,0,0,0,1,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
  vector<bool> packet (packetArray, packetArray + sizeof(packetArray) / sizeof(bool) );

  bool correctArray[48] = { 1,1,0,1,0,0,1,1,1,0,1,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,0,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,1,0,1,0,0 };
  vector<bool> correct (correctArray, correctArray + sizeof(correctArray) / sizeof(bool) );

  vector<bool> output; // output of function
  Unpacker unpacker;

  unpacker.unpack(packet, output);

  ASSERT_THAT(output, testing::ContainerEq(correct));
}


TEST(unpacker, with_1_error_in_data)
{
  bool packetArray[101] = {1,0,1,0,0,0,1,0,1,1,0,1,0,1,0,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,0,1,1,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,0,0,0,1,1,0,0,1,1,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
  vector<bool> packet (packetArray, packetArray + sizeof(packetArray) / sizeof(bool) );

  bool correctArray[48] = { 1,1,0,1,0,0,1,1,1,0,1,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,0,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,1,0,1,0,0 };
  vector<bool> correct (correctArray, correctArray + sizeof(correctArray) / sizeof(bool) );

  vector<bool> output; // output of function
  Unpacker unpacker;

  unpacker.unpack(packet, output);

  ASSERT_THAT(output, testing::ContainerEq(correct));
}
