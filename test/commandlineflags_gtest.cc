#include <cstdlib>

#include "../src/commandlineflags.h"
#include "../src/internal_macros.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace benchmark {
namespace {

#if defined(BENCHMARK_OS_WINDOWS)
int setenv(const char* name, const char* value, int overwrite) {
  if (!overwrite) {
    // NOTE: getenv_s is far superior but not available under mingw.
    char* env_value = getenv(name);
    if (env_value == nullptr) {
      return -1;
    }
  }
  return _putenv_s(name, value);
}

int unsetenv(const char* name) { return _putenv_s(name, ""); }

#endif  // BENCHMARK_OS_WINDOWS

TEST(BoolFromEnv, Default) {
  ASSERT_EQ(unsetenv("NOT_IN_ENV"), 0);
  EXPECT_EQ(BoolFromEnv("not_in_env", true), true);
}

TEST(BoolFromEnv, False) {
  ASSERT_EQ(setenv("IN_ENV", "0", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "N", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "n", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "NO", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "No", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "no", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "F", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "f", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "FALSE", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "False", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "false", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "OFF", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "Off", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "off", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", true), false);
  unsetenv("IN_ENV");
}

TEST(BoolFromEnv, True) {
  ASSERT_EQ(setenv("IN_ENV", "1", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "Y", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "y", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "YES", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "Yes", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "yes", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "T", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "t", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "TRUE", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "True", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "true", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "ON", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "On", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

  ASSERT_EQ(setenv("IN_ENV", "on", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");

#ifndef BENCHMARK_OS_WINDOWS
  ASSERT_EQ(setenv("IN_ENV", "", 1), 0);
  EXPECT_EQ(BoolFromEnv("in_env", false), true);
  unsetenv("IN_ENV");
#endif
}

TEST(Int32FromEnv, NotInEnv) {
  ASSERT_EQ(unsetenv("NOT_IN_ENV"), 0);
  EXPECT_EQ(Int32FromEnv("not_in_env", 42), 42);
}

TEST(Int32FromEnv, InvalidInteger) {
  ASSERT_EQ(setenv("IN_ENV", "foo", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 42), 42);
  unsetenv("IN_ENV");
}

TEST(Int32FromEnv, ValidInteger) {
  ASSERT_EQ(setenv("IN_ENV", "42", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 64), 42);
  unsetenv("IN_ENV");
}

TEST(Int32FromEnv, EmptyValueReturnsDefault) {
  // An empty value is not the integer 0. (On Windows, setting a variable to
  // the empty string removes it, which gives the same result.)
  ASSERT_EQ(setenv("IN_ENV", "", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 42), 42);
  unsetenv("IN_ENV");
}

TEST(Int32FromEnv, Int32Limits) {
  ASSERT_EQ(setenv("IN_ENV", "2147483647", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 42), 2147483647);
  ASSERT_EQ(setenv("IN_ENV", "-2147483648", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 42), -2147483647 - 1);
  ASSERT_EQ(setenv("IN_ENV", "2147483648", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 42), 42);
  ASSERT_EQ(setenv("IN_ENV", "-2147483649", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 42), 42);
  ASSERT_EQ(setenv("IN_ENV", "99999999999999999999", 1), 0);
  EXPECT_EQ(Int32FromEnv("in_env", 42), 42);
  unsetenv("IN_ENV");
}

TEST(DoubleFromEnv, EmptyValueReturnsDefault) {
  ASSERT_EQ(setenv("IN_ENV", "", 1), 0);
  EXPECT_EQ(DoubleFromEnv("in_env", 0.51), 0.51);
  unsetenv("IN_ENV");
}

TEST(DoubleFromEnv, NotInEnv) {
  ASSERT_EQ(unsetenv("NOT_IN_ENV"), 0);
  EXPECT_EQ(DoubleFromEnv("not_in_env", 0.51), 0.51);
}

TEST(DoubleFromEnv, InvalidReal) {
  ASSERT_EQ(setenv("IN_ENV", "foo", 1), 0);
  EXPECT_EQ(DoubleFromEnv("in_env", 0.51), 0.51);
  unsetenv("IN_ENV");
}

TEST(DoubleFromEnv, ValidReal) {
  ASSERT_EQ(setenv("IN_ENV", "0.51", 1), 0);
  EXPECT_EQ(DoubleFromEnv("in_env", 0.71), 0.51);
  unsetenv("IN_ENV");
}

TEST(StringFromEnv, Default) {
  ASSERT_EQ(unsetenv("NOT_IN_ENV"), 0);
  EXPECT_STREQ(StringFromEnv("not_in_env", "foo"), "foo");
}

TEST(StringFromEnv, Valid) {
  ASSERT_EQ(setenv("IN_ENV", "foo", 1), 0);
  EXPECT_STREQ(StringFromEnv("in_env", "bar"), "foo");
  unsetenv("IN_ENV");
}

TEST(KvPairsFromEnv, Default) {
  ASSERT_EQ(unsetenv("NOT_IN_ENV"), 0);
  EXPECT_THAT(KvPairsFromEnv("not_in_env", {{"foo", "bar"}}),
              testing::ElementsAre(testing::Pair("foo", "bar")));
}

TEST(KvPairsFromEnv, MalformedReturnsDefault) {
  ASSERT_EQ(setenv("IN_ENV", "foo", 1), 0);
  EXPECT_THAT(KvPairsFromEnv("in_env", {{"foo", "bar"}}),
              testing::ElementsAre(testing::Pair("foo", "bar")));
  unsetenv("IN_ENV");
}

TEST(KvPairsFromEnv, Single) {
  ASSERT_EQ(setenv("IN_ENV", "foo=bar", 1), 0);
  EXPECT_THAT(KvPairsFromEnv("in_env", {}),
              testing::ElementsAre(testing::Pair("foo", "bar")));
  unsetenv("IN_ENV");
}

TEST(KvPairsFromEnv, Multiple) {
  ASSERT_EQ(setenv("IN_ENV", "foo=bar,baz=qux", 1), 0);
  EXPECT_THAT(KvPairsFromEnv("in_env", {}),
              testing::UnorderedElementsAre(testing::Pair("foo", "bar"),
                                            testing::Pair("baz", "qux")));
  unsetenv("IN_ENV");
}

TEST(ParseInt32Flag, EmptyValueIsRejected) {
  int32_t value = 42;
  EXPECT_FALSE(ParseInt32Flag(
      "--benchmark_repetitions=", "benchmark_repetitions", &value));
  EXPECT_EQ(value, 42);
}

TEST(ParseInt32Flag, AcceptsInt32Max) {
  int32_t value = 42;
  EXPECT_TRUE(ParseInt32Flag("--benchmark_repetitions=2147483647",
                             "benchmark_repetitions", &value));
  EXPECT_EQ(value, 2147483647);
}

TEST(ParseDoubleFlag, EmptyValueIsRejected) {
  double value = 0.5;
  EXPECT_FALSE(ParseDoubleFlag(
      "--benchmark_min_warmup_time=", "benchmark_min_warmup_time", &value));
  EXPECT_EQ(value, 0.5);
}

}  // namespace
}  // namespace benchmark
