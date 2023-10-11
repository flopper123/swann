#pragma once

#include "../global.hpp"
#include "point.hpp"

/**
 * @brief A log of the query. Can be passed to indexes when querying to 
 *        log additional information about the query
 */
struct QueryLog {
  ui32 mask_index = 0, // The index of the mask we stopped at
       hdist = 0,      // The hamming distance of the mask we stopped at
       found = 0,      // The total number of points found
       visited = 0;    // The number of buckets visited
  ui32 kth_dist = 0;   // The hamming distance of the kth point found
  float fail_prob = 1.0; // The failure probability of the query

  friend std::ostream &operator<<(std::ostream &os, const QueryLog &log)
  {
    os << "QueryLog: " << std::endl
       << "\tStopped at mask_index: " << log.mask_index << std::endl
       << "\tStopped at hdist: " << log.hdist << std::endl
       << "\tTotal points found: " << log.found << std::endl
       << "\tBuckets visited: " << log.visited << std::endl
       << "\tFailure probability: " << log.fail_prob << std::endl
        << "\tHamming distance of kth point: " << log.kth_dist << std::endl;
    return os;
  }
};

template<ui32 D>
class Index {
  public:
    /**
     * @returns Number of points inserted into the index 
     */
    virtual ui32 size() const noexcept = 0;

    /**
     * @brief prepares the index for querying by building the index 
     *        on the currently inserted points
     */
    virtual void build() = 0;

    /**
     * @param point Point to insert into the index
     */
    virtual void insert(Point<D>& point) = 0;
    
    /**
     * @param point Point to insert into the index
     * @param k number of nearest neighbors to return
     * @param recall The precision of the query
     * @param log A ptr to a query log to use for storing additional query information.
     */
    virtual std::vector<ui32> query(const Point<D>& point, int k, float recall, QueryLog* log = nullptr) = 0;

    /**
     * @param i index of the point to return
     */
    virtual const Point<D> &operator[](ui32 i) const noexcept = 0;
};
