// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <ostream>
#include <utility>

#include "gmock/gmock.h"
#include "networktables/NetworkTableValue.h"

namespace nt {

class ValueMatcher : public ::testing::MatcherInterface<Value> {
 public:
  explicit ValueMatcher(Value goodval_) : goodval(std::move(goodval_)) {}

  bool MatchAndExplain(Value msg,
                       ::testing::MatchResultListener* listener) const override;
  void DescribeTo(::std::ostream* os) const override;
  void DescribeNegationTo(::std::ostream* os) const override;

 private:
  Value goodval;
};

inline ::testing::Matcher<Value> ValueEq(const Value& goodval) {
  return ::testing::MakeMatcher(new ValueMatcher(goodval));
}

}  // namespace nt
