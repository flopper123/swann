#include <iostream>
#include "./dataset/load.hpp"

int main() {
  auto in = load_hdf5(DataSize::XS);
  return 0;
}
