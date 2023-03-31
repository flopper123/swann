#include <iostream>

#include "global.hpp"
#include "./dataset/load.hpp"
#include "./hash/hashpool.hpp"
#include "./index/lshmapfactory.hpp"
#include "./index/lshforest.hpp"
#include "./statistics/statsgenerator.hpp"
#include "./util/ranges.hpp"
#include "./hash/dependenthashfamilyfactory.hpp"


int main()
{
  std::cout << "[+] Loading Dataset..." << std::endl;
  PointsDataset<D> in = load_hdf5<D>(DataSize::XS);
  std::vector<Point<D>> sample_points;
  std::sample(ALL(in), std::back_inserter(sample_points), in.size()/10, std::mt19937{std::random_device{}()});
  
  double avg_bits = std::accumulate(ALL(in), 0.0, [](double acc, const Point<D> &p)
                                       { return acc + p.count(); }) / in.size();

  std::cout << "[+] Average bits set: " << avg_bits << std::endl;
  const ui32 trie_depth = log(in.size())+2, trie_count = 8;
  
  std::cout << "[+] Initializing hashes on subset of " << in.size() << " points" << std::endl;
  HashFamily<D> hashes = DependentHashFamilyFactory<D>::createHDist(ALL(sample_points), trie_depth*trie_count);

  std::cout << "[+] Loading Index..." << std::endl;
  auto maps = LSHMapFactory<D>::create(hashes, trie_depth, trie_count);
  LSHForest<D> index(maps, in, HammingSizeFailure);
  std::cout << "[+] Building Index..." << std::endl;
  index.build();
  
  std::cout << "[+] Index build on:"
            << "\n\t" << index.size() << " points  "
            << "\n\t" << index.getMaps().size() << " tries" << std::endl
            << "[+] ready for querying <3" << std::endl;
  
  StatsGenerator<D>::runStats(&index);

  return 0;
}
