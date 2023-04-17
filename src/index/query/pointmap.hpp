#include "../point.hpp"
#include <unordered_set>

/**
 * @brief An utility class for storing points in a map-like structure 
 *        that allows for fast retrieval of points with a given hamming distance to a query point.
 *        
 *        The primary motivation behind this class is to avoid sorting points by their hamming distance
 * @tparam D 
 */
template <ui32 D>
class PointMap
{
  //! Consider mapping to a point pointer instead of a bool
  std::unordered_set<ui32> seen;  // seen[i]   : whether the point with idx i has been seen
  
  //! Retrieval can be done faster by keeping track of the first non-empty distance index and the inverse
  std::vector<ui32> dist[D];      // dist[d]   : list of points with hamming distance of d from query point

  //! Consider using a unique_ptr here to avoid copying the points
  std::vector<Point<D>> points;
  Point<D> query;

public:
  PointMap(std::vector<Point<D>>& points, Point<D>& query) 
    : seen(), points(points), query(query) 
  {
    for (ui32 i = 0; i < D; ++i)
      dist[i] = std::vector<ui32>();
  };
  
  ui32 size() const { return seen.size(); }

  /**
   * @brief Get the k points with the lowest hamming distance to the query target
   *        in O(k+D) time.
   * @param k number of objects to return
   * @return std::vector<ui32> A vector containing the indices of the k nearest points 
   *         in sorted ascending order by hamming distance to query point
   */
  std::vector<ui32> get_nearest(const ui32 k) const {
    std::vector<ui32> ret;
    for (ui32 i = 0; i < D && ret.size() < k; ++i) {
      int j = std::min(k - ret.size(), dist[i].size()); 
      ret.insert(ret.end(), dist[i].begin(), dist[i].begin() + j);
    }
    return ret;
  }
  
  /**
   * @brief Returns true if this map contains the point with the given idx in asymptotic O(1) time.
   * @param idx index of the point to check 
   */
  bool contains(const ui32 idx) const { return seen.contains(idx); }
  
  /**
   * @brief Inserts the point with the given idx into this map in asymptotic O(D) time 
   *        (upper bound is distance computation of point, which is only executed in case no entry exist).
   * @param idx index of the point to insert
   */
  void insert(const ui32 idx) {
    if (this->contains(idx)) return;
    seen.emplace(idx);
    dist[query.distance(points[idx])].emplace_back(idx);
  }
  
  template<iterator_to<ui32> IdxIterator>
  void insert(IdxIterator beg, IdxIterator end) {
    for (auto it = beg; it != end; ++it) {
      this->insert(*it);
    }
  }
};