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

#include <fstream>

TEST(CborDiscardedTest, Case)
{
    // discarded values are not serialized
    json j = json::value_t::discarded;
    const auto result = json::to_cbor(j);
    ASSERT_TRUE(result.empty());
}

TEST(CborNullTest, Case)
{
    json j = nullptr;
    std::vector<uint8_t> expected = {0xf6};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

TEST(CborBooleanTest, True)
{
    json j = true;
    std::vector<uint8_t> expected = {0xf5};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

TEST(CborBooleanTest, False)
{
    json j = false;
    std::vector<uint8_t> expected = {0xf4};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// -9223372036854775808..-4294967297
class CborSignedNeg8Test : public ::testing::TestWithParam<int64_t> {};
TEST_P(CborSignedNeg8Test, Case)
{
    // create JSON value with integer number
    json j = GetParam();

    // check type
    ASSERT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0x3b));
    uint64_t positive = static_cast<uint64_t>(-1 - GetParam());
    expected.push_back(static_cast<uint8_t>((positive >> 56) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 48) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 40) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 32) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(positive & 0xff));

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), 9u);

    // check individual bytes
    EXPECT_EQ(result[0], 0x3b);
    uint64_t restored = (static_cast<uint64_t>(static_cast<uint8_t>(result[1])) << 070) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[2])) << 060) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[3])) << 050) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[4])) << 040) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[5])) << 030) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[6])) << 020) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[7])) << 010) +
                        static_cast<uint64_t>(static_cast<uint8_t>(result[8]));
    EXPECT_EQ(restored, positive);
    EXPECT_EQ(-1 - static_cast<int64_t>(restored), GetParam());

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

static const int64_t neg8_numbers[] = {
    INT64_MIN,
    -1000000000000000000,
    -100000000000000000,
    -10000000000000000,
    -1000000000000000,
    -100000000000000,
    -10000000000000,
    -1000000000000,
    -100000000000,
    -10000000000,
    -4294967297,
};

INSTANTIATE_TEST_CASE_P(CborSignedNeg8Tests, CborSignedNeg8Test,
                        ::testing::ValuesIn(neg8_numbers), );

