#include <iostream>

#include "./dataset/load.hpp"

int main() {
  Dataset in = load_hdf5(DataSize::XS);
  
  for (int i = 0; i < 64; ++i) {
    std::cout << "Bitset[" << i << "]: " << in[i] << std::endl;
  }
  return 0;
}
