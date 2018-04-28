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

#include <algorithm>

class JsonAlgorithmsTest : public ::testing::Test {
 protected:
    json j_array = {13, 29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz"};
    json j_object = {{"one", 1}, {"two", 2}};
};

// non-modifying sequence operations
TEST_F(JsonAlgorithmsTest, AllOf)
{
    EXPECT_TRUE(std::all_of(j_array.begin(), j_array.end(), [](const json & value)
    {
        return value.size() > 0;
    }));
    EXPECT_TRUE(std::all_of(j_object.begin(), j_object.end(), [](const json & value)
    {
        return value.type() == json::value_t::number_integer;
    }));
}

TEST_F(JsonAlgorithmsTest, AnyOf)
{
    EXPECT_TRUE(std::any_of(j_array.begin(), j_array.end(), [](const json & value)
    {
        return value.is_string() && value.get<std::string>() == "foo";
    }));
    EXPECT_TRUE(std::any_of(j_object.begin(), j_object.end(), [](const json & value)
    {
        return value.get<int>() > 1;
    }));
}

TEST_F(JsonAlgorithmsTest, NoneOf)
{
    EXPECT_TRUE(std::none_of(j_array.begin(), j_array.end(), [](const json & value)
    {
        return value.size() == 0;
    }));
    EXPECT_TRUE(std::none_of(j_object.begin(), j_object.end(), [](const json & value)
    {
        return value.get<int>() <= 0;
    }));
}

TEST_F(JsonAlgorithmsTest, ForEachReading)
{
    int sum = 0;

    std::for_each(j_array.cbegin(), j_array.cend(), [&sum](const json & value)
    {
        if (value.is_number())
        {
            sum += static_cast<int>(value);
        }
    });

    EXPECT_EQ(sum, 45);
}

TEST_F(JsonAlgorithmsTest, ForEachWriting)
{
    auto add17 = [](json & value)
    {
        if (value.is_array())
        {
            value.push_back(17);
        }
    };

    std::for_each(j_array.begin(), j_array.end(), add17);

    EXPECT_EQ(j_array[6], json({1, 2, 3, 17}));
}

TEST_F(JsonAlgorithmsTest, Count)
{
    EXPECT_EQ(std::count(j_array.begin(), j_array.end(), json(true)), 1);
}

TEST_F(JsonAlgorithmsTest, CountIf)
{
    auto count1 = std::count_if(j_array.begin(), j_array.end(), [](const json & value)
    {
        return (value.is_number());
    });
    EXPECT_EQ(count1, 3);
    auto count2 = std::count_if(j_array.begin(), j_array.end(), [](const json&)
    {
        return true;
    });
    EXPECT_EQ(count2, 9);
}

TEST_F(JsonAlgorithmsTest, Mismatch)
{
    json j_array2 = {13, 29, 3, {{"one", 1}, {"two", 2}, {"three", 3}}, true, false, {1, 2, 3}, "foo", "baz"};
    auto res = std::mismatch(j_array.begin(), j_array.end(), j_array2.begin());
    EXPECT_EQ(*res.first, json({{"one", 1}, {"two", 2}}));
    EXPECT_EQ(*res.second, json({{"one", 1}, {"two", 2}, {"three", 3}}));
}

TEST_F(JsonAlgorithmsTest, EqualOperatorEquals)
{
    EXPECT_TRUE(std::equal(j_array.begin(), j_array.end(), j_array.begin()));
    EXPECT_TRUE(std::equal(j_object.begin(), j_object.end(), j_object.begin()));
    EXPECT_FALSE(std::equal(j_array.begin(), j_array.end(), j_object.begin()));
}

TEST_F(JsonAlgorithmsTest, EqualUserComparison)
{
    // compare objects only by size of its elements
    json j_array2 = {13, 29, 3, {"Hello", "World"}, true, false, {{"one", 1}, {"two", 2}, {"three", 3}}, "foo", "baz"};
    EXPECT_FALSE(std::equal(j_array.begin(), j_array.end(), j_array2.begin()));
    EXPECT_TRUE(std::equal(j_array.begin(), j_array.end(), j_array2.begin(),
                     [](const json & a, const json & b)
    {
        return (a.size() == b.size());
    }));
}

TEST_F(JsonAlgorithmsTest, Find)
{
    auto it = std::find(j_array.begin(), j_array.end(), json(false));
    EXPECT_EQ(std::distance(j_array.begin(), it), 5);
}

TEST_F(JsonAlgorithmsTest, FindIf)
{
    auto it = std::find_if(j_array.begin(), j_array.end(),
                           [](const json & value)
    {
        return value.is_boolean();
    });
    EXPECT_EQ(std::distance(j_array.begin(), it), 4);
}

TEST_F(JsonAlgorithmsTest, FindIfNot)
{
    auto it = std::find_if_not(j_array.begin(), j_array.end(),
                               [](const json & value)
    {
        return value.is_number();
    });
    EXPECT_EQ(std::distance(j_array.begin(), it), 3);
}

TEST_F(JsonAlgorithmsTest, AdjacentFind)
{
    EXPECT_EQ(std::adjacent_find(j_array.begin(), j_array.end()), j_array.end());
    auto it = std::adjacent_find(j_array.begin(), j_array.end(),
                             [](const json & v1, const json & v2)
    {
        return v1.type() == v2.type();
    });
    EXPECT_EQ(it, j_array.begin());
}

// modifying sequence operations
TEST_F(JsonAlgorithmsTest, Reverse)
{
    std::reverse(j_array.begin(), j_array.end());
    EXPECT_EQ(j_array, json({"baz", "foo", {1, 2, 3}, false, true, {{"one", 1}, {"two", 2}}, 3, 29, 13}));
}

TEST_F(JsonAlgorithmsTest, Rotate)
{
    std::rotate(j_array.begin(), j_array.begin() + 1, j_array.end());
    EXPECT_EQ(j_array, json({29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz", 13}));
}

TEST_F(JsonAlgorithmsTest, Partition)
{
    auto it = std::partition(j_array.begin(), j_array.end(), [](const json & v)
    {
        return v.is_string();
    });
    EXPECT_EQ(std::distance(j_array.begin(), it), 2);
    EXPECT_FALSE(it[2].is_string());
}

// sorting operations
TEST_F(JsonAlgorithmsTest, SortOperatorEquals)
{
    json j = {13, 29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz", nullptr};
    std::sort(j.begin(), j.end());
    EXPECT_EQ(j, json({nullptr, false, true, 3, 13, 29, {{"one", 1}, {"two", 2}}, {1, 2, 3}, "baz", "foo"}));
}

TEST_F(JsonAlgorithmsTest, SortUserComparison)
{
    json j = {3, {{"one", 1}, {"two", 2}}, {1, 2, 3}, nullptr};
    std::sort(j.begin(), j.end(), [](const json & a, const json & b)
    {
        return a.size() < b.size();
    });
    EXPECT_EQ(j, json({nullptr, 3, {{"one", 1}, {"two", 2}}, {1, 2, 3}}));
}

TEST_F(JsonAlgorithmsTest, SortObject)
{
    json j({{"one", 1}, {"two", 2}});
    EXPECT_THROW_MSG(std::sort(j.begin(), j.end()), json::invalid_iterator,
                     "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
}

TEST_F(JsonAlgorithmsTest, PartialSort)
{
    json j = {13, 29, 3, {{"one", 1}, {"two", 2}}, true, false, {1, 2, 3}, "foo", "baz", nullptr};
    std::partial_sort(j.begin(), j.begin() + 4, j.end());
    EXPECT_EQ(j, json({nullptr, false, true, 3, {{"one", 1}, {"two", 2}}, 29, {1, 2, 3}, "foo", "baz", 13}));
}

// set operations
TEST_F(JsonAlgorithmsTest, Merge)
{
    json j1 = {2, 4, 6, 8};
    json j2 = {1, 2, 3, 5, 7};
    json j3;

    std::merge(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
    EXPECT_EQ(j3, json({1, 2, 2, 3, 4, 5, 6, 7, 8}));
}

TEST_F(JsonAlgorithmsTest, SetDifference)
{
    json j1 = {1, 2, 3, 4, 5, 6, 7, 8};
    json j2 = {1, 2, 3, 5, 7};
    json j3;

    std::set_difference(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
    EXPECT_EQ(j3, json({4, 6, 8}));
}

TEST_F(JsonAlgorithmsTest, SetIntersection)
{
    json j1 = {1, 2, 3, 4, 5, 6, 7, 8};
    json j2 = {1, 2, 3, 5, 7};
    json j3;

    std::set_intersection(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
    EXPECT_EQ(j3, json({1, 2, 3, 5, 7}));
}

TEST_F(JsonAlgorithmsTest, SetUnion)
{
    json j1 = {2, 4, 6, 8};
    json j2 = {1, 2, 3, 5, 7};
    json j3;

    std::set_union(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
    EXPECT_EQ(j3, json({1, 2, 3, 4, 5, 6, 7, 8}));
}

TEST_F(JsonAlgorithmsTest, SetSymmetricDifference)
{
    json j1 = {2, 4, 6, 8};
    json j2 = {1, 2, 3, 5, 7};
    json j3;

    std::set_symmetric_difference(j1.begin(), j1.end(), j2.begin(), j2.end(), std::back_inserter(j3));
    EXPECT_EQ(j3, json({1, 3, 4, 5, 6, 7, 8}));
}

TEST_F(JsonAlgorithmsTest, HeapOperations)
{
    std::make_heap(j_array.begin(), j_array.end());
    EXPECT_TRUE(std::is_heap(j_array.begin(), j_array.end()));
    std::sort_heap(j_array.begin(), j_array.end());
    EXPECT_EQ(j_array, json({false, true, 3, 13, 29, {{"one", 1}, {"two", 2}}, {1, 2, 3}, "baz", "foo"}));
}
