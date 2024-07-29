// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MessageMatcher3.h"

namespace nt::net3 {

bool MessageMatcher::MatchAndExplain(
    Message3 msg, ::testing::MatchResultListener* listener) const {
  bool match = true;
  if (msg.str() != goodmsg.str()) {
    *listener << "str mismatch ";
    match = false;
  }
  if ((!msg.value() && goodmsg.value()) || (msg.value() && !goodmsg.value()) ||
      (msg.value() && goodmsg.value() && msg.value() != goodmsg.value())) {
    *listener << "value mismatch ";
    match = false;
  }
  if (msg.id() != goodmsg.id()) {
    *listener << "id mismatch ";
    match = false;
  }
  if (msg.flags() != goodmsg.flags()) {
    *listener << "flags mismatch";
    match = false;
  }
  if (msg.seq_num_uid() != goodmsg.seq_num_uid()) {
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

}  // namespace nt::net3
