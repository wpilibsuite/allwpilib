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

TEST(JsonClearTest, Boolean)
{
    json j = true;

    j.clear();
    EXPECT_EQ(j, json(json::value_t::boolean));
}

TEST(JsonClearTest, String)
{
    json j = "hello world";

    j.clear();
    EXPECT_EQ(j, json(json::value_t::string));
}

TEST(JsonClearTest, ArrayEmpty)
{
    json j = json::array();

    j.clear();
    EXPECT_TRUE(j.empty());
    EXPECT_EQ(j, json(json::value_t::array));
}

TEST(JsonClearTest, ArrayFilled)
{
    json j = {1, 2, 3};

    j.clear();
    EXPECT_TRUE(j.empty());
    EXPECT_EQ(j, json(json::value_t::array));
}

TEST(JsonClearTest, ObjectEmpty)
{
    json j = json::object();

    j.clear();
    EXPECT_TRUE(j.empty());
    EXPECT_EQ(j, json(json::value_t::object));
}

TEST(JsonClearTest, ObjectFilled)
{
    json j = {{"one", 1}, {"two", 2}, {"three", 3}};

    j.clear();
    EXPECT_TRUE(j.empty());
    EXPECT_EQ(j, json(json::value_t::object));
}

TEST(JsonClearTest, Integer)
{
    json j = 23;

    j.clear();
    EXPECT_EQ(j, json(json::value_t::number_integer));
}

TEST(JsonClearTest, Unsigned)
{
    json j = 23u;

    j.clear();
    EXPECT_EQ(j, json(json::value_t::number_integer));
}

TEST(JsonClearTest, Float)
{
    json j = 23.42;

    j.clear();
    EXPECT_EQ(j, json(json::value_t::number_float));
}

TEST(JsonClearTest, Null)
{
    json j = nullptr;

    j.clear();
    EXPECT_EQ(j, json(json::value_t::null));
}

TEST(JsonPushBackArrayTest, RRefNull)
{
    json j;
    j.push_back(1);
    j.push_back(2);
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2}));
}

TEST(JsonPushBackArrayTest, RRefArray)
{
    json j = {1, 2, 3};
    j.push_back("Hello");
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2, 3, "Hello"}));
}

TEST(JsonPushBackArrayTest, RRefOther)
{
    json j = 1;
    EXPECT_THROW_MSG(j.push_back("Hello"), json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with number");
}

TEST(JsonPushBackArrayTest, LRefNull)
{
    json j;
    json k(1);
    j.push_back(k);
    j.push_back(k);
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 1}));
}

TEST(JsonPushBackArrayTest, LRefArray)
{
    json j = {1, 2, 3};
    json k("Hello");
    j.push_back(k);
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2, 3, "Hello"}));
}

TEST(JsonPushBackArrayTest, LRefOther)
{
    json j = 1;
    json k("Hello");
    EXPECT_THROW_MSG(j.push_back(k), json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with number");
}
#if 0
TEST(JsonPushBackObjectTest, Null)
{
    json j;
    j.push_back(json::object_t::value_type({"one", 1}));
    j.push_back(json::object_t::value_type({"two", 2}));
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j.size(), 2u);
    EXPECT_EQ(j["one"], json(1));
    EXPECT_EQ(j["two"], json(2));
}

TEST(JsonPushBackObjectTest, Object)
{
    json j(json::value_t::object);
    j.push_back(json::object_t::value_type({"one", 1}));
    j.push_back(json::object_t::value_type({"two", 2}));
    EXPECT_EQ(j.size(), 2u);
    EXPECT_EQ(j["one"], json(1));
    EXPECT_EQ(j["two"], json(2));
}

TEST(JsonPushBackObjectTest, Other)
{
    json j = 1;
    json k("Hello");
    EXPECT_THROW_MSG(j.push_back(json::object_t::value_type({"one", 1})), json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with number");
}
#endif
TEST(JsonPushBackInitListTest, Null)
{
    json j;
    j.push_back({"foo", "bar"});
    EXPECT_EQ(j, json::array({{"foo", "bar"}}));

    json k;
    k.push_back({1, 2, 3});
    EXPECT_EQ(k, json::array({{1, 2, 3}}));
}

TEST(JsonPushBackInitListTest, Array)
{
    json j = {1, 2, 3};
    j.push_back({"foo", "bar"});
    EXPECT_EQ(j, json({1, 2, 3, {"foo", "bar"}}));

    json k = {1, 2, 3};
    k.push_back({1, 2, 3});
    EXPECT_EQ(k, json({1, 2, 3, {1, 2, 3}}));
}

TEST(JsonPushBackInitListTest, Object)
{
    json j = {{"key1", 1}};
    j.push_back({"key2", "bar"});
    EXPECT_EQ(j, json({{"key1", 1}, {"key2", "bar"}}));

    json k = {{"key1", 1}};
    EXPECT_THROW_MSG(k.push_back({1, 2, 3, 4}), json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with object");
}

TEST(JsonEmplaceBackArrayTest, Null)
{
    json j;
    j.emplace_back(1);
    j.emplace_back(2);
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2}));
}

