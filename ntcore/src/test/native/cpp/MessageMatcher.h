// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_MESSAGEMATCHER_H_
#define NTCORE_MESSAGEMATCHER_H_

#include <memory>
#include <ostream>
#include <utility>

#include "Message.h"
#include "TestPrinters.h"
#include "gmock/gmock.h"

namespace nt {

class MessageMatcher
    : public ::testing::MatcherInterface<std::shared_ptr<Message>> {
 public:
  explicit MessageMatcher(std::shared_ptr<Message> goodmsg_)
      : goodmsg(std::move(goodmsg_)) {}

  bool MatchAndExplain(std::shared_ptr<Message> msg,
                       ::testing::MatchResultListener* listener) const override;
  void DescribeTo(::std::ostream* os) const override;
  void DescribeNegationTo(::std::ostream* os) const override;

 private:
  std::shared_ptr<Message> goodmsg;
};

inline ::testing::Matcher<std::shared_ptr<Message>> MessageEq(
    std::shared_ptr<Message> goodmsg) {
  return ::testing::MakeMatcher(new MessageMatcher(goodmsg));
}

}  // namespace nt

#endif  // NTCORE_MESSAGEMATCHER_H_
