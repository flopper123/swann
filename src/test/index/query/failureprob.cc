#include <gtest/gtest.h>
#include "../../../index/query/failureprob.hpp"

TEST(SingleBitFailure_Accumulative, DoesFailureOfAllMapsAcc_EqualsV1) {
  const ui32 D = 10, hdist = 2, tdepth = 10, tcount = 5;

  for (int depth = 0; depth < 4; ++depth) {
    // depth+1 Because accumulative expects depth to be the current depth searched,
    // but SingleBitFailure expects depth to be the depth for which all buckets are searched
    float exp = SingleBitFailure<D>(tcount, tdepth, depth+1, 0, 10, hdist), 
          act = SingleBitFailure_Accumulative<D>(tcount, tcount, tdepth, depth, hdist, LSHHashMap<D>::mask_count(tdepth, depth));
    ASSERT_FLOAT_EQ(exp, act) << "Received: " 
                              << "\n\tD: " << D 
                              << "\n\thdist: " << hdist
                              << "\n\ttdepth: " << tdepth 
                              << "\n\ttcount: " << tcount
                              << "\n\tdepth: " << depth;
  }
}

TEST(SingleBitFailure_Accumulative, FailureIfNoneSearched) {
  float exp = 1.0,
        act = SingleBitFailure_Accumulative<10>(10, 0, 10, 0, 1, 0);
  ASSERT_FLOAT_EQ(exp, act);
}


TEST(SingleBitFailure_Accumulative, DecreasesForEachTrie) {
  const ui32 D = 10, hdist = 2, tdepth = 10, tcount = 10, depth = 2;
  float prv = 1.0, act = 0;
  for (ui32 t=1; t <= tcount; ++t, prv = act) {
    act = SingleBitFailure_Accumulative<D>(tcount, t, tdepth, depth, hdist, LSHHashMap<D>::mask_count(tdepth, depth));
    ASSERT_GT(prv, act) << "Received: " 
                        << "\n\tD: " << D 
                        << "\n\thdist: " << hdist
                        << "\n\ttdepth: " << tdepth 
                        << "\n\ttrie: " << t
                        << "\n\ttcount: " << tcount
                        << "\n\tdepth: " << depth;
  }
}

TEST(SingleBitFailure_Accumulative, AllBucketsOnDepthD_Eq_NoBucketsOnDpp) {
  const ui32 D = 10, hdist = 2, tdepth = 10, tcount = 10, depth = 2, t=tcount;
  for (ui32 depth=0; depth < tdepth-1; ++depth) {
    float total_buckets = LSHHashMap<D>::mask_count(tdepth, depth);
    float exp = SingleBitFailure_Accumulative<D>(tcount, t, tdepth, depth, hdist, total_buckets),
          act = SingleBitFailure_Accumulative<D>(tcount, t, tdepth, depth+1, hdist, 0);
    ASSERT_FLOAT_EQ(exp, act) << "Received: " 
                              << "\n\tTotal Buckets: " << total_buckets
                              << "\n\tD: " << D 
                              << "\n\thdist: " << hdist
                              << "\n\ttdepth: " << tdepth 
                              << "\n\ttrie: " << t
                              << "\n\ttcount: " << tcount
                              << "\n\tdepth: " << depth;
  }
}

TEST(SingleBitFailure_Accumulative, DecreasesForEachBucket) {
  const ui32 D = 10, hdist = 2, tdepth = 10, tcount = 10, t=tcount/2;
  float prv = 1.0, act = 0;
  for (ui32 depth=0; depth < tdepth; ++depth) {
    for (ui32 buckets=1; buckets <= LSHHashMap<D>::mask_count(tdepth, depth); ++buckets, prv = act) {
      act = SingleBitFailure_Accumulative<D>(tcount, t, tdepth, depth, hdist, buckets);
      ASSERT_GT(prv, act) << "Received: " 
                          << "\n\tD: " << D 
                          << "\n\thdist: " << hdist
                          << "\n\ttdepth: " << tdepth 
                          << "\n\ttrie: " << t
                          << "\n\ttcount: " << tcount
                          << "\n\tdepth: " << depth;
    }
  }
}

