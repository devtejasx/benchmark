//===---------------------------------------------------------------------===//
// statistics_test - Unit tests for src/statistics.cc
//===---------------------------------------------------------------------===//

#include "../src/statistics.h"
#include "gtest/gtest.h"

namespace {
TEST(StatisticsTest, Mean) {
  EXPECT_DOUBLE_EQ(benchmark::StatisticsMean({42, 42, 42, 42}), 42.0);
  EXPECT_DOUBLE_EQ(benchmark::StatisticsMean({1, 2, 3, 4}), 2.5);
  EXPECT_DOUBLE_EQ(benchmark::StatisticsMean({1, 2, 5, 10, 10, 14}), 7.0);
}

TEST(StatisticsTest, Median) {
  EXPECT_DOUBLE_EQ(benchmark::StatisticsMedian({42, 42, 42, 42}), 42.0);
  EXPECT_DOUBLE_EQ(benchmark::StatisticsMedian({1, 2, 3, 4}), 2.5);
  EXPECT_DOUBLE_EQ(benchmark::StatisticsMedian({1, 2, 5, 10, 10}), 5.0);
}

TEST(StatisticsTest, StdDev) {
  EXPECT_DOUBLE_EQ(benchmark::StatisticsStdDev({101, 101, 101, 101}), 0.0);
  EXPECT_DOUBLE_EQ(benchmark::StatisticsStdDev({1, 2, 3}), 1.0);
  EXPECT_DOUBLE_EQ(benchmark::StatisticsStdDev({2.5, 2.4, 3.3, 4.2, 5.1}),
                   1.151086443322134);
}

TEST(StatisticsTest, CV) {
  EXPECT_DOUBLE_EQ(benchmark::StatisticsCV({101, 101, 101, 101}), 0.0);
  EXPECT_DOUBLE_EQ(benchmark::StatisticsCV({1, 2, 3}), 1. / 2.);
  ASSERT_NEAR(benchmark::StatisticsCV({2.5, 2.4, 3.3, 4.2, 5.1}),
              0.32888184094918121, 1e-15);
}

// Samples that are large and close together, which is what repetitions of a
// real benchmark look like: a one-second run reported in nanoseconds is ~1e9,
// and repetitions differ by a handful. Computing the variance as
// mean(x^2) - mean(x)^2 cancels away every significant digit at that
// magnitude, reporting 0 for the first case and an answer ~19x too large for
// the second.
TEST(StatisticsTest, StdDevOfLargeCloseSamples) {
  EXPECT_NEAR(
      benchmark::StatisticsStdDev({1e9, 1e9 + 1, 1e9 + 2, 1e9 + 3, 1e9 + 4}),
      1.5811388300841898, 1e-9);
  EXPECT_NEAR(benchmark::StatisticsStdDev(
                  {5e9, 5e9 + 2, 5e9 + 4, 5e9 + 6, 5e9 + 8, 5e9 + 10}),
              3.7416573867739413, 1e-9);
  // Identical large samples have no spread at all.
  EXPECT_DOUBLE_EQ(benchmark::StatisticsStdDev({1e9, 1e9, 1e9, 1e9}), 0.0);
}

TEST(StatisticsTest, CVOfLargeCloseSamples) {
  EXPECT_NEAR(
      benchmark::StatisticsCV({1e9, 1e9 + 1, 1e9 + 2, 1e9 + 3, 1e9 + 4}),
      1.5811388269219122e-09, 1e-18);
}

}  // end namespace
