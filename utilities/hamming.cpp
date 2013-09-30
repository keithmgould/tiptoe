#include <math.h>
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
  ArrayVertb input_vector;
  ArrayHorzb output_vector;
  int data_bit_length;
  int parity_bit_length;
  Hamming(int parity_bits);
  void encode(vector<bool>& input, vector<bool>& output);
  void decode(vector<bool>& input, vector<bool>& output);

  private:
  void build_generator_matrix();
  void build_parity_check_matrix();
};

Hamming::Hamming(int parity_bit_length)
{
  this->parity_bit_length = parity_bit_length;
  data_bit_length = pow((double) 2,(double) parity_bit_length) - parity_bit_length - 1;
  build_generator_matrix();
  build_parity_check_matrix();
  input_vector.resize(data_bit_length,Eigen::NoChange);
  output_vector.resize(Eigen::NoChange,data_bit_length + parity_bit_length);
}

void Hamming::encode(vector<bool>& input, vector<bool>& output)
{
  for(int i=0;i<input.size();i++){ input_vector(i) = input.at(i); }
  output_vector = (generator_matrix.colwise() * input_vector).colwise().redux(logical_xor());
  for(int i=0;i< data_bit_length + parity_bit_length;i++){ output.push_back(output_vector(0,i)); }
}

void Hamming::decode(vector<bool>& input, vector<bool>& output)
{
  // first we check to see if there are errors via parity bits
  // if errors
  // else
}

void Hamming::build_generator_matrix()
{
  generator_matrix.resize(data_bit_length, data_bit_length + parity_bit_length);

  // first generate the parity bit columns
  for(int p = 0; p<parity_bit_length; p++)
  {
    for(int i=0; i<data_bit_length; i++)
    {
      generator_matrix(i,p) = (p != i);
    }
  }
  // then the data bit columns
  int total_bit_length = parity_bit_length + data_bit_length;
  for(int d = parity_bit_length; d<total_bit_length; d++)
  {
    for(int i = 0; i<data_bit_length; i++)
    {
      generator_matrix(i,d) = (d - parity_bit_length == i);
    }
  }
}

void Hamming::build_parity_check_matrix()
{
  parity_check_matrix.resize(parity_bit_length, data_bit_length + parity_bit_length);
  MatrixXXb temp1 = generator_matrix.block(0, 0, data_bit_length,parity_bit_length).transpose();
  MatrixXXb temp2;
  temp2.setIdentity(parity_bit_length,parity_bit_length);
  parity_check_matrix.block(0,0,parity_bit_length,parity_bit_length) = temp2;
  parity_check_matrix.block(0,data_bit_length-1,parity_bit_length,data_bit_length) = temp1;
}

