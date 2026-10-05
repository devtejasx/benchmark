#include <vector>

#include "../src/complexity.h"
#include "gtest/gtest.h"

namespace benchmark {
namespace {

std::vector<BenchmarkReporter::Run> RunsWithoutComplexityN(BigO complexity) {
  std::vector<BenchmarkReporter::Run> runs(3);
  for (auto& run : runs) {
    run.complexity = complexity;
    run.iterations = 1;
    run.real_accumulated_time = 1;
    run.cpu_accumulated_time = 1;
    // complexity_n stays 0: SetComplexityN() was never called.
  }
  return runs;
}

// Unlike most BM_CHECKs, this one is enforced in release builds too: there the
// fit would otherwise report "nan N" for oN, or "(1)" for oAuto.
TEST(ComputeBigOTest, DiesWithoutComplexityN) {
#if GTEST_HAS_DEATH_TEST
  ASSERT_DEATH_IF_SUPPORTED(
      { ComputeBigO(RunsWithoutComplexityN(oN)); },
      "Did you forget to call SetComplexityN\\?");
  ASSERT_DEATH_IF_SUPPORTED(
      { ComputeBigO(RunsWithoutComplexityN(oAuto)); },
      "Did you forget to call SetComplexityN\\?");
#endif
}

TEST(ComputeBigOTest, FitsWithComplexityN) {
  std::vector<BenchmarkReporter::Run> runs = RunsWithoutComplexityN(oN);
  for (size_t i = 0; i < runs.size(); ++i) {
    runs[i].complexity_n = static_cast<ComplexityN>(1) << i;
    runs[i].real_accumulated_time = static_cast<double>(runs[i].complexity_n);
    runs[i].cpu_accumulated_time = static_cast<double>(runs[i].complexity_n);
  }
  const std::vector<BenchmarkReporter::Run> results = ComputeBigO(runs);
  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].complexity, oN);
  EXPECT_DOUBLE_EQ(results[0].real_accumulated_time, 1.0);
}

}  // namespace
}  // namespace benchmark
