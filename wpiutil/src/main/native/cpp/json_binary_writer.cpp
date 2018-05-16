/*----------------------------------------------------------------------------*/
/* Modifications Copyright (c) 2017-2018 FIRST. All Rights Reserved.          */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++
|  |  |__   |  |  | | | |  version 3.1.2
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2013-2018 Niels Lohmann <http://nlohmann.me>.

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
#include "wpi/json.h"

#include "wpi/raw_ostream.h"

namespace wpi {

/*!
@brief serialization to CBOR and MessagePack values
*/
class json::binary_writer
{
    using CharType = unsigned char;

  public:
    /*!
    @brief create a binary writer

    @param[in] adapter  output adapter to write to
    */
    explicit binary_writer(raw_ostream& s) : o(s)
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
    @param[in] j  JSON value to serialize
    @param[in] use_count   whether to use '#' prefixes (optimized format)
    @param[in] use_type    whether to use '$' prefixes (optimized format)
    @param[in] add_prefix  whether prefixes need to be used for this value
    */
    void write_ubjson(const json& j, const bool use_count,
                      const bool use_type, const bool add_prefix = true);

  private:
    void write_cbor_string(StringRef str);

    void write_msgpack_string(StringRef str);

    /*
    @brief write a number to output input

    @param[in] n number of type @a NumberType
    @tparam NumberType the type of the number

    @note This function needs to respect the system's endianess, because bytes
          in CBOR, MessagePack, and UBJSON are stored in network order (big
          endian) and therefore need reordering on little endian systems.
    */
    template<typename NumberType>
    void write_number(const NumberType n);

    // UBJSON: write number (floating point)
    template<typename NumberType, typename std::enable_if<
                 std::is_floating_point<NumberType>::value, int>::type = 0>
    void write_number_with_ubjson_prefix(const NumberType n,
                                         const bool add_prefix)
    {
        if (add_prefix)
        {
            o << get_ubjson_float_prefix(n);
        }
        write_number(n);
    }

    // UBJSON: write number (unsigned integer)
    template<typename NumberType, typename std::enable_if<
                 std::is_unsigned<NumberType>::value, int>::type = 0>
    void write_number_with_ubjson_prefix(const NumberType n,
                                         const bool add_prefix);

    // UBJSON: write number (signed integer)
    template<typename NumberType, typename std::enable_if<
                 std::is_signed<NumberType>::value and
                 not std::is_floating_point<NumberType>::value, int>::type = 0>
    void write_number_with_ubjson_prefix(const NumberType n,
                                         const bool add_prefix);

    /*!
    @brief determine the type prefix of container values

    @note This function does not need to be 100% accurate when it comes to
          integer limits. In case a number exceeds the limits of int64_t,
          this will be detected by a later call to function
          write_number_with_ubjson_prefix. Therefore, we return 'L' for any
          value that does not fit the previous limits.
    */
    CharType ubjson_prefix(const json& j) const noexcept;

    static constexpr CharType get_cbor_float_prefix(float)
    {
        return static_cast<CharType>(0xFA);  // Single-Precision Float
    }

    static constexpr CharType get_cbor_float_prefix(double)
    {
        return static_cast<CharType>(0xFB);  // Double-Precision Float
    }

    static constexpr CharType get_msgpack_float_prefix(float)
    {
        return static_cast<CharType>(0xCA);  // float 32
    }

    static constexpr CharType get_msgpack_float_prefix(double)
    {
        return static_cast<CharType>(0xCB);  // float 64
    }

    static constexpr CharType get_ubjson_float_prefix(float)
    {
        return 'd';  // float 32
    }

    static constexpr CharType get_ubjson_float_prefix(double)
    {
        return 'D';  // float 64
    }

  private:
    static bool little_endianess(int num = 1) noexcept
    {
        return (*reinterpret_cast<char*>(&num) == 1);
    }

    /// whether we can assume little endianess
    const bool is_little_endian = little_endianess();

