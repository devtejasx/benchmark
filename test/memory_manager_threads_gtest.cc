// The MemoryManager pass re-runs the benchmark body on a single thread, so
// State::threads() must report 1 there. Reporting the configured thread count
// deadlocks any body that synchronises across State::threads() participants.
// See #1849.

#include <atomic>
#include <vector>

#include "benchmark/benchmark.h"
#include "gtest/gtest.h"

namespace benchmark {
namespace {

constexpr int kThreads = 4;

std::atomic<bool> in_measurement_window{false};
std::atomic<int> threads_seen_in_window{0};
std::atomic<int> threads_seen_outside_window{0};

class ThreadCountingMemoryManager : public MemoryManager {
 public:
  void Start() override { in_measurement_window = true; }
  void Stop(Result& result) override {
    in_measurement_window = false;
    result.num_allocs = 0;
    result.max_bytes_used = 0;
  }
};

void BM_threads(State& state) {
  for (auto _ : state) {
    if (in_measurement_window) {
      threads_seen_in_window = state.threads();
    } else {
      threads_seen_outside_window = state.threads();
    }
  }
}
BENCHMARK(BM_threads)->Iterations(1)->Threads(kThreads);

// Swallows reporter output.
class NullReporter : public BenchmarkReporter {
 public:
  bool ReportContext(const Context&) override { return true; }
  void ReportRuns(const std::vector<Run>&) override {}
};

}  // namespace

TEST(MemoryManagerThreads, MeasurementPassReportsOneThread) {
  ThreadCountingMemoryManager mm;
  RegisterMemoryManager(&mm);
  NullReporter reporter;
  const size_t ran = RunSpecifiedBenchmarks(&reporter);
  RegisterMemoryManager(nullptr);

  EXPECT_GT(ran, 0u);
  EXPECT_EQ(threads_seen_outside_window.load(), kThreads)
      << "the timed pass should report the configured thread count";
  EXPECT_EQ(threads_seen_in_window.load(), 1)
      << "the MemoryManager pass runs on one thread, so State::threads() must "
         "report 1 - a body waiting for State::threads() participants would "
         "otherwise never make progress";
}

}  // namespace benchmark
