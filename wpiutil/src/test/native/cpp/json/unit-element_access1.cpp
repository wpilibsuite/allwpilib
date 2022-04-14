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

class JsonElementArrayAccessTestBase {
 public:
    JsonElementArrayAccessTestBase() : j_const(j) {}

 protected:
    json j = {1, 1u, true, nullptr, "string", 42.23, json::object(), {1, 2, 3}};
    const json j_const;
};

class JsonElementArrayAccessTest : public ::testing::Test,
                                   public JsonElementArrayAccessTestBase {};

TEST_F(JsonElementArrayAccessTest, AtWithinBounds)
{
    EXPECT_EQ(j.at(0), json(1));
    EXPECT_EQ(j.at(1), json(1u));
    EXPECT_EQ(j.at(2), json(true));
    EXPECT_EQ(j.at(3), json(nullptr));
    EXPECT_EQ(j.at(4), json("string"));
    EXPECT_EQ(j.at(5), json(42.23));
    EXPECT_EQ(j.at(6), json::object());
    EXPECT_EQ(j.at(7), json({1, 2, 3}));

    EXPECT_EQ(j_const.at(0), json(1));
    EXPECT_EQ(j_const.at(1), json(1u));
    EXPECT_EQ(j_const.at(2), json(true));
    EXPECT_EQ(j_const.at(3), json(nullptr));
    EXPECT_EQ(j_const.at(4), json("string"));
    EXPECT_EQ(j_const.at(5), json(42.23));
    EXPECT_EQ(j_const.at(6), json::object());
    EXPECT_EQ(j_const.at(7), json({1, 2, 3}));
}

TEST_F(JsonElementArrayAccessTest, AtOutsideBounds)
{
    EXPECT_THROW_MSG(j.at(8), json::out_of_range,
                     "[json.exception.out_of_range.401] array index 8 is out of range");
    EXPECT_THROW_MSG(j_const.at(8), json::out_of_range,
                     "[json.exception.out_of_range.401] array index 8 is out of range");
}

