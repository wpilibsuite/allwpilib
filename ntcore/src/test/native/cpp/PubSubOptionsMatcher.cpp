// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PubSubOptionsMatcher.hpp"

#include "TestPrinters.hpp"

namespace wpi::nt {

bool PubSubOptionsMatcher::MatchAndExplain(
    const PubSubOptionsImpl& val,
    ::testing::MatchResultListener* listener) const {
  bool match = true;
  if (val.periodicMs != good.periodicMs) {
    *listener << "periodic mismatch ";
    match = false;
  }
  if (val.pollStorage != good.pollStorage) {
    *listener << "pollStorage mismatch ";
    match = false;
  }
  if (val.sendAll != good.sendAll) {
    *listener << "sendAll mismatch ";
    match = false;
  }
  if (val.keepDuplicates != good.keepDuplicates) {
    *listener << "keepDuplicates mismatch ";
    match = false;
  }
  return match;
}

void PubSubOptionsMatcher::DescribeTo(::std::ostream* os) const {
  PrintTo(good, os);
}

void PubSubOptionsMatcher::DescribeNegationTo(::std::ostream* os) const {
  *os << "is not equal to ";
  PrintTo(good, os);
}

}  // namespace wpi::nt
