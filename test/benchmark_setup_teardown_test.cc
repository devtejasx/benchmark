#include <atomic>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>

#include "benchmark/benchmark_api.h"
#include "benchmark/registration.h"
#include "benchmark/state.h"

// Test that Setup() and Teardown() are called exactly once
// for each benchmark run (single-threaded).
namespace {
namespace singlethreaded {
static int setup_call = 0;
static int teardown_call = 0;
}  // namespace singlethreaded
}  // namespace
static void DoSetup1(const benchmark::State& state) {
  ++singlethreaded::setup_call;

  // Setup/Teardown should never be called with any thread_idx != 0.
  assert(state.thread_index() == 0);
}

static void DoTeardown1(const benchmark::State& state) {
  ++singlethreaded::teardown_call;
  assert(state.thread_index() == 0);
}

static void BM_with_setup(benchmark::State& state) {
  for (auto s : state) {
  }
}
BENCHMARK(BM_with_setup)
    ->Arg(1)
    ->Arg(3)
    ->Arg(5)
    ->Arg(7)
    ->Iterations(100)
    ->Setup(DoSetup1)
    ->Teardown(DoTeardown1);

// Test that Setup() and Teardown() are called once for each group of threads.
namespace {
namespace concurrent {
static std::atomic<int> setup_call(0);
static std::atomic<int> teardown_call(0);
static std::atomic<int> func_call(0);
}  // namespace concurrent

void DoSetup2(const benchmark::State& state) {
  concurrent::setup_call.fetch_add(1, std::memory_order_acquire);
  assert(state.thread_index() == 0);
}

void DoTeardown2(const benchmark::State& state) {
  concurrent::teardown_call.fetch_add(1, std::memory_order_acquire);
  assert(state.thread_index() == 0);
}

void BM_concurrent(benchmark::State& state) {
  for (auto s : state) {
  }
  concurrent::func_call.fetch_add(1, std::memory_order_acquire);
}

BENCHMARK(BM_concurrent)
    ->Setup(DoSetup2)
    ->Teardown(DoTeardown2)
    ->Iterations(100)
    ->Threads(5)
    ->Threads(10)
    ->Threads(15);

// Testing interaction with Fixture::Setup/Teardown
namespace fixture_interaction {
int setup = 0;
int fixture_setup = 0;
}  // namespace fixture_interaction

#define FIXTURE_BECHMARK_NAME MyFixture

class FIXTURE_BECHMARK_NAME : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& /*unused*/) override {
    fixture_interaction::fixture_setup++;
  }

  ~FIXTURE_BECHMARK_NAME() override {}
};

BENCHMARK_F(FIXTURE_BECHMARK_NAME, BM_WithFixture)(benchmark::State& st) {
  for (auto _ : st) {
  }
}

void DoSetupWithFixture(const benchmark::State& /*unused*/) {
  fixture_interaction::setup++;
}

BENCHMARK_REGISTER_F(FIXTURE_BECHMARK_NAME, BM_WithFixture)
    ->Arg(1)
    ->Arg(3)
    ->Arg(5)
    ->Arg(7)
    ->Setup(DoSetupWithFixture)
    ->Repetitions(1)
    ->Iterations(100);

// Testing repetitions.
namespace repetitions {
int setup = 0;
}

void DoSetupWithRepetitions(const benchmark::State& /*unused*/) {
  repetitions::setup++;
}
void BM_WithRep(benchmark::State& state) {
  for (auto _ : state) {
  }
}

BENCHMARK(BM_WithRep)
    ->Arg(1)
    ->Arg(3)
    ->Arg(5)
    ->Arg(7)
    ->Setup(DoSetupWithRepetitions)
    ->Iterations(100)
    ->Repetitions(4);

// Testing a benchmark that does not fix its iteration count. Every case above
// pins one with Iterations(), which is what makes their call counts land on a
// single setup per benchmark. Without it the runner calls the benchmark
// function again and again while it searches for an iteration count that
// reaches --benchmark_min_time, and the callbacks bracket every one of those
// runs, so the total is larger than one and is not knowable in advance. What
// holds either way is one setup and one teardown per run.
namespace iteration_search {
int setup = 0;
int teardown = 0;
int runs = 0;
}  // namespace iteration_search

void DoSetupWithIterationSearch(const benchmark::State& /*unused*/) {
  iteration_search::setup++;
}
void DoTeardownWithIterationSearch(const benchmark::State& /*unused*/) {
  iteration_search::teardown++;
}
void BM_WithIterationSearch(benchmark::State& state) {
  iteration_search::runs++;
  for (auto _ : state) {
  }
}

BENCHMARK(BM_WithIterationSearch)
    ->Setup(DoSetupWithIterationSearch)
    ->Teardown(DoTeardownWithIterationSearch);
}  // namespace

int main(int argc, char** argv) {
  benchmark::MaybeReenterWithoutASLR(argc, argv);

  benchmark::Initialize(&argc, argv);

  size_t ret = benchmark::RunSpecifiedBenchmarks(".");
  assert(ret > 0);

  // Setup/Teardown is called once for each arg group (1,3,5,7).
  assert(singlethreaded::setup_call == 4);
  assert(singlethreaded::teardown_call == 4);

  // 3 group of threads calling this function (3,5,10).
  assert(concurrent::setup_call.load(std::memory_order_relaxed) == 3);
  assert(concurrent::teardown_call.load(std::memory_order_relaxed) == 3);
  assert((5 + 10 + 15) ==
         concurrent::func_call.load(std::memory_order_relaxed));

  // Setup is called 4 times, once for each arg group (1,3,5,7)
  assert(fixture_interaction::setup == 4);
  // Fixture::Setup is called every time the bm routine is run.
  // The exact number is indeterministic, so we just assert that
  // it's more than setup.
  assert(fixture_interaction::fixture_setup > fixture_interaction::setup);

  // Setup is call once for each repetition * num_arg =  4 * 4 = 16.
  assert(repetitions::setup == 16);

  // With no explicit iteration count the benchmark function is run repeatedly
  // while the iteration count is searched for. The number of runs depends on
  // the machine, so only the invariant is asserted: one setup and one teardown
  // around each run, and more than one run.
  assert(iteration_search::runs > 1);
  assert(iteration_search::setup == iteration_search::runs);
  assert(iteration_search::teardown == iteration_search::runs);

  return 0;
}
