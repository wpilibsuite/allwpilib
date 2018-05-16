/*----------------------------------------------------------------------------*/
/* Modifications Copyright (c) FIRST 2017. All Rights Reserved.               */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++ (test suite)
|  |  |__   |  |  | | | |  version 2.1.1
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "gtest/gtest.h"

#include "unit-json.h"
#include "wpi/raw_istream.h"
using wpi::json;

#include <valarray>

TEST(JsonDeserializationTest, SuccessfulStream)
{
    std::string s = "[\"foo\",1,2,3,false,{\"one\":1}]";
    wpi::raw_mem_istream ss(s.data(), s.size());
    json j = json::parse(ss);
    ASSERT_EQ(j, json({"foo", 1, 2, 3, false, {{"one", 1}}}));
}

TEST(JsonDeserializationTest, SuccessfulStringLiteral)
{
    auto s = "[\"foo\",1,2,3,false,{\"one\":1}]";
    json j = json::parse(s);
    ASSERT_EQ(j, json({"foo", 1, 2, 3, false, {{"one", 1}}}));
}

TEST(JsonDeserializationTest, SuccessfulStdString)
{
    std::string s = "[\"foo\",1,2,3,false,{\"one\":1}]";
    json j = json::parse(s);
    ASSERT_EQ(j, json({"foo", 1, 2, 3, false, {{"one", 1}}}));
}

TEST(JsonDeserializationTest, SuccessfulStreamOperator)
{
    std::string s = "[\"foo\",1,2,3,false,{\"one\":1}]";
    wpi::raw_mem_istream ss(s.data(), s.size());
    json j;
    ss >> j;
    ASSERT_EQ(j, json({"foo", 1, 2, 3, false, {{"one", 1}}}));
}

TEST(JsonDeserializationTest, SuccessfulUserStringLiteral)
{
    ASSERT_EQ("[\"foo\",1,2,3,false,{\"one\":1}]"_json, json({"foo", 1, 2, 3, false, {{"one", 1}}}));
}

TEST(JsonDeserializationTest, UnsuccessfulStream)
{
    std::string s = "[\"foo\",1,2,3,false,{\"one\":1}";
    wpi::raw_mem_istream ss(s.data(), s.size());
    ASSERT_THROW_MSG(json::parse(ss), json::parse_error,
                     "[json.exception.parse_error.101] parse error at 29: syntax error - unexpected end of input; expected ']'");
}

TEST(JsonDeserializationTest, UnsuccessfulStdString)
{
    std::string s = "[\"foo\",1,2,3,false,{\"one\":1}";
    ASSERT_THROW_MSG(json::parse(s), json::parse_error,
                     "[json.exception.parse_error.101] parse error at 29: syntax error - unexpected end of input; expected ']'");
}

TEST(JsonDeserializationTest, UnsuccessfulStreamOperator)
{
    std::string s = "[\"foo\",1,2,3,false,{\"one\":1}";
    wpi::raw_mem_istream ss(s.data(), s.size());
    json j;
    ASSERT_THROW_MSG(ss >> j, json::parse_error,
                     "[json.exception.parse_error.101] parse error at 29: syntax error - unexpected end of input; expected ']'");
}

TEST(JsonDeserializationTest, UnsuccessfulUserStringLiteral)
{
    ASSERT_THROW_MSG("[\"foo\",1,2,3,false,{\"one\":1}"_json, json::parse_error,
                     "[json.exception.parse_error.101] parse error at 29: syntax error - unexpected end of input; expected ']'");
}

// these cases are required for 100% line coverage
class JsonDeserializationErrorTest
    : public ::testing::TestWithParam<const char*> {};

TEST_P(JsonDeserializationErrorTest, ErrorCase)
{
    ASSERT_THROW(json::parse(GetParam()), json::parse_error);
}

static const char* error_cases[] = {
    "\"aaaaaa\\u",
    "\"aaaaaa\\u1",
    "\"aaaaaa\\u11111111",
    "\"aaaaaau11111111\\",
    "\"\x7F\xC1",
    "\"\x7F\xDF\x7F",
    "\"\x7F\xDF\xC0",
    "\"\x7F\xE0\x9F",
    "\"\x7F\xEF\xC0",
    "\"\x7F\xED\x7F",
    "\"\x7F\xF0\x8F",
    "\"\x7F\xF0\xC0",
    "\"\x7F\xF3\x7F",
    "\"\x7F\xF3\xC0",
    "\"\x7F\xF4\x7F",
};

INSTANTIATE_TEST_CASE_P(JsonDeserializationErrorTests,
                        JsonDeserializationErrorTest,
                        ::testing::ValuesIn(error_cases), );
