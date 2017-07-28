/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "support/Base64.h"

#include "gtest/gtest.h"

namespace wpi {

struct Base64TestParam {
  int plain_len;
  const char* plain;
  const char* encoded;
};

std::ostream& operator<<(std::ostream& os, const Base64TestParam& param) {
  os << "Base64TestParam(Len: " << param.plain_len << ", "
     << "Plain: \"" << param.plain << "\", "
     << "Encoded: \"" << param.encoded << "\")";
  return os;
}

class Base64Test : public ::testing::TestWithParam<Base64TestParam> {
 protected:
  llvm::StringRef GetPlain() {
    if (GetParam().plain_len < 0)
      return llvm::StringRef(GetParam().plain);
    else
      return llvm::StringRef(GetParam().plain, GetParam().plain_len);
  }
};

TEST_P(Base64Test, Encode) {
  std::string s;
  Base64Encode(GetPlain(), &s);
  ASSERT_EQ(GetParam().encoded, s);
}

TEST_P(Base64Test, Decode) {
  std::string s;
  llvm::StringRef encoded = GetParam().encoded;
  EXPECT_EQ(encoded.size(), Base64Decode(encoded, &s));
  ASSERT_EQ(GetPlain(), s);
}

static Base64TestParam sample[] = {
    {-1, "Send reinforcements", "U2VuZCByZWluZm9yY2VtZW50cw=="},
    {-1, "Now is the time for all good coders\n to learn C++",
     "Tm93IGlzIHRoZSB0aW1lIGZvciBhbGwgZ29vZCBjb2RlcnMKIHRvIGxlYXJuIEMrKw=="},
    {-1,
     "This is line one\nThis is line two\nThis is line three\nAnd so on...\n",
     "VGhpcyBpcyBsaW5lIG9uZQpUaGlzIGlzIGxpbmUgdHdvClRoaXMgaXMgbGluZSB0aHJlZQpBb"
     "mQgc28gb24uLi4K"},
};

INSTANTIATE_TEST_CASE_P(Base64Sample, Base64Test, ::testing::ValuesIn(sample));

static Base64TestParam standard[] = {
    {0, "", ""},
    {1, "\0", "AA=="},
    {2, "\0\0", "AAA="},
    {3, "\0\0\0", "AAAA"},
    {1, "\377", "/w=="},
    {2, "\377\377", "//8="},
    {3, "\377\377\377", "////"},
    {2, "\xff\xef", "/+8="},
};

INSTANTIATE_TEST_CASE_P(Base64Standard, Base64Test,
                        ::testing::ValuesIn(standard));

}  // namespace wpi
