#include <iostream>
#include <ctime>
#include <random>

#include "global.hpp"
#include "./dataset/load.hpp"
#include "./hash/hashpool.hpp"
#include "./index/lshmapfactory.hpp"
#include "./index/lshforest.hpp"
#include "./statistics/statsgenerator.hpp"
#include "./util/ranges.hpp"
#include "./hash/dependenthashfamilyfactory.hpp"
#include "./hash/hashfamilyfactory.hpp"


int main()
{
  srand(time(NULL));

  std::cout << "[+] Loading Dataset..." << std::endl;
  PointsDataset<D> dataset = load_hdf5<D>(DataSize::S);

  ui32 depth = log(dataset.size()) + 1;
  ui32 count = 6;

  std::cout << "[+] Initializing hashes" << std::endl;
  HashFamily<D> pool = HashFamilyFactory<D>::createRandomBitsConcat(D);

  std::cout << "[+] Loading Index..." << std::endl;
  auto maps = LSHMapFactory<D>::create_optimized(dataset, pool, depth, count);

  std::cout << "Building index" << std::endl;
  LSHForest<D> index(maps, dataset, SingleBitFailure<D>);

  std::cout << "[+] Building Index..." << std::endl;
  index.build();
  
  std::cout << "[+] Index build on:"
            << "\n\t" << index.size() << " points  "
            << "\n\t" << index.getMaps().size() << " tries" << std::endl
            << "\n\t" << depth << " depth" << std::endl
            << "[+] ready for querying <3" << std::endl;
  
  StatsGenerator<D>::runStats(&index);

  return 0;
}
