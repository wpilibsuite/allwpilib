// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <ostream>
#include <utility>

#include "PubSubOptions.h"
#include "gmock/gmock.h"

namespace nt {

class PubSubOptionsMatcher
    : public ::testing::MatcherInterface<const PubSubOptionsImpl&> {
 public:
  explicit PubSubOptionsMatcher(PubSubOptionsImpl good)
      : good{std::move(good)} {}

  bool MatchAndExplain(const PubSubOptionsImpl& val,
                       ::testing::MatchResultListener* listener) const override;
  void DescribeTo(::std::ostream* os) const override;
  void DescribeNegationTo(::std::ostream* os) const override;

 private:
  PubSubOptionsImpl good;
};

inline ::testing::Matcher<const PubSubOptionsImpl&> PubSubOptionsEq(
    PubSubOptionsImpl good) {
  return ::testing::MakeMatcher(new PubSubOptionsMatcher(std::move(good)));
}

}  // namespace nt
