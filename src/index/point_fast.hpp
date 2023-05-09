#pragma once

#include <random>
#include <bitset>
#include <functional>
#include "../global.hpp"

static inline ui64 fast_popcount64(ui64 x) noexcept
{
  x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
  return (x * 0x0101010101010101ULL) >> 56;
}

/** Binary represented by a custom bitset optimized for distance and xOR  */
template<ui32 D>
class PointFast {
  public:
    static constexpr size_t wordCnt = std::ceil(D / 64.0f); // Use std::ceil to keep as constexpr
    ui64 words[wordCnt]; // D bits fragmented into 64-bit pieces (words)

    PointFast() {
      memset(words, 0, sizeof(words));
    };
    
    PointFast(const PointFast<D>& other) {
      memcpy(words, other.words, sizeof(words));
    }

    inline PointFast<D> operator~() const noexcept {
      PointFast<D> ret(*this);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] = ~ret.words[i];
      }
      return ret;
    }

    friend inline PointFast<D> operator^(const PointFast<D>& p1, const PointFast<D>& p2) noexcept {
      PointFast<D> ret(p1);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] ^= p2.words[i];
      }
      return ret;
    }

    inline PointFast& operator^=(const PointFast<D> &other) noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        words[i] ^= other.words[i];
      }
    }

    ui64 count() const noexcept {
      ui64 cnt = 0;
      for(ui32 i = 0; i < wordCnt; ++i) {
        cnt += fast_popcount64(words[i]);
      }
      return cnt;
    }
    
    bool empty() const noexcept {
      return this->count() == 0;
    }

    ui32 distance(const PointFast<D>&other) const noexcept {
      return (*this ^ other).count();
    }    

    /** 
     * @brief Generates a random point from a Bernoulli distribution
     * @param p controls the Bernoulli distribution
     */
    static inline PointFast<D> Random(double p = 0.5) {
      PointFast<D> bset;
      std::random_device rd;
      std::mt19937 gen(rd());
      std::bernoulli_distribution d(p);
      
      for(int w = 0; w < bset.wordCnt; ++w) {
        ui64 wrd = 0x0;
        for (ui64 b = 0; b < 64ULL; ++b) {
          if (d(gen)) wrd |= (1ULL << b);
        }
        bset.words[w] = wrd;
      }

      return bset;
    }
};

