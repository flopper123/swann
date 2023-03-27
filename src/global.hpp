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

#define ALL(o) (o).begin(), (o).end()
#define hmap std::unordered_map

typedef uint32_t ui32;
typedef uint64_t ui64;

template<class T, typename TVal>
concept iterator_to = 
  std::is_same_v<typename std::iterator_traits<T>::value_type, TVal>;

template <typename IteratorType>
concept iterator = std::input_iterator<IteratorType>;