/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
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
using wpi::JsonTest;

TEST(JsonPointerTest, Errors)
{
    EXPECT_THROW_MSG(json::json_pointer("foo"), json::parse_error,
                     "[json.exception.parse_error.107] parse error at 1: JSON pointer must be empty or begin with '/' - was: 'foo'");

    EXPECT_THROW_MSG(json::json_pointer("/~~"), json::parse_error,
                     "[json.exception.parse_error.108] parse error: escape character '~' must be followed with '0' or '1'");

    EXPECT_THROW_MSG(json::json_pointer("/~"), json::parse_error,
                     "[json.exception.parse_error.108] parse error: escape character '~' must be followed with '0' or '1'");

    json::json_pointer p;
    EXPECT_THROW_MSG(JsonTest::top(p), json::out_of_range,
                     "[json.exception.out_of_range.405] JSON pointer has no parent");
    EXPECT_THROW_MSG(JsonTest::pop_back(p), json::out_of_range,
                     "[json.exception.out_of_range.405] JSON pointer has no parent");
}

// examples from RFC 6901
TEST(JsonPointerTest, AccessNonConst)
{
    json j = R"(
    {
        "foo": ["bar", "baz"],
        "": 0,
        "a/b": 1,
        "c%d": 2,
        "e^f": 3,
        "g|h": 4,
        "i\\j": 5,
        "k\"l": 6,
        " ": 7,
        "m~n": 8
    }
    )"_json;

    // the whole document
    EXPECT_EQ(j[json::json_pointer()], j);
    EXPECT_EQ(j[json::json_pointer("")], j);

    // array access
    EXPECT_EQ(j[json::json_pointer("/foo")], j["foo"]);
    EXPECT_EQ(j[json::json_pointer("/foo/0")], j["foo"][0]);
    EXPECT_EQ(j[json::json_pointer("/foo/1")], j["foo"][1]);
    EXPECT_EQ(j["/foo/1"_json_pointer], j["foo"][1]);

    // checked array access
    EXPECT_EQ(j.at(json::json_pointer("/foo/0")), j["foo"][0]);
    EXPECT_EQ(j.at(json::json_pointer("/foo/1")), j["foo"][1]);

    // empty string access
    EXPECT_EQ(j[json::json_pointer("/")], j[""]);

    // other cases
    EXPECT_EQ(j[json::json_pointer("/ ")], j[" "]);
    EXPECT_EQ(j[json::json_pointer("/c%d")], j["c%d"]);
    EXPECT_EQ(j[json::json_pointer("/e^f")], j["e^f"]);
    EXPECT_EQ(j[json::json_pointer("/g|h")], j["g|h"]);
    EXPECT_EQ(j[json::json_pointer("/i\\j")], j["i\\j"]);
    EXPECT_EQ(j[json::json_pointer("/k\"l")], j["k\"l"]);

    // checked access
    EXPECT_EQ(j.at(json::json_pointer("/ ")), j[" "]);
    EXPECT_EQ(j.at(json::json_pointer("/c%d")), j["c%d"]);
    EXPECT_EQ(j.at(json::json_pointer("/e^f")), j["e^f"]);
    EXPECT_EQ(j.at(json::json_pointer("/g|h")), j["g|h"]);
    EXPECT_EQ(j.at(json::json_pointer("/i\\j")), j["i\\j"]);
    EXPECT_EQ(j.at(json::json_pointer("/k\"l")), j["k\"l"]);

    // escaped access
    EXPECT_EQ(j[json::json_pointer("/a~1b")], j["a/b"]);
    EXPECT_EQ(j[json::json_pointer("/m~0n")], j["m~n"]);

    // unescaped access
    // access to nonexisting values yield object creation
    EXPECT_NO_THROW(j[json::json_pointer("/a/b")] = 42);
    EXPECT_EQ(j["a"]["b"], json(42));
    EXPECT_NO_THROW(j[json::json_pointer("/a/c/1")] = 42);
    EXPECT_EQ(j["a"]["c"], json({nullptr, 42}));
    EXPECT_NO_THROW(j[json::json_pointer("/a/d/-")] = 42);
    EXPECT_EQ(j["a"]["d"], json::array({42}));
    // "/a/b" works for JSON {"a": {"b": 42}}
    EXPECT_EQ(json({{"a", {{"b", 42}}}})[json::json_pointer("/a/b")], json(42));

    // unresolved access
    json j_primitive = 1;
    EXPECT_THROW_MSG(j_primitive["/foo"_json_pointer], json::out_of_range,
                     "[json.exception.out_of_range.404] unresolved reference token 'foo'");
    EXPECT_THROW_MSG(j_primitive.at("/foo"_json_pointer), json::out_of_range,
                     "[json.exception.out_of_range.404] unresolved reference token 'foo'");
}

