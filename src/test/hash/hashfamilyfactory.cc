#include <gtest/gtest.h>
#include "../../hash/hashfamilyfactory.hpp"

constexpr ui32 D = 100;

TEST(HashFamilyFactoryCreateRandomBits, ConstructsHFContaining_SizeHashFunctions)
{
  auto HF = HashFamilyFactory<D>::createRandomBits(0);
  ASSERT_EQ(HF.size(), 0);

  // Arrange
  const ui32 N = 10;
  // Act
  HF = HashFamilyFactory<D>::createRandomBits(N);
  // Assert
  ASSERT_EQ(HF.size(), N);
}

TEST(HashFamilyFactoryCreateRandomMasks, ConstructsHFContaining_SizeHashFunctions)
{
  auto HF = HashFamilyFactory<D>::createRandomMasks(0);
  ASSERT_EQ(HF.size(), 0);
  // Arrange
  const ui32 N = 10;
  // Act
  HF = HashFamilyFactory<D>::createRandomMasks(N);
  // Assert
  ASSERT_EQ(HF.size(), N);
}

TEST(HashFamilyFactoryCreateRandomMasks, ConstructsHFContaining_RandomMasks)
{
  //! Cant check type of lambdas so have to construct masks with all bits set to check
  // Arrange
  const ui32 N = 10;
  // Act
  auto HF_ALL = HashFamilyFactory<D>::createRandomMasks(N, 1.0),
       HF_NONE = HashFamilyFactory<D>::createRandomMasks(N, 0.0);
  // Assert
  ASSERT_EQ(HF_ALL(random_point<D>(1.0)), 0b1111111111);
  ASSERT_EQ(HF_NONE(random_point<D>(0.0)), 0b1111111111);
}

TEST(HashFamilyFactoryCreateHDist, ConstructsHFContaining_SizeHashFunctions)
{
  auto HF = HashFamilyFactory<D>::createRandomHDist(0);
  ASSERT_EQ(HF.size(), 0);

  // Arrange
  const ui32 N = 10;
  // Act
  HF = HashFamilyFactory<D>::createRandomHDist(N);
  // Assert
  ASSERT_EQ(HF.size(), N);
}

TEST(HashFamilyFactoryCreate, ConstructsHFContaining_SizeHashFunctions_ifSizeIsDivisableByFlags)
{
  auto HF = HashFamilyFactory<D>::create(2, HashType::Bit | HashType::Mask | HashType::Hamming);
  ASSERT_EQ(HF.size(), 0);
  const ui32 N = 9;
  // Act
  HF = HashFamilyFactory<D>::create(N, HashType::Bit | HashType::Mask | HashType::Hamming);;
  // Assert
  ASSERT_EQ(HF.size(), N);
}