// -4294967296..-65537
class CborSignedNeg4Test : public ::testing::TestWithParam<int64_t> {};
TEST_P(CborSignedNeg4Test, Case)
{
    // create JSON value with integer number
    json j = GetParam();

    // check type
    ASSERT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(static_cast<uint8_t>(0x3a));
    uint32_t positive = static_cast<uint32_t>(static_cast<uint64_t>(-1 - GetParam()) & 0x00000000ffffffff);
    expected.push_back(static_cast<uint8_t>((positive >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((positive >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(positive & 0xff));

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), 5u);

    // check individual bytes
    EXPECT_EQ(result[0], 0x3a);
    uint32_t restored = (static_cast<uint32_t>(static_cast<uint8_t>(result[1])) << 030) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[2])) << 020) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[3])) << 010) +
                        static_cast<uint32_t>(static_cast<uint8_t>(result[4]));
    EXPECT_EQ(restored, positive);
    EXPECT_EQ(-1ll - restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

static const int64_t neg4_numbers[] = {
    -65537,
    -100000,
    -1000000,
    -10000000,
    -100000000,
    -1000000000,
    -4294967296,
};

INSTANTIATE_TEST_CASE_P(CborSignedNeg4Tests, CborSignedNeg4Test,
                        ::testing::ValuesIn(neg4_numbers), );

// -65536..-257
TEST(CborSignedTest, Neg2)
{
    for (int32_t i = -65536; i <= -257; ++i) {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = i;

        // check type
        ASSERT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0x39));
        uint16_t positive = static_cast<uint16_t>(-1 - i);
        expected.push_back(static_cast<uint8_t>((positive >> 8) & 0xff));
        expected.push_back(static_cast<uint8_t>(positive & 0xff));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 3u);

        // check individual bytes
        EXPECT_EQ(result[0], 0x39);
        uint16_t restored = static_cast<uint16_t>(static_cast<uint8_t>(result[1]) * 256 + static_cast<uint8_t>(result[2]));
        EXPECT_EQ(restored, positive);
        EXPECT_EQ(-1 - restored, i);

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// -9263 (int 16)
TEST(CborSignedTest, NegInt16)
{
    json j = -9263;
    std::vector<uint8_t> expected = {0x39,0x24,0x2e};

    const auto result = json::to_cbor(j);
    ASSERT_EQ(result, expected);

    int16_t restored = static_cast<int16_t>(-1 - ((result[1] << 8) + result[2]));
    EXPECT_EQ(restored, -9263);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// -256..-24
TEST(CborSignedTest, Neg1)
{
    for (auto i = -256; i < -24; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = i;

        // check type
        ASSERT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0x38));
        expected.push_back(static_cast<uint8_t>(-1 - i));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 2u);

        // check individual bytes
        EXPECT_EQ(result[0], 0x38);
        EXPECT_EQ(static_cast<int16_t>(-1 - static_cast<uint8_t>(result[1])), i);

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// -24..-1
TEST(CborSignedTest, Neg0)
{
    for (auto i = -24; i <= -1; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = i;

        // check type
        ASSERT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0x20 - 1 - static_cast<uint8_t>(i)));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 1u);

        // check individual bytes
        EXPECT_EQ(static_cast<int8_t>(0x20 - 1 - result[0]), i);

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// 0..23
TEST(CborSignedTest, Pos0)
{
    for (size_t i = 0; i <= 23; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = -1;
        j.get_ref<int64_t&>() = static_cast<int64_t>(i);

        // check type
        ASSERT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 1u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(i));

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// 24..255
TEST(CborSignedTest, Pos1)
{
    for (size_t i = 24; i <= 255; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = -1;
        j.get_ref<int64_t&>() = static_cast<int64_t>(i);

        // check type
        ASSERT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0x18));
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 2u);

        // check individual bytes
        EXPECT_EQ(result[0], 0x18);
        EXPECT_EQ(result[1], static_cast<uint8_t>(i));

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// 256..65535
TEST(CborSignedTest, Pos2)
{
    for (size_t i = 256; i <= 65535; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = -1;
        j.get_ref<int64_t&>() = static_cast<int64_t>(i);

        // check type
        ASSERT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0x19));
        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
        expected.push_back(static_cast<uint8_t>(i & 0xff));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 3u);

        // check individual bytes
        EXPECT_EQ(result[0], 0x19);
        uint16_t restored = static_cast<uint16_t>(static_cast<uint8_t>(result[1]) * 256 + static_cast<uint8_t>(result[2]));
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// 65536..4294967295
class CborSignedPos4Test : public ::testing::TestWithParam<uint32_t> {};
TEST_P(CborSignedPos4Test, Case)
{
    // create JSON value with integer number
    json j = -1;
    j.get_ref<int64_t&>() =
        static_cast<int64_t>(GetParam());

    // check type
    ASSERT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(0x1a);
    expected.push_back(static_cast<uint8_t>((GetParam() >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), 5u);

    // check individual bytes
    EXPECT_EQ(result[0], 0x1a);
    uint32_t restored = (static_cast<uint32_t>(static_cast<uint8_t>(result[1])) << 030) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[2])) << 020) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[3])) << 010) +
                        static_cast<uint32_t>(static_cast<uint8_t>(result[4]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

static const uint32_t pos4_numbers[] = {
    65536u,
    77777u,
    1048576u,
};

INSTANTIATE_TEST_CASE_P(CborSignedPos4Tests, CborSignedPos4Test,
                        ::testing::ValuesIn(pos4_numbers), );

// 4294967296..4611686018427387903
class CborSignedPos8Test : public ::testing::TestWithParam<uint64_t> {};
TEST_P(CborSignedPos8Test, Case)
{
    // create JSON value with integer number
    json j = -1;
    j.get_ref<int64_t&>() =
        static_cast<int64_t>(GetParam());

    // check type
    ASSERT_TRUE(j.is_number_integer());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(0x1b);
    expected.push_back(static_cast<uint8_t>((GetParam() >> 070) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 060) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 050) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 040) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 030) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 020) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 010) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), 9u);

    // check individual bytes
    EXPECT_EQ(result[0], 0x1b);
    uint64_t restored = (static_cast<uint64_t>(static_cast<uint8_t>(result[1])) << 070) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[2])) << 060) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[3])) << 050) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[4])) << 040) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[5])) << 030) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[6])) << 020) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[7])) << 010) +
                        static_cast<uint64_t>(static_cast<uint8_t>(result[8]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

static const uint64_t pos8_numbers[] = {
    4294967296ul,
    4611686018427387903ul
};

INSTANTIATE_TEST_CASE_P(CborSignedPos8Tests, CborSignedPos8Test,
                        ::testing::ValuesIn(pos8_numbers), );

/*
// -32768..-129 (int 16)
{
    for (int16_t i = -32768; i <= -129; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with integer number
        json j = i;

        // check type
        ASSERT_TRUE(j.is_number_integer());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(0xd1);
        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
        expected.push_back(static_cast<uint8_t>(i & 0xff));

        // compare result + size
        const auto result = json::to_msgpack(j);
        EXPECT_EQ(result == expected);
        ASSERT_EQ(result.size(), 3u);

        // check individual bytes
        EXPECT_EQ(result[0], 0xd1);
        int16_t restored = (result[1] << 8) + result[2];
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_msgpack(result), j);
    }
}
*/

// 0..23 (Integer)
TEST(CborUnsignedTest, Pos0)
{
    for (size_t i = 0; i <= 23; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with unsigned integer number
        json j = i;

        // check type
        ASSERT_TRUE(j.is_number_unsigned());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 1u);

        // check individual bytes
        EXPECT_EQ(result[0], static_cast<uint8_t>(i));

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// 24..255 (one-byte uint8_t)
TEST(CborUnsignedTest, Pos1)
{
    for (size_t i = 24; i <= 255; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with unsigned integer number
        json j = i;

        // check type
        ASSERT_TRUE(j.is_number_unsigned());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(0x18);
        expected.push_back(static_cast<uint8_t>(i));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 2u);

        // check individual bytes
        EXPECT_EQ(result[0], 0x18);
        uint8_t restored = static_cast<uint8_t>(result[1]);
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// 256..65535 (two-byte uint16_t)
TEST(CborUnsignedTest, Pos2)
{
    for (size_t i = 256; i <= 65535; ++i)
    {
        SCOPED_TRACE(i);

        // create JSON value with unsigned integer number
        json j = i;

        // check type
        ASSERT_TRUE(j.is_number_unsigned());

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(0x19);
        expected.push_back(static_cast<uint8_t>((i >> 8) & 0xff));
        expected.push_back(static_cast<uint8_t>(i & 0xff));

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), 3u);

        // check individual bytes
        EXPECT_EQ(result[0], 0x19);
        uint16_t restored = static_cast<uint16_t>(static_cast<uint8_t>(result[1]) * 256 + static_cast<uint8_t>(result[2]));
        EXPECT_EQ(restored, i);

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// 65536..4294967295 (four-byte uint32_t)
class CborUnsignedPos4Test : public ::testing::TestWithParam<uint32_t> {};
TEST_P(CborUnsignedPos4Test, Case)
{
    // create JSON value with unsigned integer number
    json j = GetParam();

    // check type
    ASSERT_TRUE(j.is_number_unsigned());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(0x1a);
    expected.push_back(static_cast<uint8_t>((GetParam() >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), 5u);

    // check individual bytes
    EXPECT_EQ(result[0], 0x1a);
    uint32_t restored = (static_cast<uint32_t>(static_cast<uint8_t>(result[1])) << 030) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[2])) << 020) +
                        (static_cast<uint32_t>(static_cast<uint8_t>(result[3])) << 010) +
                        static_cast<uint32_t>(static_cast<uint8_t>(result[4]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

INSTANTIATE_TEST_CASE_P(CborUnsignedPos4Tests, CborUnsignedPos4Test,
                        ::testing::ValuesIn(pos4_numbers), );

// 4294967296..4611686018427387903 (eight-byte uint64_t)
class CborUnsignedPos8Test : public ::testing::TestWithParam<uint64_t> {};
TEST_P(CborUnsignedPos8Test, Case)
{
    // create JSON value with integer number
    json j = GetParam();

    // check type
    ASSERT_TRUE(j.is_number_unsigned());

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(0x1b);
    expected.push_back(static_cast<uint8_t>((GetParam() >> 070) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 060) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 050) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 040) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 030) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 020) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 010) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), 9u);

    // check individual bytes
    EXPECT_EQ(result[0], 0x1b);
    uint64_t restored = (static_cast<uint64_t>(static_cast<uint8_t>(result[1])) << 070) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[2])) << 060) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[3])) << 050) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[4])) << 040) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[5])) << 030) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[6])) << 020) +
                        (static_cast<uint64_t>(static_cast<uint8_t>(result[7])) << 010) +
                        static_cast<uint64_t>(static_cast<uint8_t>(result[8]));
    EXPECT_EQ(restored, GetParam());

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

