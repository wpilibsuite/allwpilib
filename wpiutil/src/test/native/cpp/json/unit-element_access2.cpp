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

class JsonElementObjectAccessTestBase {
 public:
    JsonElementObjectAccessTestBase() : j_const(j) {}

 protected:
    json j = {{"integer", 1}, {"unsigned", 1u}, {"floating", 42.23}, {"null", nullptr}, {"string", "hello world"}, {"boolean", true}, {"object", json::object()}, {"array", {1, 2, 3}}};
    const json j_const;
};

class JsonElementObjectAccessTest : public ::testing::Test,
                                    public JsonElementObjectAccessTestBase {};

TEST_F(JsonElementObjectAccessTest, AtWithinBounds)
{
    EXPECT_EQ(j.at("integer"), json(1));
    EXPECT_EQ(j.at("unsigned"), json(1u));
    EXPECT_EQ(j.at("boolean"), json(true));
    EXPECT_EQ(j.at("null"), json(nullptr));
    EXPECT_EQ(j.at("string"), json("hello world"));
    EXPECT_EQ(j.at("floating"), json(42.23));
    EXPECT_EQ(j.at("object"), json::object());
    EXPECT_EQ(j.at("array"), json({1, 2, 3}));

    EXPECT_EQ(j_const.at("integer"), json(1));
    EXPECT_EQ(j_const.at("unsigned"), json(1u));
    EXPECT_EQ(j_const.at("boolean"), json(true));
    EXPECT_EQ(j_const.at("null"), json(nullptr));
    EXPECT_EQ(j_const.at("string"), json("hello world"));
    EXPECT_EQ(j_const.at("floating"), json(42.23));
    EXPECT_EQ(j_const.at("object"), json::object());
    EXPECT_EQ(j_const.at("array"), json({1, 2, 3}));
}

TEST_F(JsonElementObjectAccessTest, AtOutsideBounds)
{
    EXPECT_THROW_MSG(j.at("foo"), json::out_of_range,
                     "[json.exception.out_of_range.403] key 'foo' not found");
    EXPECT_THROW_MSG(j_const.at("foo"), json::out_of_range,
                     "[json.exception.out_of_range.403] key 'foo' not found");
}

