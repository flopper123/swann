#include <iostream>

#include "./dataset/load.hpp"
#include "./hash/hashpool.hpp"
#include "./index/lshmapfactory.hpp"
#include "./index/lshforest.hpp"

constexpr ui32 TRIE_COUNT = 10, TRIE_DEPTH = 10;
HashPool<D> hashes;

int main()
{
  std::cout << "Loading Dataset..." << std::endl;
  Dataset in = load_hdf5(DataSize::XS);
  std::cout << "Loading Index..." << std::endl;
  auto maps = LSHMapFactory<D>::create(hashes, TRIE_DEPTH, TRIE_COUNT);
  LSHForest<D> index(maps, in);
  std::cout << "Building Index..." << std::endl;
  index.build();
  std::cout << "Index build on:"
            << "\n\t" << index.size() << " points  "
            << "\n\t" << index.getMaps().size() << " tries" << std::endl
            << "ready for querying <3" << std::endl;
  return 0;
}