TEST(JsonPointerTest, AccessConst)
{
    const json j = R"(
    {
        "foo": ["bar", "baz"],
        "": 0,
        "a/b": 1,
        "c%d": 2,
        "e^f": 3,
        "g|h": 4,
        "i\\j": 5,
        "k\"l": 6,
        " ": 7,
        "m~n": 8
    }
    )"_json;

    // the whole document
    EXPECT_EQ(j[json::json_pointer()], j);
    EXPECT_EQ(j[json::json_pointer("")], j);

    // array access
    EXPECT_EQ(j[json::json_pointer("/foo")], j["foo"]);
    EXPECT_EQ(j[json::json_pointer("/foo/0")], j["foo"][0]);
    EXPECT_EQ(j[json::json_pointer("/foo/1")], j["foo"][1]);
    EXPECT_EQ(j["/foo/1"_json_pointer], j["foo"][1]);

    // checked array access
    EXPECT_EQ(j.at(json::json_pointer("/foo/0")), j["foo"][0]);
    EXPECT_EQ(j.at(json::json_pointer("/foo/1")), j["foo"][1]);

    // empty string access
    EXPECT_EQ(j[json::json_pointer("/")], j[""]);

    // other cases
    EXPECT_EQ(j[json::json_pointer("/ ")], j[" "]);
    EXPECT_EQ(j[json::json_pointer("/c%d")], j["c%d"]);
    EXPECT_EQ(j[json::json_pointer("/e^f")], j["e^f"]);
    EXPECT_EQ(j[json::json_pointer("/g|h")], j["g|h"]);
    EXPECT_EQ(j[json::json_pointer("/i\\j")], j["i\\j"]);
    EXPECT_EQ(j[json::json_pointer("/k\"l")], j["k\"l"]);

    // checked access
    EXPECT_EQ(j.at(json::json_pointer("/ ")), j[" "]);
    EXPECT_EQ(j.at(json::json_pointer("/c%d")), j["c%d"]);
    EXPECT_EQ(j.at(json::json_pointer("/e^f")), j["e^f"]);
    EXPECT_EQ(j.at(json::json_pointer("/g|h")), j["g|h"]);
    EXPECT_EQ(j.at(json::json_pointer("/i\\j")), j["i\\j"]);
    EXPECT_EQ(j.at(json::json_pointer("/k\"l")), j["k\"l"]);

    // escaped access
    EXPECT_EQ(j[json::json_pointer("/a~1b")], j["a/b"]);
    EXPECT_EQ(j[json::json_pointer("/m~0n")], j["m~n"]);

    // unescaped access
    EXPECT_THROW_MSG(j.at(json::json_pointer("/a/b")), json::out_of_range,
                     "[json.exception.out_of_range.403] key 'a' not found");

    // unresolved access
    const json j_primitive = 1;
    EXPECT_THROW_MSG(j_primitive["/foo"_json_pointer], json::out_of_range,
                     "[json.exception.out_of_range.404] unresolved reference token 'foo'");
    EXPECT_THROW_MSG(j_primitive.at("/foo"_json_pointer), json::out_of_range,
                     "[json.exception.out_of_range.404] unresolved reference token 'foo'");
}

