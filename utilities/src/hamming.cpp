#include "../include/hamming.h"
#include <iostream>
using namespace std;

/*
  This Hamming library both encodes and decodes data with parity bits.
  The parity bit length is arbitrary.
  http://en.wikipedia.org/wiki/Hamming_code
 */

/*
 The constructor runs expensive code so it is only done once at the
 beginning of application.  It builds the generator matrix as well as
 the parity check matrix
*/
Hamming::Hamming(int parity_bit_length)
{
  this->parity_bit_length = parity_bit_length;
  data_bit_length = pow((double) 2,(double) parity_bit_length) - parity_bit_length - 1;
  codeword_length = parity_bit_length + data_bit_length;
  build_parity_check_matrix();
  build_generator_matrix();
  encode_input_vector.resize(data_bit_length,Eigen::NoChange);
  encode_output_vector.resize(Eigen::NoChange,codeword_length);
  decode_input_vector.resize(Eigen::NoChange, codeword_length);
  decode_output_vector.resize(data_bit_length,Eigen::NoChange);
}

void Hamming::encode(vector<bool>& input, vector<bool>& output)
{
  for(int i=0;i<data_bit_length;i++){ encode_input_vector(i) = input.at(i); }
  encode_output_vector = (generator_matrix.colwise() * encode_input_vector).colwise().redux(logical_xor());
  for(int i=0;i<codeword_length;i++){ output.push_back(encode_output_vector(0,i)); }
}

void Hamming::decode(vector<bool>& input, vector<bool>& output)
{
  // determine syndrome
  for(int i=0;i<codeword_length;i++){ decode_input_vector(i) = input.at(i); }
  decode_output_vector = (parity_check_matrix.rowwise() * decode_input_vector).rowwise().redux(logical_xor());

  // if no syndrome (no errors)
  if(decode_output_vector.sum() == 0)
  {
    output.assign(input.begin()+parity_bit_length, input.end());
  }else{
    int syndrome_at = 0;
    for(int i=0;i<parity_bit_length;i++)
    {
      if (decode_output_vector(i,0) == 1)
      {
        syndrome_at += (int) pow( (double) 2, (double) i);
      }
    }
    syndrome_at--;
    input[syndrome_at] = (input[syndrome_at] + 1) % 2;
    output.assign(input.begin()+parity_bit_length, input.end());
  }
}

void Hamming::build_parity_check_matrix()
{
  parity_check_matrix.resize(parity_bit_length, codeword_length);
  std::bitset < 100 > bits;

  int powers_found = 0;
  for(int i = 0; i < codeword_length; i++)
  {
    bits = (unsigned long) (i + 1);
    for(int j = 0; j < parity_bit_length; j++)
    {
      parity_check_matrix(j,i) = bits[j];
    }

    // if we found a power of 2, swap it to get the matrix in "standard" form.
    // (1 bit being on means a power of two)
    if(bits.count() == 1)
    {
      parity_check_matrix.col(powers_found).swap(parity_check_matrix.col(i));
      powers_found++;
    }
  }
}

void Hamming::build_generator_matrix()
{
  generator_matrix.resize(data_bit_length, codeword_length);
  MatrixXXb temp1 = parity_check_matrix.block(0, parity_bit_length, parity_bit_length,data_bit_length).transpose();
  MatrixXXb temp2;
  temp2.setIdentity(data_bit_length,data_bit_length);
  generator_matrix.block(0,0,data_bit_length,parity_bit_length) = temp1;
  generator_matrix.block(0,parity_bit_length, data_bit_length,data_bit_length) = temp2;
}
