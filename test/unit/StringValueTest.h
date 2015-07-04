/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_TEST_STRINGVALUETEST_H_
#define NT_TEST_STRINGVALUETEST_H_

#include "Value.h"

#include "gtest/gtest.h"

namespace ntimpl {

class StringValueTest : public ::testing::Test {
 public:
  static NT_String& ToNT(StringValue& v) { return v; }
};

}  // namespace ntimpl

#endif
