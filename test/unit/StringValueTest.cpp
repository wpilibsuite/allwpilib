/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "StringValueTest.h"

namespace ntimpl {

TEST_F(StringValueTest, ConstructEmpty) {
  StringValue v;
  ASSERT_EQ(nullptr, ToNT(v).str);
  ASSERT_EQ(0u, ToNT(v).len);
}

TEST_F(StringValueTest, ConstructStringRef) {
  StringValue v("hello");
  ASSERT_EQ("hello", llvm::StringRef(v));
  ASSERT_EQ(llvm::StringRef("hello"), ToNT(v).str);
  ASSERT_EQ(5u, ToNT(v).len);
}

TEST_F(StringValueTest, ConstructMove) {
  StringValue v("hello");
  char* orig = ToNT(v).str;
  ASSERT_NE(orig, nullptr);
  ASSERT_EQ(5u, ToNT(v).len);
  StringValue v2(std::move(v));
  ASSERT_EQ(nullptr, ToNT(v).str);
  ASSERT_EQ(0u, ToNT(v).len);
  ASSERT_EQ(orig, ToNT(v2).str);
  ASSERT_EQ(5u, ToNT(v2).len);
}

TEST_F(StringValueTest, AssignMove) {
  StringValue v("hello");
  char* orig = ToNT(v).str;
  ASSERT_NE(orig, nullptr);
  ASSERT_EQ(5u, ToNT(v).len);
  StringValue v2("goodbye");
  ASSERT_NE(ToNT(v2).str, nullptr);
  ASSERT_EQ(7u, ToNT(v2).len);
  v2 = std::move(v);
  ASSERT_EQ(nullptr, ToNT(v).str);
  ASSERT_EQ(0u, ToNT(v).len);
  ASSERT_EQ(orig, ToNT(v2).str);
  ASSERT_EQ(5u, ToNT(v2).len);
}

}  // namespace ntimpl
