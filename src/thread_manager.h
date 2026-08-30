#ifndef BENCHMARK_THREAD_MANAGER_H
#define BENCHMARK_THREAD_MANAGER_H

#include <atomic>

#include "benchmark/counter.h"
#include "benchmark/statistics.h"
#include "benchmark/types.h"
#include "mutex.h"

namespace benchmark {
namespace internal {

class ThreadManager {
 public:
  explicit ThreadManager(int num_threads)
      : num_threads_(num_threads), start_stop_barrier_(num_threads) {}

  // The number of threads that will actually run the benchmark body under
  // this manager. This is the benchmark's configured thread count for a
  // normal repetition, but 1 for the single-threaded memory-manager and
  // profiler passes.
  int num_threads() const { return num_threads_; }

  Mutex& GetBenchmarkMutex() const RETURN_CAPABILITY(benchmark_mutex_) {
    return benchmark_mutex_;
  }

  bool StartStopBarrier() { return start_stop_barrier_.wait(); }

  void NotifyThreadComplete() { start_stop_barrier_.removeThread(); }

  struct Result {
    IterationCount iterations = 0;
    double real_time_used = 0;
    double cpu_time_used = 0;
    double manual_time_used = 0;
    int64_t complexity_n = 0;
    std::string report_label_;
    std::string skip_message_;
    internal::Skipped skipped_ = internal::NotSkipped;
    UserCounters counters;
  };
  GUARDED_BY(GetBenchmarkMutex()) Result results;

 private:
  const int num_threads_;
  mutable Mutex benchmark_mutex_;
  Barrier start_stop_barrier_;
};

}  // namespace internal
}  // namespace benchmark

#endif  // BENCHMARK_THREAD_MANAGER_H
