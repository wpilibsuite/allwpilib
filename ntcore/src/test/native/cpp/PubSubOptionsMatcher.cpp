// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PubSubOptionsMatcher.h"

#include "TestPrinters.h"

namespace nt {

bool PubSubOptionsMatcher::MatchAndExplain(
    const PubSubOptions& val, ::testing::MatchResultListener* listener) const {
  bool match = true;
  if (val.periodic != good.periodic) {
    *listener << "periodic mismatch ";
    match = false;
  }
  if (val.pollStorageSize != good.pollStorageSize) {
    *listener << "pollStorageSize mismatch ";
    match = false;
  }
  if (val.sendAll != good.sendAll) {
    *listener << "logging mismatch ";
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

}  // namespace nt