    /// the output
    raw_ostream& o;
};

void json::binary_writer::write_cbor(const json& j)
{
    switch (j.type())
    {
        case value_t::null:
        {
            o << static_cast<CharType>(0xF6);
            break;
        }

        case value_t::boolean:
        {
            o << static_cast<CharType>(j.m_value.boolean ? 0xF5 : 0xF4);
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
                    o << static_cast<CharType>(0x18);
                    write_number(static_cast<uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer <= (std::numeric_limits<uint16_t>::max)())
                {
                    o << static_cast<CharType>(0x19);
                    write_number(static_cast<uint16_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer <= (std::numeric_limits<uint32_t>::max)())
                {
                    o << static_cast<CharType>(0x1A);
                    write_number(static_cast<uint32_t>(j.m_value.number_integer));
                }
                else
                {
                    o << static_cast<CharType>(0x1B);
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
                    o << static_cast<CharType>(0x38);
                    write_number(static_cast<uint8_t>(positive_number));
                }
                else if (positive_number <= (std::numeric_limits<uint16_t>::max)())
                {
                    o << static_cast<CharType>(0x39);
                    write_number(static_cast<uint16_t>(positive_number));
                }
                else if (positive_number <= (std::numeric_limits<uint32_t>::max)())
                {
                    o << static_cast<CharType>(0x3A);
                    write_number(static_cast<uint32_t>(positive_number));
                }
                else
                {
                    o << static_cast<CharType>(0x3B);
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
                o << static_cast<CharType>(0x18);
                write_number(static_cast<uint8_t>(j.m_value.number_unsigned));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint16_t>::max)())
            {
                o << static_cast<CharType>(0x19);
                write_number(static_cast<uint16_t>(j.m_value.number_unsigned));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint32_t>::max)())
            {
                o << static_cast<CharType>(0x1A);
                write_number(static_cast<uint32_t>(j.m_value.number_unsigned));
            }
            else
            {
                o << static_cast<CharType>(0x1B);
                write_number(static_cast<uint64_t>(j.m_value.number_unsigned));
            }
            break;
        }

        case value_t::number_float:
        {
            o << get_cbor_float_prefix(j.m_value.number_float);
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
            else if (N <= (std::numeric_limits<uint8_t>::max)())
            {
                o << static_cast<CharType>(0x98);
                write_number(static_cast<uint8_t>(N));
            }
            else if (N <= (std::numeric_limits<uint16_t>::max)())
            {
                o << static_cast<CharType>(0x99);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= (std::numeric_limits<uint32_t>::max)())
            {
                o << static_cast<CharType>(0x9A);
                write_number(static_cast<uint32_t>(N));
            }
            // LCOV_EXCL_START
            else if (N <= (std::numeric_limits<uint64_t>::max)())
            {
                o << static_cast<CharType>(0x9B);
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
                write_number(static_cast<uint8_t>(0xA0 + N));
            }
            else if (N <= (std::numeric_limits<uint8_t>::max)())
            {
                o << static_cast<CharType>(0xB8);
                write_number(static_cast<uint8_t>(N));
            }
            else if (N <= (std::numeric_limits<uint16_t>::max)())
            {
                o << static_cast<CharType>(0xB9);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= (std::numeric_limits<uint32_t>::max)())
            {
                o << static_cast<CharType>(0xBA);
                write_number(static_cast<uint32_t>(N));
            }
            // LCOV_EXCL_START
            else /*if (N <= (std::numeric_limits<uint64_t>::max)())*/
            {
                o << static_cast<CharType>(0xBB);
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
            break;
    }
}

void json::binary_writer::write_msgpack(const json& j)
{
    switch (j.type())
    {
        case value_t::null: // nil
        {
            o << static_cast<CharType>(0xC0);
            break;
        }

        case value_t::boolean: // true and false
        {
            o << static_cast<CharType>(j.m_value.boolean ? 0xC3 : 0xC2);
            break;
        }

        case value_t::number_integer:
        {
            if (j.m_value.number_integer >= 0)
            {
                // MessagePack does not differentiate between positive
                // signed integers and unsigned integers. Therefore, we used
                // the code from the value_t::number_unsigned case here.
                if (j.m_value.number_unsigned < 128)
                {
                    // positive fixnum
                    write_number(static_cast<uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint8_t>::max)())
                {
                    // uint 8
                    o << static_cast<CharType>(0xCC);
                    write_number(static_cast<uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint16_t>::max)())
                {
                    // uint 16
                    o << static_cast<CharType>(0xCD);
                    write_number(static_cast<uint16_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint32_t>::max)())
                {
                    // uint 32
                    o << static_cast<CharType>(0xCE);
                    write_number(static_cast<uint32_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<uint64_t>::max)())
                {
                    // uint 64
                    o << static_cast<CharType>(0xCF);
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
                else if (j.m_value.number_integer >= (std::numeric_limits<int8_t>::min)() and
                         j.m_value.number_integer <= (std::numeric_limits<int8_t>::max)())
                {
                    // int 8
                    o << static_cast<CharType>(0xD0);
                    write_number(static_cast<int8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer >= (std::numeric_limits<int16_t>::min)() and
                         j.m_value.number_integer <= (std::numeric_limits<int16_t>::max)())
                {
                    // int 16
                    o << static_cast<CharType>(0xD1);
                    write_number(static_cast<int16_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer >= (std::numeric_limits<int32_t>::min)() and
                         j.m_value.number_integer <= (std::numeric_limits<int32_t>::max)())
                {
                    // int 32
                    o << static_cast<CharType>(0xD2);
                    write_number(static_cast<int32_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_integer >= (std::numeric_limits<int64_t>::min)() and
                         j.m_value.number_integer <= (std::numeric_limits<int64_t>::max)())
                {
                    // int 64
                    o << static_cast<CharType>(0xD3);
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
                o << static_cast<CharType>(0xCC);
                write_number(static_cast<uint8_t>(j.m_value.number_integer));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint16_t>::max)())
            {
                // uint 16
                o << static_cast<CharType>(0xCD);
                write_number(static_cast<uint16_t>(j.m_value.number_integer));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint32_t>::max)())
            {
                // uint 32
                o << static_cast<CharType>(0xCE);
                write_number(static_cast<uint32_t>(j.m_value.number_integer));
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint64_t>::max)())
            {
                // uint 64
                o << static_cast<CharType>(0xCF);
                write_number(static_cast<uint64_t>(j.m_value.number_integer));
            }
            break;
        }

        case value_t::number_float:
        {
            o << get_msgpack_float_prefix(j.m_value.number_float);
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
            else if (N <= (std::numeric_limits<uint16_t>::max)())
            {
                // array 16
                o << static_cast<CharType>(0xDC);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= (std::numeric_limits<uint32_t>::max)())
            {
                // array 32
                o << static_cast<CharType>(0xDD);
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
                write_number(static_cast<uint8_t>(0x80 | (N & 0xF)));
            }
            else if (N <= (std::numeric_limits<uint16_t>::max)())
            {
                // map 16
                o << static_cast<CharType>(0xDE);
                write_number(static_cast<uint16_t>(N));
            }
            else if (N <= (std::numeric_limits<uint32_t>::max)())
            {
                // map 32
                o << static_cast<CharType>(0xDF);
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
            break;
    }
}

void json::binary_writer::write_ubjson(const json& j, const bool use_count,
                  const bool use_type, const bool add_prefix)
{
    switch (j.type())
    {
        case value_t::null:
        {
            if (add_prefix)
            {
                o << static_cast<CharType>('Z');
            }
            break;
        }

        case value_t::boolean:
        {
            if (add_prefix)
                o << static_cast<CharType>(j.m_value.boolean ? 'T' : 'F');
            break;
        }

        case value_t::number_integer:
        {
            write_number_with_ubjson_prefix(j.m_value.number_integer, add_prefix);
            break;
        }

        case value_t::number_unsigned:
        {
            write_number_with_ubjson_prefix(j.m_value.number_unsigned, add_prefix);
            break;
        }

        case value_t::number_float:
        {
            write_number_with_ubjson_prefix(j.m_value.number_float, add_prefix);
            break;
        }

        case value_t::string:
        {
            if (add_prefix)
            {
                o << static_cast<CharType>('S');
            }
            write_number_with_ubjson_prefix(j.m_value.string->size(), true);
            o << j.m_value.string;
            break;
        }

        case value_t::array:
        {
            if (add_prefix)
            {
                o << static_cast<CharType>('[');
            }

            bool prefix_required = true;
            if (use_type and not j.m_value.array->empty())
            {
                assert(use_count);
                const CharType first_prefix = ubjson_prefix(j.front());
                const bool same_prefix = std::all_of(j.begin() + 1, j.end(),
                                                     [this, first_prefix](const json & v)
                {
                    return ubjson_prefix(v) == first_prefix;
                });

                if (same_prefix)
                {
                    prefix_required = false;
                    o << static_cast<CharType>('$');
                    o << first_prefix;
                }
            }

            if (use_count)
            {
                o << static_cast<CharType>('#');
                write_number_with_ubjson_prefix(j.m_value.array->size(), true);
            }

            for (const auto& el : *j.m_value.array)
            {
                write_ubjson(el, use_count, use_type, prefix_required);
            }

            if (not use_count)
            {
                o << static_cast<CharType>(']');
            }

            break;
        }

        case value_t::object:
        {
            if (add_prefix)
            {
                o << static_cast<CharType>('{');
            }

            bool prefix_required = true;
            if (use_type and not j.m_value.object->empty())
            {
                assert(use_count);
                const CharType first_prefix = ubjson_prefix(j.front());
                const bool same_prefix = std::all_of(j.begin(), j.end(),
                                                     [this, first_prefix](const json & v)
                {
                    return ubjson_prefix(v) == first_prefix;
                });

                if (same_prefix)
                {
                    prefix_required = false;
                    o << static_cast<CharType>('$');
                    o << first_prefix;
                }
            }

            if (use_count)
            {
                o << static_cast<CharType>('#');
                write_number_with_ubjson_prefix(j.m_value.object->size(), true);
            }

            for (const auto& el : *j.m_value.object)
            {
                write_number_with_ubjson_prefix(el.first().size(), true);
                o << el.first();
                write_ubjson(el.second, use_count, use_type, prefix_required);
            }

            if (not use_count)
            {
                o << static_cast<CharType>('}');
            }

            break;
        }

        default:
            break;
    }
}

void json::binary_writer::write_cbor_string(StringRef str)
{
    // step 1: write control byte and the string length
    const auto N = str.size();
    if (N <= 0x17)
    {
        write_number(static_cast<uint8_t>(0x60 + N));
    }
    else if (N <= (std::numeric_limits<uint8_t>::max)())
    {
        o << static_cast<CharType>(0x78);
        write_number(static_cast<uint8_t>(N));
    }
    else if (N <= (std::numeric_limits<uint16_t>::max)())
    {
        o << static_cast<CharType>(0x79);
        write_number(static_cast<uint16_t>(N));
    }
    else if (N <= (std::numeric_limits<uint32_t>::max)())
    {
        o << static_cast<CharType>(0x7A);
        write_number(static_cast<uint32_t>(N));
    }
    // LCOV_EXCL_START
    else if (N <= (std::numeric_limits<uint64_t>::max)())
    {
        o << static_cast<CharType>(0x7B);
        write_number(static_cast<uint64_t>(N));
    }
    // LCOV_EXCL_STOP

    // step 2: write the string
    o << str;
}

void json::binary_writer::write_msgpack_string(StringRef str)
{
    // step 1: write control byte and the string length
    const auto N = str.size();
    if (N <= 31)
    {
        // fixstr
        write_number(static_cast<uint8_t>(0xA0 | N));
    }
    else if (N <= (std::numeric_limits<uint8_t>::max)())
    {
        // str 8
        o << static_cast<CharType>(0xD9);
        write_number(static_cast<uint8_t>(N));
    }
    else if (N <= (std::numeric_limits<uint16_t>::max)())
    {
        // str 16
        o << static_cast<CharType>(0xDA);
        write_number(static_cast<uint16_t>(N));
    }
    else if (N <= (std::numeric_limits<uint32_t>::max)())
    {
        // str 32
        o << static_cast<CharType>(0xDB);
        write_number(static_cast<uint32_t>(N));
    }

    // step 2: write the string
    o << str;
}

template<typename NumberType>
void json::binary_writer::write_number(const NumberType n)
{
    // step 1: write number to array of length NumberType
    std::array<uint8_t, sizeof(NumberType)> vec;
    std::memcpy(vec.data(), &n, sizeof(NumberType));

    // step 2: write array to output (with possible reordering)
    if (is_little_endian)
    {
        // reverse byte order prior to conversion if necessary
        std::reverse(vec.begin(), vec.end());
    }

    o << ArrayRef<uint8_t>(vec.data(), sizeof(NumberType));
}

template<typename NumberType, typename std::enable_if<
             std::is_unsigned<NumberType>::value, int>::type>
void json::binary_writer::write_number_with_ubjson_prefix(const NumberType n,
                                     const bool add_prefix)
{
    if (n <= static_cast<uint64_t>((std::numeric_limits<int8_t>::max)()))
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('i');  // int8
        }
        write_number(static_cast<uint8_t>(n));
    }
    else if (n <= (std::numeric_limits<uint8_t>::max)())
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('U');  // uint8
        }
        write_number(static_cast<uint8_t>(n));
    }
    else if (n <= static_cast<uint64_t>((std::numeric_limits<int16_t>::max)()))
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('I');  // int16
        }
        write_number(static_cast<int16_t>(n));
    }
    else if (n <= static_cast<uint64_t>((std::numeric_limits<int32_t>::max)()))
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('l');  // int32
        }
        write_number(static_cast<int32_t>(n));
    }
    else if (n <= static_cast<uint64_t>((std::numeric_limits<int64_t>::max)()))
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('L');  // int64
        }
        write_number(static_cast<int64_t>(n));
    }
    else
    {
        JSON_THROW(out_of_range::create(407, "number overflow serializing " + Twine(n)));
    }
}

