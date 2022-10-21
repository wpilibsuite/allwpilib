// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <ostream>
#include <utility>

#include "../TestPrinters.h"
#include "gmock/gmock.h"
#include "net3/Message3.h"

namespace nt::net3 {

class MessageMatcher : public ::testing::MatcherInterface<Message3> {
 public:
  explicit MessageMatcher(Message3 goodmsg_) : goodmsg(std::move(goodmsg_)) {}

  bool MatchAndExplain(Message3 msg,
                       ::testing::MatchResultListener* listener) const override;
  void DescribeTo(::std::ostream* os) const override;
  void DescribeNegationTo(::std::ostream* os) const override;

 private:
  Message3 goodmsg;
};

inline ::testing::Matcher<Message3> MessageEq(Message3 goodmsg) {
  return ::testing::MakeMatcher(new MessageMatcher(std::move(goodmsg)));
}

}  // namespace nt::net3
