/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ValueMatcher.h"

#include "TestPrinters.h"

namespace nt {

bool ValueMatcher::MatchAndExplain(
    std::shared_ptr<Value> val,
    ::testing::MatchResultListener* listener) const {
  if ((!val && goodval) || (val && !goodval) ||
      (val && goodval && *val != *goodval)) {
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

}  // namespace nt
