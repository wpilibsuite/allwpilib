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

TEST(JsonEmptyTest, Boolean)
{
    json j = true;
    json j_const(j);

    // result of empty
    {
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, String)
{
    json j = "hello world";
    json j_const(j);

    // result of empty
    {
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, ArrayEmpty)
{
    json j = json::array();
    json j_const(j);

    // result of empty
    {
        EXPECT_TRUE(j.empty());
        EXPECT_TRUE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, ArrayFilled)
{
    json j = {1, 2, 3};
    json j_const(j);

    // result of empty
    {
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, ObjectEmpty)
{
    json j = json::object();
    json j_const(j);

    // result of empty
    {
        EXPECT_TRUE(j.empty());
        EXPECT_TRUE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, ObjectFilled)
{
    json j = {{"one", 1}, {"two", 2}, {"three", 3}};
    json j_const(j);

    // result of empty
    {
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, NumberInteger)
{
    json j = 23;
    json j_const(j);

    // result of empty
    {
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, NumberUnsigned)
{
    json j = 23u;
    json j_const(j);

    // result of empty
    {
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, NumberFloat)
{
    json j = 23.42;
    json j_const(j);

    // result of empty
    {
        EXPECT_FALSE(j.empty());
        EXPECT_FALSE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonEmptyTest, Null)
{
    json j = nullptr;
    json j_const(j);

    // result of empty
    {
        EXPECT_TRUE(j.empty());
        EXPECT_TRUE(j_const.empty());
    }

    // definition of empty
    {
        EXPECT_EQ(j.empty(), (j.begin() == j.end()));
        EXPECT_EQ(j_const.empty(), (j_const.begin() == j_const.end()));
    }
}

TEST(JsonSizeTest, Boolean)
{
    json j = true;
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 1u);
        EXPECT_EQ(j_const.size(), 1u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, String)
{
    json j = "hello world";
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 1u);
        EXPECT_EQ(j_const.size(), 1u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, ArrayEmpty)
{
    json j = json::array();
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 0u);
        EXPECT_EQ(j_const.size(), 0u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, ArrayFilled)
{
    json j = {1, 2, 3};
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 3u);
        EXPECT_EQ(j_const.size(), 3u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, ObjectEmpty)
{
    json j = json::object();
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 0u);
        EXPECT_EQ(j_const.size(), 0u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, ObjectFilled)
{
    json j = {{"one", 1}, {"two", 2}, {"three", 3}};
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 3u);
        EXPECT_EQ(j_const.size(), 3u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, NumberInteger)
{
    json j = 23;
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 1u);
        EXPECT_EQ(j_const.size(), 1u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, NumberUnsigned)
{
    json j = 23u;
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 1u);
        EXPECT_EQ(j_const.size(), 1u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, NumberFloat)
{
    json j = 23.42;
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 1u);
        EXPECT_EQ(j_const.size(), 1u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonSizeTest, Null)
{
    json j = nullptr;
    json j_const(j);

    // result of size
    {
        EXPECT_EQ(j.size(), 0u);
        EXPECT_EQ(j_const.size(), 0u);
    }

    // definition of size
    {
        EXPECT_EQ(std::distance(j.begin(), j.end()), static_cast<int>(j.size()));
        EXPECT_EQ(std::distance(j_const.begin(), j_const.end()),
                  static_cast<int>(j_const.size()));
    }
}

TEST(JsonMaxSizeTest, Boolean)
{
    json j = true;
    json j_const(j);

    // result of max_size
    {
        EXPECT_EQ(j.max_size(), 1u);
        EXPECT_EQ(j_const.max_size(), 1u);
    }
}

TEST(JsonMaxSizeTest, String)
{
    json j = "hello world";
    json j_const(j);

    // result of max_size
    {
        EXPECT_EQ(j.max_size(), 1u);
        EXPECT_EQ(j_const.max_size(), 1u);
    }
}

TEST(JsonMaxSizeTest, ArrayEmpty)
{
    json j = json::array();
    json j_const(j);

    // result of max_size
    {
        EXPECT_GE(j.max_size(), j.size());
        EXPECT_GE(j_const.max_size(), j_const.size());
    }
}

TEST(JsonMaxSizeTest, ArrayFilled)
{
    json j = {1, 2, 3};
    json j_const(j);

    // result of max_size
    {
        EXPECT_GE(j.max_size(), j.size());
        EXPECT_GE(j_const.max_size(), j_const.size());
    }
}

TEST(JsonMaxSizeTest, ObjectEmpty)
{
    json j = json::object();
    json j_const(j);

    // result of max_size
    {
        EXPECT_GE(j.max_size(), j.size());
        EXPECT_GE(j_const.max_size(), j_const.size());
    }
}

TEST(JsonMaxSizeTest, ObjectFilled)
{
    json j = {{"one", 1}, {"two", 2}, {"three", 3}};
    json j_const(j);

    // result of max_size
    {
        EXPECT_GE(j.max_size(), j.size());
        EXPECT_GE(j_const.max_size(), j_const.size());
    }
}

TEST(JsonMaxSizeTest, NumberInteger)
{
    json j = 23;
    json j_const(j);

    // result of max_size
    {
        EXPECT_EQ(j.max_size(), 1u);
        EXPECT_EQ(j_const.max_size(), 1u);
    }
}

TEST(JsonMaxSizeTest, NumberUnsigned)
{
    json j = 23u;
    json j_const(j);

    // result of max_size
    {
        EXPECT_EQ(j.max_size(), 1u);
        EXPECT_EQ(j_const.max_size(), 1u);
    }
}

TEST(JsonMaxSizeTest, NumberFloat)
{
    json j = 23.42;
    json j_const(j);

    // result of max_size
    {
        EXPECT_EQ(j.max_size(), 1u);
        EXPECT_EQ(j_const.max_size(), 1u);
    }
}

TEST(JsonMaxSizeTest, Null)
{
    json j = nullptr;
    json j_const(j);

    // result of max_size
    {
        EXPECT_EQ(j.max_size(), 0u);
        EXPECT_EQ(j_const.max_size(), 0u);
    }
}
