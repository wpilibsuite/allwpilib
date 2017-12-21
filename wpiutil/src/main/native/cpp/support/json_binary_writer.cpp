/*----------------------------------------------------------------------------*/
/* Modifications Copyright (c) FIRST 2017. All Rights Reserved.               */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++
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
#define WPI_JSON_IMPLEMENTATION
#include "support/json.h"

#include <array>
#include <clocale> // lconv, localeconv
#include <locale> // locale
#include <numeric> // accumulate

#include "llvm/raw_ostream.h"
#include "llvm/SmallString.h"
#include "llvm/StringExtras.h"

using namespace wpi;

/*!
@brief serialization to CBOR and MessagePack values
*/
class json::binary_writer
{
  public:
    /*!
    @brief create a binary writer

    @param[in] adapter  output adapter to write to
    */
    explicit binary_writer(llvm::raw_ostream& s)
        : is_little_endian(little_endianess()), o(s)
    {
    }

    /*!
    @brief[in] j  JSON value to serialize
    */
    void write_cbor(const json& j);

    /*!
    @brief[in] j  JSON value to serialize
    */
    void write_msgpack(const json& j);

    /*!
    @brief determine system byte order

    @return true iff system's byte order is little endian

    @note from http://stackoverflow.com/a/1001328/266378
    */
    static bool little_endianess() noexcept
    {
        int num = 1;
        return (*reinterpret_cast<char*>(&num) == 1);
    }

  private:
    /*!
    @brief[in] str string to serialize
    */
    void write_cbor_string(llvm::StringRef str);

    /*!
    @brief[in] str string to serialize
    */
    void write_msgpack_string(llvm::StringRef str);

    /*
    @brief write a number to output input

    @param[in] n number of type @a T
    @tparam T the type of the number

    @note This function needs to respect the system's endianess, because
          bytes in CBOR and MessagePack are stored in network order (big
          endian) and therefore need reordering on little endian systems.
    */
    template<typename T>
    void write_number(T n)
    {
        // step 1: write number to array of length T
        std::array<uint8_t, sizeof(T)> vec;
        std::memcpy(vec.data(), &n, sizeof(T));

        // step 2: write array to output (with possible reordering)
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            // reverse byte order prior to conversion if necessary
            if (is_little_endian)
            {
                o << static_cast<unsigned char>(vec[sizeof(T) - i - 1]);
            }
            else
            {
                o << static_cast<unsigned char>(vec[i]);
            }
        }
    }

  private:
    /// whether we can assume little endianess
    const bool is_little_endian = true;

    /// the output
    llvm::raw_ostream& o;
};