INSTANTIATE_TEST_CASE_P(CborUnsignedPos8Tests, CborUnsignedPos8Test,
                        ::testing::ValuesIn(pos8_numbers), );

// 3.1415925
TEST(CborFloatTest, Number)
{
    double v = 3.1415925;
    json j = v;
    std::vector<uint8_t> expected = {0xfb,0x40,0x09,0x21,0xfb,0x3f,0xa6,0xde,0xfc};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
    EXPECT_EQ(json::from_cbor(result), v);
}

TEST(CborFloatTest, HalfInfinity)
{
    json j = json::from_cbor(std::vector<uint8_t>({0xf9,0x7c,0x00}));
    double d = j;
    EXPECT_FALSE(std::isfinite(d));
    EXPECT_EQ(j.dump(), "null");
}

TEST(CborFloatTest, HalfNaN)
{
    json j = json::from_cbor(std::vector<uint8_t>({0xf9,0x7c,0x01}));
    double d = j;
    EXPECT_TRUE(std::isnan(d));
    EXPECT_EQ(j.dump(), "null");
}

// N = 0..23
TEST(CborStringTest, String1)
{
    for (size_t N = 0; N <= 0x17; ++N)
    {
        SCOPED_TRACE(N);

        // create JSON value with string containing of N * 'x'
        const auto s = std::string(N, 'x');
        json j = s;

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0x60 + N));
        for (size_t i = 0; i < N; ++i)
        {
            expected.push_back('x');
        }

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), N + 1);
        // check that no null byte is appended
        if (N > 0)
        {
            EXPECT_NE(result.back(), '\x00');
        }

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// N = 24..255
TEST(CborStringTest, String2)
{
    for (size_t N = 24; N <= 255; ++N)
    {
        SCOPED_TRACE(N);

        // create JSON value with string containing of N * 'x'
        const auto s = std::string(N, 'x');
        json j = s;

        // create expected byte vector
        std::vector<uint8_t> expected;
        expected.push_back(static_cast<uint8_t>(0x78));
        expected.push_back(static_cast<uint8_t>(N));
        for (size_t i = 0; i < N; ++i)
        {
            expected.push_back('x');
        }

        // compare result + size
        const auto result = json::to_cbor(j);
        EXPECT_EQ(result, expected);
        ASSERT_EQ(result.size(), N + 2);
        // check that no null byte is appended
        EXPECT_NE(result.back(), '\x00');

        // roundtrip
        EXPECT_EQ(json::from_cbor(result), j);
    }
}

// N = 256..65535
class CborString3Test : public ::testing::TestWithParam<size_t> {};
TEST_P(CborString3Test, Case)
{
    // create JSON value with string containing of N * 'x'
    const auto s = std::string(GetParam(), 'x');
    json j = s;

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(0x79);
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));
    for (size_t i = 0; i < GetParam(); ++i)
    {
        expected.push_back('x');
    }

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), GetParam() + 3);
    // check that no null byte is appended
    EXPECT_NE(result.back(), '\x00');

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

static size_t string3_lens[] = {
    256u,
    999u,
    1025u,
    3333u,
    2048u,
    65535u
};

INSTANTIATE_TEST_CASE_P(CborString3Tests, CborString3Test,
                        ::testing::ValuesIn(string3_lens), );

// N = 65536..4294967295
class CborString5Test : public ::testing::TestWithParam<size_t> {};
TEST_P(CborString5Test, Case)
{
    // create JSON value with string containing of N * 'x'
    const auto s = std::string(GetParam(), 'x');
    json j = s;

    // create expected byte vector
    std::vector<uint8_t> expected;
    expected.push_back(0x7a);
    expected.push_back(static_cast<uint8_t>((GetParam() >> 24) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 16) & 0xff));
    expected.push_back(static_cast<uint8_t>((GetParam() >> 8) & 0xff));
    expected.push_back(static_cast<uint8_t>(GetParam() & 0xff));
    for (size_t i = 0; i < GetParam(); ++i)
    {
        expected.push_back('x');
    }

    // compare result + size
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);
    ASSERT_EQ(result.size(), GetParam() + 5);
    // check that no null byte is appended
    EXPECT_NE(result.back(), '\x00');

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

