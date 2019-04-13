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

TEST(JsonIteratorTest, Comparisons)
{
    json j_values = {nullptr, true, 42, 42u, 23.23, {{"one", 1}, {"two", 2}}, {1, 2, 3, 4, 5}, "Hello, world"};

    for (json& j : j_values)
    {
        SCOPED_TRACE(j.dump());
        auto it1 = j.begin();
        auto it2 = j.begin();
        auto it3 = j.begin();
        ++it2;
        ++it3;
        ++it3;
        auto it1_c = j.cbegin();
        auto it2_c = j.cbegin();
        auto it3_c = j.cbegin();
        ++it2_c;
        ++it3_c;
        ++it3_c;

        // comparison: equal
        {
            EXPECT_TRUE(it1 == it1);
            EXPECT_FALSE(it1 == it2);
            EXPECT_FALSE(it1 == it3);
            EXPECT_FALSE(it2 == it3);
            EXPECT_TRUE(it1_c == it1_c);
            EXPECT_FALSE(it1_c == it2_c);
            EXPECT_FALSE(it1_c == it3_c);
            EXPECT_FALSE(it2_c == it3_c);
        }

        // comparison: not equal
        {
            // check definition
            EXPECT_EQ( (it1 != it1), !(it1 == it1) );
            EXPECT_EQ( (it1 != it2), !(it1 == it2) );
            EXPECT_EQ( (it1 != it3), !(it1 == it3) );
            EXPECT_EQ( (it2 != it3), !(it2 == it3) );
            EXPECT_EQ( (it1_c != it1_c), !(it1_c == it1_c) );
            EXPECT_EQ( (it1_c != it2_c), !(it1_c == it2_c) );
            EXPECT_EQ( (it1_c != it3_c), !(it1_c == it3_c) );
            EXPECT_EQ( (it2_c != it3_c), !(it2_c == it3_c) );
        }

        // comparison: smaller
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 < it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 < it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 < it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 < it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c < it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c < it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c < it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c < it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                EXPECT_FALSE(it1 < it1);
                EXPECT_TRUE(it1 < it2);
                EXPECT_TRUE(it1 < it3);
                EXPECT_TRUE(it2 < it3);
                EXPECT_FALSE(it1_c < it1_c);
                EXPECT_TRUE(it1_c < it2_c);
                EXPECT_TRUE(it1_c < it3_c);
                EXPECT_TRUE(it2_c < it3_c);
            }
        }

        // comparison: less than or equal
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 <= it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 <= it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 <= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 <= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c <= it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c <= it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c <= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c <= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                // check definition
                EXPECT_EQ( (it1 <= it1), !(it1 < it1) );
                EXPECT_EQ( (it1 <= it2), !(it2 < it1) );
                EXPECT_EQ( (it1 <= it3), !(it3 < it1) );
                EXPECT_EQ( (it2 <= it3), !(it3 < it2) );
                EXPECT_EQ( (it1_c <= it1_c), !(it1_c < it1_c) );
                EXPECT_EQ( (it1_c <= it2_c), !(it2_c < it1_c) );
                EXPECT_EQ( (it1_c <= it3_c), !(it3_c < it1_c) );
                EXPECT_EQ( (it2_c <= it3_c), !(it3_c < it2_c) );
            }
        }

        // comparison: greater than
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 > it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 > it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 > it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 > it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c > it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c > it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c > it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c > it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                // check definition
                EXPECT_EQ( (it1 > it1), (it1 < it1) );
                EXPECT_EQ( (it1 > it2), (it2 < it1) );
                EXPECT_EQ( (it1 > it3), (it3 < it1) );
                EXPECT_EQ( (it2 > it3), (it3 < it2) );
                EXPECT_EQ( (it1_c > it1_c), (it1_c < it1_c) );
                EXPECT_EQ( (it1_c > it2_c), (it2_c < it1_c) );
                EXPECT_EQ( (it1_c > it3_c), (it3_c < it1_c) );
                EXPECT_EQ( (it2_c > it3_c), (it3_c < it2_c) );
            }
        }

        // comparison: greater than or equal
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 >= it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 >= it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 >= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 >= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c >= it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c >= it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c >= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c >= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                // check definition
                EXPECT_EQ( (it1 >= it1), !(it1 < it1) );
                EXPECT_EQ( (it1 >= it2), !(it1 < it2) );
                EXPECT_EQ( (it1 >= it3), !(it1 < it3) );
                EXPECT_EQ( (it2 >= it3), !(it2 < it3) );
                EXPECT_EQ( (it1_c >= it1_c), !(it1_c < it1_c) );
                EXPECT_EQ( (it1_c >= it2_c), !(it1_c < it2_c) );
                EXPECT_EQ( (it1_c >= it3_c), !(it1_c < it3_c) );
                EXPECT_EQ( (it2_c >= it3_c), !(it2_c < it3_c) );
            }
        }
    }

    // check exceptions if different objects are compared
    for (auto j : j_values)
    {
        for (auto k : j_values)
        {
            if (j != k)
            {
                EXPECT_THROW_MSG(j.begin() == k.begin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");
                EXPECT_THROW_MSG(j.cbegin() == k.cbegin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");

                EXPECT_THROW_MSG(j.begin() < k.begin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");
                EXPECT_THROW_MSG(j.cbegin() < k.cbegin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");
            }
        }
    }
}

class JsonIteratorArithmeticTest : public ::testing::Test {
 protected:
    json j_object = {{"one", 1}, {"two", 2}, {"three", 3}};
    json j_array = {1, 2, 3, 4, 5, 6};
    json j_null = nullptr;
    json j_value = 42;
};

TEST_F(JsonIteratorArithmeticTest, AddSubObject)
{
    {
        auto it = j_object.begin();
        EXPECT_THROW_MSG(it += 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.cbegin();
        EXPECT_THROW_MSG(it += 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.begin();
        EXPECT_THROW_MSG(it + 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.cbegin();
        EXPECT_THROW_MSG(it + 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.begin();
        EXPECT_THROW_MSG(1 + it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.cbegin();
        EXPECT_THROW_MSG(1 + it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.begin();
        EXPECT_THROW_MSG(it -= 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.cbegin();
        EXPECT_THROW_MSG(it -= 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.begin();
        EXPECT_THROW_MSG(it - 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.cbegin();
        EXPECT_THROW_MSG(it - 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.begin();
        EXPECT_THROW_MSG(it - it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.cbegin();
        EXPECT_THROW_MSG(it - it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
}

TEST_F(JsonIteratorArithmeticTest, AddSubArray)
{
    auto it = j_array.begin();
    it += 3;
    EXPECT_EQ((j_array.begin() + 3), it);
    EXPECT_EQ(json::iterator(3 + j_array.begin()), it);
    EXPECT_EQ((it - 3), j_array.begin());
    EXPECT_EQ((it - j_array.begin()), 3);
    EXPECT_EQ(*it, json(4));
    it -= 2;
    EXPECT_EQ(*it, json(2));
}

TEST_F(JsonIteratorArithmeticTest, AddSubArrayConst)
{
    auto it = j_array.cbegin();
    it += 3;
    EXPECT_EQ((j_array.cbegin() + 3), it);
    EXPECT_EQ(json::const_iterator(3 + j_array.cbegin()), it);
    EXPECT_EQ((it - 3), j_array.cbegin());
    EXPECT_EQ((it - j_array.cbegin()), 3);
    EXPECT_EQ(*it, json(4));
    it -= 2;
    EXPECT_EQ(*it, json(2));
}

TEST_F(JsonIteratorArithmeticTest, AddSubNull)
{
    auto it = j_null.begin();
    it += 3;
    EXPECT_EQ((j_null.begin() + 3), it);
    EXPECT_EQ(json::iterator(3 + j_null.begin()), it);
    EXPECT_EQ((it - 3), j_null.begin());
    EXPECT_EQ((it - j_null.begin()), 3);
    EXPECT_NE(it, j_null.end());
    it -= 3;
    EXPECT_EQ(it, j_null.end());
}

TEST_F(JsonIteratorArithmeticTest, AddSubNullConst)
{
    auto it = j_null.cbegin();
    it += 3;
    EXPECT_EQ((j_null.cbegin() + 3), it);
    EXPECT_EQ(json::const_iterator(3 + j_null.cbegin()), it);
    EXPECT_EQ((it - 3), j_null.cbegin());
    EXPECT_EQ((it - j_null.cbegin()), 3);
    EXPECT_NE(it, j_null.cend());
    it -= 3;
    EXPECT_EQ(it, j_null.cend());
}

TEST_F(JsonIteratorArithmeticTest, AddSubValue)
{
    auto it = j_value.begin();
    it += 3;
    EXPECT_EQ((j_value.begin() + 3), it);
    EXPECT_EQ(json::iterator(3 + j_value.begin()), it);
    EXPECT_EQ((it - 3), j_value.begin());
    EXPECT_EQ((it - j_value.begin()), 3);
    EXPECT_NE(it, j_value.end());
    it -= 3;
    EXPECT_EQ(*it, json(42));
}

TEST_F(JsonIteratorArithmeticTest, AddSubValueConst)
{
    auto it = j_value.cbegin();
    it += 3;
    EXPECT_EQ((j_value.cbegin() + 3), it);
    EXPECT_EQ(json::const_iterator(3 + j_value.cbegin()), it);
    EXPECT_EQ((it - 3), j_value.cbegin());
    EXPECT_EQ((it - j_value.cbegin()), 3);
    EXPECT_NE(it, j_value.cend());
    it -= 3;
    EXPECT_EQ(*it, json(42));
}

TEST_F(JsonIteratorArithmeticTest, SubscriptObject)
{
    auto it = j_object.begin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.208] cannot use operator[] for object iterators");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.208] cannot use operator[] for object iterators");
}

TEST_F(JsonIteratorArithmeticTest, SubscriptObjectConst)
{
    auto it = j_object.cbegin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.208] cannot use operator[] for object iterators");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.208] cannot use operator[] for object iterators");
}

TEST_F(JsonIteratorArithmeticTest, SubscriptArray)
{
    auto it = j_array.begin();
    EXPECT_EQ(it[0], json(1));
    EXPECT_EQ(it[1], json(2));
    EXPECT_EQ(it[2], json(3));
    EXPECT_EQ(it[3], json(4));
    EXPECT_EQ(it[4], json(5));
    EXPECT_EQ(it[5], json(6));
}

TEST_F(JsonIteratorArithmeticTest, SubscriptArrayConst)
{
    auto it = j_array.cbegin();
    EXPECT_EQ(it[0], json(1));
    EXPECT_EQ(it[1], json(2));
    EXPECT_EQ(it[2], json(3));
    EXPECT_EQ(it[3], json(4));
    EXPECT_EQ(it[4], json(5));
    EXPECT_EQ(it[5], json(6));
}

TEST_F(JsonIteratorArithmeticTest, SubscriptNull)
{
    auto it = j_null.begin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST_F(JsonIteratorArithmeticTest, SubscriptNullConst)
{
    auto it = j_null.cbegin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST_F(JsonIteratorArithmeticTest, SubscriptValue)
{
    auto it = j_value.begin();
    EXPECT_EQ(it[0], json(42));
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST_F(JsonIteratorArithmeticTest, SubscriptValueConst)
{
    auto it = j_value.cbegin();
    EXPECT_EQ(it[0], json(42));
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}
#if 0
TEST(JsonReverseIteratorTest, Comparisons)
{
    json j_values = {nullptr, true, 42, 42u, 23.23, {{"one", 1}, {"two", 2}}, {1, 2, 3, 4, 5}, "Hello, world"};

    for (json& j : j_values)
    {
        SCOPED_TRACE(j.dump());
        auto it1 = j.rbegin();
        auto it2 = j.rbegin();
        auto it3 = j.rbegin();
        ++it2;
        ++it3;
        ++it3;
        auto it1_c = j.crbegin();
        auto it2_c = j.crbegin();
        auto it3_c = j.crbegin();
        ++it2_c;
        ++it3_c;
        ++it3_c;

        // comparison: equal
        {
            EXPECT_TRUE(it1 == it1);
            EXPECT_FALSE(it1 == it2);
            EXPECT_FALSE(it1 == it3);
            EXPECT_FALSE(it2 == it3);
            EXPECT_TRUE(it1_c == it1_c);
            EXPECT_FALSE(it1_c == it2_c);
            EXPECT_FALSE(it1_c == it3_c);
            EXPECT_FALSE(it2_c == it3_c);
        }

        // comparison: not equal
        {
            // check definition
            EXPECT_EQ( (it1 != it1), !(it1 == it1) );
            EXPECT_EQ( (it1 != it2), !(it1 == it2) );
            EXPECT_EQ( (it1 != it3), !(it1 == it3) );
            EXPECT_EQ( (it2 != it3), !(it2 == it3) );
            EXPECT_EQ( (it1_c != it1_c), !(it1_c == it1_c) );
            EXPECT_EQ( (it1_c != it2_c), !(it1_c == it2_c) );
            EXPECT_EQ( (it1_c != it3_c), !(it1_c == it3_c) );
            EXPECT_EQ( (it2_c != it3_c), !(it2_c == it3_c) );
        }

        // comparison: smaller
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 < it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 < it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 < it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 < it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c < it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c < it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c < it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c < it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                EXPECT_FALSE(it1 < it1);
                EXPECT_TRUE(it1 < it2);
                EXPECT_TRUE(it1 < it3);
                EXPECT_TRUE(it2 < it3);
                EXPECT_FALSE(it1_c < it1_c);
                EXPECT_TRUE(it1_c < it2_c);
                EXPECT_TRUE(it1_c < it3_c);
                EXPECT_TRUE(it2_c < it3_c);
            }
        }

        // comparison: less than or equal
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 <= it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 <= it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 <= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 <= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c <= it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c <= it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c <= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c <= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                // check definition
                EXPECT_EQ( (it1 <= it1), !(it1 < it1) );
                EXPECT_EQ( (it1 <= it2), !(it2 < it1) );
                EXPECT_EQ( (it1 <= it3), !(it3 < it1) );
                EXPECT_EQ( (it2 <= it3), !(it3 < it2) );
                EXPECT_EQ( (it1_c <= it1_c), !(it1_c < it1_c) );
                EXPECT_EQ( (it1_c <= it2_c), !(it2_c < it1_c) );
                EXPECT_EQ( (it1_c <= it3_c), !(it3_c < it1_c) );
                EXPECT_EQ( (it2_c <= it3_c), !(it3_c < it2_c) );
            }
        }

        // comparison: greater than
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 > it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 > it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 > it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 > it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c > it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c > it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c > it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c > it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                // check definition
                EXPECT_EQ( (it1 > it1), (it1 < it1) );
                EXPECT_EQ( (it1 > it2), (it2 < it1) );
                EXPECT_EQ( (it1 > it3), (it3 < it1) );
                EXPECT_EQ( (it2 > it3), (it3 < it2) );
                EXPECT_EQ( (it1_c > it1_c), (it1_c < it1_c) );
                EXPECT_EQ( (it1_c > it2_c), (it2_c < it1_c) );
                EXPECT_EQ( (it1_c > it3_c), (it3_c < it1_c) );
                EXPECT_EQ( (it2_c > it3_c), (it3_c < it2_c) );
            }
        }

        // comparison: greater than or equal
        {
            if (j.type() == json::value_t::object)
            {
                EXPECT_THROW_MSG(it1 >= it1, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 >= it2, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2 >= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1 >= it3, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c >= it1_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c >= it2_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it2_c >= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
                EXPECT_THROW_MSG(it1_c >= it3_c, json::invalid_iterator,
                                 "[json.exception.invalid_iterator.213] cannot compare order of object iterators");
            }
            else
            {
                // check definition
                EXPECT_EQ( (it1 >= it1), !(it1 < it1) );
                EXPECT_EQ( (it1 >= it2), !(it1 < it2) );
                EXPECT_EQ( (it1 >= it3), !(it1 < it3) );
                EXPECT_EQ( (it2 >= it3), !(it2 < it3) );
                EXPECT_EQ( (it1_c >= it1_c), !(it1_c < it1_c) );
                EXPECT_EQ( (it1_c >= it2_c), !(it1_c < it2_c) );
                EXPECT_EQ( (it1_c >= it3_c), !(it1_c < it3_c) );
                EXPECT_EQ( (it2_c >= it3_c), !(it2_c < it3_c) );
            }
        }
    }

    // check exceptions if different objects are compared
    for (auto j : j_values)
    {
        for (auto k : j_values)
        {
            if (j != k)
            {
                EXPECT_THROW_MSG(j.rbegin() == k.rbegin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");
                EXPECT_THROW_MSG(j.crbegin() == k.crbegin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");

                EXPECT_THROW_MSG(j.rbegin() < k.rbegin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");
                EXPECT_THROW_MSG(j.crbegin() < k.crbegin(), json::invalid_iterator,
                                 "[json.exception.invalid_iterator.212] cannot compare iterators of different containers");
            }
        }
    }
}

class JsonReverseIteratorArithmeticTest : public ::testing::Test {
 protected:
    json j_object = {{"one", 1}, {"two", 2}, {"three", 3}};
    json j_array = {1, 2, 3, 4, 5, 6};
    json j_null = nullptr;
    json j_value = 42;
};

TEST_F(JsonReverseIteratorArithmeticTest, AddSubObject)
{
    {
        auto it = j_object.rbegin();
        EXPECT_THROW_MSG(it += 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.crbegin();
        EXPECT_THROW_MSG(it += 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.rbegin();
        EXPECT_THROW_MSG(it + 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.crbegin();
        EXPECT_THROW_MSG(it + 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.rbegin();
        EXPECT_THROW_MSG(1 + it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.crbegin();
        EXPECT_THROW_MSG(1 + it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.rbegin();
        EXPECT_THROW_MSG(it -= 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.crbegin();
        EXPECT_THROW_MSG(it -= 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.rbegin();
        EXPECT_THROW_MSG(it - 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.crbegin();
        EXPECT_THROW_MSG(it - 1, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.rbegin();
        EXPECT_THROW_MSG(it - it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
    {
        auto it = j_object.crbegin();
        EXPECT_THROW_MSG(it - it, json::invalid_iterator,
                         "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    }
}

TEST_F(JsonReverseIteratorArithmeticTest, AddSubArray)
{
    auto it = j_array.rbegin();
    it += 3;
    EXPECT_EQ((j_array.rbegin() + 3), it);
    EXPECT_EQ(json::reverse_iterator(3 + j_array.rbegin()), it);
    EXPECT_EQ((it - 3), j_array.rbegin());
    EXPECT_EQ((it - j_array.rbegin()), 3);
    EXPECT_EQ(*it, json(3));
    it -= 2;
    EXPECT_EQ(*it, json(5));
}

TEST_F(JsonReverseIteratorArithmeticTest, AddSubArrayConst)
{
    auto it = j_array.crbegin();
    it += 3;
    EXPECT_EQ((j_array.crbegin() + 3), it);
    EXPECT_EQ(json::const_reverse_iterator(3 + j_array.crbegin()), it);
    EXPECT_EQ((it - 3), j_array.crbegin());
    EXPECT_EQ((it - j_array.crbegin()), 3);
    EXPECT_EQ(*it, json(3));
    it -= 2;
    EXPECT_EQ(*it, json(5));
}

TEST_F(JsonReverseIteratorArithmeticTest, AddSubNull)
{
    auto it = j_null.rbegin();
    it += 3;
    EXPECT_EQ((j_null.rbegin() + 3), it);
    EXPECT_EQ(json::reverse_iterator(3 + j_null.rbegin()), it);
    EXPECT_EQ((it - 3), j_null.rbegin());
    EXPECT_EQ((it - j_null.rbegin()), 3);
    EXPECT_NE(it, j_null.rend());
    it -= 3;
    EXPECT_EQ(it, j_null.rend());
}

TEST_F(JsonReverseIteratorArithmeticTest, AddSubNullConst)
{
    auto it = j_null.crbegin();
    it += 3;
    EXPECT_EQ((j_null.crbegin() + 3), it);
    EXPECT_EQ(json::const_reverse_iterator(3 + j_null.crbegin()), it);
    EXPECT_EQ((it - 3), j_null.crbegin());
    EXPECT_EQ((it - j_null.crbegin()), 3);
    EXPECT_NE(it, j_null.crend());
    it -= 3;
    EXPECT_EQ(it, j_null.crend());
}

TEST_F(JsonReverseIteratorArithmeticTest, AddSubValue)
{
    auto it = j_value.rbegin();
    it += 3;
    EXPECT_EQ((j_value.rbegin() + 3), it);
    EXPECT_EQ(json::reverse_iterator(3 + j_value.rbegin()), it);
    EXPECT_EQ((it - 3), j_value.rbegin());
    EXPECT_EQ((it - j_value.rbegin()), 3);
    EXPECT_NE(it, j_value.rend());
    it -= 3;
    EXPECT_EQ(*it, json(42));
}

TEST_F(JsonReverseIteratorArithmeticTest, AddSubValueConst)
{
    auto it = j_value.crbegin();
    it += 3;
    EXPECT_EQ((j_value.crbegin() + 3), it);
    EXPECT_EQ(json::const_reverse_iterator(3 + j_value.crbegin()), it);
    EXPECT_EQ((it - 3), j_value.crbegin());
    EXPECT_EQ((it - j_value.crbegin()), 3);
    EXPECT_NE(it, j_value.crend());
    it -= 3;
    EXPECT_EQ(*it, json(42));
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptObject)
{
    auto it = j_object.rbegin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptObjectConst)
{
    auto it = j_object.crbegin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.209] cannot use offsets with object iterators");
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptArray)
{
    auto it = j_array.rbegin();
    EXPECT_EQ(it[0], json(6));
    EXPECT_EQ(it[1], json(5));
    EXPECT_EQ(it[2], json(4));
    EXPECT_EQ(it[3], json(3));
    EXPECT_EQ(it[4], json(2));
    EXPECT_EQ(it[5], json(1));
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptArrayConst)
{
    auto it = j_array.crbegin();
    EXPECT_EQ(it[0], json(6));
    EXPECT_EQ(it[1], json(5));
    EXPECT_EQ(it[2], json(4));
    EXPECT_EQ(it[3], json(3));
    EXPECT_EQ(it[4], json(2));
    EXPECT_EQ(it[5], json(1));
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptNull)
{
    auto it = j_null.rbegin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptNullConst)
{
    auto it = j_null.crbegin();
    EXPECT_THROW_MSG(it[0], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptValue)
{
    auto it = j_value.rbegin();
    EXPECT_EQ(it[0], json(42));
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}

TEST_F(JsonReverseIteratorArithmeticTest, SubscriptValueConst)
{
    auto it = j_value.crbegin();
    EXPECT_EQ(it[0], json(42));
    EXPECT_THROW_MSG(it[1], json::invalid_iterator,
                     "[json.exception.invalid_iterator.214] cannot get value");
}
#endif
