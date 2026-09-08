#include "../src/benchmark_runner.h"
#include "gtest/gtest.h"

namespace {

TEST(ParseMinTimeTest, InvalidInput) {
#if GTEST_HAS_DEATH_TEST
  // A malformed value is rejected in every build. It used to be a BM_CHECK,
  // which compiles away under NDEBUG, so a release build silently misread the
  // flag instead of reporting it.
  ASSERT_DEATH_IF_SUPPORTED(
      { benchmark::internal::ParseBenchMinTime("abc"); },
      "Malformed value passed to --benchmark_min_time: `abc`");

  ASSERT_DEATH_IF_SUPPORTED(
      { benchmark::internal::ParseBenchMinTime("123ms"); },
      "Malformed value passed to --benchmark_min_time: `123ms`");

  ASSERT_DEATH_IF_SUPPORTED(
      { benchmark::internal::ParseBenchMinTime("1z"); },
      "Malformed value passed to --benchmark_min_time: `1z`");

  ASSERT_DEATH_IF_SUPPORTED(
      { benchmark::internal::ParseBenchMinTime("1hs"); },
      "Malformed value passed to --benchmark_min_time: `1hs`");

  // No digits at all before the 'x' suffix. strtol consumes nothing and leaves
  // p_end on the 'x', so this used to parse as zero iterations.
  ASSERT_DEATH_IF_SUPPORTED(
      { benchmark::internal::ParseBenchMinTime("xyzx"); },
      "Malformed value passed to --benchmark_min_time: `xyzx`");

  ASSERT_DEATH_IF_SUPPORTED(
      { benchmark::internal::ParseBenchMinTime("x"); },
      "Malformed value passed to --benchmark_min_time: `x`");

  // Out of range for double. strtod sets ERANGE and returns HUGE_VAL, which
  // used to be taken as the minimum time, so the benchmark never finished.
  ASSERT_DEATH_IF_SUPPORTED(
      { benchmark::internal::ParseBenchMinTime("1e999s"); },
      "Malformed value passed to --benchmark_min_time: `1e999s`");
#endif
}

TEST(ParseMinTimeTest, ValidInput) {
  auto seconds = benchmark::internal::ParseBenchMinTime("0.5s");
  EXPECT_EQ(seconds.tag, benchmark::internal::BenchTimeType::TIME);
  EXPECT_DOUBLE_EQ(seconds.time, 0.5);

  // The suffix is optional; it only warns.
  auto bare = benchmark::internal::ParseBenchMinTime("2");
  EXPECT_EQ(bare.tag, benchmark::internal::BenchTimeType::TIME);
  EXPECT_DOUBLE_EQ(bare.time, 2.0);

  auto iters = benchmark::internal::ParseBenchMinTime("3x");
  EXPECT_EQ(iters.tag, benchmark::internal::BenchTimeType::ITERS);
  EXPECT_EQ(iters.iters, 3);

  auto empty = benchmark::internal::ParseBenchMinTime("");
  EXPECT_EQ(empty.tag, benchmark::internal::BenchTimeType::TIME);
  EXPECT_DOUBLE_EQ(empty.time, 0.0);
}
}  // namespace
