#include <UnitTest++.h>
#include "../utility.cpp"

SUITE (utilities) {
  TEST(float_to_byte_array)
  {
    float starting_value = 0.112358;
    unsigned char *result = Utility::FloatToByteArray(starting_value);
    unsigned char correct[4] = { 0xF3, 0x1B, 0xE6, 0x3D };
    CHECK_ARRAY_EQUAL(correct, result, 4);
  }

  TEST(byte_array_to_float)
  {
    unsigned char starting_value[4] = { 0xF3, 0x1B, 0xE6, 0x3D };
    float result = Utility::ByteArrayToFloat(starting_value);
    float correct = 0.112358;
    CHECK_EQUAL(correct, result);
  }

  TEST(float_array_to_byte_array)
  {
    float float_array[2] = { 0.112358, -0.010111 };
    unsigned char correct[8] = { 0xF3, 0x1B, 0xE6, 0x3D, 0x9C, 0xA8, 0x25, 0xBC };
    unsigned char result[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
    Utility::FloatArrayToByteArray(float_array, 2, result);
    CHECK_ARRAY_EQUAL(correct, result, 8);
  }

  TEST(byte_array_to_float_array)
  {
    unsigned char starting_value[8] = { 0xF3, 0x1B, 0xE6, 0x3D, 0x9C, 0xA8, 0x25, 0xBC };
    float correct[2] = { 0.112358, -0.010111 };
    float result[2] = { 0.0, 0.0 };
    Utility::ByteArrayToFloatArray(starting_value, 8, result);
    CHECK_ARRAY_EQUAL(correct, result, 2);
  }

  TEST(byte_array_to_string)
  {
    unsigned char starting_value[5] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f};
    std::string result;
    Utility::ByteArrayToString(starting_value, &result, 5);
    std::string correct = "hello";
    CHECK_EQUAL(correct, result);
  }

  TEST(string_to_byte_array){
    std::string starting_value = "hello";
    unsigned char result[5] = { 0x00,0x00,0x00,0x00,0x00 };
    Utility::StringToByteArray(&starting_value, result);
    unsigned char correct[5] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f};
    CHECK_ARRAY_EQUAL(correct, result, 5);
  }
}
