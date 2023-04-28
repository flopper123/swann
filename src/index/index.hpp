#pragma once

#include "../global.hpp"
#include "point.hpp"

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
     */
    virtual std::vector<ui32> query(const Point<D>& point, int k, float recall) = 0;

    /**
     * @param i index of the point to return
     */
    virtual const Point<D> &operator[](ui32 i) const noexcept = 0;
};
