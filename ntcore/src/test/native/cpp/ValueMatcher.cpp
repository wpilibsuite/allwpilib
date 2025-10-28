// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ValueMatcher.hpp"

#include "TestPrinters.hpp"

namespace wpi::nt {

bool ValueMatcher::MatchAndExplain(
    Value val, ::testing::MatchResultListener* listener) const {
  if ((!val && goodval) || (val && !goodval) ||
      (val && goodval && val != goodval)) {
    return false;
  }
  return true;
}

void ValueMatcher::DescribeTo(::std::ostream* os) const {
  PrintTo(goodval, os);
}

void ValueMatcher::DescribeNegationTo(::std::ostream* os) const {
  *os << "is not equal to ";
  PrintTo(goodval, os);
}

}  // namespace wpi::nt