static size_t string5_lens[] = {
    65536u,
    77777u,
    1048576u
};

INSTANTIATE_TEST_CASE_P(CborString5Tests, CborString5Test,
                        ::testing::ValuesIn(string5_lens), );

TEST(CborArrayTest, Empty)
{
    json j = json::array();
    std::vector<uint8_t> expected = {0x80};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// [null]
TEST(CborArrayTest, Null)
{
    json j = {nullptr};
    std::vector<uint8_t> expected = {0x81,0xf6};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// [1,2,3,4,5]
TEST(CborArrayTest, Simple)
{
    json j = json::parse("[1,2,3,4,5]");
    std::vector<uint8_t> expected = {0x85,0x01,0x02,0x03,0x04,0x05};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// [[[[]]]]
TEST(CborArrayTest, NestEmpty)
{
    json j = json::parse("[[[[]]]]");
    std::vector<uint8_t> expected = {0x81,0x81,0x81,0x80};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// array with uint16_t elements
TEST(CborArrayTest, UInt16)
{
    json j(257, nullptr);
    std::vector<uint8_t> expected(j.size() + 3, 0xf6); // all null
    expected[0] = static_cast<char>(0x99); // array 16 bit
    expected[1] = 0x01; // size (0x0101), byte 0
    expected[2] = 0x01; // size (0x0101), byte 1
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// array with uint32_t elements
TEST(CborArrayTest, UInt32)
{
    json j(65793, nullptr);
    std::vector<uint8_t> expected(j.size() + 5, 0xf6); // all null
    expected[0] = static_cast<char>(0x9a); // array 32 bit
    expected[1] = 0x00; // size (0x00010101), byte 0
    expected[2] = 0x01; // size (0x00010101), byte 1
    expected[3] = 0x01; // size (0x00010101), byte 2
    expected[4] = 0x01; // size (0x00010101), byte 3
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

/*
// array with uint64_t elements
TEST(CborArrayTest, UInt64)
{
    json j(4294967296, nullptr);
    std::vector<uint8_t> expected(j.size() + 9, 0xf6); // all null
    expected[0] = 0x9b; // array 64 bit
    expected[1] = 0x00; // size (0x0000000100000000), byte 0
    expected[2] = 0x00; // size (0x0000000100000000), byte 1
    expected[3] = 0x00; // size (0x0000000100000000), byte 2
    expected[4] = 0x01; // size (0x0000000100000000), byte 3
    expected[5] = 0x00; // size (0x0000000100000000), byte 4
    expected[6] = 0x00; // size (0x0000000100000000), byte 5
    expected[7] = 0x00; // size (0x0000000100000000), byte 6
    expected[8] = 0x00; // size (0x0000000100000000), byte 7
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}
*/

TEST(CborObjectTest, Empty)
{
    json j = json::object();
    std::vector<uint8_t> expected = {0xa0};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// {"":null}
TEST(CborObjectTest, EmptyKey)
{
    json j = {{"", nullptr}};
    std::vector<uint8_t> expected = {0xa1,0x60,0xf6};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// {"a": {"b": {"c": {}}}}
TEST(CborObjectTest, NestedEmpty)
{
    json j = json::parse("{\"a\": {\"b\": {\"c\": {}}}}");
    std::vector<uint8_t> expected = {0xa1,0x61,0x61,0xa1,0x61,0x62,0xa1,0x61,0x63,0xa0};
    const auto result = json::to_cbor(j);
    EXPECT_EQ(result, expected);

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// object with uint8_t elements
TEST(CborObjectTest, UInt8)
{
    json j;
    for (auto i = 0; i < 255; ++i)
    {
        // format i to a fixed width of 5
        // each entry will need 7 bytes: 6 for string, 1 for null
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << i;
        j.emplace(ss.str(), nullptr);
    }

    const auto result = json::to_cbor(j);

    // Checking against an expected vector byte by byte is
    // difficult, because no assumption on the order of key/value
    // pairs are made. We therefore only check the prefix (type and
    // size and the overall size. The rest is then handled in the
    // roundtrip check.
    ASSERT_EQ(result.size(), 1787u); // 1 type, 1 size, 255*7 content
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xb8)); // map 8 bit
    EXPECT_EQ(result[1], static_cast<uint8_t>(0xff)); // size byte (0xff)
    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// object with uint16_t elements
TEST(CborObjectTest, UInt16)
{
    json j;
    for (auto i = 0; i < 256; ++i)
    {
        // format i to a fixed width of 5
        // each entry will need 7 bytes: 6 for string, 1 for null
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << i;
        j.emplace(ss.str(), nullptr);
    }

    const auto result = json::to_cbor(j);

    // Checking against an expected vector byte by byte is
    // difficult, because no assumption on the order of key/value
    // pairs are made. We therefore only check the prefix (type and
    // size and the overall size. The rest is then handled in the
    // roundtrip check.
    ASSERT_EQ(result.size(), 1795u); // 1 type, 2 size, 256*7 content
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xb9)); // map 16 bit
    EXPECT_EQ(result[1], 0x01); // byte 0 of size (0x0100)
    EXPECT_EQ(result[2], 0x00); // byte 1 of size (0x0100)

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// object with uint32_t elements
TEST(CborObjectTest, UInt32)
{
    json j;
    for (auto i = 0; i < 65536; ++i)
    {
        // format i to a fixed width of 5
        // each entry will need 7 bytes: 6 for string, 1 for null
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << i;
        j.emplace(ss.str(), nullptr);
    }

    const auto result = json::to_cbor(j);

    // Checking against an expected vector byte by byte is
    // difficult, because no assumption on the order of key/value
    // pairs are made. We therefore only check the prefix (type and
    // size and the overall size. The rest is then handled in the
    // roundtrip check.
    ASSERT_EQ(result.size(), 458757u); // 1 type, 4 size, 65536*7 content
    EXPECT_EQ(result[0], static_cast<uint8_t>(0xba)); // map 32 bit
    EXPECT_EQ(result[1], 0x00); // byte 0 of size (0x00010000)
    EXPECT_EQ(result[2], 0x01); // byte 1 of size (0x00010000)
    EXPECT_EQ(result[3], 0x00); // byte 2 of size (0x00010000)
    EXPECT_EQ(result[4], 0x00); // byte 3 of size (0x00010000)

    // roundtrip
    EXPECT_EQ(json::from_cbor(result), j);
}

// 0x7b (string)
TEST(CborAdditionalDeserializationTest, Case7b)
{
    std::vector<uint8_t> given{0x7b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x61};
    json j = json::from_cbor(given);
    EXPECT_EQ(j, "a");
}

// 0x9b (array)
TEST(CborAdditionalDeserializationTest, Case9b)
{
    std::vector<uint8_t> given{0x9b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf4};
    json j = json::from_cbor(given);
    EXPECT_EQ(j, json::parse("[false]"));
}

// 0xbb (map)
TEST(CborAdditionalDeserializationTest, Casebb)
{
    std::vector<uint8_t> given{0xbb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x60,0xf4};
    json j = json::from_cbor(given);
    EXPECT_EQ(j, json::parse("{\"\": false}"));
}

TEST(CborErrorTest, TooShortByteVector)
{
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x18})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x19})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x19,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 3: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1a})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1a,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 3: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1a,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 4: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1a,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 5: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 2: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 3: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 4: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 5: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 6: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b,0x00,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 7: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b,0x00,0x00,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 8: unexpected end of input");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1b,0x00,0x00,0x00,0x00,0x00,0x00,0x00})), json::parse_error,
                     "[json.exception.parse_error.110] parse error at 9: unexpected end of input");
}

