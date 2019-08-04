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
using wpi::json;

TEST(JsonConvTypeCheckTest, Object)
{
    json j {{"foo", 1}, {"bar", false}};
    EXPECT_FALSE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_FALSE(j.is_number());
    EXPECT_FALSE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_TRUE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_FALSE(j.is_primitive());
    EXPECT_TRUE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, Array)
{
    json j {"foo", 1, 1u, 42.23, false};
    EXPECT_FALSE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_FALSE(j.is_number());
    EXPECT_FALSE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_TRUE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_FALSE(j.is_primitive());
    EXPECT_TRUE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, Null)
{
    json j(nullptr);
    EXPECT_TRUE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_FALSE(j.is_number());
    EXPECT_FALSE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_TRUE(j.is_primitive());
    EXPECT_FALSE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, Boolean)
{
    json j(true);
    EXPECT_FALSE(j.is_null());
    EXPECT_TRUE(j.is_boolean());
    EXPECT_FALSE(j.is_number());
    EXPECT_FALSE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_TRUE(j.is_primitive());
    EXPECT_FALSE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, String)
{
    json j("Hello world");
    EXPECT_FALSE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_FALSE(j.is_number());
    EXPECT_FALSE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_TRUE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_TRUE(j.is_primitive());
    EXPECT_FALSE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, Integer)
{
    json j(42);
    EXPECT_FALSE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_TRUE(j.is_number());
    EXPECT_TRUE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_TRUE(j.is_primitive());
    EXPECT_FALSE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, Unsigned)
{
    json j(42u);
    EXPECT_FALSE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_TRUE(j.is_number());
    EXPECT_TRUE(j.is_number_integer());
    EXPECT_TRUE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_TRUE(j.is_primitive());
    EXPECT_FALSE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, Float)
{
    json j(42.23);
    EXPECT_FALSE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_TRUE(j.is_number());
    EXPECT_FALSE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_TRUE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_FALSE(j.is_discarded());
    EXPECT_TRUE(j.is_primitive());
    EXPECT_FALSE(j.is_structured());
}

TEST(JsonConvTypeCheckTest, Discarded)
{
    json j(json::value_t::discarded);
    EXPECT_FALSE(j.is_null());
    EXPECT_FALSE(j.is_boolean());
    EXPECT_FALSE(j.is_number());
    EXPECT_FALSE(j.is_number_integer());
    EXPECT_FALSE(j.is_number_unsigned());
    EXPECT_FALSE(j.is_number_float());
    EXPECT_FALSE(j.is_object());
    EXPECT_FALSE(j.is_array());
    EXPECT_FALSE(j.is_string());
    EXPECT_TRUE(j.is_discarded());
    EXPECT_FALSE(j.is_primitive());
    EXPECT_FALSE(j.is_structured());
}

class JsonConvSerializationTest : public ::testing::Test {
 protected:
    json j {{"object", json::object()}, {"array", {1, 2, 3, 4}}, {"number", 42}, {"boolean", false}, {"null", nullptr}, {"string", "Hello world"} };
};
#if 0
// no indent / indent=-1
TEST_F(JsonConvSerializationTest, NoIndent)
{
    EXPECT_EQ(j.dump(),
          "{\"array\":[1,2,3,4],\"boolean\":false,\"null\":null,\"number\":42,\"object\":{},\"string\":\"Hello world\"}");

    EXPECT_EQ(j.dump(), j.dump(-1));
}

// indent=0
TEST_F(JsonConvSerializationTest, Indent0)
{
    EXPECT_EQ(j.dump(0),
          "{\n\"array\": [\n1,\n2,\n3,\n4\n],\n\"boolean\": false,\n\"null\": null,\n\"number\": 42,\n\"object\": {},\n\"string\": \"Hello world\"\n}");
}

// indent=1, space='\t'
TEST_F(JsonConvSerializationTest, Indent1)
{
    EXPECT_EQ(j.dump(1, '\t'),
          "{\n\t\"array\": [\n\t\t1,\n\t\t2,\n\t\t3,\n\t\t4\n\t],\n\t\"boolean\": false,\n\t\"null\": null,\n\t\"number\": 42,\n\t\"object\": {},\n\t\"string\": \"Hello world\"\n}");
}

