// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Base64.hpp"

#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "wpi/util/SmallString.hpp"

namespace wpi::util {

struct Base64TestParam {
  int plain_len;
  const char* plain;
  const char* encoded;
};

std::ostream& operator<<(std::ostream& os, const Base64TestParam& param) {
  os << "Base64TestParam(Len: " << param.plain_len << ", " << "Plain: \""
     << param.plain << "\", " << "Encoded: \"" << param.encoded << "\")";
  return os;
}

std::string_view GetPlain(const Base64TestParam& param) {
  if (param.plain_len < 0) {
    return param.plain;
  } else {
    return std::string_view(param.plain, param.plain_len);
  }
}

static Base64TestParam testParams[] = {
    {-1, "Send reinforcements", "U2VuZCByZWluZm9yY2VtZW50cw=="},
    {-1, "Now is the time for all good coders\n to learn C++",
     "Tm93IGlzIHRoZSB0aW1lIGZvciBhbGwgZ29vZCBjb2RlcnMKIHRvIGxlYXJuIEMrKw=="},
    {-1,
     "This is line one\nThis is line two\nThis is line three\nAnd so on...\n",
     "VGhpcyBpcyBsaW5lIG9uZQpUaGlzIGlzIGxpbmUgdHdvClRoaXMgaXMgbGluZSB0aHJlZQpBb"
     "mQgc28gb24uLi4K"},
    {0, "", ""},
    {1, "\0", "AA=="},
    {2, "\0\0", "AAA="},
    {3, "\0\0\0", "AAAA"},
    {1, "\377", "/w=="},
    {2, "\377\377", "//8="},
    {3, "\377\377\377", "////"},
    {2, "\xff\xef", "/+8="},
};

TEST_CASE("Base64Test EncodeStdString", "[wpiutil][base64]") {
  const auto& param = GENERATE_REF(Catch::Generators::from_range(testParams));
  std::string s;
  Base64Encode(GetPlain(param), &s);
  REQUIRE((param.encoded) == (s));

  // text already in s
  Base64Encode(GetPlain(param), &s);
  REQUIRE((param.encoded) == (s));
}

TEST_CASE("Base64Test EncodeSmallString", "[wpiutil][base64]") {
  const auto& param = GENERATE_REF(Catch::Generators::from_range(testParams));
  SmallString<128> buf;
  REQUIRE((param.encoded) == (Base64Encode(GetPlain(param), buf)));
  // reuse buf
  REQUIRE((param.encoded) == (Base64Encode(GetPlain(param), buf)));
}

TEST_CASE("Base64Test DecodeStdString", "[wpiutil][base64]") {
  const auto& param = GENERATE_REF(Catch::Generators::from_range(testParams));
  std::string s;
  std::string_view encoded = param.encoded;
  CHECK((encoded.size()) == (Base64Decode(encoded, &s)));
  REQUIRE((GetPlain(param)) == (s));

  // text already in s
  Base64Decode(encoded, &s);
  REQUIRE((GetPlain(param)) == (s));
}

TEST_CASE("Base64Test DecodeSmallString", "[wpiutil][base64]") {
  const auto& param = GENERATE_REF(Catch::Generators::from_range(testParams));
  SmallString<128> buf;
  std::string_view encoded = param.encoded;
  size_t len;
  std::string_view plain = Base64Decode(encoded, &len, buf);
  CHECK((encoded.size()) == (len));
  REQUIRE((GetPlain(param)) == (plain));

  // reuse buf
  plain = Base64Decode(encoded, &len, buf);
  REQUIRE((GetPlain(param)) == (plain));
}

}  // namespace wpi::util
