#ifndef HAMMING_H
#define HAMMING_H

#include <math.h>
#include <bitset>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

typedef Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> ArrayXXb;
typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> MatrixXXb;
typedef Eigen::Array<bool, Eigen::Dynamic, 1> ArrayVertb;
typedef Eigen::Array<bool, 1, Eigen::Dynamic> ArrayHorzb;

struct logical_xor {
  bool operator() (bool a, bool b) const
  {
    return a != b;
  }
};

class Hamming
{
  public:
  ArrayXXb generator_matrix;
  ArrayXXb parity_check_matrix;
  Hamming(int parity_bits);
  void encode(vector<bool>& input, vector<bool>& output);
  void decode(vector<bool>& input, vector<bool>& output);

  private:
  int data_bit_length;
  int parity_bit_length;
  int codeword_length;
  ArrayVertb encode_input_vector;
  ArrayHorzb encode_output_vector;
  ArrayHorzb decode_input_vector;
  ArrayVertb decode_output_vector;
  void build_generator_matrix();
  void build_parity_check_matrix();
};

#endif
