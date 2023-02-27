#include <iostream>

#include "./dataset/load.hpp"

Dataset in;

int main() {
  load_hdf5(in, DataSize::XS);
  
  for (int i = 0; i < 64; ++i) {
    std::cout << "Bitset[" << i << "]: " << in[i] << std::endl;
  }
  return 0;
}
