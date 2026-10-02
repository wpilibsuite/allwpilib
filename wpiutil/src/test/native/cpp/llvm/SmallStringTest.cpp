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

#include "wpi/util/SmallString.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <climits>
#include <cstring>
#include <stdarg.h>
#include <string_view>

using namespace wpi::util;

namespace {

// Test fixture class
class SmallStringTest {
protected:
  using StringType = SmallString<40>;

  StringType theString;

  void assertEmpty(StringType & v) {
    // Size tests
    CHECK(0u == v.size());
    CHECK(v.empty());
    // Iterator tests
    CHECK(v.begin() == v.end());
  }
};

// New string test.
TEST_CASE_METHOD(SmallStringTest, "SmallStringTest EmptyStringTest", "[wpiutil][llvm]") {
  UNSCOPED_INFO("EmptyStringTest");
  assertEmpty(theString);
  CHECK(theString.rbegin() == theString.rend());
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AssignRepeated", "[wpiutil][llvm]") {
  theString.assign(3, 'a');
  CHECK(3u == theString.size());
  CHECK(std::string_view{"aaa"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AssignIterPair", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  theString.assign(abc.begin(), abc.end());
  CHECK(3u == theString.size());
  CHECK(std::string_view{"abc"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AssignStringView", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  theString.assign(abc);
  CHECK(3u == theString.size());
  CHECK(std::string_view{"abc"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AssignSmallVector", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  SmallVector<char, 10> abcVec(abc.begin(), abc.end());
  theString.assign(abcVec);
  CHECK(3u == theString.size());
  CHECK(std::string_view{"abc"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AssignStringViews", "[wpiutil][llvm]") {
  theString.assign({"abc", "def", "ghi"});
  CHECK(9u == theString.size());
  CHECK(std::string_view{"abcdefghi"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AppendIterPair", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  theString.append(abc.begin(), abc.end());
  theString.append(abc.begin(), abc.end());
  CHECK(6u == theString.size());
  CHECK(std::string_view{"abcabc"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AppendStringView", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  theString.append(abc);
  theString.append(abc);
  CHECK(6u == theString.size());
  CHECK(std::string_view{"abcabc"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AppendSmallVector", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  SmallVector<char, 10> abcVec(abc.begin(), abc.end());
  theString.append(abcVec);
  theString.append(abcVec);
  CHECK(6u == theString.size());
  CHECK(std::string_view{"abcabc"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest AppendStringViews", "[wpiutil][llvm]") {
  theString.append({"abc", "def", "ghi"});
  CHECK(9u == theString.size());
  CHECK(std::string_view{"abcdefghi"} == std::string_view{theString.c_str()});
  std::string_view Jkl = "jkl";
  std::string Mno = "mno";
  SmallString<4> Pqr("pqr");
  const char *Stu = "stu";
  theString.append({Jkl, Mno, Pqr, Stu});
  CHECK(21u == theString.size());
  CHECK(std::string_view{"abcdefghijklmnopqrstu"} == std::string_view{theString.c_str()});
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest StringViewConversion", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  theString.assign(abc.begin(), abc.end());
  std::string_view theStringView = theString;
  CHECK("abc" == theStringView);
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest StdStringConversion", "[wpiutil][llvm]") {
  std::string_view abc = "abc";
  theString.assign(abc.begin(), abc.end());
  std::string theStdString = std::string(theString);
  CHECK("abc" == theStdString);
}

TEST_CASE_METHOD(SmallStringTest, "SmallStringTest Find", "[wpiutil][llvm]") {
  theString = "hello";
  CHECK(2U == theString.find('l'));
  CHECK(std::string_view::npos == theString.find('z'));
  CHECK(std::string_view::npos == theString.find("helloworld"));
  CHECK(0U == theString.find("hello"));
  CHECK(1U == theString.find("ello"));
  CHECK(std::string_view::npos == theString.find("zz"));
  CHECK(2U == theString.find("ll", 2));
  CHECK(std::string_view::npos == theString.find("ll", 3));
  CHECK(0U == theString.find(""));

  CHECK(3U == theString.rfind('l'));
  CHECK(std::string_view::npos == theString.rfind('z'));
  CHECK(std::string_view::npos == theString.rfind("helloworld"));
  CHECK(0U == theString.rfind("hello"));
  CHECK(1U == theString.rfind("ello"));
  CHECK(std::string_view::npos == theString.rfind("zz"));

  CHECK(2U == theString.find_first_of('l'));
  CHECK(1U == theString.find_first_of("el"));
  CHECK(std::string_view::npos == theString.find_first_of("xyz"));

  CHECK(1U == theString.find_first_not_of('h'));
  CHECK(4U == theString.find_first_not_of("hel"));
  CHECK(std::string_view::npos == theString.find_first_not_of("hello"));

  theString = "hellx xello hell ello world foo bar hello";
  CHECK(36U == theString.find("hello"));
  CHECK(28U == theString.find("foo"));
  CHECK(12U == theString.find("hell", 2));
  CHECK(0U == theString.find(""));
}

} // namespace