TEST(CborErrorTest, UnsupportedBytesConcrete)
{
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0x1c})), json::parse_error,
                     "[json.exception.parse_error.112] parse error at 1: error reading CBOR; last byte: 0x1c");
    EXPECT_THROW_MSG(json::from_cbor(std::vector<uint8_t>({0xf8})), json::parse_error,
                     "[json.exception.parse_error.112] parse error at 1: error reading CBOR; last byte: 0xf8");
}

class CborUnsupportedBytesTest : public ::testing::TestWithParam<uint8_t> {
};
TEST_P(CborUnsupportedBytesTest, Case)
{
    EXPECT_THROW(json::from_cbor(std::vector<uint8_t>({GetParam()})), json::parse_error);
}

static const uint8_t unsupported_bytes_cases[] = {
    // ?
    0x1c, 0x1d, 0x1e, 0x1f,
    // ?
    0x3c, 0x3d, 0x3e, 0x3f,
    // byte strings
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    // byte strings
    0x58, 0x59, 0x5a, 0x5b,
    // ?
    0x5c, 0x5d, 0x5e,
    // byte string
    0x5f,
    // ?
    0x7c, 0x7d, 0x7e,
    // ?
    0x9c, 0x9d, 0x9e,
    // ?
    0xbc, 0xbd, 0xbe,
    // date/time
    0xc0, 0xc1,
    // bignum
    0xc2, 0xc3,
    // fraction
    0xc4,
    // bigfloat
    0xc5,
    // tagged item
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
    // expected conversion
    0xd5, 0xd6, 0xd7,
    // more tagged items
    0xd8, 0xd9, 0xda, 0xdb,
    // ?
    0xdc, 0xdd, 0xde, 0xdf,
    // (simple value)
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3,
    // undefined
    0xf7,
    // simple value
    0xf8,
};

INSTANTIATE_TEST_CASE_P(CborUnsupportedBytesTests, CborUnsupportedBytesTest,
                        ::testing::ValuesIn(unsupported_bytes_cases), );
#if 0
// use this testcase outside [hide] to run it with Valgrind
TEST(CborRoundtripTest, Sample)
{
    std::string filename = "test/data/json_testsuite/sample.json";

    // parse JSON file
    std::ifstream f_json(filename);
    json j1 = json::parse(f_json);

    // parse CBOR file
    std::ifstream f_cbor(filename + ".cbor", std::ios::binary);
    std::vector<uint8_t> packed((std::istreambuf_iterator<char>(f_cbor)),
                                std::istreambuf_iterator<char>());
    json j2;
    j2 = json::from_cbor(packed);

    // compare parsed JSON values
    EXPECT_EQ(j1, j2);

    // check with different start index
    packed.insert(packed.begin(), 5, 0xff);
    EXPECT_EQ(j1, json::from_cbor(packed, 5));
}

/*
The following test cases were found during a two-day session with
AFL-Fuzz. As a result, empty byte vectors and excessive lengths are
detected.
*/
class CborRegressionFuzzTest : public ::testing::TestWithParam<const char*> {};
TEST_P(CborRegressionFuzzTest, Case)
{
    try
    {
        // parse CBOR file
        std::ifstream f_cbor(GetParam(), std::ios::binary);
        std::vector<uint8_t> vec1(
            (std::istreambuf_iterator<char>(f_cbor)),
            std::istreambuf_iterator<char>());
        json j1 = json::from_cbor(vec1);

        try
        {
            // step 2: round trip
            std::string vec2 = json::to_cbor(j1);

            // parse serialization
            json j2 = json::from_cbor(vec2);

            // deserializations must match
            EXPECT_EQ(j1, j2);
        }
        catch (const json::parse_error&)
        {
            // parsing a CBOR serialization must not fail
            FAIL();
        }
    }
    catch (const json::parse_error&)
    {
        // parse errors are ok, because input may be random bytes
    }
}

