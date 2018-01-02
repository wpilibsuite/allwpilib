/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MessageMatcher.h"

namespace nt {

bool MessageMatcher::MatchAndExplain(
    std::shared_ptr<Message> msg,
    ::testing::MatchResultListener* listener) const {
  bool match = true;
  if (!msg) return false;
  if (msg->str() != goodmsg->str()) {
    *listener << "str mismatch ";
    match = false;
  }
  if ((!msg->value() && goodmsg->value()) ||
      (msg->value() && !goodmsg->value()) ||
      (msg->value() && goodmsg->value() &&
       *msg->value() != *goodmsg->value())) {
    *listener << "value mismatch ";
    match = false;
  }
  if (msg->id() != goodmsg->id()) {
    *listener << "id mismatch ";
    match = false;
  }
  if (msg->flags() != goodmsg->flags()) {
    *listener << "flags mismatch";
    match = false;
  }
  if (msg->seq_num_uid() != goodmsg->seq_num_uid()) {
    *listener << "seq_num_uid mismatch";
    match = false;
  }
  return match;
}

void MessageMatcher::DescribeTo(::std::ostream* os) const {
  PrintTo(goodmsg, os);
}

void MessageMatcher::DescribeNegationTo(::std::ostream* os) const {
  *os << "is not equal to ";
  PrintTo(goodmsg, os);
}

}  // namespace nt
