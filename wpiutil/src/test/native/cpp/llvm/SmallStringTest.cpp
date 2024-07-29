//===- llvm/unittest/ADT/SmallStringTest.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// SmallString unit tests.
//
//===----------------------------------------------------------------------===//

#include "wpi/SmallString.h"
#include "gtest/gtest.h"
#include <climits>
#include <cstring>
#include <stdarg.h>

using namespace wpi;

namespace {

// Test fixture class
class SmallStringTest : public testing::Test {
protected:
  typedef SmallString<40> StringType;

  StringType theString;

  void assertEmpty(StringType & v) {
    // Size tests
    EXPECT_EQ(0u, v.size());
    EXPECT_TRUE(v.empty());
    // Iterator tests
    EXPECT_TRUE(v.begin() == v.end());
  }
};

// New string test.
TEST_F(SmallStringTest, EmptyStringTest) {
  SCOPED_TRACE("EmptyStringTest");
  assertEmpty(theString);
  EXPECT_TRUE(theString.rbegin() == theString.rend());
}

TEST_F(SmallStringTest, AssignRepeated) {
  theString.assign(3, 'a');
  EXPECT_EQ(3u, theString.size());
  EXPECT_STREQ("aaa", theString.c_str());
}

TEST_F(SmallStringTest, AssignIterPair) {
  std::string_view abc = "abc";
  theString.assign(abc.begin(), abc.end());
  EXPECT_EQ(3u, theString.size());
  EXPECT_STREQ("abc", theString.c_str());
}

TEST_F(SmallStringTest, AssignStringView) {
  std::string_view abc = "abc";
  theString.assign(abc);
  EXPECT_EQ(3u, theString.size());
  EXPECT_STREQ("abc", theString.c_str());
}

TEST_F(SmallStringTest, AssignSmallVector) {
  std::string_view abc = "abc";
  SmallVector<char, 10> abcVec(abc.begin(), abc.end());
  theString.assign(abcVec);
  EXPECT_EQ(3u, theString.size());
  EXPECT_STREQ("abc", theString.c_str());
}

TEST_F(SmallStringTest, AssignStringViews) {
  theString.assign({"abc", "def", "ghi"});
  EXPECT_EQ(9u, theString.size());
  EXPECT_STREQ("abcdefghi", theString.c_str());
}

TEST_F(SmallStringTest, AppendIterPair) {
  std::string_view abc = "abc";
  theString.append(abc.begin(), abc.end());
  theString.append(abc.begin(), abc.end());
  EXPECT_EQ(6u, theString.size());
  EXPECT_STREQ("abcabc", theString.c_str());
}

TEST_F(SmallStringTest, AppendStringView) {
  std::string_view abc = "abc";
  theString.append(abc);
  theString.append(abc);
  EXPECT_EQ(6u, theString.size());
  EXPECT_STREQ("abcabc", theString.c_str());
}

TEST_F(SmallStringTest, AppendSmallVector) {
  std::string_view abc = "abc";
  SmallVector<char, 10> abcVec(abc.begin(), abc.end());
  theString.append(abcVec);
  theString.append(abcVec);
  EXPECT_EQ(6u, theString.size());
  EXPECT_STREQ("abcabc", theString.c_str());
}

TEST_F(SmallStringTest, AppendStringViews) {
  theString.append({"abc", "def", "ghi"});
  EXPECT_EQ(9u, theString.size());
  EXPECT_STREQ("abcdefghi", theString.c_str());
  std::string_view Jkl = "jkl";
  std::string Mno = "mno";
  SmallString<4> Pqr("pqr");
  const char *Stu = "stu";
  theString.append({Jkl, Mno, Pqr, Stu});
  EXPECT_EQ(21u, theString.size());
  EXPECT_STREQ("abcdefghijklmnopqrstu", theString.c_str());
}

TEST_F(SmallStringTest, StringViewConversion) {
  std::string_view abc = "abc";
  theString.assign(abc.begin(), abc.end());
  std::string_view theStringView = theString;
  EXPECT_EQ("abc", theStringView);
}

TEST_F(SmallStringTest, StdStringConversion) {
  std::string_view abc = "abc";
  theString.assign(abc.begin(), abc.end());
  std::string theStdString = std::string(theString);
  EXPECT_EQ("abc", theStdString);
}

TEST_F(SmallStringTest, Find) {
  theString = "hello";
  EXPECT_EQ(2U, theString.find('l'));
  EXPECT_EQ(std::string_view::npos, theString.find('z'));
  EXPECT_EQ(std::string_view::npos, theString.find("helloworld"));
  EXPECT_EQ(0U, theString.find("hello"));
  EXPECT_EQ(1U, theString.find("ello"));
  EXPECT_EQ(std::string_view::npos, theString.find("zz"));
  EXPECT_EQ(2U, theString.find("ll", 2));
  EXPECT_EQ(std::string_view::npos, theString.find("ll", 3));
  EXPECT_EQ(0U, theString.find(""));

  EXPECT_EQ(3U, theString.rfind('l'));
  EXPECT_EQ(std::string_view::npos, theString.rfind('z'));
  EXPECT_EQ(std::string_view::npos, theString.rfind("helloworld"));
  EXPECT_EQ(0U, theString.rfind("hello"));
  EXPECT_EQ(1U, theString.rfind("ello"));
  EXPECT_EQ(std::string_view::npos, theString.rfind("zz"));

  EXPECT_EQ(2U, theString.find_first_of('l'));
  EXPECT_EQ(1U, theString.find_first_of("el"));
  EXPECT_EQ(std::string_view::npos, theString.find_first_of("xyz"));

  EXPECT_EQ(1U, theString.find_first_not_of('h'));
  EXPECT_EQ(4U, theString.find_first_not_of("hel"));
  EXPECT_EQ(std::string_view::npos, theString.find_first_not_of("hello"));

  theString = "hellx xello hell ello world foo bar hello";
  EXPECT_EQ(36U, theString.find("hello"));
  EXPECT_EQ(28U, theString.find("foo"));
  EXPECT_EQ(12U, theString.find("hell", 2));
  EXPECT_EQ(0U, theString.find(""));
}

} // namespace