template<typename NumberType, typename std::enable_if<
             std::is_signed<NumberType>::value and
             not std::is_floating_point<NumberType>::value, int>::type>
void json::binary_writer::write_number_with_ubjson_prefix(const NumberType n,
                                     const bool add_prefix)
{
    if ((std::numeric_limits<int8_t>::min)() <= n and n <= (std::numeric_limits<int8_t>::max)())
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('i');  // int8
        }
        write_number(static_cast<int8_t>(n));
    }
    else if (static_cast<int64_t>((std::numeric_limits<uint8_t>::min)()) <= n and n <= static_cast<int64_t>((std::numeric_limits<uint8_t>::max)()))
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('U');  // uint8
        }
        write_number(static_cast<uint8_t>(n));
    }
    else if ((std::numeric_limits<int16_t>::min)() <= n and n <= (std::numeric_limits<int16_t>::max)())
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('I');  // int16
        }
        write_number(static_cast<int16_t>(n));
    }
    else if ((std::numeric_limits<int32_t>::min)() <= n and n <= (std::numeric_limits<int32_t>::max)())
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('l');  // int32
        }
        write_number(static_cast<int32_t>(n));
    }
    else if ((std::numeric_limits<int64_t>::min)() <= n and n <= (std::numeric_limits<int64_t>::max)())
    {
        if (add_prefix)
        {
            o << static_cast<CharType>('L');  // int64
        }
        write_number(static_cast<int64_t>(n));
    }
    // LCOV_EXCL_START
    else
    {
        JSON_THROW(out_of_range::create(407, "number overflow serializing " + Twine(n)));
    }
    // LCOV_EXCL_STOP
}

