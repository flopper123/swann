#pragma once

#include <stdint.h>
#include <math.h>
#include <unordered_map>
#include <bitset>
#include <vector>
#include <numeric>
#include <cstring> // memcpy and memsets
#include <algorithm> 
#include <cassert>
#include <functional>
#include <concepts>
#include <thread>
#include <execution>
#include <mutex>
#include <queue>

#define ALL(o) (o).begin(), (o).end()
#define hmap std::unordered_map

typedef uint32_t ui32;
typedef uint64_t ui64;

/**
 * @brief A type definition for a maximum priority queue that returns the maximum element.
 *        type T must be comparable using std::less<T>
 * @tparam T type of elements in the queue
 */
template<typename T>
using max_queue = std::priority_queue<T, std::vector<T>, std::less<T>>; 

template<class T, typename TVal>
concept iterator_to = 
  std::is_same_v<typename std::iterator_traits<T>::value_type, TVal>;

template <typename IteratorType>
concept iterator = std::input_iterator<IteratorType>;