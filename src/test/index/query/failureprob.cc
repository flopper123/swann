#include <gtest/gtest.h>

#include "../../../index/query/failureprob.hpp"

TEST(SingleBitFailure_Accumulative, DoesFailureOfAllMapsAcc_EqualsV1) {
  const ui32 D = 10, hdist = 2, tdepth = 10, tcount = 5;

  for (int depth = 0; depth < 4; ++depth) {
    float exp = SingleBitFailure<D>(tcount, tdepth, depth, 0, 10, hdist),
          act = SingleBitFailure_Accumulative<D>(tcount, tcount, tdepth, depth, hdist, LSHHashMap<D>::mask_count(tdepth, depth));
    ASSERT_FLOAT_EQ(exp, act) << "Received: " 
                              << "\n\tD: " << D 
                              << "\n\thdist: " << hdist
                              << "\n\ttdepth: " << tdepth 
                              << "\n\ttcount: " << tcount
                              << "\n\tdepth: " << depth;
  }
}


TEST(SingleBitFailure_Accumulative, CanCountBucketFraction) {
  const ui32 D = 10, hdist = 2, tdepth = 11, tcount = 5, depth = 1, 
             buckets = LSHHashMap<D>::mask_count(tdepth, depth), t = 2;

    float exp = 0.5543,
          act = SingleBitFailure_Accumulative<D>(tcount, t, tdepth, depth, hdist, buckets);
    ASSERT_FLOAT_EQ(exp, act) << "Received: " 
                              << "\n\tBuckets: " << buckets
                              << "\n\tD: " << D 
                              << "\n\thdist: " << hdist
                              << "\n\ttdepth: " << tdepth 
                              << "\n\ttrie: " << t
                              << "\n\ttcount: " << tcount
                              << "\n\tdepth: " << depth;
}

