#include "gmock/gmock.h"
#include "../include/constants.h"
#include "../src/unpacker.cpp"
#include <vector>

TEST(unpacker, basic)
{
  vector<bool> output; // output of function

  bool packetArray[120];
  vector<bool> packet (packetArray, packetArray + sizeof(packetArray) / sizeof(bool) );

  bool correctArray[48] = { 1,1,0,1,0,0,1,1,1,0,1,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1,0,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,1,0,1,0,0 };
  vector<bool> correct (correctArray, correctArray + sizeof(correctArray) / sizeof(bool) );

  Unpacker unpacker;
  unpacker.unpack(packet, output);
  ASSERT_THAT(output, testing::ContainerEq(correct));
}