json::binary_writer::CharType json::binary_writer::ubjson_prefix(const json& j) const noexcept
{
    switch (j.type())
    {
        case value_t::null:
            return 'Z';

        case value_t::boolean:
            return j.m_value.boolean ? 'T' : 'F';

        case value_t::number_integer:
        {
            if ((std::numeric_limits<int8_t>::min)() <= j.m_value.number_integer and j.m_value.number_integer <= (std::numeric_limits<int8_t>::max)())
            {
                return 'i';
            }
            else if ((std::numeric_limits<uint8_t>::min)() <= j.m_value.number_integer and j.m_value.number_integer <= (std::numeric_limits<uint8_t>::max)())
            {
                return 'U';
            }
            else if ((std::numeric_limits<int16_t>::min)() <= j.m_value.number_integer and j.m_value.number_integer <= (std::numeric_limits<int16_t>::max)())
            {
                return 'I';
            }
            else if ((std::numeric_limits<int32_t>::min)() <= j.m_value.number_integer and j.m_value.number_integer <= (std::numeric_limits<int32_t>::max)())
            {
                return 'l';
            }
            else  // no check and assume int64_t (see note above)
            {
                return 'L';
            }
        }

        case value_t::number_unsigned:
        {
            if (j.m_value.number_unsigned <= (std::numeric_limits<int8_t>::max)())
            {
                return 'i';
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<uint8_t>::max)())
            {
                return 'U';
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<int16_t>::max)())
            {
                return 'I';
            }
            else if (j.m_value.number_unsigned <= (std::numeric_limits<int32_t>::max)())
            {
                return 'l';
            }
            else  // no check and assume int64_t (see note above)
            {
                return 'L';
            }
        }

        case value_t::number_float:
            return get_ubjson_float_prefix(j.m_value.number_float);

        case value_t::string:
            return 'S';

        case value_t::array:
            return '[';

        case value_t::object:
            return '{';

        default:  // discarded values
            return 'N';
    }
}