TEST(JsonElementNonArrayAtAccessTest, Null)
{
    json j_nonarray(json::value_t::null);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with null");
    EXPECT_THROW_MSG(j_nonarray_const.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with null");
}

TEST(JsonElementNonArrayAtAccessTest, Boolean)
{
    json j_nonarray(json::value_t::boolean);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with boolean");
    EXPECT_THROW_MSG(j_nonarray_const.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with boolean");
}

TEST(JsonElementNonArrayAtAccessTest, String)
{
    json j_nonarray(json::value_t::string);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with string");
    EXPECT_THROW_MSG(j_nonarray_const.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with string");
}

TEST(JsonElementNonArrayAtAccessTest, Object)
{
    json j_nonarray(json::value_t::object);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with object");
    EXPECT_THROW_MSG(j_nonarray_const.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with object");
}

TEST(JsonElementNonArrayAtAccessTest, Integer)
{
    json j_nonarray(json::value_t::number_integer);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
    EXPECT_THROW_MSG(j_nonarray_const.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
}

TEST(JsonElementNonArrayAtAccessTest, Unsigned)
{
    json j_nonarray(json::value_t::number_unsigned);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
    EXPECT_THROW_MSG(j_nonarray_const.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
}

TEST(JsonElementNonArrayAtAccessTest, Float)
{
    json j_nonarray(json::value_t::number_float);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
    EXPECT_THROW_MSG(j_nonarray_const.at(0), json::type_error,
                     "[json.exception.type_error.304] cannot use at() with number");
}

TEST_F(JsonElementArrayAccessTest, FrontAndBack)
{
    EXPECT_EQ(j.front(), json(1));
    EXPECT_EQ(j_const.front(), json(1));
    EXPECT_EQ(j.back(), json({1, 2, 3}));
    EXPECT_EQ(j_const.back(), json({1, 2, 3}));
}

TEST_F(JsonElementArrayAccessTest, OperatorWithinBounds)
{
    EXPECT_EQ(j[0], json(1));
    EXPECT_EQ(j[1], json(1u));
    EXPECT_EQ(j[2], json(true));
    EXPECT_EQ(j[3], json(nullptr));
    EXPECT_EQ(j[4], json("string"));
    EXPECT_EQ(j[5], json(42.23));
    EXPECT_EQ(j[6], json::object());
    EXPECT_EQ(j[7], json({1, 2, 3}));

    EXPECT_EQ(j_const[0], json(1));
    EXPECT_EQ(j_const[1], json(1u));
    EXPECT_EQ(j_const[2], json(true));
    EXPECT_EQ(j_const[3], json(nullptr));
    EXPECT_EQ(j_const[4], json("string"));
    EXPECT_EQ(j_const[5], json(42.23));
    EXPECT_EQ(j_const[6], json::object());
    EXPECT_EQ(j_const[7], json({1, 2, 3}));
}

TEST(JsonElementNonArrayOperatorAccessTest, NullStandard)
{
    json j_nonarray(json::value_t::null);
    const json j_nonarray_const(j_nonarray);
    EXPECT_NO_THROW(j_nonarray[0]);
    EXPECT_THROW_MSG(j_nonarray_const[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with null");
}

// implicit transformation to properly filled array
TEST(JsonElementNonArrayOperatorAccessTest, NullImplicitFilled)
{
    json j_nonarray;
    j_nonarray[3] = 42;
    EXPECT_EQ(j_nonarray, json({nullptr, nullptr, nullptr, 42}));
}

TEST(JsonElementNonArrayOperatorAccessTest, Boolean)
{
    json j_nonarray(json::value_t::boolean);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with boolean");
    EXPECT_THROW_MSG(j_nonarray_const[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with boolean");
}

TEST(JsonElementNonArrayOperatorAccessTest, String)
{
    json j_nonarray(json::value_t::string);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with string");
    EXPECT_THROW_MSG(j_nonarray_const[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with string");
}

TEST(JsonElementNonArrayOperatorAccessTest, Object)
{
    json j_nonarray(json::value_t::object);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with object");
    EXPECT_THROW_MSG(j_nonarray_const[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with object");
}

TEST(JsonElementNonArrayOperatorAccessTest, Integer)
{
    json j_nonarray(json::value_t::number_integer);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_nonarray_const[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
}

TEST(JsonElementNonArrayOperatorAccessTest, Unsigned)
{
    json j_nonarray(json::value_t::number_unsigned);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_nonarray_const[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
}

TEST(JsonElementNonArrayOperatorAccessTest, Float)
{
    json j_nonarray(json::value_t::number_float);
    const json j_nonarray_const(j_nonarray);
    EXPECT_THROW_MSG(j_nonarray[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
    EXPECT_THROW_MSG(j_nonarray_const[0], json::type_error,
                     "[json.exception.type_error.305] cannot use operator[] with number");
}

class JsonElementArrayRemoveTest : public ::testing::Test,
                                   public JsonElementArrayAccessTestBase {};


// remove element by index
TEST_F(JsonElementArrayRemoveTest, Index0)
{
    j.erase(0);
    EXPECT_EQ(j, json({1u, true, nullptr, "string", 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, Index1)
{
    j.erase(1);
    EXPECT_EQ(j, json({1, true, nullptr, "string", 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, Index2)
{
    j.erase(2);
    EXPECT_EQ(j, json({1, 1u, nullptr, "string", 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, Index3)
{
    j.erase(3);
    EXPECT_EQ(j, json({1, 1u, true, "string", 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, Index4)
{
    j.erase(4);
    EXPECT_EQ(j, json({1, 1u, true, nullptr, 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, Index5)
{
    j.erase(5);
    EXPECT_EQ(j, json({1, 1u, true, nullptr, "string", json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, Index6)
{
    j.erase(6);
    EXPECT_EQ(j, json({1, 1u, true, nullptr, "string", 42.23, {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, Index7)
{
    j.erase(7);
    EXPECT_EQ(j, json({1, 1u, true, nullptr, "string", 42.23, json::object()}));
}

TEST_F(JsonElementArrayRemoveTest, Index8)
{
    EXPECT_THROW_MSG(j.erase(8), json::out_of_range,
                     "[json.exception.out_of_range.401] array index 8 is out of range");
}

// erase(begin())
TEST_F(JsonElementArrayRemoveTest, Begin)
{
    j.erase(j.begin());
    EXPECT_EQ(j, json({1u, true, nullptr, "string", 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, BeginConst)
{
    j.erase(j.cbegin());
    EXPECT_EQ(j, json({1u, true, nullptr, "string", 42.23, json::object(), {1, 2, 3}}));
}

// erase(begin(), end())
TEST_F(JsonElementArrayRemoveTest, BeginEnd)
{
    j.erase(j.begin(), j.end());
    EXPECT_EQ(j, json::array());
}
TEST_F(JsonElementArrayRemoveTest, BeginEndConst)
{
    j.erase(j.cbegin(), j.cend());
    EXPECT_EQ(j, json::array());
}

// erase(begin(), begin())
TEST_F(JsonElementArrayRemoveTest, BeginBegin)
{
    j.erase(j.begin(), j.begin());
    EXPECT_EQ(j, json({1, 1u, true, nullptr, "string", 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, BeginBeginConst)
{
    j.erase(j.cbegin(), j.cbegin());
    EXPECT_EQ(j, json({1, 1u, true, nullptr, "string", 42.23, json::object(), {1, 2, 3}}));
}

// erase at offset
TEST_F(JsonElementArrayRemoveTest, Offset)
{
    json::iterator it = j.begin() + 4;
    j.erase(it);
    EXPECT_EQ(j, json({1, 1u, true, nullptr, 42.23, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, OffsetConst)
{
    json::const_iterator it = j.cbegin() + 4;
    j.erase(it);
    EXPECT_EQ(j, json({1, 1u, true, nullptr, 42.23, json::object(), {1, 2, 3}}));
}

// erase subrange
TEST_F(JsonElementArrayRemoveTest, Subrange)
{
    j.erase(j.begin() + 3, j.begin() + 6);
    EXPECT_EQ(j, json({1, 1u, true, json::object(), {1, 2, 3}}));
}

TEST_F(JsonElementArrayRemoveTest, SubrangeConst)
{
    j.erase(j.cbegin() + 3, j.cbegin() + 6);
    EXPECT_EQ(j, json({1, 1u, true, json::object(), {1, 2, 3}}));
}

// different arrays
TEST_F(JsonElementArrayRemoveTest, Different)
{
    json j2 = {"foo", "bar"};
    EXPECT_THROW_MSG(j.erase(j2.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(j.erase(j.begin(), j2.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(j.erase(j2.begin(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(j.erase(j2.begin(), j2.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
}

TEST_F(JsonElementArrayRemoveTest, DifferentConst)
{
    json j2 = {"foo", "bar"};
    EXPECT_THROW_MSG(j.erase(j2.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(j.erase(j.cbegin(), j2.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(j.erase(j2.cbegin(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
    EXPECT_THROW_MSG(j.erase(j2.cbegin(), j2.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.203] iterators do not fit current value");
}

// remove element by index in non-array type
TEST(JsonElementNonArrayIndexRemoveTest, Null)
{
    json j_nonobject(json::value_t::null);
    EXPECT_THROW_MSG(j_nonobject.erase(0), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with null");
}

TEST(JsonElementNonArrayIndexRemoveTest, Boolean)
{
    json j_nonobject(json::value_t::boolean);
    EXPECT_THROW_MSG(j_nonobject.erase(0), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with boolean");
}

TEST(JsonElementNonArrayIndexRemoveTest, String)
{
    json j_nonobject(json::value_t::string);
    EXPECT_THROW_MSG(j_nonobject.erase(0), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with string");
}

TEST(JsonElementNonArrayIndexRemoveTest, Object)
{
    json j_nonobject(json::value_t::object);
    EXPECT_THROW_MSG(j_nonobject.erase(0), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with object");
}

TEST(JsonElementNonArrayIndexRemoveTest, Integer)
{
    json j_nonobject(json::value_t::number_integer);
    EXPECT_THROW_MSG(j_nonobject.erase(0), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with number");
}

TEST(JsonElementNonArrayIndexRemoveTest, Unsigned)
{
    json j_nonobject(json::value_t::number_unsigned);
    EXPECT_THROW_MSG(j_nonobject.erase(0), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with number");
}

TEST(JsonElementNonArrayIndexRemoveTest, Float)
{
    json j_nonobject(json::value_t::number_float);
    EXPECT_THROW_MSG(j_nonobject.erase(0), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with number");
}

TEST(JsonElementNonArrayFrontBackAccessTest, Null)
{
    json j;
    EXPECT_THROW_MSG(j.front(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(j.back(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST(JsonElementNonArrayFrontBackAccessTest, NullConst)
{
    const json j{};
    EXPECT_THROW_MSG(j.front(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(j.back(), json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST(JsonElementNonArrayFrontBackAccessTest, String)
{
    json j = "foo";
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, StringConst)
{
    const json j = "bar";
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, Boolean)
{
    json j = false;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, BooleanConst)
{
    const json j = true;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, Integer)
{
    json j = 17;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, IntegerConst)
{
    const json j = 17;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, Unsigned)
{
    json j = 17u;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, UnsignedConst)
{
    const json j = 17u;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, Float)
{
    json j = 23.42;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayFrontBackAccessTest, FloatConst)
{
    const json j = 23.42;
    EXPECT_EQ(j.front(), j);
    EXPECT_EQ(j.back(), j);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, Null)
{
    json j;
    EXPECT_THROW_MSG(j.erase(j.begin()), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with null");
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, NullConst)
{
    json j;
    EXPECT_THROW_MSG(j.erase(j.cbegin()), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with null");
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, String)
{
    json j = "foo";
    j.erase(j.begin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, StringConst)
{
    json j = "bar";
    j.erase(j.cbegin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, Boolean)
{
    json j = false;
    j.erase(j.begin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, BooleanConst)
{
    json j = true;
    j.erase(j.cbegin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, Integer)
{
    json j = 17;
    j.erase(j.begin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, IntegerConst)
{
    json j = 17;
    j.erase(j.cbegin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, Unsigned)
{
    json j = 17u;
    j.erase(j.begin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, UnsignedConst)
{
    json j = 17u;
    j.erase(j.cbegin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, Float)
{
    json j = 23.42;
    j.erase(j.begin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneValidIteratorRemoveTest, FloatConst)
{
    json j = 23.42;
    j.erase(j.cbegin());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, String)
{
    json j = "foo";
    EXPECT_THROW_MSG(j.erase(j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, StringConst)
{
    json j = "bar";
    EXPECT_THROW_MSG(j.erase(j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, Boolean)
{
    json j = false;
    EXPECT_THROW_MSG(j.erase(j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, BooleanConst)
{
    json j = true;
    EXPECT_THROW_MSG(j.erase(j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, Integer)
{
    json j = 17;
    EXPECT_THROW_MSG(j.erase(j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, IntegerConst)
{
    json j = 17;
    EXPECT_THROW_MSG(j.erase(j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, Unsigned)
{
    json j = 17u;
    EXPECT_THROW_MSG(j.erase(j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, UnsignedConst)
{
    json j = 17u;
    EXPECT_THROW_MSG(j.erase(j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, Float)
{
    json j = 23.42;
    EXPECT_THROW_MSG(j.erase(j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayOneInvalidIteratorRemoveTest, FloatConst)
{
    json j = 23.42;
    EXPECT_THROW_MSG(j.erase(j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.205] iterator out of range");
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, Null)
{
    json j;
    EXPECT_THROW_MSG(j.erase(j.begin(), j.end()), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with null");
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, NullConst)
{
    json j;
    EXPECT_THROW_MSG(j.erase(j.cbegin(), j.cend()), json::type_error,
                     "[json.exception.type_error.307] cannot use erase() with null");
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, String)
{
    json j = "foo";
    j.erase(j.begin(), j.end());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, StringConst)
{
    json j = "bar";
    j.erase(j.cbegin(), j.cend());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, Boolean)
{
    json j = false;
    j.erase(j.begin(), j.end());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, BooleanConst)
{
    json j = true;
    j.erase(j.cbegin(), j.cend());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, Integer)
{
    json j = 17;
    j.erase(j.begin(), j.end());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, IntegerConst)
{
    json j = 17;
    j.erase(j.cbegin(), j.cend());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, Unsigned)
{
    json j = 17u;
    j.erase(j.begin(), j.end());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, UnsignedConst)
{
    json j = 17u;
    j.erase(j.cbegin(), j.cend());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, Float)
{
    json j = 23.42;
    j.erase(j.begin(), j.end());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoValidIteratorRemoveTest, FloatConst)
{
    json j = 23.42;
    j.erase(j.cbegin(), j.cend());
    EXPECT_EQ(j.type(), json::value_t::null);
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, String)
{
    json j = "foo";
    EXPECT_THROW_MSG(j.erase(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, StringConst)
{
    json j = "bar";
    EXPECT_THROW_MSG(j.erase(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, Boolean)
{
    json j = false;
    EXPECT_THROW_MSG(j.erase(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, BooleanConst)
{
    json j = true;
    EXPECT_THROW_MSG(j.erase(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, Integer)
{
    json j = 17;
    EXPECT_THROW_MSG(j.erase(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, IntegerConst)
{
    json j = 17;
    EXPECT_THROW_MSG(j.erase(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, Unsigned)
{
    json j = 17u;
    EXPECT_THROW_MSG(j.erase(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, UnsignedConst)
{
    json j = 17u;
    EXPECT_THROW_MSG(j.erase(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, Float)
{
    json j = 23.42;
    EXPECT_THROW_MSG(j.erase(j.end(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.begin(), j.begin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}

TEST(JsonElementNonArrayTwoInvalidIteratorRemoveTest, FloatConst)
{
    json j = 23.42;
    EXPECT_THROW_MSG(j.erase(j.cend(), j.cend()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
    EXPECT_THROW_MSG(j.erase(j.cbegin(), j.cbegin()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.204] iterators out of range");
}