static const char* fuzz_test_cases[] = {
    "test/data/cbor_regression/test01",
    "test/data/cbor_regression/test02",
    "test/data/cbor_regression/test03",
    "test/data/cbor_regression/test04",
    "test/data/cbor_regression/test05",
    "test/data/cbor_regression/test06",
    "test/data/cbor_regression/test07",
    "test/data/cbor_regression/test08",
    "test/data/cbor_regression/test09",
    "test/data/cbor_regression/test10",
    "test/data/cbor_regression/test11",
    "test/data/cbor_regression/test12",
    "test/data/cbor_regression/test13",
    "test/data/cbor_regression/test14",
    "test/data/cbor_regression/test15",
    "test/data/cbor_regression/test16",
    "test/data/cbor_regression/test17",
    "test/data/cbor_regression/test18",
    "test/data/cbor_regression/test19",
    "test/data/cbor_regression/test20",
    "test/data/cbor_regression/test21",
};

INSTANTIATE_TEST_CASE_P(CborRegressionFuzzTests, CborRegressionFuzzTest,
                        ::testing::ValuesIn(fuzz_test_cases));

class CborRegressionFlynnTest : public ::testing::TestWithParam<const char*> {};
TEST_F(CborRegressionFlynnTest, Case)
{
    // parse JSON file
    std::ifstream f_json(GetParam());
    json j1 = json::parse(f_json);

    // parse CBOR file
    std::ifstream f_cbor(filename + ".cbor", std::ios::binary);
    std::vector<uint8_t> packed(
        (std::istreambuf_iterator<char>(f_cbor)),
        std::istreambuf_iterator<char>());
    json j2;
    j2 = json::from_cbor(packed);

    // compare parsed JSON values
    EXPECT_EQ(j1, j2);
}

static const char* flynn_test_cases[] = {
    "test/data/json_nlohmann_tests/all_unicode.json",
    "test/data/json.org/1.json",
    "test/data/json.org/2.json",
    "test/data/json.org/3.json",
    "test/data/json.org/4.json",
    "test/data/json.org/5.json",
    "test/data/json_roundtrip/roundtrip01.json",
    "test/data/json_roundtrip/roundtrip02.json",
    "test/data/json_roundtrip/roundtrip03.json",
    "test/data/json_roundtrip/roundtrip04.json",
    "test/data/json_roundtrip/roundtrip05.json",
    "test/data/json_roundtrip/roundtrip06.json",
    "test/data/json_roundtrip/roundtrip07.json",
    "test/data/json_roundtrip/roundtrip08.json",
    "test/data/json_roundtrip/roundtrip09.json",
    "test/data/json_roundtrip/roundtrip10.json",
    "test/data/json_roundtrip/roundtrip11.json",
    "test/data/json_roundtrip/roundtrip12.json",
    "test/data/json_roundtrip/roundtrip13.json",
    "test/data/json_roundtrip/roundtrip14.json",
    "test/data/json_roundtrip/roundtrip15.json",
    "test/data/json_roundtrip/roundtrip16.json",
    "test/data/json_roundtrip/roundtrip17.json",
    "test/data/json_roundtrip/roundtrip18.json",
    "test/data/json_roundtrip/roundtrip19.json",
    "test/data/json_roundtrip/roundtrip20.json",
    "test/data/json_roundtrip/roundtrip21.json",
    "test/data/json_roundtrip/roundtrip22.json",
    "test/data/json_roundtrip/roundtrip23.json",
    "test/data/json_roundtrip/roundtrip24.json",
    "test/data/json_roundtrip/roundtrip25.json",
    "test/data/json_roundtrip/roundtrip26.json",
    "test/data/json_roundtrip/roundtrip27.json",
    "test/data/json_roundtrip/roundtrip28.json",
    "test/data/json_roundtrip/roundtrip29.json",
    "test/data/json_roundtrip/roundtrip30.json",
    "test/data/json_roundtrip/roundtrip31.json",
    "test/data/json_roundtrip/roundtrip32.json",
    "test/data/json_testsuite/sample.json", // kills AppVeyor
    "test/data/json_tests/pass1.json",
    "test/data/json_tests/pass2.json",
    "test/data/json_tests/pass3.json",
    "test/data/regression/floats.json",
    "test/data/regression/signed_ints.json",
    "test/data/regression/unsigned_ints.json",
    "test/data/regression/working_file.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_arraysWithSpaces.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_empty-string.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_empty.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_ending_with_newline.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_false.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_heterogeneous.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_null.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_1_and_newline.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_leading_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_several_null.json",
    "test/data/nst_json_testsuite/test_parsing/y_array_with_trailing_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_number.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_0e+1.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_0e1.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_after_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_double_close_to_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_double_huge_neg_exp.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_huge_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_int_with_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_minus_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_negative_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_negative_one.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_negative_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e_neg_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_capital_e_pos_exp.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_exponent.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_fraction_exponent.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_neg_exp.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_real_neg_overflow.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_pos_exponent.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_real_pos_overflow.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_real_underflow.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_simple_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_number_simple_real.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_too_big_neg_int.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_too_big_pos_int.json",
    //"test/data/nst_json_testsuite/test_parsing/y_number_very_big_negative_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_object.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_basic.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_duplicated_key.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_duplicated_key_and_value.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_empty.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_empty_key.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_escaped_null_in_key.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_extreme_numbers.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_long_strings.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_simple.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_string_unicode.json",
    "test/data/nst_json_testsuite/test_parsing/y_object_with_newlines.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_1_2_3_bytes_UTF-8_sequences.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_UTF-16_Surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pair.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_accepted_surrogate_pairs.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_allowed_escapes.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_backslash_and_u_escaped_zero.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_backslash_doublequotes.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_comments.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_double_escape_a.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_double_escape_n.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_escaped_control_character.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_escaped_noncharacter.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_in_array.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_in_array_with_leading_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_last_surrogates_1_and_2.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_newline_uescaped.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+10FFFF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+1FFFF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_nonCharacterInUTF-8_U+FFFF.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_null_escape.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_one-byte-utf-8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_pi.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_simple_ascii.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_space.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_three-byte-utf-8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_two-byte-utf-8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_u+2028_line_sep.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_u+2029_par_sep.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_uEscape.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unescaped_char_delete.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicodeEscapedBackslash.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_2.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_U+2064_invisible_plus.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_unicode_escaped_double_quote.json",
    // "test/data/nst_json_testsuite/test_parsing/y_string_utf16.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_utf8.json",
    "test/data/nst_json_testsuite/test_parsing/y_string_with_del_character.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_false.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_int.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_negative_real.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_null.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_string.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_lonely_true.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_string_empty.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_trailing_newline.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_true_in_array.json",
    "test/data/nst_json_testsuite/test_parsing/y_structure_whitespace_array.json",
};

