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

#include "llvm/SmallString.h"

#include "unit-json.h"

#include "support/json_serializer.h"

using wpi::json;

class JsonTypeNameTest
    : public ::testing::TestWithParam<std::pair<json::value_t, const char*>> {};
TEST_P(JsonTypeNameTest, Case)
{
    EXPECT_EQ(json(GetParam().first).type_name(), GetParam().second);
}

static const std::pair<json::value_t, const char*> type_name_cases[] = {
    {json::value_t::null, "null"},
    {json::value_t::object, "object"},
    {json::value_t::array, "array"},
    {json::value_t::number_integer, "number"},
    {json::value_t::number_unsigned, "number"},
    {json::value_t::number_float, "number"},
    {json::value_t::boolean, "boolean"},
    {json::value_t::string, "string"},
    {json::value_t::discarded, "discarded"},
};

INSTANTIATE_TEST_CASE_P(JsonTypeNameTests, JsonTypeNameTest,
                        ::testing::ValuesIn(type_name_cases), );

class JsonStringEscapeTest
    : public ::testing::TestWithParam<std::pair<const char*, const char*>> {};
TEST_P(JsonStringEscapeTest, Case)
{
    llvm::SmallString<32> buf;
    llvm::raw_svector_ostream ss(buf);
    json::serializer s(ss);
    s.dump_escaped(GetParam().first);
    EXPECT_EQ(ss.str(), llvm::StringRef(GetParam().second));
}

static const std::pair<const char*, const char*> string_escape_cases[] = {
    {"\"", "\\\""},
    {"\\", "\\\\"},
    {"\b", "\\b"},
    {"\f", "\\f"},
    {"\n", "\\n"},
    {"\r", "\\r"},
    {"\t", "\\t"},

    {"\x01", "\\u0001"},
    {"\x02", "\\u0002"},
    {"\x03", "\\u0003"},
    {"\x04", "\\u0004"},
    {"\x05", "\\u0005"},
    {"\x06", "\\u0006"},
    {"\x07", "\\u0007"},
    {"\x08", "\\b"},
    {"\x09", "\\t"},
    {"\x0a", "\\n"},
    {"\x0b", "\\u000b"},
    {"\x0c", "\\f"},
    {"\x0d", "\\r"},
    {"\x0e", "\\u000e"},
    {"\x0f", "\\u000f"},
    {"\x10", "\\u0010"},
    {"\x11", "\\u0011"},
    {"\x12", "\\u0012"},
    {"\x13", "\\u0013"},
    {"\x14", "\\u0014"},
    {"\x15", "\\u0015"},
    {"\x16", "\\u0016"},
    {"\x17", "\\u0017"},
    {"\x18", "\\u0018"},
    {"\x19", "\\u0019"},
    {"\x1a", "\\u001a"},
    {"\x1b", "\\u001b"},
    {"\x1c", "\\u001c"},
    {"\x1d", "\\u001d"},
    {"\x1e", "\\u001e"},
    {"\x1f", "\\u001f"},
};

INSTANTIATE_TEST_CASE_P(JsonStringEscapeTests, JsonStringEscapeTest,
                        ::testing::ValuesIn(string_escape_cases), );
