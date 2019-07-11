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

class JsonComparisonTypesTest : public ::testing::Test {
 protected:
    std::vector<json::value_t> j_types =
    {
        json::value_t::null,
        json::value_t::boolean,
        json::value_t::number_integer,
        json::value_t::number_unsigned,
        json::value_t::number_float,
        json::value_t::object,
        json::value_t::array,
        json::value_t::string
    };
};

TEST_F(JsonComparisonTypesTest, Less)
{
    static const std::vector<std::vector<bool>> expected =
    {
        {false, true, true, true, true, true, true, true},
        {false, false, true, true, true, true, true, true},
        {false, false, false, false, false, true, true, true},
        {false, false, false, false, false, true, true, true},
        {false, false, false, false, false, true, true, true},
        {false, false, false, false, false, false, true, true},
        {false, false, false, false, false, false, false, true},
        {false, false, false, false, false, false, false, false}
    };

    for (size_t i = 0; i < j_types.size(); ++i)
    {
        SCOPED_TRACE(i);
        for (size_t j = 0; j < j_types.size(); ++j)
        {
            SCOPED_TRACE(j);
            // check precomputed values
            EXPECT_EQ(operator<(j_types[i], j_types[j]), expected[i][j]);
        }
    }
}

class JsonComparisonValuesTest : public ::testing::Test {
 protected:
    json j_values =
    {
        nullptr, nullptr,
        17, 42,
        8u, 13u,
        3.14159, 23.42,
        "foo", "bar",
        true, false,
        {1, 2, 3}, {"one", "two", "three"},
        {{"first", 1}, {"second", 2}}, {{"a", "A"}, {"b", {"B"}}}
    };
};

TEST_F(JsonComparisonValuesTest, Equal)
{
    static const std::vector<std::vector<bool>> expected =
    {
        {true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
        {true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
        {false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false},
        {false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false},
        {false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false},
        {false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false},
        {false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false},
        {false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false},
        {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true}
    };

    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        for (size_t j = 0; j < j_values.size(); ++j)
        {
            SCOPED_TRACE(j);
            // check precomputed values
            EXPECT_EQ(j_values[i] == j_values[j], expected[i][j]);
        }
    }

    // comparison with discarded elements
    json j_discarded(json::value_t::discarded);
    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        EXPECT_FALSE(j_values[i] == j_discarded);
        EXPECT_FALSE(j_discarded == j_values[i]);
        EXPECT_FALSE(j_discarded == j_discarded);
    }

    // compare with null pointer
    json j_null;
    EXPECT_TRUE(j_null == nullptr);
    EXPECT_TRUE(nullptr == j_null);
}

TEST_F(JsonComparisonValuesTest, NotEqual)
{
    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        for (size_t j = 0; j < j_values.size(); ++j)
        {
            SCOPED_TRACE(j);
            // check definition
            EXPECT_EQ(j_values[i] != j_values[j], !(j_values[i] == j_values[j]));
        }
    }

    // compare with null pointer
    json j_null;
    EXPECT_FALSE(j_null != nullptr);
    EXPECT_FALSE(nullptr != j_null);
    EXPECT_EQ(j_null != nullptr, !(j_null == nullptr));
    EXPECT_EQ(nullptr != j_null, !(nullptr == j_null));
}

TEST_F(JsonComparisonValuesTest, Less)
{
    static const std::vector<std::vector<bool>> expected =
    {
        {false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true},
        {false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true},
        {false, false, false, true, false, false, false, true, true, true, false, false, true, true, true, true},
        {false, false, false, false, false, false, false, false, true, true, false, false, true, true, true, true},
        {false, false, true, true, false, true, false, true, true, true, false, false, true, true, true, true},
        {false, false, true, true, false, false, false, true, true, true, false, false, true, true, true, true},
        {false, false, true, true, true, true, false, true, true, true, false, false, true, true, true, true},
        {false, false, false, true, false, false, false, false, true, true, false, false, true, true, true, true},
        {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false},
        {false, false, true, true, true, true, true, true, true, true, false, false, true, true, true, true},
        {false, false, true, true, true, true, true, true, true, true, true, false, true, true, true, true},
        {false, false, false, false, false, false, false, false, true, true, false, false, false, true, false, false},
        {false, false, false, false, false, false, false, false, true, true, false, false, false, false, false, false},
        {false, false, false, false, false, false, false, false, true, true, false, false, true, true, false, false},
        {false, false, false, false, false, false, false, false, true, true, false, false, true, true, true, false}
    };

    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        for (size_t j = 0; j < j_values.size(); ++j)
        {
            SCOPED_TRACE(j);
            // check precomputed values
            EXPECT_EQ(j_values[i] < j_values[j], expected[i][j]);
        }
    }

    // comparison with discarded elements
    json j_discarded(json::value_t::discarded);
    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        EXPECT_FALSE(j_values[i] < j_discarded);
        EXPECT_FALSE(j_discarded < j_values[i]);
        EXPECT_FALSE(j_discarded < j_discarded);
    }
}

TEST_F(JsonComparisonValuesTest, LessEqual)
{
    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        for (size_t j = 0; j < j_values.size(); ++j)
        {
            SCOPED_TRACE(j);
            // check definition
            EXPECT_EQ(j_values[i] <= j_values[j], !(j_values[j] < j_values[i]));
        }
    }
}

TEST_F(JsonComparisonValuesTest, Greater)
{
    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        for (size_t j = 0; j < j_values.size(); ++j)
        {
            SCOPED_TRACE(j);
            // check definition
            EXPECT_EQ(j_values[i] > j_values[j], j_values[j] < j_values[i]);
        }
    }
}

TEST_F(JsonComparisonValuesTest, GreaterEqual)
{
    for (size_t i = 0; i < j_values.size(); ++i)
    {
        SCOPED_TRACE(i);
        for (size_t j = 0; j < j_values.size(); ++j)
        {
            SCOPED_TRACE(j);
            // check definition
            EXPECT_EQ(j_values[i] >= j_values[j], !(j_values[i] < j_values[j]));
        }
    }
}