TEST(JsonEmplaceBackArrayTest, Array)
{
    json j = {1, 2, 3};
    j.emplace_back("Hello");
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2, 3, "Hello"}));
}

TEST(JsonEmplaceBackArrayTest, MultipleValues)
{
    json j;
    j.emplace_back(3, "foo");
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({{"foo", "foo", "foo"}}));
}

TEST(JsonEmplaceBackArrayTest, Other)
{
    json j = 1;
    EXPECT_THROW_MSG(j.emplace_back("Hello"), json::type_error,
                     "[json.exception.type_error.311] cannot use emplace_back() with number");
}

TEST(JsonEmplaceObjectTest, Null)
{
    // start with a null value
    json j;

    // add a new key
    auto res1 = j.emplace("foo", "bar");
    EXPECT_EQ(res1.second, true);
    EXPECT_EQ(*res1.first, "bar");

    // the null value is changed to an object
    EXPECT_EQ(j.type(), json::value_t::object);

    // add a new key
    auto res2 = j.emplace("baz", "bam");
    EXPECT_EQ(res2.second, true);
    EXPECT_EQ(*res2.first, "bam");

    // we try to insert at given key - no change
    auto res3 = j.emplace("baz", "bad");
    EXPECT_EQ(res3.second, false);
    EXPECT_EQ(*res3.first, "bam");

    // the final object
    EXPECT_EQ(j, json({{"baz", "bam"}, {"foo", "bar"}}));
}

TEST(JsonEmplaceObjectTest, Object)
{
    // start with an object
    json j = {{"foo", "bar"}};

    // add a new key
    auto res1 = j.emplace("baz", "bam");
    EXPECT_EQ(res1.second, true);
    EXPECT_EQ(*res1.first, "bam");

    // add an existing key
    auto res2 = j.emplace("foo", "bad");
    EXPECT_EQ(res2.second, false);
    EXPECT_EQ(*res2.first, "bar");

    // check final object
    EXPECT_EQ(j, json({{"baz", "bam"}, {"foo", "bar"}}));
}

TEST(JsonEmplaceObjectTest, Other)
{
    json j = 1;
    EXPECT_THROW_MSG(j.emplace("foo", "bar"), json::type_error,
                     "[json.exception.type_error.311] cannot use emplace() with number");
}

TEST(JsonPlusEqualArrayTest, RRefNull)
{
    json j;
    j += 1;
    j += 2;
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2}));
}

TEST(JsonPlusEqualArrayTest, RRefArray)
{
    json j = {1, 2, 3};
    j += "Hello";
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2, 3, "Hello"}));
}

TEST(JsonPlusEqualArrayTest, RRefOther)
{
    json j = 1;
    EXPECT_THROW_MSG(j += "Hello", json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with number");
}

TEST(JsonPlusEqualArrayTest, LRefNull)
{
    json j;
    json k(1);
    j += k;
    j += k;
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 1}));
}

TEST(JsonPlusEqualArrayTest, LRefArray)
{
    json j = {1, 2, 3};
    json k("Hello");
    j += k;
    EXPECT_EQ(j.type(), json::value_t::array);
    EXPECT_EQ(j, json({1, 2, 3, "Hello"}));
}

