#include "../point.hpp"
#include <unordered_set>
#include "../../global.hpp"
/**
 * @brief An utility class for storing points in a map-like structure 
 *        that allows for fast retrieval of the knn points within the minimum hamming distance 
 *        to a query point.
 *        
 *        The primary motivation behind this class is to avoid sorting points by their hamming distance
 *        and to instead maintain a queue of the k-nearest-neighbours to the query point among all points
 *        inserted into this map.
 *        
 *        This allows for asymptotic bounds of 
 *          * KNN Extraction in O(log(k)*k)
 *          * Insertion in O(log(k) + D) if the point is not already in the map, O(1) otherwise.
 *          * Distance of the kth point from the query point in O(1)
 * @tparam D 
 */
template <ui32 D>
class PointMap
{
  /**
   * @brief A queue of the k-nearest-neighbours to the query point 
   *        among all points inserted into this map.
   *        The elements are kept as pairs of the form (hamming distance, point index)
   */
  max_queue<std::pair<ui32, ui32>> knn;     

  /**
   * @brief Contains all points we have computed hamming distances for so far 
   */
  std::unordered_set<ui32> seen;       

  const std::vector<Point<D>> &points; // reference to points for look up of point by idx
  const ui32 k; // k : number of nearest points to query after
  const Point<D>& query;

public:
  /** 
   * @brief Construct a new Point Map object
   * @arg points A pointer to the vector of points to use as the source-order for the indices inserted into this map
   */
  PointMap(std::vector<Point<D>>& points, const Point<D>& query, ui32 k = 10) 
    : knn(), seen(), points(points), query(query), k(k) 
  {
    assert(k > 0 && k <= points.size());
  };

  /**
   * @brief Returns the number of points inserted into this map, 
   *        note that this is not the same as the number of initially given points
   * @return ui32 
   */
  inline ui32 size() const noexcept { return this->seen.size(); }
  
  /**
   * @brief The hamming distance of the kth point from the query point
   *        among all points inserted into this map, or UINT32_MAX if no points in this map
   * @return ui32 The hamming distance of the kth point from the query point
   */
  inline ui32 get_kth_dist() const noexcept {
    return knn.empty() ? UINT32_MAX : knn.top().first;
  }

  /**
   * @brief Extracts the k points with the lowest hamming distance to the query target
   *        in O(log(k)*k) time. 
   * @warning The PointMap is invalidated after this operation, and as such should not be used again.
   * @return std::vector<ui32> A vector containing the indices of the k nearest points 
   *         returned in ascending order by hamming distance to query point
   */
  std::vector<ui32> extract_k_nearest() noexcept {
    std::vector<ui32> ret(knn.size(), UINT32_MAX);
    for (int i = knn.size(); !knn.empty(); knn.pop()) {
      ret[--i] = knn.top().second;
    }
    return ret;
  }
  
  /**
   * @brief Returns true if this map contains the point with the given idx in asymptotic O(1) time.
   * @param idx index of the point to check 
   */
  inline bool contains(const ui32& idx) const noexcept { return seen.find(idx) != seen.end(); }
  
  /**
   * @brief Inserts the point with the given idx into this map in asymptotic O(log(k) + D) time 
   *        (upper bound is distance computation of point, which is only executed in case no entry exist).
   * @param idx index of the point to insert
   */
  void insert(const ui32& idx) noexcept {
    if (this->contains(idx)) return;
    seen.emplace(idx);

    ui32 hdist = query.distance(points[idx]);
    if (knn.size() < k) {
      knn.emplace(hdist, idx);
    } else if (hdist < knn.top().first) {
      knn.pop();
      knn.emplace(hdist, idx);
    }
  }
  
  template<iterator_to<ui32> IdxIterator>
  void insert(IdxIterator beg, IdxIterator end) noexcept {
    for (auto it = beg; it != end; ++it) {
      this->insert(*it);
    }
  }
};