INSTANTIATE_TEST_CASE_P(CborRegressionFlynnTests, CborRegressionFlynnTest,
                        ::testing::ValuesIn(flynn_test_cases));

#endif
TEST(CborFirstBytesTest, Unsupported)
{
    // these bytes will fail immediately with exception parse_error.112
    std::set<uint8_t> unsupported =
    {
        //// types not supported by this library

        // byte strings
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        // byte strings
        0x58, 0x59, 0x5a, 0x5b, 0x5f,
        // date/time
        0xc0, 0xc1,
        // bignum
        0xc2, 0xc3,
        // decimal fracion
        0xc4,
        // bigfloat
        0xc5,
        // tagged item
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd,
        0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd8,
        0xd9, 0xda, 0xdb,
        // expected conversion
        0xd5, 0xd6, 0xd7,
        // simple value
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
        0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xef, 0xf0,
        0xf1, 0xf2, 0xf3,
        0xf8,
        // undefined
        0xf7,

        //// bytes not specified by CBOR

        0x1c, 0x1d, 0x1e, 0x1f,
        0x3c, 0x3d, 0x3e, 0x3f,
        0x5c, 0x5d, 0x5e,
        0x7c, 0x7d, 0x7e,
        0x9c, 0x9d, 0x9e,
        0xbc, 0xbd, 0xbe,
        0xdc, 0xdd, 0xde, 0xdf,
        0xee,
        0xfc, 0xfe, 0xfd,

        /// break cannot be the first byte

        0xff
    };

    for (auto i = 0; i < 256; ++i)
    {
        const auto byte = static_cast<uint8_t>(i);

        try
        {
            json::from_cbor(std::vector<uint8_t>(1, byte));
        }
        catch (const json::parse_error& e)
        {
            // check that parse_error.112 is only thrown if the
            // first byte is in the unsupported set
            SCOPED_TRACE(e.what());
            if (std::find(unsupported.begin(), unsupported.end(),
                          static_cast<uint8_t>(byte)) != unsupported.end())
            {
                EXPECT_EQ(e.id, 112);
            }
            else
            {
                EXPECT_NE(e.id, 112);
            }
        }
    }
}

// examples from RFC 7049 Appendix A
namespace internal {
struct CborRoundtripTestParam {
  const char* plain;
  std::vector<uint8_t> encoded;
  bool test_encode;
};
}  // namespace internal

class CborRoundtripTest
    : public ::testing::TestWithParam<internal::CborRoundtripTestParam> {
};
TEST_P(CborRoundtripTest, Case)
{
    if (GetParam().test_encode)
    {
        EXPECT_EQ(json::to_cbor(json::parse(GetParam().plain)), GetParam().encoded);
    }
    EXPECT_EQ(json::parse(GetParam().plain), json::from_cbor(GetParam().encoded));
}

