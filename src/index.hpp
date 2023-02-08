#include <bitset>
#include <vector>
#include <algorithm> 
#include "global.hpp"


/**
 * Binary vector point 
 */
template<int D>
class Point : std::bitset<D> {
  public:
    using std::bitset<D>::bitset;
  
    // Computes the Hamming distance between to points
    friend inline int distance(const Point<D>& p1, const Point<D>& p2) {
      return (p1 ^ p2).count();
    }
};

template<int D>
class Index {
  public:
    virtual Index() = 0;
    virtual Index(std::vector<Point<D>>& points) = 0;
    virtual ui32 size() = 0;
    virtual void build() = 0;
    virtual void insert(Point<D>& point) = 0;
    virtual std::vector<index> query(Point<D> point, int k) = 0;
    virtual Point<D> &operator[](index i) = 0;
};

template<int D>
class BFIndex : Index<D> {
  std::vector<Point<D>> points;

  public:
    BFIndex() {
      std::cout << "I am building\n";
    }

    BFIndex(std::vector<Point<D>>& points) {
      std::cout << "I am building\n";
      this->points = points;
    }
    
    ui32 size() { 
      return this->points.size(); 
    }
    
    void build() {}

    void insert(Point<D>& p) {
      this->points.push_back(p);
    }
    
    std::vector<index> query(const Point<D>& point, int k) {
      // Create distance vector
      std::vector<std::pair<ui32, index>> distance;

      // Find Hamming distance to all points
      for (index i = 0; i < this->size(); i++) {
        distance.emplace_back(Point<D>.distance(point, this->points[i]), i);
      }
      
      // Sort list
      std::sort(ALL(distance));

      // Take k elements with lowest distance
      std::vector<index> ret;
      std::transform(distance.begin(), distance.begin() + k,
                     ret.begin(),
                     [](const pair<ui32, index> &dist) -> index
                     { return dist.second; });
      return ret;
    }    
};