TEST(JsonPointerTest, UserStringLiteral)
{
    json j = R"(
    {
        "foo": ["bar", "baz"],
        "": 0,
        "a/b": 1,
        "c%d": 2,
        "e^f": 3,
        "g|h": 4,
        "i\\j": 5,
        "k\"l": 6,
        " ": 7,
        "m~n": 8
    }
    )"_json;

    // the whole document
    EXPECT_EQ(j[""_json_pointer], j);

    // array access
    EXPECT_EQ(j["/foo"_json_pointer], j["foo"]);
    EXPECT_EQ(j["/foo/0"_json_pointer], j["foo"][0]);
    EXPECT_EQ(j["/foo/1"_json_pointer], j["foo"][1]);
}

TEST(JsonPointerTest, ArrayNonConst)
{
    json j = {1, 2, 3};
    const json j_const = j;

    // check reading access
    EXPECT_EQ(j["/0"_json_pointer], j[0]);
    EXPECT_EQ(j["/1"_json_pointer], j[1]);
    EXPECT_EQ(j["/2"_json_pointer], j[2]);

    // assign to existing index
    j["/1"_json_pointer] = 13;
    EXPECT_EQ(j[1], json(13));

    // assign to nonexisting index
    j["/3"_json_pointer] = 33;
    EXPECT_EQ(j[3], json(33));

    // assign to nonexisting index (with gap)
    j["/5"_json_pointer] = 55;
    EXPECT_EQ(j, json({1, 13, 3, 33, nullptr, 55}));

    // error with leading 0
    EXPECT_THROW_MSG(j["/01"_json_pointer], json::parse_error,
                     "[json.exception.parse_error.106] parse error: array index '01' must not begin with '0'");
    EXPECT_THROW_MSG(j_const["/01"_json_pointer], json::parse_error,
                     "[json.exception.parse_error.106] parse error: array index '01' must not begin with '0'");
    EXPECT_THROW_MSG(j.at("/01"_json_pointer), json::parse_error,
                     "[json.exception.parse_error.106] parse error: array index '01' must not begin with '0'");
    EXPECT_THROW_MSG(j_const.at("/01"_json_pointer), json::parse_error,
                     "[json.exception.parse_error.106] parse error: array index '01' must not begin with '0'");

    // error with incorrect numbers
    EXPECT_THROW_MSG(j["/one"_json_pointer] = 1, json::parse_error,
                     "[json.exception.parse_error.109] parse error: array index 'one' is not a number");
    EXPECT_THROW_MSG(j_const["/one"_json_pointer] == 1, json::parse_error,
                     "[json.exception.parse_error.109] parse error: array index 'one' is not a number");

    EXPECT_THROW_MSG(j.at("/one"_json_pointer) = 1, json::parse_error,
                     "[json.exception.parse_error.109] parse error: array index 'one' is not a number");
    EXPECT_THROW_MSG(j_const.at("/one"_json_pointer) == 1, json::parse_error,
                     "[json.exception.parse_error.109] parse error: array index 'one' is not a number");

    EXPECT_THROW_MSG(json({{"/list/0", 1}, {"/list/1", 2}, {"/list/three", 3}}).unflatten(), json::parse_error,
                     "[json.exception.parse_error.109] parse error: array index 'three' is not a number");

    // assign to "-"
    j["/-"_json_pointer] = 99;
    EXPECT_EQ(j, json({1, 13, 3, 33, nullptr, 55, 99}));

    // error when using "-" in const object
    EXPECT_THROW_MSG(j_const["/-"_json_pointer], json::out_of_range,
                     "[json.exception.out_of_range.402] array index '-' (3) is out of range");

    // error when using "-" with at
    EXPECT_THROW_MSG(j.at("/-"_json_pointer), json::out_of_range,
                     "[json.exception.out_of_range.402] array index '-' (7) is out of range");
    EXPECT_THROW_MSG(j_const.at("/-"_json_pointer), json::out_of_range,
                     "[json.exception.out_of_range.402] array index '-' (3) is out of range");
}

