#include <iostream>
using namespace std;
class Utility {

  public:

  /******************************************
   *
   * Convert 32 bit float to a 4 byte array
   */
  static unsigned char * FloatToByteArray(float f)
  {
    static unsigned char p[4];
    unsigned char * x = reinterpret_cast<unsigned char *>(&f);
    for (int i = 0; i < 4; ++i)
    {
      p[i] = x[i];
    }
    return p;
  }


  /******************************************
   * Convert array of 32 bit floats to array
   * of bytes
   */
  static void FloatArrayToByteArray(float f[], int float_count, unsigned char byte_array[])
  {
    cout << endl;
    for(int i = 0; i < float_count; i++)
    {
      unsigned char * x = reinterpret_cast<unsigned char *>(&f[i]);
      for (int j = 0; j < 4; ++j)
      {
        byte_array[4 * i + j] = x[j];
      }
    }
  }

  /******************************************
   *
   * Convert a 4 byte array to a 32 bit float
   *
   * Note: this is an endian dependent method
   */
  static float ByteArrayToFloat(unsigned char foo[])
  {
    union test
    {
      unsigned char buf[4];
      float number;
    }test;

    test.buf[0] = foo[0];
    test.buf[1] = foo[1];
    test.buf[2] = foo[2];
    test.buf[3] = foo[3];

    return test.number;
  }

  /******************************************
   *
   * Convert a byte array to a arry of
   * 32 bit floats
   *
   * Note: this is an endian dependent method
   */
  static void ByteArrayToFloatArray(unsigned char byte_array[], int byte_count, float float_array[])
  {
    union test
    {
      unsigned char buf[4];
      float number;
    }test;

    for(int i=0;i < (byte_count / 4); i++)
    {
      test.buf[0] = byte_array[ i * 4 + 0];
      test.buf[1] = byte_array[ i * 4 + 1];
      test.buf[2] = byte_array[ i * 4 + 2];
      test.buf[3] = byte_array[ i * 4 + 3];
      float_array[i] = test.number;
    }
  }

  /******************************************
   *
   * Convert from byte array to string
   */
  static void ByteArrayToString(unsigned char byte_array[], std::string * str, int byte_count)
  {
    std::string recasted( reinterpret_cast< const char * >(byte_array), byte_count ) ;
    *str = recasted;
  }
};