std::vector<uint8_t> json::to_cbor(const json& j)
{
    std::vector<uint8_t> result;
    raw_uvector_ostream os(result);
    to_cbor(os, j);
    return result;
}

ArrayRef<uint8_t> json::to_cbor(const json& j, std::vector<uint8_t>& buf)
{
    buf.clear();
    raw_uvector_ostream os(buf);
    to_cbor(os, j);
    return os.array();
}

ArrayRef<uint8_t> json::to_cbor(const json& j, SmallVectorImpl<uint8_t>& buf)
{
    buf.clear();
    raw_usvector_ostream os(buf);
    to_cbor(os, j);
    return os.array();
}

void json::to_cbor(raw_ostream& os, const json& j)
{
    binary_writer(os).write_cbor(j);
}

std::vector<uint8_t> json::to_msgpack(const json& j)
{
    std::vector<uint8_t> result;
    raw_uvector_ostream os(result);
    to_msgpack(os, j);
    return result;
}

ArrayRef<uint8_t> json::to_msgpack(const json& j, std::vector<uint8_t>& buf)
{
    buf.clear();
    raw_uvector_ostream os(buf);
    to_msgpack(os, j);
    return os.array();
}

ArrayRef<uint8_t> json::to_msgpack(const json& j, SmallVectorImpl<uint8_t>& buf)
{
    buf.clear();
    raw_usvector_ostream os(buf);
    to_msgpack(os, j);
    return os.array();
}

void json::to_msgpack(raw_ostream& os, const json& j)
{
    binary_writer(os).write_msgpack(j);
}

std::vector<uint8_t> json::to_ubjson(const json& j,
                                     const bool use_size,
                                     const bool use_type)
{
    std::vector<uint8_t> result;
    raw_uvector_ostream os(result);
    to_ubjson(os, j, use_size, use_type);
    return result;
}

ArrayRef<uint8_t> json::to_ubjson(const json& j, std::vector<uint8_t>& buf,
                                  const bool use_size, const bool use_type)
{
    buf.clear();
    raw_uvector_ostream os(buf);
    to_ubjson(os, j, use_size, use_type);
    return os.array();
}

ArrayRef<uint8_t> json::to_ubjson(const json& j, SmallVectorImpl<uint8_t>& buf,
                                  const bool use_size, const bool use_type)
{
    buf.clear();
    raw_usvector_ostream os(buf);
    to_ubjson(os, j, use_size, use_type);
    return os.array();
}

void json::to_ubjson(raw_ostream& os, const json& j,
                     const bool use_size, const bool use_type)
{
    binary_writer(os).write_ubjson(j, use_size, use_type);
}

}  // namespace wpi
