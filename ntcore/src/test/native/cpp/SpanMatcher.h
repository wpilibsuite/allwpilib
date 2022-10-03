// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_SPANMATCHER_H_
#define NTCORE_SPANMATCHER_H_

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

#include <wpi/span.h>

#include "TestPrinters.h"
#include "gmock/gmock.h"

namespace wpi {

template <typename T>
class SpanMatcher : public ::testing::MatcherInterface<span<T>> {
 public:
  explicit SpanMatcher(span<T> good_) : good{good_.begin(), good_.end()} {}

  bool MatchAndExplain(span<T> val,
                       ::testing::MatchResultListener* listener) const override;
  void DescribeTo(::std::ostream* os) const override;
  void DescribeNegationTo(::std::ostream* os) const override;

 private:
  std::vector<std::remove_cv_t<T>> good;
};

template <typename T>
inline ::testing::Matcher<span<const T>> SpanEq(span<const T> good) {
  return ::testing::MakeMatcher(new SpanMatcher(good));
}

template <typename T>
inline ::testing::Matcher<span<const T>> SpanEq(
    std::initializer_list<const T> good) {
  return ::testing::MakeMatcher(
      new SpanMatcher<const T>({good.begin(), good.end()}));
}

template <typename T>
bool SpanMatcher<T>::MatchAndExplain(
    span<T> val, ::testing::MatchResultListener* listener) const {
  if (val.size() != good.size() ||
      !std::equal(val.begin(), val.end(), good.begin())) {
    return false;
  }
  return true;
}

template <typename T>
void SpanMatcher<T>::DescribeTo(::std::ostream* os) const {
  PrintTo(span<T>{good}, os);
}

template <typename T>
void SpanMatcher<T>::DescribeNegationTo(::std::ostream* os) const {
  *os << "is not equal to ";
  PrintTo(span<T>{good}, os);
}

}  // namespace wpi

inline wpi::span<const uint8_t> operator"" _us(const char* str, size_t len) {
  return {reinterpret_cast<const uint8_t*>(str), len};
}

#endif  // NTCORE_SPANMATCHER_H_