static const internal::CborRoundtripTestParam rfc7049_appendix_a_numbers[] = {
    {"0", {0x00}, true},
    {"1", {0x01}, true},
    {"10", {0x0a}, true},
    {"23", {0x17}, true},
    {"24", {0x18,0x18}, true},
    {"25", {0x18,0x19}, true},
    {"100", {0x18,0x64}, true},
    {"1000", {0x19,0x03,0xe8}, true},
    {"1000000", {0x1a,0x00,0x0f,0x42,0x40}, true},
    {"1000000000000", {0x1b,0x00,0x00,0x00,0xe8,0xd4,0xa5,0x10,0x00}, true},
    {"18446744073709551615", {0x1b,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, true},
    // positive bignum is not supported
    //{"18446744073709551616", {0xc2,0x49,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00", 11), true},
    //{"-18446744073709551616", {0x3b,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, true},
    // negative bignum is not supported
    //{"-18446744073709551617", {0xc3,0x49,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, true},
    {"-1", {0x20}, true},
    {"-10", {0x29}, true},
    {"-100", {0x38,0x63}, true},
    {"-1000", {0x39,0x03,0xe7}, true},
    // half-precision float
    {"0.0", {0xf9,0x00,0x00}, false},
    // half-precision float
    {"-0.0", {0xf9,0x80,0x00}, false},
    // half-precision float
    {"1.0", {0xf9,0x3c,0x00}, false},
    {"1.1", {0xfb,0x3f,0xf1,0x99,0x99,0x99,0x99,0x99,0x9a}, true},
    // half-precision float
    {"1.5", {0xf9,0x3e,0x00}, false},
    // half-precision float
    {"65504.0", {0xf9,0x7b,0xff}, false},
    {"100000.0", {0xfa,0x47,0xc3,0x50,0x00}, false},
    {"3.4028234663852886e+38", {0xfa,0x7f,0x7f,0xff,0xff}, false},
    {"1.0e+300", {0xfb,0x7e,0x37,0xe4,0x3c,0x88,0x00,0x75,0x9c}, true},
    // half-precision float
    {"5.960464477539063e-8", {0xf9,0x00,0x01}, false},
    // half-precision float
    {"0.00006103515625", {0xf9,0x04,0x00}, false},
    // half-precision float
    {"-4.0", {0xf9,0xc4,0x00}, false},
    {"-4.1", {0xfb,0xc0,0x10,0x66,0x66,0x66,0x66,0x66,0x66}, true},
};

INSTANTIATE_TEST_CASE_P(CborRfc7049AppendixANumberTests, CborRoundtripTest,
                        ::testing::ValuesIn(rfc7049_appendix_a_numbers), );

static const internal::CborRoundtripTestParam rfc7049_appendix_a_simple_values[] = {
    {"false", {0xf4}, true},
    {"true", {0xf5}, true},
};

INSTANTIATE_TEST_CASE_P(CborRfc7049AppendixASimpleValueTests, CborRoundtripTest,
                        ::testing::ValuesIn(rfc7049_appendix_a_simple_values), );

static const internal::CborRoundtripTestParam rfc7049_appendix_a_strings[] = {
    {"\"\"", {0x60}, true},
    {"\"a\"", {0x61,0x61}, true},
    {"\"IETF\"", {0x64,0x49,0x45,0x54,0x46}, true},
    {"\"\\u00fc\"", {0x62,0xc3,0xbc}, true},
    {"\"\\u6c34\"", {0x63,0xe6,0xb0,0xb4}, true},
    {"\"\\ud800\\udd51\"", {0x64,0xf0,0x90,0x85,0x91}, true},
    // indefinite length strings
    {"\"streaming\"", {0x7f,0x65,0x73,0x74,0x72,0x65,0x61,0x64,0x6d,0x69,0x6e,0x67,0xff}, false},
};

INSTANTIATE_TEST_CASE_P(CborRfc7049AppendixAStringTests, CborRoundtripTest,
                        ::testing::ValuesIn(rfc7049_appendix_a_strings), );

static const internal::CborRoundtripTestParam rfc7049_appendix_a_arrays[] = {
    {"[]", {0x80}, true},
    {"[1, 2, 3]", {0x83,0x01,0x02,0x03}, true},
    {"[1, [2, 3], [4, 5]]", {0x83,0x01,0x82,0x02,0x03,0x82,0x04,0x05}, true},
    {"[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]", {0x98,0x19,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x18,0x18,0x19}, true},
    // indefinite length arrays
    {"[]", {0x9f,0xff}, false},
    {"[1, [2, 3], [4, 5]] ", {0x9f,0x01,0x82,0x02,0x03,0x9f,0x04,0x05,0xff,0xff}, false},
    {"[1, [2, 3], [4, 5]]", {0x9f,0x01,0x82,0x02,0x03,0x82,0x04,0x05,0xff}, false},
    {"[1, [2, 3], [4, 5]]", {0x83,0x01,0x82,0x02,0x03,0x9f,0x04,0x05,0xff}, false},
    {"[1, [2, 3], [4, 5]]", {0x83,0x01,0x9f,0x02,0x03,0xff,0x82,0x04,0x05}, false},
    {"[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]", {0x9f,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x18,0x18,0x19,0xff}, false},
};

INSTANTIATE_TEST_CASE_P(CborRfc7049AppendixAArrayTests, CborRoundtripTest,
                        ::testing::ValuesIn(rfc7049_appendix_a_arrays), );

static const internal::CborRoundtripTestParam rfc7049_appendix_a_objects[] = {
    {"{}", {0xa0}, true},
    {"{\"a\": 1, \"b\": [2, 3]}", {0xa2,0x61,0x61,0x01,0x61,0x62,0x82,0x02,0x03}, true},
    {"[\"a\", {\"b\": \"c\"}]", {0x82,0x61,0x61,0xa1,0x61,0x62,0x61,0x63}, true},
    {"{\"a\": \"A\", \"b\": \"B\", \"c\": \"C\", \"d\": \"D\", \"e\": \"E\"}", {0xa5,0x61,0x61,0x61,0x41,0x61,0x62,0x61,0x42,0x61,0x63,0x61,0x43,0x61,0x64,0x61,0x44,0x61,0x65,0x61,0x45}, true},
    // indefinite length objects
    {"{\"a\": 1, \"b\": [2, 3]}", {0xbf,0x61,0x61,0x01,0x61,0x62,0x9f,0x02,0x03,0xff,0xff}, false},
    {"[\"a\", {\"b\": \"c\"}]", {0x82,0x61,0x61,0xbf,0x61,0x62,0x61,0x63,0xff}, false},
    {"{\"Fun\": true, \"Amt\": -2}", {0xbf,0x63,0x46,0x75,0x6e,0xf5,0x63,0x41,0x6d,0x74,0x21,0xff}, false},
};

INSTANTIATE_TEST_CASE_P(CborRfc7049AppendixAObjectTests, CborRoundtripTest,
                        ::testing::ValuesIn(rfc7049_appendix_a_objects), );