void json::binary_writer::write_cbor(const json& j)
{
    switch (j.type())
    {
        case value_t::null:
        {
            o << static_cast<unsigned char>(0xf6);
            break;
        }

        case value_t::boolean:
        {
            o << static_cast<unsigned char>(j.m_value.boolean ? 0xf5 : 0xf4);
            break;
        }

        case value_t::number_integer:
        {
            if (j.m_value.number_integer >= 0)
            {
                // CBOR does not differentiate between positive signed
                // integers and unsigned integers. Therefore, we used the
                // code from the value_t::number_unsigned case here.
                if (j.m_value.number_integer <= 0x17)
                {
                    write_number(static_cast<uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer <= (std::numeric_limits<uint8_t>::max)())
                {
                    o << static_cast<unsigned char>(0x18);
                    write_number(static_cast<uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer <= (std::numeric_limits<uint16_t>::max)())
                {
                    o << static_cast<unsigned char>(0x19);
                    write_number(static_cast<uint16_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer <= (std::numeric_limits<uint32_t>::max)())
                {
                    o << static_cast<unsigned char>(0x1a);
                    write_number(static_cast<uint32_t>(j.m_value.number_integer));
                }
                else
                {
                    o << static_cast<unsigned char>(0x1b);
                    write_number(static_cast<uint64_t>(j.m_value.number_integer));
                }
            }
            else
            {
                // The conversions below encode the sign in the first
                // byte, and the value is converted to a positive number.
                const auto positive_number = -1 - j.m_value.number_integer;
                if (j.m_value.number_integer >= -24)
                {
                    write_number(static_cast<uint8_t>(0x20 + positive_number));
                }
                else if (positive_number <= (std::numeric_limits<uint8_t>::max)())
                {
                    o << static_cast<unsigned char>(0x38);
                    write_number(static_cast<uint8_t>(positive_number));
                }
                else if (positive_number <= (std::numeric_limits<uint16_t>::max)())
                {
                    o << static_cast<unsigned char>(0x39);
                    write_number(static_cast<uint16_t>(positive_number));
                }
                else if (positive_number <= (std::numeric_limits<uint32_t>::max)())
                {
                    o << static_cast<unsigned char>(0x3a);
                    write_number(static_cast<uint32_t>(positive_number));
                }
                else
                {
                    o << static_cast<unsigned char>(0x3b);
                    write_number(static_cast<uint64_t>(positive_number));
                }
            }
            break;
        }

        case value_t::number_unsigned:
        {
            if (j.m_value.number_unsigned <= 0x17)
            {
                write_number(static_cast<uint8_t>(j.m_value.number_unsigned));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint8_t>::max)())
            {
                o << static_cast<unsigned char>(0x18);
                write_number(static_cast<uint8_t>(j.m_value.number_unsigned));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint16_t>::max)())
            {
                o << static_cast<unsigned char>(0x19);
                write_number(static_cast<uint16_t>(j.m_value.number_unsigned));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint32_t>::max)())
            {
                o << static_cast<unsigned char>(0x1a);
                write_number(static_cast<uint32_t>(j.m_value.number_unsigned));
            }
            else
            {
                o << static_cast<unsigned char>(0x1b);
                write_number(static_cast<uint64_t>(j.m_value.number_unsigned));
            }
            break;
        }

        case value_t::number_float:
        {
            // Double-Precision Float
            o << static_cast<unsigned char>(0xfb);
            write_number(j.m_value.number_float);
            break;
        }

        case value_t::string:
        {
            write_cbor_string(*j.m_value.string);
            break;
        }

        case value_t::array:
        {
            // step 1: write control byte and the array size
            const auto N = j.m_value.array->size();
            if (N <= 0x17)
            {
                write_number(static_cast<uint8_t>(0x80 + N));
            }
            else if (N <= 0xff)
            {
                o << static_cast<unsigned char>(0x98);
                write_number(static_cast<uint8_t>(N));
            }
            else if (N <= 0xffff)
            {
                o << static_cast<unsigned char>(0x99);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= 0xffffffff)
            {
                o << static_cast<unsigned char>(0x9a);
                write_number(static_cast<uint32_t>(N));
            }
            // LCOV_EXCL_START
            else if (N <= 0xffffffffffffffff)
            {
                o << static_cast<unsigned char>(0x9b);
                write_number(static_cast<uint64_t>(N));
            }
            // LCOV_EXCL_STOP

            // step 2: write each element
            for (const auto& el : *j.m_value.array)
            {
                write_cbor(el);
            }
            break;
        }

        case value_t::object:
        {
            // step 1: write control byte and the object size
            const auto N = j.m_value.object->size();
            if (N <= 0x17)
            {
                write_number(static_cast<uint8_t>(0xa0 + N));
            }
            else if (N <= 0xff)
            {
                o << static_cast<unsigned char>(0xb8);
                write_number(static_cast<uint8_t>(N));
            }
            else if (N <= 0xffff)
            {
                o << static_cast<unsigned char>(0xb9);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= 0xffffffff)
            {
                o << static_cast<unsigned char>(0xba);
                write_number(static_cast<uint32_t>(N));
            }
            // LCOV_EXCL_START
            else if (N <= 0xffffffffffffffff)
            {
                o << static_cast<unsigned char>(0xbb);
                write_number(static_cast<uint64_t>(N));
            }
            // LCOV_EXCL_STOP

            // step 2: write each element
            for (const auto& el : *j.m_value.object)
            {
                write_cbor_string(el.first());
                write_cbor(el.second);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

void json::binary_writer::write_cbor_string(llvm::StringRef str)
{
    // step 1: write control byte and the string length
    const auto N = str.size();
    if (N <= 0x17)
    {
        write_number(static_cast<uint8_t>(0x60 + N));
    }
    else if (N <= 0xff)
    {
        o << static_cast<unsigned char>(0x78);
        write_number(static_cast<uint8_t>(N));
    }
    else if (N <= 0xffff)
    {
        o << static_cast<unsigned char>(0x79);
        write_number(static_cast<uint16_t>(N));
    }
    else if (N <= 0xffffffff)
    {
        o << static_cast<unsigned char>(0x7a);
        write_number(static_cast<uint32_t>(N));
    }
    // LCOV_EXCL_START
    else if (N <= 0xffffffffffffffff)
    {
        o << static_cast<unsigned char>(0x7b);
        write_number(static_cast<uint64_t>(N));
    }
    // LCOV_EXCL_STOP

    // step 2: write the string
    o << str;
}

void json::binary_writer::write_msgpack(const json& j)
{
    switch (j.type())
    {
        case value_t::null:
        {
            // nil
            o << static_cast<unsigned char>(0xc0);
            break;
        }

        case value_t::boolean:
        {
            // true and false
            o << static_cast<unsigned char>(j.m_value.boolean ? 0xc3 : 0xc2);
            break;
        }

        case value_t::number_integer:
        {
            if (j.m_value.number_integer >= 0)
            {
                // MessagePack does not differentiate between positive
                // signed integers and unsigned integers. Therefore, we
                // used the code from the value_t::number_unsigned case
                // here.
                if (j.m_value.number_unsigned < 128)
                {
                    // positive fixnum
                    write_number(static_cast<uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint8_t>::max)())
                {
                    // uint 8
                    o << static_cast<unsigned char>(0xcc);
                    write_number(static_cast<uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint16_t>::max)())
                {
                    // uint 16
                    o << static_cast<unsigned char>(0xcd);
                    write_number(static_cast<uint16_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint32_t>::max)())
                {
                    // uint 32
                    o << static_cast<unsigned char>(0xce);
                    write_number(static_cast<uint32_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint64_t>::max)())
                {
                    // uint 64
                    o << static_cast<unsigned char>(0xcf);
                    write_number(static_cast<uint64_t>(j.m_value.number_integer));
                }
            }
            else
            {
                if (j.m_value.number_integer >= -32)
                {
                    // negative fixnum
                    write_number(static_cast<int8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer >= (std::numeric_limits<int8_t>::min)() && j.m_value.number_integer <= (std::numeric_limits<int8_t>::max)())
                {
                    // int 8
                    o << static_cast<unsigned char>(0xd0);
                    write_number(static_cast<int8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer >= (std::numeric_limits<int16_t>::min)() && j.m_value.number_integer <= (std::numeric_limits<int16_t>::max)())
                {
                    // int 16
                    o << static_cast<unsigned char>(0xd1);
                    write_number(static_cast<int16_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer >= (std::numeric_limits<int32_t>::min)() && j.m_value.number_integer <= (std::numeric_limits<int32_t>::max)())
                {
                    // int 32
                    o << static_cast<unsigned char>(0xd2);
                    write_number(static_cast<int32_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer >= (std::numeric_limits<int64_t>::min)() && j.m_value.number_integer <= (std::numeric_limits<int64_t>::max)())
                {
                    // int 64
                    o << static_cast<unsigned char>(0xd3);
                    write_number(static_cast<int64_t>(j.m_value.number_integer));
                }
            }
            break;
        }

        case value_t::number_unsigned:
        {
            if (j.m_value.number_unsigned < 128)
            {
                // positive fixnum
                write_number(static_cast<uint8_t>(j.m_value.number_integer));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint8_t>::max)())
            {
                // uint 8
                o << static_cast<unsigned char>(0xcc);
                write_number(static_cast<uint8_t>(j.m_value.number_integer));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint16_t>::max)())
            {
                // uint 16
                o << static_cast<unsigned char>(0xcd);
                write_number(static_cast<uint16_t>(j.m_value.number_integer));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint32_t>::max)())
            {
                // uint 32
                o << static_cast<unsigned char>(0xce);
                write_number(static_cast<uint32_t>(j.m_value.number_integer));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint64_t>::max)())
            {
                // uint 64
                o << static_cast<unsigned char>(0xcf);
                write_number(static_cast<uint64_t>(j.m_value.number_integer));
            }
            break;
        }

        case value_t::number_float:
        {
            // float 64
            o << static_cast<unsigned char>(0xcb);
            write_number(j.m_value.number_float);
            break;
        }

        case value_t::string:
        {
            write_msgpack_string(*j.m_value.string);
            break;
        }

        case value_t::array:
        {
            // step 1: write control byte and the array size
            const auto N = j.m_value.array->size();
            if (N <= 15)
            {
                // fixarray
                write_number(static_cast<uint8_t>(0x90 | N));
            }
            else if (N <= 0xffff)
            {
                // array 16
                o << static_cast<unsigned char>(0xdc);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= 0xffffffff)
            {
                // array 32
                o << static_cast<unsigned char>(0xdd);
                write_number(static_cast<uint32_t>(N));
            }

            // step 2: write each element
            for (const auto& el : *j.m_value.array)
            {
                write_msgpack(el);
            }
            break;
        }

        case value_t::object:
        {
            // step 1: write control byte and the object size
            const auto N = j.m_value.object->size();
            if (N <= 15)
            {
                // fixmap
                write_number(static_cast<uint8_t>(0x80 | (N & 0xf)));
            }
            else if (N <= 65535)
            {
                // map 16
                o << static_cast<unsigned char>(0xde);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= 4294967295)
            {
                // map 32
                o << static_cast<unsigned char>(0xdf);
                write_number(static_cast<uint32_t>(N));
            }

            // step 2: write each element
            for (const auto& el : *j.m_value.object)
            {
                write_msgpack_string(el.first());
                write_msgpack(el.second);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

void json::binary_writer::write_msgpack_string(llvm::StringRef str)
{
    // step 1: write control byte and the string length
    const auto N = str.size();
    if (N <= 31)
    {
        // fixstr
        write_number(static_cast<uint8_t>(0xa0 | N));
    }
    else if (N <= 255)
    {
        // str 8
        o << static_cast<unsigned char>(0xd9);
        write_number(static_cast<uint8_t>(N));
    }
    else if (N <= 65535)
    {
        // str 16
        o << static_cast<unsigned char>(0xda);
        write_number(static_cast<uint16_t>(N));
    }
    else if (N <= 4294967295)
    {
        // str 32
        o << static_cast<unsigned char>(0xdb);
        write_number(static_cast<uint32_t>(N));
    }

    // step 2: write the string
    o << str;
}

void json::to_cbor(llvm::raw_ostream& os, const json& j)
{
    binary_writer bw(os);
    bw.write_cbor(j);
}

llvm::StringRef json::to_cbor(const json& j, llvm::SmallVectorImpl<char> buf)
{
    llvm::raw_svector_ostream os(buf);
    binary_writer bw(os);
    bw.write_cbor(j);
    return os.str();
}

std::string json::to_cbor(const json& j)
{
    std::string s;
    llvm::raw_string_ostream os(s);
    binary_writer bw(os);
    bw.write_cbor(j);
    os.flush();
    return s;
}

void json::to_msgpack(llvm::raw_ostream& os, const json& j)
{
    binary_writer bw(os);
    bw.write_msgpack(j);
}

llvm::StringRef json::to_msgpack(const json& j, llvm::SmallVectorImpl<char> buf)
{
    llvm::raw_svector_ostream os(buf);
    binary_writer bw(os);
    bw.write_msgpack(j);
    return os.str();
}

std::string json::to_msgpack(const json& j)
{
    std::string s;
    llvm::raw_string_ostream os(s);
    binary_writer bw(os);
    bw.write_msgpack(j);
    os.flush();
    return s;
}