TEST(JsonPlusEqualArrayTest, LRefOther)
{
    json j = 1;
    json k("Hello");
    EXPECT_THROW_MSG(j += k, json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with number");
}
#if 0
TEST(JsonPlusEqualObjectTest, Null)
{
    json j;
    j += json::object_t::value_type({"one", 1});
    j += json::object_t::value_type({"two", 2});
    EXPECT_EQ(j.type(), json::value_t::object);
    EXPECT_EQ(j.size(), 2u);
    EXPECT_EQ(j["one"], json(1));
    EXPECT_EQ(j["two"], json(2));
}

TEST(JsonPlusEqualObjectTest, Object)
{
    json j(json::value_t::object);
    j += json::object_t::value_type({"one", 1});
    j += json::object_t::value_type({"two", 2});
    EXPECT_EQ(j.size(), 2u);
    EXPECT_EQ(j["one"], json(1));
    EXPECT_EQ(j["two"], json(2));
}

TEST(JsonPlusEqualObjectTest, Other)
{
    json j = 1;
    json k("Hello");
    EXPECT_THROW_MSG(j += json::object_t::value_type({"one", 1}), json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with number");
}
#endif
TEST(JsonPlusEqualInitListTest, Null)
{
    json j;
    j += {"foo", "bar"};
    EXPECT_EQ(j, json::array({{"foo", "bar"}}));

    json k;
    k += {1, 2, 3};
    EXPECT_EQ(k, json::array({{1, 2, 3}}));
}

TEST(JsonPlusEqualInitListTest, Array)
{
    json j = {1, 2, 3};
    j += {"foo", "bar"};
    EXPECT_EQ(j, json({1, 2, 3, {"foo", "bar"}}));

    json k = {1, 2, 3};
    k += {1, 2, 3};
    EXPECT_EQ(k, json({1, 2, 3, {1, 2, 3}}));
}

TEST(JsonPlusEqualInitListTest, Object)
{
    json j = {{"key1", 1}};
    j += {"key2", "bar"};
    EXPECT_EQ(j, json({{"key1", 1}, {"key2", "bar"}}));

    json k = {{"key1", 1}};
    EXPECT_THROW_MSG((k += {1, 2, 3, 4}), json::type_error,
                     "[json.exception.type_error.308] cannot use push_back() with object");
}

class JsonInsertTest : public ::testing::Test {
 protected:
    json j_array = {1, 2, 3, 4};
    json j_value = 5;
    json j_other_array = {"first", "second"};
    json j_object1 = {{"one", "eins"}, {"two", "zwei"}};
    json j_object2 = {{"eleven", "elf"}, {"seventeen", "siebzehn"}};
};

TEST_F(JsonInsertTest, ValueBegin)
{
    auto it = j_array.insert(j_array.begin(), j_value);
    EXPECT_EQ(j_array.size(), 5u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ(j_array.begin(), it);
    EXPECT_EQ(j_array, json({5, 1, 2, 3, 4}));
}

TEST_F(JsonInsertTest, ValueMiddle)
{
    auto it = j_array.insert(j_array.begin() + 2, j_value);
    EXPECT_EQ(j_array.size(), 5u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ((it - j_array.begin()), 2);
    EXPECT_EQ(j_array, json({1, 2, 5, 3, 4}));
}

TEST_F(JsonInsertTest, ValueEnd)
{
    auto it = j_array.insert(j_array.end(), j_value);
    EXPECT_EQ(j_array.size(), 5u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ((j_array.end() - it), 1);
    EXPECT_EQ(j_array, json({1, 2, 3, 4, 5}));
}

TEST_F(JsonInsertTest, RvalueBegin)
{
    auto it = j_array.insert(j_array.begin(), 5);
    EXPECT_EQ(j_array.size(), 5u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ(j_array.begin(), it);
    EXPECT_EQ(j_array, json({5, 1, 2, 3, 4}));
}

TEST_F(JsonInsertTest, RvalueMiddle)
{
    auto it = j_array.insert(j_array.begin() + 2, 5);
    EXPECT_EQ(j_array.size(), 5u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ((it - j_array.begin()), 2);
    EXPECT_EQ(j_array, json({1, 2, 5, 3, 4}));
}

TEST_F(JsonInsertTest, RvalueEnd)
{
    auto it = j_array.insert(j_array.end(), 5);
    EXPECT_EQ(j_array.size(), 5u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ((j_array.end() - it), 1);
    EXPECT_EQ(j_array, json({1, 2, 3, 4, 5}));
}

TEST_F(JsonInsertTest, CopyBegin)
{
    auto it = j_array.insert(j_array.begin(), 3, 5);
    EXPECT_EQ(j_array.size(), 7u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ(j_array.begin(), it);
    EXPECT_EQ(j_array, json({5, 5, 5, 1, 2, 3, 4}));
}

TEST_F(JsonInsertTest, CopyMiddle)
{
    auto it = j_array.insert(j_array.begin() + 2, 3, 5);
    EXPECT_EQ(j_array.size(), 7u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ((it - j_array.begin()), 2);
    EXPECT_EQ(j_array, json({1, 2, 5, 5, 5, 3, 4}));
}

TEST_F(JsonInsertTest, CopyEnd)
{
    auto it = j_array.insert(j_array.end(), 3, 5);
    EXPECT_EQ(j_array.size(), 7u);
    EXPECT_EQ(*it, j_value);
    EXPECT_EQ((j_array.end() - it), 3);
    EXPECT_EQ(j_array, json({1, 2, 3, 4, 5, 5, 5}));
}

TEST_F(JsonInsertTest, CopyNothing)
{
    auto it = j_array.insert(j_array.end(), 0, 5);
    EXPECT_EQ(j_array.size(), 4u);
    // the returned iterator points to the first inserted element;
    // there were 4 elements, so it should point to the 5th
    EXPECT_EQ(it, j_array.begin() + 4);
    EXPECT_EQ(j_array, json({1, 2, 3, 4}));
}

TEST_F(JsonInsertTest, RangeForArrayProper)
{
    auto it = j_array.insert(j_array.end(), j_other_array.begin(), j_other_array.end());
    EXPECT_EQ(j_array.size(), 6u);
    EXPECT_EQ(*it, *j_other_array.begin());
    EXPECT_EQ((j_array.end() - it), 2);
    EXPECT_EQ(j_array, json({1, 2, 3, 4, "first", "second"}));
}

TEST_F(JsonInsertTest, RangeForArrayEmpty)
{
    auto it = j_array.insert(j_array.end(), j_other_array.begin(), j_other_array.begin());
    EXPECT_EQ(j_array.size(), 4u);
    EXPECT_EQ(it, j_array.end());
    EXPECT_EQ(j_array, json({1, 2, 3, 4}));
}

TEST_F(JsonInsertTest, RangeForArrayInvalid)
{
    json j_other_array2 = {"first", "second"};

    EXPECT_THROW_MSG(j_array.insert(j_array.end(), j_array.begin(), j_array.end()),
                    json::invalid_iterator,
                    "[json.exception.invalid_iterator.211] passed iterators may not belong to container");
    EXPECT_THROW_MSG(j_array.insert(j_array.end(), j_other_array.begin(), j_other_array2.end()),
                    json::invalid_iterator,
                    "[json.exception.invalid_iterator.210] iterators do not fit");
}

TEST_F(JsonInsertTest, RangeForObjectProper)
{
    j_object1.insert(j_object2.begin(), j_object2.end());
    EXPECT_EQ(j_object1.size(), 4u);
}

TEST_F(JsonInsertTest, RangeForObjectEmpty)
{
    j_object1.insert(j_object2.begin(), j_object2.begin());
    EXPECT_EQ(j_object1.size(), 2u);
}

TEST_F(JsonInsertTest, RangeForObjectInvalid)
{
    json j_other_array2 = {"first", "second"};

    EXPECT_THROW_MSG(j_array.insert(j_object2.begin(), j_object2.end()), json::type_error,
                     "[json.exception.type_error.309] cannot use insert() with array");
    EXPECT_THROW_MSG(j_object1.insert(j_object1.begin(), j_object2.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.210] iterators do not fit");
    EXPECT_THROW_MSG(j_object1.insert(j_array.begin(), j_array.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterators first and last must point to objects");
}

TEST_F(JsonInsertTest, InitListBegin)
{
    auto it = j_array.insert(j_array.begin(), {7, 8, 9});
    EXPECT_EQ(j_array.size(), 7u);
    EXPECT_EQ(*it, json(7));
    EXPECT_EQ(j_array.begin(), it);
    EXPECT_EQ(j_array, json({7, 8, 9, 1, 2, 3, 4}));
}

TEST_F(JsonInsertTest, InitListMiddle)
{
    auto it = j_array.insert(j_array.begin() + 2, {7, 8, 9});
    EXPECT_EQ(j_array.size(), 7u);
    EXPECT_EQ(*it, json(7));
    EXPECT_EQ((it - j_array.begin()), 2);
    EXPECT_EQ(j_array, json({1, 2, 7, 8, 9, 3, 4}));
}

TEST_F(JsonInsertTest, InitListEnd)
{
    auto it = j_array.insert(j_array.end(), {7, 8, 9});
    EXPECT_EQ(j_array.size(), 7u);
    EXPECT_EQ(*it, json(7));
    EXPECT_EQ((j_array.end() - it), 3);
    EXPECT_EQ(j_array, json({1, 2, 3, 4, 7, 8, 9}));
}

TEST_F(JsonInsertTest, InvalidIterator)
{
    // pass iterator to a different array
    json j_another_array = {1, 2};
    json j_yet_another_array = {"first", "second"};
    EXPECT_THROW_MSG(j_array.insert(j_another_array.end(), 10), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(j_array.insert(j_another_array.end(), j_value), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(j_array.insert(j_another_array.end(), 10, 11), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(j_array.insert(j_another_array.end(), j_yet_another_array.begin(), j_yet_another_array.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
    EXPECT_THROW_MSG(j_array.insert(j_another_array.end(), {1, 2, 3, 4}), json::invalid_iterator,
                     "[json.exception.invalid_iterator.202] iterator does not fit current value");
}

TEST_F(JsonInsertTest, NonArray)
{
    // call insert on a non-array type
    json j_nonarray = 3;
    json j_yet_another_array = {"first", "second"};
    EXPECT_THROW_MSG(j_nonarray.insert(j_nonarray.end(), 10), json::type_error,
                     "[json.exception.type_error.309] cannot use insert() with number");
    EXPECT_THROW_MSG(j_nonarray.insert(j_nonarray.end(), j_value), json::type_error,
                     "[json.exception.type_error.309] cannot use insert() with number");
    EXPECT_THROW_MSG(j_nonarray.insert(j_nonarray.end(), 10, 11), json::type_error,
                     "[json.exception.type_error.309] cannot use insert() with number");
    EXPECT_THROW_MSG(j_nonarray.insert(j_nonarray.end(), j_yet_another_array.begin(),
                                       j_yet_another_array.end()), json::type_error,
                     "[json.exception.type_error.309] cannot use insert() with number");
    EXPECT_THROW_MSG(j_nonarray.insert(j_nonarray.end(), {1, 2, 3, 4}), json::type_error,
                     "[json.exception.type_error.309] cannot use insert() with number");
}

TEST(JsonSwapTest, JsonMember)
{
    json j("hello world");
    json k(42.23);

    j.swap(k);

    EXPECT_EQ(j, json(42.23));
    EXPECT_EQ(k, json("hello world"));
}

TEST(JsonSwapTest, JsonNonMember)
{
    json j("hello world");
    json k(42.23);

    std::swap(j, k);

    EXPECT_EQ(j, json(42.23));
    EXPECT_EQ(k, json("hello world"));
}

TEST(JsonSwapTest, ArrayT)
{
    json j = {1, 2, 3, 4};
    json::array_t a = {"foo", "bar", "baz"};

    j.swap(a);

    EXPECT_EQ(j, json({"foo", "bar", "baz"}));

    j.swap(a);

    EXPECT_EQ(j, json({1, 2, 3, 4}));
}

TEST(JsonSwapTest, NonArrayT)
{
    json j = 17;
    json::array_t a = {"foo", "bar", "baz"};

    EXPECT_THROW_MSG(j.swap(a), json::type_error,
                     "[json.exception.type_error.310] cannot use swap() with number");
}

TEST(JsonSwapTest, ObjectT)
{
    json j = {{"one", 1}, {"two", 2}};
    json::object_t o = {{"cow", "Kuh"}, {"chicken", "Huhn"}};

    j.swap(o);

    EXPECT_EQ(j, json({{"cow", "Kuh"}, {"chicken", "Huhn"}}));

    j.swap(o);

    EXPECT_EQ(j, json({{"one", 1}, {"two", 2}}));
}

TEST(JsonSwapTest, NonObjectT)
{
    json j = 17;
    json::object_t o = {{"cow", "Kuh"}, {"chicken", "Huhn"}};

    EXPECT_THROW_MSG(j.swap(o), json::type_error,
                     "[json.exception.type_error.310] cannot use swap() with number");
}

TEST(JsonSwapTest, StringT)
{
    json j = "Hello world";
    std::string s = "Hallo Welt";

    j.swap(s);

    EXPECT_EQ(j, json("Hallo Welt"));

    j.swap(s);

    EXPECT_EQ(j, json("Hello world"));
}

TEST(JsonSwapTest, NonStringT)
{
    json j = 17;
    std::string s = "Hallo Welt";

    EXPECT_THROW_MSG(j.swap(s), json::type_error,
                     "[json.exception.type_error.310] cannot use swap() with number");
}
