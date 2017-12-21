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

class JsonIteratorWrapperObjectTest : public ::testing::Test {
 protected:
    json j = {{"A", 1}, {"B", 2}};
};

TEST_F(JsonIteratorWrapperObjectTest, Value)
{
    int counter = 1;

    for (auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperObjectTest, Reference)
{
    int counter = 1;

    for (auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));

                // change the value
                i.value() = json(11);
                EXPECT_EQ(i.value(), json(11));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));

                // change the value
                i.value() = json(22);
                EXPECT_EQ(i.value(), json(22));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);

    // check if values where changed
    EXPECT_EQ(j, json({{"A", 11}, {"B", 22}}));
}

TEST_F(JsonIteratorWrapperObjectTest, ConstValue)
{
    int counter = 1;

    for (const auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperObjectTest, ConstReference)
{
    json j = {{"A", 1}, {"B", 2}};
    int counter = 1;

    for (const auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

class JsonIteratorWrapperConstObjectTest : public ::testing::Test {
 protected:
    const json j = {{"A", 1}, {"B", 2}};
};

TEST_F(JsonIteratorWrapperConstObjectTest, Value)
{
    int counter = 1;

    for (auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperConstObjectTest, Reference)
{
    int counter = 1;

    for (auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperConstObjectTest, ConstValue)
{
    int counter = 1;

    for (const auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperConstObjectTest, ConstReference)
{
    int counter = 1;

    for (const auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "A");
                EXPECT_EQ(i.value(), json(1));
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "B");
                EXPECT_EQ(i.value(), json(2));
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

class JsonIteratorWrapperArrayTest : public ::testing::Test {
 protected:
    json j = {"A", "B"};
};

TEST_F(JsonIteratorWrapperArrayTest, Value)
{
    int counter = 1;

    for (auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperArrayTest, Reference)
{
    int counter = 1;

    for (auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");

                // change the value
                i.value() = "AA";
                EXPECT_EQ(i.value(), "AA");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");

                // change the value
                i.value() = "BB";
                EXPECT_EQ(i.value(), "BB");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);

    // check if values where changed
    EXPECT_EQ(j, json({"AA", "BB"}));
}

TEST_F(JsonIteratorWrapperArrayTest, ConstValue)
{
    int counter = 1;

    for (const auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperArrayTest, ConstReference)
{
    int counter = 1;

    for (const auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

class JsonIteratorWrapperConstArrayTest : public ::testing::Test {
 protected:
    const json j = {"A", "B"};
};

TEST_F(JsonIteratorWrapperConstArrayTest, Value)
{
    int counter = 1;

    for (auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperConstArrayTest, Reference)
{
    int counter = 1;

    for (auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperConstArrayTest, ConstValue)
{
    int counter = 1;

    for (const auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

TEST_F(JsonIteratorWrapperConstArrayTest, ConstReference)
{
    int counter = 1;

    for (const auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        switch (counter++)
        {
            case 1:
            {
                EXPECT_EQ(i.key(), "0");
                EXPECT_EQ(i.value(), "A");
                break;
            }

            case 2:
            {
                EXPECT_EQ(i.key(), "1");
                EXPECT_EQ(i.value(), "B");
                break;
            }

            default:
            {
                break;
            }
        }
    }

    EXPECT_EQ(counter, 3);
}

class JsonIteratorWrapperPrimitiveTest : public ::testing::Test {
 protected:
    json j = 1;
};

TEST_F(JsonIteratorWrapperPrimitiveTest, Value)
{
    int counter = 1;

    for (auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));
    }

    EXPECT_EQ(counter, 2);
}

TEST_F(JsonIteratorWrapperPrimitiveTest, Reference)
{
    int counter = 1;

    for (auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));

        // change value
        i.value() = json(2);
    }

    EXPECT_EQ(counter, 2);

    // check if value has changed
    EXPECT_EQ(j, json(2));
}

TEST_F(JsonIteratorWrapperPrimitiveTest, ConstValue)
{
    int counter = 1;

    for (const auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));
    }

    EXPECT_EQ(counter, 2);
}

TEST_F(JsonIteratorWrapperPrimitiveTest, ConstReference)
{
    int counter = 1;

    for (const auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));
    }

    EXPECT_EQ(counter, 2);
}

class JsonIteratorWrapperConstPrimitiveTest : public ::testing::Test {
 protected:
    const json j = 1;
};

TEST_F(JsonIteratorWrapperConstPrimitiveTest, Value)
{
    int counter = 1;

    for (auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));
    }

    EXPECT_EQ(counter, 2);
}

TEST_F(JsonIteratorWrapperConstPrimitiveTest, Reference)
{
    int counter = 1;

    for (auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));
    }

    EXPECT_EQ(counter, 2);
}

TEST_F(JsonIteratorWrapperConstPrimitiveTest, ConstValue)
{
    int counter = 1;

    for (const auto i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));
    }

    EXPECT_EQ(counter, 2);
}

TEST_F(JsonIteratorWrapperConstPrimitiveTest, ConstReference)
{
    int counter = 1;

    for (const auto& i : json::iterator_wrapper(j))
    {
        SCOPED_TRACE(counter);
        ++counter;
        EXPECT_EQ(i.key(), "");
        EXPECT_EQ(i.value(), json(1));
    }

    EXPECT_EQ(counter, 2);
}