// indent=4
TEST_F(JsonConvSerializationTest, Indent4)
{
    EXPECT_EQ(j.dump(4),
          "{\n    \"array\": [\n        1,\n        2,\n        3,\n        4\n    ],\n    \"boolean\": false,\n    \"null\": null,\n    \"number\": 42,\n    \"object\": {},\n    \"string\": \"Hello world\"\n}");
}
#endif
// indent=x
TEST_F(JsonConvSerializationTest, IndentX)
{
    EXPECT_EQ(j.dump().size(), 94u);
    EXPECT_EQ(j.dump(1).size(), 127u);
    EXPECT_EQ(j.dump(2).size(), 142u);
    EXPECT_EQ(j.dump(512).size(), 7792u);
}

// dump and floating-point numbers
TEST_F(JsonConvSerializationTest, Float)
{
    auto s = json(42.23).dump();
    EXPECT_NE(s.find("42.23"), std::string::npos);
}

// dump and small floating-point numbers
TEST_F(JsonConvSerializationTest, SmallFloat)
{
    auto s = json(1.23456e-78).dump();
    EXPECT_NE(s.find("1.23456e-78"), std::string::npos);
}

// dump and non-ASCII characters
TEST_F(JsonConvSerializationTest, NonAscii)
{
    EXPECT_EQ(json("ä").dump(), "\"ä\"");
    EXPECT_EQ(json("Ö").dump(), "\"Ö\"");
    EXPECT_EQ(json("❤️").dump(), "\"❤️\"");
}

// serialization of discarded element
TEST_F(JsonConvSerializationTest, Discarded)
{
    json j_discarded(json::value_t::discarded);
    EXPECT_EQ(j_discarded.dump(), "<discarded>");
}

TEST(JsonConvRoundTripTest, Case)
{
    for (const auto& s :
{"3.141592653589793", "1000000000000000010E5"
})
    {
        SCOPED_TRACE(s);
        json j1 = json::parse(s);
        std::string s1 = j1.dump();
        json j2 = json::parse(s1);
        std::string s2 = j2.dump();
        EXPECT_EQ(s1, s2);
    }
}

// return the type of the object (explicit)
TEST(JsonConvTypeExplicitTest, Null)
{
    json j = nullptr;
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonConvTypeExplicitTest, Object)
{
    json j = {{"foo", "bar"}};
    EXPECT_EQ(j.type(), json::value_t::object);
}

TEST(JsonConvTypeExplicitTest, Array)
{
    json j = {1, 2, 3, 4};
    EXPECT_EQ(j.type(), json::value_t::array);
}

TEST(JsonConvTypeExplicitTest, Boolean)
{
    json j = true;
    EXPECT_EQ(j.type(), json::value_t::boolean);
}

TEST(JsonConvTypeExplicitTest, String)
{
    json j = "Hello world";
    EXPECT_EQ(j.type(), json::value_t::string);
}

TEST(JsonConvTypeExplicitTest, Integer)
{
    json j = 23;
    EXPECT_EQ(j.type(), json::value_t::number_integer);
}

TEST(JsonConvTypeExplicitTest, Unsigned)
{
    json j = 23u;
    EXPECT_EQ(j.type(), json::value_t::number_unsigned);
}

TEST(JsonConvTypeExplicitTest, Float)
{
    json j = 42.23;
    EXPECT_EQ(j.type(), json::value_t::number_float);
}

// return the type of the object (implicit)
TEST(JsonConvTypeImplicitTest, Null)
{
    json j = nullptr;
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}

TEST(JsonConvTypeImplicitTest, Object)
{
    json j = {{"foo", "bar"}};
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}

TEST(JsonConvTypeImplicitTest, Array)
{
    json j = {1, 2, 3, 4};
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}

TEST(JsonConvTypeImplicitTest, Boolean)
{
    json j = true;
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}

TEST(JsonConvTypeImplicitTest, String)
{
    json j = "Hello world";
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}

TEST(JsonConvTypeImplicitTest, Integer)
{
    json j = 23;
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}

TEST(JsonConvTypeImplicitTest, Unsigned)
{
    json j = 23u;
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}

TEST(JsonConvTypeImplicitTest, Float)
{
    json j = 42.23;
    json::value_t t = j;
    EXPECT_EQ(t, j.type());
}
