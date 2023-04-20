#include "../point.hpp"
#include <unordered_set>

/**
 * @brief An utility class for storing points in a map-like structure 
 *        that allows for fast retrieval of points with the minimum or maximum hamming distance to a query point.
 *        
 *        The primary motivation behind this class is to avoid sorting points by their hamming distance
 * @tparam D 
 */
template <ui32 D>
class PointMap
{
  std::unordered_set<ui32> seen;      // seen[i]          : whether the point with idx i has been seen
  std::vector<ui32> dist2points[D+1]; // dist2points[d]   : list of points with hamming distance of d from query point

  const std::vector<Point<D>>& points; // reference to points for look up of point by idx

  ui32 lo_d, // lo_d : first non-empty index in dist2points, 
       hi_d, // hi_d : last non-empty index in dist2points
       sz;   // sz   : number of points inserted into dist2points

  const Point<D> query;
public:
  /** 
   * @brief Construct a new Point Map object
   * @arg points A pointer to the vector of points to use as the source-order for the indices inserted into this map
   */
  PointMap(std::vector<Point<D>>& points, const Point<D>& query) 
    : seen(), points(points), query(query), lo_d(UINT32_MAX), hi_d(0), sz(0)
  {};
  
  /**
   * @brief Returns the number of points inserted into this map, 
   *        note that this is not the same as the number of initially given points
   * @return ui32 
   */
  ui32 size() const { return sz; }
  
  /**
   * @brief the idx of the kth distance point from the query point
   *        among all points inserted into this map
   * @param k The number of points to skip
   * @return ui32 
   */
  ui32 get_kth_dist(const ui32 k) const {
    ui32 i = lo_d, cnt = 0;
    for (; i < hi_d+1 && cnt < k; ++i) {
      cnt += dist2points[i].size();
      if (cnt > k) return i; // if we overshoot, return the current distance to avoid end increment of i
    }
    return i;
  }

  /**
   * @brief Get the k points with the lowest hamming distance to the query target
   *        in O(k+D) time.
   * @param k number of objects to return
   * @return std::vector<ui32> A vector containing the indices of the k nearest points 
   *         in sorted ascending order by hamming distance to query point
   */
  std::vector<ui32> get_k_nearest(const ui32 k) const {
    std::vector<ui32> ret;
    for (ui32 i = lo_d; 
         i < hi_d+1 && ret.size() < k; 
         ++i) 
    {
      int j = std::min(k - ret.size(), dist2points[i].size()); 
      ret.insert(ret.end(), dist2points[i].begin(), dist2points[i].begin() + j);
    }
    return ret;
  }
  
  /**
   * @brief Returns true if this map contains the point with the given idx in asymptotic O(1) time.
   * @param idx index of the point to check 
   */
  bool contains(const ui32 idx) const { return seen.find(idx) != seen.end(); }
  
  /**
   * @brief Inserts the point with the given idx into this map in asymptotic O(D) time 
   *        (upper bound is distance computation of point, which is only executed in case no entry exist).
   * @param idx index of the point to insert
   */
  void insert(const ui32 idx) {
    if (this->contains(idx)) return;
    ++sz;
    seen.emplace(idx);
    ui32 hdist = query.distance(points[idx]);
    dist2points[hdist].emplace_back(idx);
    lo_d = std::min(lo_d, hdist);
    hi_d = std::max(hi_d, hdist);
  }
  
  template<iterator_to<ui32> IdxIterator>
  void insert(IdxIterator beg, IdxIterator end) {
    for (auto it = beg; it != end; ++it) {
      this->insert(*it);
    }
  }
};
