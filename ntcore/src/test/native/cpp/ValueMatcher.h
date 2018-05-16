/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_VALUEMATCHER_H_
#define NTCORE_VALUEMATCHER_H_

#include <memory>
#include <ostream>

#include "gmock/gmock.h"
#include "networktables/NetworkTableValue.h"

namespace nt {

class ValueMatcher
    : public ::testing::MatcherInterface<std::shared_ptr<Value>> {
 public:
  explicit ValueMatcher(std::shared_ptr<Value> goodval_) : goodval(goodval_) {}

  bool MatchAndExplain(std::shared_ptr<Value> msg,
                       ::testing::MatchResultListener* listener) const override;
  void DescribeTo(::std::ostream* os) const override;
  void DescribeNegationTo(::std::ostream* os) const override;

 private:
  std::shared_ptr<Value> goodval;
};

inline ::testing::Matcher<std::shared_ptr<Value>> ValueEq(
    std::shared_ptr<Value> goodval) {
  return ::testing::MakeMatcher(new ValueMatcher(goodval));
}

}  // namespace nt

#endif  // NTCORE_VALUEMATCHER_H_