TEST(JsonElementNonObjectAtAccessTest, Null)
{
    json j_nonobject(json::value_t::null);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with null");
    EXPECT_THROW_MSG(j_nonobject_const.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with null");
}

TEST(JsonElementNonObjectAtAccessTest, Boolean)
{
    json j_nonobject(json::value_t::boolean);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with boolean");
    EXPECT_THROW_MSG(j_nonobject_const.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with boolean");
}

TEST(JsonElementNonObjectAtAccessTest, String)
{
    json j_nonobject(json::value_t::string);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with string");
    EXPECT_THROW_MSG(j_nonobject_const.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with string");
}

TEST(JsonElementNonObjectAtAccessTest, Array)
{
    json j_nonobject(json::value_t::array);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with array");
    EXPECT_THROW_MSG(j_nonobject_const.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with array");
}

TEST(JsonElementNonObjectAtAccessTest, Integer)
{
    json j_nonobject(json::value_t::number_integer);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
    EXPECT_THROW_MSG(j_nonobject_const.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
}

TEST(JsonElementNonObjectAtAccessTest, Unsigned)
{
    json j_nonobject(json::value_t::number_unsigned);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
    EXPECT_THROW_MSG(j_nonobject_const.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
}

TEST(JsonElementNonObjectAtAccessTest, Float)
{
    json j_nonobject(json::value_t::number_float);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
    EXPECT_THROW_MSG(j_nonobject_const.at("foo"), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
}

TEST_F(JsonElementObjectAccessTest, KeyValueExist)
{
    EXPECT_EQ(j.value("integer", 2), 1);
    EXPECT_LT(std::fabs(j.value("integer", 1.0) - 1), 0.001);
    EXPECT_EQ(j.value("unsigned", 2), 1);
    EXPECT_LT(std::fabs(j.value("unsigned", 1.0) - 1), 0.001);
    EXPECT_EQ(j.value("null", json(1)), json());
    EXPECT_EQ(j.value("boolean", false), true);
    EXPECT_EQ(j.value("string", "bar"), "hello world");
    EXPECT_EQ(j.value("string", std::string("bar")), "hello world");
    EXPECT_LT(std::fabs(j.value("floating", 12.34) - 42.23), 0.001);
    EXPECT_EQ(j.value("floating", 12), 42);
    EXPECT_EQ(j.value("object", json({{"foo", "bar"}})), json::object());
    EXPECT_EQ(j.value("array", json({10, 100})), json({1, 2, 3}));

    EXPECT_EQ(j_const.value("integer", 2), 1);
    EXPECT_LT(std::fabs(j_const.value("integer", 1.0) - 1), 0.001);
    EXPECT_EQ(j_const.value("unsigned", 2), 1);
    EXPECT_LT(std::fabs(j_const.value("unsigned", 1.0) - 1), 0.001);
    EXPECT_EQ(j_const.value("boolean", false), true);
    EXPECT_EQ(j_const.value("string", "bar"), "hello world");
    EXPECT_EQ(j_const.value("string", std::string("bar")), "hello world");
    EXPECT_LT(std::fabs(j_const.value("floating", 12.34) - 42.23), 0.001);
    EXPECT_EQ(j_const.value("floating", 12), 42);
    EXPECT_EQ(j_const.value("object", json({{"foo", "bar"}})), json::object());
    EXPECT_EQ(j_const.value("array", json({10, 100})), json({1, 2, 3}));
}

TEST_F(JsonElementObjectAccessTest, KeyValueNotExist)
{
    EXPECT_EQ(j.value("_", 2), 2);
    EXPECT_EQ(j.value("_", 2u), 2u);
    EXPECT_EQ(j.value("_", false), false);
    EXPECT_EQ(j.value("_", "bar"), "bar");
    EXPECT_LT(std::fabs(j.value("_", 12.34) - 12.34), 0.001);
    EXPECT_EQ(j.value("_", json({{"foo", "bar"}})), json({{"foo", "bar"}}));
    EXPECT_EQ(j.value("_", json({10, 100})), json({10, 100}));

    EXPECT_EQ(j_const.value("_", 2), 2);
    EXPECT_EQ(j_const.value("_", 2u), 2u);
    EXPECT_EQ(j_const.value("_", false), false);
    EXPECT_EQ(j_const.value("_", "bar"), "bar");
    EXPECT_LT(std::fabs(j_const.value("_", 12.34) - 12.34), 0.001);
    EXPECT_EQ(j_const.value("_", json({{"foo", "bar"}})), json({{"foo", "bar"}}));
    EXPECT_EQ(j_const.value("_", json({10, 100})), json({10, 100}));
}

TEST(JsonElementNonObjectKeyValueAccessTest, Null)
{
    json j_nonobject(json::value_t::null);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with null");
    EXPECT_THROW_MSG(j_nonobject_const.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with null");
}

TEST(JsonElementNonObjectKeyValueAccessTest, Boolean)
{
    json j_nonobject(json::value_t::boolean);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with boolean");
    EXPECT_THROW_MSG(j_nonobject_const.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with boolean");
}

TEST(JsonElementNonObjectKeyValueAccessTest, String)
{
    json j_nonobject(json::value_t::string);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with string");
    EXPECT_THROW_MSG(j_nonobject_const.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with string");
}

TEST(JsonElementNonObjectKeyValueAccessTest, Array)
{
    json j_nonobject(json::value_t::array);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with array");
    EXPECT_THROW_MSG(j_nonobject_const.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with array");
}

TEST(JsonElementNonObjectKeyValueAccessTest, Integer)
{
    json j_nonobject(json::value_t::number_integer);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
    EXPECT_THROW_MSG(j_nonobject_const.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
}

TEST(JsonElementNonObjectKeyValueAccessTest, Unsigned)
{
    json j_nonobject(json::value_t::number_unsigned);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
    EXPECT_THROW_MSG(j_nonobject_const.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
}

TEST(JsonElementNonObjectKeyValueAccessTest, Float)
{
    json j_nonobject(json::value_t::number_float);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
    EXPECT_THROW_MSG(j_nonobject_const.value("foo", 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
}

TEST_F(JsonElementObjectAccessTest, PointerValueExist)
{
    EXPECT_EQ(j.value("/integer"_json_pointer, 2), 1);
    EXPECT_LT(std::fabs(j.value("/integer"_json_pointer, 1.0) - 1), 0.001);
    EXPECT_EQ(j.value("/unsigned"_json_pointer, 2), 1);
    EXPECT_LT(std::fabs(j.value("/unsigned"_json_pointer, 1.0) - 1), 0.001);
    EXPECT_EQ(j.value("/null"_json_pointer, json(1)), json());
    EXPECT_EQ(j.value("/boolean"_json_pointer, false), true);
    EXPECT_EQ(j.value("/string"_json_pointer, "bar"), "hello world");
    EXPECT_EQ(j.value("/string"_json_pointer, std::string("bar")), "hello world");
    EXPECT_LT(std::fabs(j.value("/floating"_json_pointer, 12.34) - 42.23), 0.001);
    EXPECT_EQ(j.value("/floating"_json_pointer, 12), 42);
    EXPECT_EQ(j.value("/object"_json_pointer, json({{"foo", "bar"}})), json::object());
    EXPECT_EQ(j.value("/array"_json_pointer, json({10, 100})), json({1, 2, 3}));

    EXPECT_EQ(j_const.value("/integer"_json_pointer, 2), 1);
    EXPECT_LT(std::fabs(j_const.value("/integer"_json_pointer, 1.0) - 1), 0.001);
    EXPECT_EQ(j_const.value("/unsigned"_json_pointer, 2), 1);
    EXPECT_LT(std::fabs(j_const.value("/unsigned"_json_pointer, 1.0) - 1), 0.001);
    EXPECT_EQ(j_const.value("/boolean"_json_pointer, false), true);
    EXPECT_EQ(j_const.value("/string"_json_pointer, "bar"), "hello world");
    EXPECT_EQ(j_const.value("/string"_json_pointer, std::string("bar")), "hello world");
    EXPECT_LT(std::fabs(j_const.value("/floating"_json_pointer, 12.34) - 42.23), 0.001);
    EXPECT_EQ(j_const.value("/floating"_json_pointer, 12), 42);
    EXPECT_EQ(j_const.value("/object"_json_pointer, json({{"foo", "bar"}})), json::object());
    EXPECT_EQ(j_const.value("/array"_json_pointer, json({10, 100})), json({1, 2, 3}));
}

TEST(JsonElementNonObjectPointerValueAccessTest, Null)
{
    json j_nonobject(json::value_t::null);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with null");
    EXPECT_THROW_MSG(j_nonobject_const.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with null");
}

TEST(JsonElementNonObjectPointerValueAccessTest, Boolean)
{
    json j_nonobject(json::value_t::boolean);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with boolean");
    EXPECT_THROW_MSG(j_nonobject_const.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with boolean");
}

TEST(JsonElementNonObjectPointerValueAccessTest, String)
{
    json j_nonobject(json::value_t::string);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with string");
    EXPECT_THROW_MSG(j_nonobject_const.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with string");
}

TEST(JsonElementNonObjectPointerValueAccessTest, Array)
{
    json j_nonobject(json::value_t::array);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with array");
    EXPECT_THROW_MSG(j_nonobject_const.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with array");
}

TEST(JsonElementNonObjectPointerValueAccessTest, Integer)
{
    json j_nonobject(json::value_t::number_integer);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
    EXPECT_THROW_MSG(j_nonobject_const.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
}

TEST(JsonElementNonObjectPointerValueAccessTest, Unsigned)
{
    json j_nonobject(json::value_t::number_unsigned);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
    EXPECT_THROW_MSG(j_nonobject_const.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
}

TEST(JsonElementNonObjectPointerValueAccessTest, Float)
{
    json j_nonobject(json::value_t::number_float);
    const json j_nonobject_const(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
    EXPECT_THROW_MSG(j_nonobject_const.value("/foo"_json_pointer, 1), json::type_error,
                     "[json.exception.type_error.306] cannot use value() with number");
}
#if 0
TEST_F(JsonElementObjectAccessTest, FrontAndBack)
{
    // "array" is the smallest key
    EXPECT_EQ(j.front(), json({1, 2, 3}));
    EXPECT_EQ(j_const.front(), json({1, 2, 3}));
    // "unsigned" is the largest key
    EXPECT_EQ(j.back(), json(1u));
    EXPECT_EQ(j_const.back(), json(1u));
}
#endif
TEST_F(JsonElementObjectAccessTest, OperatorWithinBounds)
{
    EXPECT_EQ(j["integer"], json(1));
    EXPECT_EQ(j[json::object_t::key_type("integer")], j["integer"]);

    EXPECT_EQ(j["unsigned"], json(1u));
    EXPECT_EQ(j[json::object_t::key_type("unsigned")], j["unsigned"]);

    EXPECT_EQ(j["boolean"], json(true));
    EXPECT_EQ(j[json::object_t::key_type("boolean")], j["boolean"]);

    EXPECT_EQ(j["null"], json(nullptr));
    EXPECT_EQ(j[json::object_t::key_type("null")], j["null"]);

    EXPECT_EQ(j["string"], json("hello world"));
    EXPECT_EQ(j[json::object_t::key_type("string")], j["string"]);

    EXPECT_EQ(j["floating"], json(42.23));
    EXPECT_EQ(j[json::object_t::key_type("floating")], j["floating"]);

    EXPECT_EQ(j["object"], json::object());
    EXPECT_EQ(j[json::object_t::key_type("object")], j["object"]);

    EXPECT_EQ(j["array"], json({1, 2, 3}));
    EXPECT_EQ(j[json::object_t::key_type("array")], j["array"]);

    EXPECT_EQ(j_const["integer"], json(1));
    EXPECT_EQ(j_const[json::object_t::key_type("integer")], j["integer"]);

    EXPECT_EQ(j_const["boolean"], json(true));
    EXPECT_EQ(j_const[json::object_t::key_type("boolean")], j["boolean"]);

    EXPECT_EQ(j_const["null"], json(nullptr));
    EXPECT_EQ(j_const[json::object_t::key_type("null")], j["null"]);

    EXPECT_EQ(j_const["string"], json("hello world"));
    EXPECT_EQ(j_const[json::object_t::key_type("string")], j["string"]);

    EXPECT_EQ(j_const["floating"], json(42.23));
    EXPECT_EQ(j_const[json::object_t::key_type("floating")], j["floating"]);

    EXPECT_EQ(j_const["object"], json::object());
    EXPECT_EQ(j_const[json::object_t::key_type("object")], j["object"]);

    EXPECT_EQ(j_const["array"], json({1, 2, 3}));
    EXPECT_EQ(j_const[json::object_t::key_type("array")], j["array"]);
}

TEST(JsonElementNonObjectOperatorAccessTest, Null)
{
    json j_nonobject(json::value_t::null);
    json j_nonobject2(json::value_t::null);
    const json j_const_nonobject(j_nonobject);
    EXPECT_NO_THROW(j_nonobject["foo"]);
    EXPECT_NO_THROW(j_nonobject2[json::object_t::key_type("foo")]);
    EXPECT_THROW_MSG(j_const_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with null");
    EXPECT_THROW_MSG(j_const_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with null");
}

TEST(JsonElementNonObjectOperatorAccessTest, Boolean)
{
    json j_nonobject(json::value_t::boolean);
    const json j_const_nonobject(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with boolean");
    EXPECT_THROW_MSG(j_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with boolean");
    EXPECT_THROW_MSG(j_const_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with boolean");
    EXPECT_THROW_MSG(j_const_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with boolean");
}

TEST(JsonElementNonObjectOperatorAccessTest, String)
{
    json j_nonobject(json::value_t::string);
    const json j_const_nonobject(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with string");
    EXPECT_THROW_MSG(j_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with string");
    EXPECT_THROW_MSG(j_const_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with string");
    EXPECT_THROW_MSG(j_const_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with string");
}

TEST(JsonElementNonObjectOperatorAccessTest, Array)
{
    json j_nonobject(json::value_t::array);
    const json j_const_nonobject(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with array");
    EXPECT_THROW_MSG(j_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with array");
    EXPECT_THROW_MSG(j_const_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with array");
    EXPECT_THROW_MSG(j_const_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with array");
}

TEST(JsonElementNonObjectOperatorAccessTest, Integer)
{
    json j_nonobject(json::value_t::number_integer);
    const json j_const_nonobject(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_const_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_const_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
}

TEST(JsonElementNonObjectOperatorAccessTest, Unsigned)
{
    json j_nonobject(json::value_t::number_unsigned);
    const json j_const_nonobject(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_const_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_const_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
}

TEST(JsonElementNonObjectOperatorAccessTest, Float)
{
    json j_nonobject(json::value_t::number_float);
    const json j_const_nonobject(j_nonobject);
    EXPECT_THROW_MSG(j_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_const_nonobject["foo"], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_const_nonobject[json::object_t::key_type("foo")], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
}

class JsonElementObjectRemoveTest : public ::testing::Test,
                                    public JsonElementObjectAccessTestBase {};

TEST_F(JsonElementObjectRemoveTest, Key)
{
    EXPECT_NE(j.find("integer"), j.end());
    EXPECT_EQ(j.erase("integer"), 1u);
    EXPECT_EQ(j.find("integer"), j.end());
    EXPECT_EQ(j.erase("integer"), 0u);

    EXPECT_NE(j.find("unsigned"), j.end());
    EXPECT_EQ(j.erase("unsigned"), 1u);
    EXPECT_EQ(j.find("unsigned"), j.end());
    EXPECT_EQ(j.erase("unsigned"), 0u);

    EXPECT_NE(j.find("boolean"), j.end());
    EXPECT_EQ(j.erase("boolean"), 1u);
    EXPECT_EQ(j.find("boolean"), j.end());
    EXPECT_EQ(j.erase("boolean"), 0u);

    EXPECT_NE(j.find("null"), j.end());
    EXPECT_EQ(j.erase("null"), 1u);
    EXPECT_EQ(j.find("null"), j.end());
    EXPECT_EQ(j.erase("null"), 0u);

    EXPECT_NE(j.find("string"), j.end());
    EXPECT_EQ(j.erase("string"), 1u);
    EXPECT_EQ(j.find("string"), j.end());
    EXPECT_EQ(j.erase("string"), 0u);

    EXPECT_NE(j.find("floating"), j.end());
    EXPECT_EQ(j.erase("floating"), 1u);
    EXPECT_EQ(j.find("floating"), j.end());
    EXPECT_EQ(j.erase("floating"), 0u);

    EXPECT_NE(j.find("object"), j.end());
    EXPECT_EQ(j.erase("object"), 1u);
    EXPECT_EQ(j.find("object"), j.end());
    EXPECT_EQ(j.erase("object"), 0u);

    EXPECT_NE(j.find("array"), j.end());
    EXPECT_EQ(j.erase("array"), 1u);
    EXPECT_EQ(j.find("array"), j.end());
    EXPECT_EQ(j.erase("array"), 0u);
}

// erase(begin())
TEST_F(JsonElementObjectRemoveTest, Begin)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    jobject.erase(jobject.begin());
    EXPECT_EQ(jobject, json({{"b", 1}, {"c", 17u}}));
}

TEST_F(JsonElementObjectRemoveTest, BeginConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    jobject.erase(jobject.cbegin());
    EXPECT_EQ(jobject, json({{"b", 1}, {"c", 17u}}));
}

// erase(begin(), end())
TEST_F(JsonElementObjectRemoveTest, BeginEnd)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json::iterator it2 = jobject.erase(jobject.begin(), jobject.end());
    EXPECT_EQ(jobject, json::object());
    EXPECT_EQ(it2, jobject.end());
#else
    EXPECT_THROW(jobject.erase(jobject.begin(), jobject.end()), json::type_error);
#endif
}

TEST_F(JsonElementObjectRemoveTest, BeginEndConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json::const_iterator it2 = jobject.erase(jobject.cbegin(), jobject.cend());
    EXPECT_EQ(jobject, json::object());
    EXPECT_EQ(it2, jobject.cend());
#else
    EXPECT_THROW(jobject.erase(jobject.cbegin(), jobject.cend()), json::type_error);
#endif
}

TEST_F(JsonElementObjectRemoveTest, BeginBegin)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json::iterator it2 = jobject.erase(jobject.begin(), jobject.begin());
    EXPECT_EQ(jobject, json({{"a", "a"}, {"b", 1}, {"c", 17u}}));
    EXPECT_EQ(*it2, json("a"));
#else
    EXPECT_THROW(jobject.erase(jobject.begin(), jobject.end()), json::type_error);
#endif
}

TEST_F(JsonElementObjectRemoveTest, BeginBeginConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
#if 0
    json::const_iterator it2 = jobject.erase(jobject.cbegin(), jobject.cbegin());
    EXPECT_EQ(jobject, json({{"a", "a"}, {"b", 1}, {"c", 17u}}));
    EXPECT_EQ(*it2, json("a"));
#else
    EXPECT_THROW(jobject.erase(jobject.cbegin(), jobject.cbegin()), json::type_error);
#endif
}

TEST_F(JsonElementObjectRemoveTest, Offset)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    json::iterator it = jobject.find("b");
    jobject.erase(it);
    EXPECT_EQ(jobject, json({{"a", "a"}, {"c", 17u}}));
}

TEST_F(JsonElementObjectRemoveTest, OffsetConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    json::const_iterator it = jobject.find("b");
    jobject.erase(it);
    EXPECT_EQ(jobject, json({{"a", "a"}, {"c", 17u}}));
}

TEST_F(JsonElementObjectRemoveTest, Subrange)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}, {"d", false}, {"e", true}};
#if 0
    json::iterator it2 = jobject.erase(jobject.find("b"), jobject.find("e"));
    EXPECT_EQ(jobject, json({{"a", "a"}, {"e", true}}));
    EXPECT_EQ(*it2, json(true));
#else
    EXPECT_THROW(jobject.erase(jobject.find("b"), jobject.find("e")), json::type_error);
#endif
}

TEST_F(JsonElementObjectRemoveTest, SubrangeConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}, {"d", false}, {"e", true}};
#if 0
    json::const_iterator it2 = jobject.erase(jobject.find("b"), jobject.find("e"));
    EXPECT_EQ(jobject, json({{"a", "a"}, {"e", true}}));
    EXPECT_EQ(*it2, json(true));
#else
    EXPECT_THROW(jobject.erase(jobject.find("b"), jobject.find("e")), json::type_error);
#endif
}

TEST_F(JsonElementObjectRemoveTest, Different)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}, {"d", false}, {"e", true}};
    json jobject2 = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    EXPECT_THROW_MSG(jobject.erase(jobject2.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(jobject.erase(jobject.begin(), jobject2.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(jobject.erase(jobject2.begin(), jobject.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(jobject.erase(jobject2.begin(), jobject2.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
}

TEST_F(JsonElementObjectRemoveTest, DifferentConst)
{
    json jobject = {{"a", "a"}, {"b", 1}, {"c", 17u}, {"d", false}, {"e", true}};
    json jobject2 = {{"a", "a"}, {"b", 1}, {"c", 17u}};
    EXPECT_THROW_MSG(jobject.erase(jobject2.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(jobject.erase(jobject.cbegin(), jobject2.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(jobject.erase(jobject2.cbegin(), jobject.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(jobject.erase(jobject2.cbegin(), jobject2.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
}

// remove element by key in non-object type
TEST(JsonElementNonObjectKeyRemoveTest, Null)
{
    json j_nonobject(json::value_t::null);
    EXPECT_THROW_MSG(j_nonobject.erase("foo"), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with null");
}

TEST(JsonElementNonObjectKeyRemoveTest, Boolean)
{
    json j_nonobject(json::value_t::boolean);
    EXPECT_THROW_MSG(j_nonobject.erase("foo"), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with boolean");
}

TEST(JsonElementNonObjectKeyRemoveTest, String)
{
    json j_nonobject(json::value_t::string);
    EXPECT_THROW_MSG(j_nonobject.erase("foo"), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with string");
}

TEST(JsonElementNonObjectKeyRemoveTest, Array)
{
    json j_nonobject(json::value_t::array);
    EXPECT_THROW_MSG(j_nonobject.erase("foo"), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with array");
}

TEST(JsonElementNonObjectKeyRemoveTest, Integer)
{
    json j_nonobject(json::value_t::number_integer);
    EXPECT_THROW_MSG(j_nonobject.erase("foo"), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with number");
}

TEST(JsonElementNonObjectKeyRemoveTest, Float)
{
    json j_nonobject(json::value_t::number_float);
    EXPECT_THROW_MSG(j_nonobject.erase("foo"), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with number");
}

TEST_F(JsonElementObjectAccessTest, FindExist)
{
    for (auto key :
            {"integer", "unsigned", "floating", "null", "string", "boolean", "object", "array"
            })
    {
        EXPECT_NE(j.find(key), j.end());
        EXPECT_EQ(*j.find(key), j.at(key));
        EXPECT_NE(j_const.find(key), j_const.end());
        EXPECT_EQ(*j_const.find(key), j_const.at(key));
    }
}

TEST_F(JsonElementObjectAccessTest, FindNotExist)
{
    EXPECT_EQ(j.find("foo"), j.end());
    EXPECT_EQ(j_const.find("foo"), j_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, Null)
{
    json j_nonarray(json::value_t::null);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, String)
{
    json j_nonarray(json::value_t::string);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, Object)
{
    json j_nonarray(json::value_t::object);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, Array)
{
    json j_nonarray(json::value_t::array);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, Boolean)
{
    json j_nonarray(json::value_t::boolean);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, Integer)
{
    json j_nonarray(json::value_t::number_integer);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, Unsigned)
{
    json j_nonarray(json::value_t::number_unsigned);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST(JsonElementNonObjectFindAccessTest, Float)
{
    json j_nonarray(json::value_t::number_float);
    const json j_nonarray_const(j_nonarray);
    EXPECT_EQ(j_nonarray.find("foo"), j_nonarray.end());
    EXPECT_EQ(j_nonarray_const.find("foo"), j_nonarray_const.end());
}

TEST_F(JsonElementObjectAccessTest, CountExist)
{
    for (auto key :
            {"integer", "unsigned", "floating", "null", "string", "boolean", "object", "array"
            })
    {
        EXPECT_EQ(j.count(key), 1u);
        EXPECT_EQ(j_const.count(key), 1u);
    }
}

TEST_F(JsonElementObjectAccessTest, CountNotExist)
{
    EXPECT_EQ(j.count("foo"), 0u);
    EXPECT_EQ(j_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, Null)
{
    json j_nonobject(json::value_t::null);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, String)
{
    json j_nonobject(json::value_t::string);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, Object)
{
    json j_nonobject(json::value_t::object);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, Array)
{
    json j_nonobject(json::value_t::array);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, Boolean)
{
    json j_nonobject(json::value_t::boolean);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, Integer)
{
    json j_nonobject(json::value_t::number_integer);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, Unsigned)
{
    json j_nonobject(json::value_t::number_unsigned);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST(JsonElementNonObjectCountAccessTest, Float)
{
    json j_nonobject(json::value_t::number_float);
    const json j_nonobject_const(j_nonobject);
    EXPECT_EQ(j_nonobject.count("foo"), 0u);
    EXPECT_EQ(j_nonobject_const.count("foo"), 0u);
}

TEST_F(JsonElementObjectAccessTest, PointerValueNotExist)
{
    EXPECT_EQ(j.value("/not/existing"_json_pointer, 2), 2);
    EXPECT_EQ(j.value("/not/existing"_json_pointer, 2u), 2u);
    EXPECT_EQ(j.value("/not/existing"_json_pointer, false), false);
    EXPECT_EQ(j.value("/not/existing"_json_pointer, "bar"), "bar");
    EXPECT_LT(std::fabs(j.value("/not/existing"_json_pointer, 12.34) - 12.34), 0.001);
    EXPECT_EQ(j.value("/not/existing"_json_pointer, json({{"foo", "bar"}})), json({{"foo", "bar"}}));
    EXPECT_EQ(j.value("/not/existing"_json_pointer, json({10, 100})), json({10, 100}));

    EXPECT_EQ(j_const.value("/not/existing"_json_pointer, 2), 2);
    EXPECT_EQ(j_const.value("/not/existing"_json_pointer, 2u), 2u);
    EXPECT_EQ(j_const.value("/not/existing"_json_pointer, false), false);
    EXPECT_EQ(j_const.value("/not/existing"_json_pointer, "bar"), "bar");
    EXPECT_LT(std::fabs(j_const.value("/not/existing"_json_pointer, 12.34) - 12.34), 0.001);
    EXPECT_EQ(j_const.value("/not/existing"_json_pointer, json({{"foo", "bar"}})), json({{"foo", "bar"}}));
    EXPECT_EQ(j_const.value("/not/existing"_json_pointer, json({10, 100})), json({10, 100}));
}
