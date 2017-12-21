/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_MESSAGEMATCHER_H_
#define NTCORE_MESSAGEMATCHER_H_

#include <memory>
#include <ostream>

#include "Message.h"
#include "TestPrinters.h"
#include "gmock/gmock.h"

namespace nt {

class MessageMatcher
    : public ::testing::MatcherInterface<std::shared_ptr<Message>> {
 public:
  explicit MessageMatcher(std::shared_ptr<Message> goodmsg_)
      : goodmsg(goodmsg_) {}

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