TEST(JsonPointerTest, ArrayConst)
{
    const json j = {1, 2, 3};

    // check reading access
    EXPECT_EQ(j["/0"_json_pointer], j[0]);
    EXPECT_EQ(j["/1"_json_pointer], j[1]);
    EXPECT_EQ(j["/2"_json_pointer], j[2]);

    // assign to nonexisting index
    EXPECT_THROW_MSG(j.at("/3"_json_pointer), json::out_of_range,
                     "[json.exception.out_of_range.401] array index 3 is out of range");

    // assign to nonexisting index (with gap)
    EXPECT_THROW_MSG(j.at("/5"_json_pointer), json::out_of_range,
                     "[json.exception.out_of_range.401] array index 5 is out of range");

    // assign to "-"
    EXPECT_THROW_MSG(j["/-"_json_pointer], json::out_of_range,
                     "[json.exception.out_of_range.402] array index '-' (3) is out of range");
    EXPECT_THROW_MSG(j.at("/-"_json_pointer), json::out_of_range,
                     "[json.exception.out_of_range.402] array index '-' (3) is out of range");
}

TEST(JsonPointerTest, Flatten)
{
    json j =
    {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {
            "answer", {
                {"everything", 42}
            }
        },
        {"list", {1, 0, 2}},
        {
            "object", {
                {"currency", "USD"},
                {"value", 42.99},
                {"", "empty string"},
                {"/", "slash"},
                {"~", "tilde"},
                {"~1", "tilde1"}
            }
        }
    };

    json j_flatten =
    {
        {"/pi", 3.141},
        {"/happy", true},
        {"/name", "Niels"},
        {"/nothing", nullptr},
        {"/answer/everything", 42},
        {"/list/0", 1},
        {"/list/1", 0},
        {"/list/2", 2},
        {"/object/currency", "USD"},
        {"/object/value", 42.99},
        {"/object/", "empty string"},
        {"/object/~1", "slash"},
        {"/object/~0", "tilde"},
        {"/object/~01", "tilde1"}
    };

    // check if flattened result is as expected
    EXPECT_EQ(j.flatten(), j_flatten);

    // check if unflattened result is as expected
    EXPECT_EQ(j_flatten.unflatten(), j);

    // error for nonobjects
    EXPECT_THROW_MSG(json(1).unflatten(), json::type_error,
                     "[json.exception.type_error.314] only objects can be unflattened");

    // error for nonprimitve values
    EXPECT_THROW_MSG(json({{"/1", {1, 2, 3}}}).unflatten(), json::type_error,
                     "[json.exception.type_error.315] values in object must be primitive");

    // error for conflicting values
    json j_error = {{"", 42}, {"/foo", 17}};
    EXPECT_THROW_MSG(j_error.unflatten(), json::type_error,
                     "[json.exception.type_error.313] invalid value to unflatten");

    // explicit roundtrip check
    EXPECT_EQ(j.flatten().unflatten(), j);

    // roundtrip for primitive values
    json j_null;
    EXPECT_EQ(j_null.flatten().unflatten(), j_null);
    json j_number = 42;
    EXPECT_EQ(j_number.flatten().unflatten(), j_number);
    json j_boolean = false;
    EXPECT_EQ(j_boolean.flatten().unflatten(), j_boolean);
    json j_string = "foo";
    EXPECT_EQ(j_string.flatten().unflatten(), j_string);

    // roundtrip for empty structured values (will be unflattened to null)
    json j_array(json::value_t::array);
    EXPECT_EQ(j_array.flatten().unflatten(), json());
    json j_object(json::value_t::object);
    EXPECT_EQ(j_object.flatten().unflatten(), json());
}

TEST(JsonPointerTest, StringRepresentation)
{
    for (auto ptr :
            {"", "/foo", "/foo/0", "/", "/a~1b", "/c%d", "/e^f", "/g|h", "/i\\j", "/k\"l", "/ ", "/m~0n"
            })
    {
        SCOPED_TRACE(ptr);
        EXPECT_EQ(json::json_pointer(ptr).to_string(), ptr);
    }
}
