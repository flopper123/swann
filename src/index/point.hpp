#pragma once

#include <random>
#include <bitset>
#include <functional>
#include "../global.hpp"
#include <ostream>

static inline ui64 debrujin_count(ui64 x) noexcept
{
  x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
  return (x * 0x0101010101010101ULL) >> 56;
}

/** 
 * @brief A D dimensional binary vector, optimized for Hamming distance 
 *        The interface of the implementation is made to be as similar as possible to std::bitset
 *        The only exception is a few missing constexpr and the [] operator, which does not return 
 *        a reference. Hence the bits can only be set using the set() method or the constructor.
 */
template<ui32 D>
class Point {
  public:
    static constexpr size_t wordCnt = std::ceil(D / 64.0f); // Use std::ceil to keep as constexpr
    ui64 words[wordCnt]; // D bits fragmented into 64-bit pieces (words) - words[0] is the least significant word
    
    constexpr Point() : words() {};
    constexpr Point(const Point<D>& other) = default;
    
    /**
     * @brief Constructs a point from a single word, such that all words in the point will 
     *        be set to `val`
     * @param val 
     */
    constexpr Point(ui64 val) { words[0] = val; };
    
    /** 
     * @brief Constructs a point from a vector of words
     * @param words vector of words
     */
    Point(const std::vector<ui64>& words) { 
      memcpy(this->words, words.data(), sizeof(this->words));
    }
    
    /** 
     * @brief Constructs a point from a string of 0s and 1s
     * @param str string of 0s and 1s
     * @param zero character representing 0
     * @param one character representing 1
     */
    Point(const std::string& str,
          const char zero = '0',
          const char one = '1') 
    {
      if (str.length() != D) {
        throw std::invalid_argument("String length does not match dimension");
      }
      const ui64 hi = std::ceil(str.length() / 64.0f);
      for (ui64 i = 0; i < hi; ++i) {
        for (ui64 l = i * 64ULL; l < (i + 1ULL) * 64ULL; ++l) {
          if (str[l] == one) {
            words[i] |= (1ULL << (l % 64ULL));
          } else if (str[l] != zero) {
            throw std::invalid_argument("Invalid character in string");
          }
        }
      }
    }
    
    constexpr ui64 size() const noexcept { return D; }
    
    inline void set(const ui64 pos, const bool val = true) noexcept {
      if (val) {
        words[pos / 64ULL] |= (1ULL << (pos % 64ULL));
      } else {
        words[pos / 64ULL] &= ~(1ULL << (pos % 64ULL));
      }
    }

    inline bool any() const noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        if (words[i] != 0) return true;
      }
      return false;
    }
    
    inline bool none() const noexcept { return !any(); }
    inline bool empty() const noexcept { return none(); }

    inline bool all() const noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        if (words[i] != ~0ULL) return false;
      }
      return true;
    }
    
    inline void flip() noexcept { 
      for(ui32 i = 0; i < wordCnt; ++i) {
        words[i] = ~words[i];
      }
    }
    
    inline void reset() noexcept { memset(words, 0, sizeof(words)); }
    
    inline ui64 count() const noexcept {
      ui64 cnt = 0;
      for(ui32 i = 0; i < wordCnt; ++i) {
        cnt += debrujin_count(words[i]);
      }
      return cnt;
    }
    
    inline ui64 distance(const Point<D>&other) const noexcept {
      return (*this ^ other).count();
    }    
    
    inline bool operator[](const ui64 pos) const noexcept {
      return (words[pos / 64ULL] >> (pos % 64ULL)) & 1ULL;
    }

    inline Point<D> operator~() const noexcept {
      Point<D> ret(*this);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] = ~ret.words[i];
      }
      return ret;
    }

    friend inline Point<D> operator^(const Point<D>& p1, const Point<D>& p2) noexcept {
      Point<D> ret(p1);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] ^= p2.words[i];
      }
      return ret;
    }

    inline Point<D>& operator^=(const Point<D> &other) noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        words[i] ^= other.words[i];
      }
      return *this;
    }

    friend inline Point<D> operator&(const Point<D> &p1, const Point<D>& p2) noexcept {
      Point<D> ret(p1);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] &= p2.words[i];
      }
      return ret;
    }
    
    inline Point<D>& operator&=(const Point<D> &other) noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        words[i] &= other.words[i];
      }
      return *this;
    }

    friend inline Point<D> operator|(const Point<D> &p1, const Point<D>& p2) noexcept {
      Point<D> ret(p1);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] |= p2.words[i];
      }
      return ret;
    }
    
    inline Point<D>& operator|=(const Point<D> &other) noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        words[i] |= other.words[i];
      }
      return *this;
    }

    inline Point<D>& operator<<=(const ui64& shift) noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        words[i] <<= shift;
      }
      return *this;
    }

    friend inline Point<D> operator<<(const Point<D>& p1, const ui64& shift) noexcept {
      Point<D> ret(p1);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] <<= shift;
      }
      return ret;
    }

    inline Point<D>& operator>>=(const ui64& shift) noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        words[i] >>= shift;
      }
      return *this;
    }

    friend inline Point<D> operator>>(const Point<D>& p1, const ui64& shift) noexcept {
      Point<D> ret(p1);
      for(ui32 i = 0; i < wordCnt; ++i) {
        ret.words[i] >>= shift;
      }
      return ret;
    }
    
    friend inline bool operator==(const Point<D>& p1, const Point<D>& p2) noexcept {
      for(ui32 i = 0; i < wordCnt; ++i) {
        if (p1.words[i] != p2.words[i]) return false;
      }
      return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point<D>& p) {
      for(ui32 i = 0; i < wordCnt; ++i) {
        os << std::bitset<64>(p.words[i]);
      }
      return os;
    }
    
    /** 
     * @brief Generates a random point from a Bernoulli distribution
     * @param p controls the Bernoulli distribution
     */
    static inline Point<D> Random(double p = 0.5) {
      Point<D> bset;
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

    static inline Point<D> Empty = Point<D>();
};
