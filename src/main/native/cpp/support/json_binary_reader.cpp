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

#include "llvm/Format.h"
#include "llvm/raw_ostream.h"
#include "support/raw_istream.h"

using namespace wpi;

namespace {

/*!
@brief deserialization of CBOR and MessagePack values
*/
class binary_reader
{
    using value_t = json::value_t;

  public:
    /*!
    @brief create a binary reader

    @param[in] adapter  input adapter to read from
    */
    explicit binary_reader(wpi::raw_istream& s)
        : is(s), is_little_endian(little_endianess())
    {
    }

    /*!
    @brief create a JSON value from CBOR input

    @param[in] get_char  whether a new character should be retrieved from
                         the input (true, default) or whether the last
                         read character should be considered instead

    @return JSON value created from CBOR input

    @throw parse_error.110 if input ended unexpectedly
    @throw parse_error.112 if unsupported byte was read
    */
    json parse_cbor(bool get_char = true);

    /*!
    @brief create a JSON value from MessagePack input

    @return JSON value created from MessagePack input

    @throw parse_error.110 if input ended unexpectedly
    @throw parse_error.112 if unsupported byte was read
    */
    json parse_msgpack();

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
    @brief get next character from the input

    This function provides the interface to the used input adapter. It does
    not throw in case the input reached EOF, but returns
    `std::char_traits<char>::eof()` in that case.

    @return character read from the input
    */
    int get()
    {
        ++chars_read;
        unsigned char c;
        is.read(c);
        if (is.has_error())
        {
            current = std::char_traits<char>::eof();
        }
        else
        {
            current = c;
        }
        return current;
    }

    /*
    @brief read a number from the input

    @tparam T the type of the number

    @return number of type @a T

    @note This function needs to respect the system's endianess, because
          bytes in CBOR and MessagePack are stored in network order (big
          endian) and therefore need reordering on little endian systems.

    @throw parse_error.110 if input has less than `sizeof(T)` bytes
    */
    template<typename T>
    T get_number()
    {
        // step 1: read input into array with system's byte order
        std::array<uint8_t, sizeof(T)> vec;
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            get();
            check_eof();

            // reverse byte order prior to conversion if necessary
            if (is_little_endian)
            {
                vec[sizeof(T) - i - 1] = static_cast<uint8_t>(current);
            }
            else
            {
                vec[i] = static_cast<uint8_t>(current);
            }
        }

        // step 2: convert array into number of type T and return
        T result;
        std::memcpy(&result, vec.data(), sizeof(T));
        return result;
    }

    /*!
    @brief create a string by reading characters from the input

    @param[in] len number of bytes to read

    @return string created by reading @a len bytes

    @throw parse_error.110 if input has less than @a len bytes
    */
    std::string get_string(const size_t len);

    /*!
    @brief reads a CBOR string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.
    Additionally, CBOR's strings with indefinite lengths are supported.

    @return string

    @throw parse_error.110 if input ended
    @throw parse_error.113 if an unexpexted byte is read
    */
    std::string get_cbor_string();

    /*!
    @brief reads a MessagePack string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.

    @return string

    @throw parse_error.110 if input ended
    @throw parse_error.113 if an unexpexted byte is read
    */
    std::string get_msgpack_string();

    /*!
    @brief check if input ended
    @throw parse_error.110 if input ended
    */
    void check_eof() const
    {
        if (JSON_UNLIKELY(current == std::char_traits<char>::eof()))
        {
            JSON_THROW(json::parse_error::create(110, chars_read, "unexpected end of input"));
        }
    }

  private:
    /// input adapter
    wpi::raw_istream& is;

    /// the current character
    int current = std::char_traits<char>::eof();

    /// the number of characters read
    size_t chars_read = 0;

    /// whether we can assume little endianess
    const bool is_little_endian = true;
};

}  // anonymous namespace

json binary_reader::parse_cbor(bool get_char)
{
    switch (get_char ? get() : current)
    {
        // EOF
        case std::char_traits<char>::eof():
        {
            JSON_THROW(json::parse_error::create(110, chars_read, "unexpected end of input"));
        }

        // Integer 0x00..0x17 (0..23)
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        {
            return static_cast<std::uint64_t>(current);
        }

        case 0x18: // Unsigned integer (one-byte uint8_t follows)
        {
            return get_number<uint8_t>();
        }

        case 0x19: // Unsigned integer (two-byte uint16_t follows)
        {
            return get_number<uint16_t>();
        }

        case 0x1a: // Unsigned integer (four-byte uint32_t follows)
        {
            return get_number<uint32_t>();
        }

        case 0x1b: // Unsigned integer (eight-byte uint64_t follows)
        {
            return get_number<uint64_t>();
        }

        // Negative integer -1-0x00..-1-0x17 (-1..-24)
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        {
            return static_cast<int8_t>(0x20 - 1 - current);
        }

        case 0x38: // Negative integer (one-byte uint8_t follows)
        {
            // must be uint8_t !
            return static_cast<std::int64_t>(-1) - get_number<uint8_t>();
        }

        case 0x39: // Negative integer -1-n (two-byte uint16_t follows)
        {
            return static_cast<std::int64_t>(-1) - get_number<uint16_t>();
        }

        case 0x3a: // Negative integer -1-n (four-byte uint32_t follows)
        {
            return static_cast<std::int64_t>(-1) - get_number<uint32_t>();
        }

        case 0x3b: // Negative integer -1-n (eight-byte uint64_t follows)
        {
            return static_cast<std::int64_t>(-1) - static_cast<std::int64_t>(get_number<uint64_t>());
        }

        // UTF-8 string (0x00..0x17 bytes follow)
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78: // UTF-8 string (one-byte uint8_t for n follows)
        case 0x79: // UTF-8 string (two-byte uint16_t for n follow)
        case 0x7a: // UTF-8 string (four-byte uint32_t for n follow)
        case 0x7b: // UTF-8 string (eight-byte uint64_t for n follow)
        case 0x7f: // UTF-8 string (indefinite length)
        {
            return get_cbor_string();
        }

        // array (0x00..0x17 data items follow)
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(current & 0x1f);
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_cbor());
            }
            return result;
        }

        case 0x98: // array (one-byte uint8_t for n follows)
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(get_number<uint8_t>());
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_cbor());
            }
            return result;
        }

        case 0x99: // array (two-byte uint16_t for n follow)
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(get_number<uint16_t>());
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_cbor());
            }
            return result;
        }

        case 0x9a: // array (four-byte uint32_t for n follow)
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(get_number<uint32_t>());
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_cbor());
            }
            return result;
        }

        case 0x9b: // array (eight-byte uint64_t for n follow)
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(get_number<uint64_t>());
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_cbor());
            }
            return result;
        }

        case 0x9f: // array (indefinite length)
        {
            json result = value_t::array;
            while (get() != 0xff)
            {
                result.push_back(parse_cbor(false));
            }
            return result;
        }

        // map (0x00..0x17 pairs of data items follow)
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(current & 0x1f);
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_cbor_string();
                result[key] = parse_cbor();
            }
            return result;
        }

        case 0xb8: // map (one-byte uint8_t for n follows)
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(get_number<uint8_t>());
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_cbor_string();
                result[key] = parse_cbor();
            }
            return result;
        }

        case 0xb9: // map (two-byte uint16_t for n follow)
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(get_number<uint16_t>());
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_cbor_string();
                result[key] = parse_cbor();
            }
            return result;
        }

        case 0xba: // map (four-byte uint32_t for n follow)
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(get_number<uint32_t>());
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_cbor_string();
                result[key] = parse_cbor();
            }
            return result;
        }

        case 0xbb: // map (eight-byte uint64_t for n follow)
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(get_number<uint64_t>());
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_cbor_string();
                result[key] = parse_cbor();
            }
            return result;
        }

        case 0xbf: // map (indefinite length)
        {
            json result = value_t::object;
            while (get() != 0xff)
            {
                auto key = get_cbor_string();
                result[key] = parse_cbor();
            }
            return result;
        }

        case 0xf4: // false
        {
            return false;
        }

        case 0xf5: // true
        {
            return true;
        }

        case 0xf6: // null
        {
            return value_t::null;
        }

        case 0xf9: // Half-Precision Float (two-byte IEEE 754)
        {
            const int byte1 = get();
            check_eof();
            const int byte2 = get();
            check_eof();

            // code from RFC 7049, Appendix D, Figure 3:
            // As half-precision floating-point numbers were only added
            // to IEEE 754 in 2008, today's programming platforms often
            // still only have limited support for them. It is very
            // easy to include at least decoding support for them even
            // without such support. An example of a small decoder for
            // half-precision floating-point numbers in the C language
            // is shown in Fig. 3.
            const int half = (byte1 << 8) + byte2;
            const int exp = (half >> 10) & 0x1f;
            const int mant = half & 0x3ff;
            double val;
            if (exp == 0)
            {
                val = std::ldexp(mant, -24);
            }
            else if (exp != 31)
            {
                val = std::ldexp(mant + 1024, exp - 25);
            }
            else
            {
                val = mant == 0
                      ? std::numeric_limits<double>::infinity()
                      : std::numeric_limits<double>::quiet_NaN();
            }
            return (half & 0x8000) != 0 ? -val : val;
        }

        case 0xfa: // Single-Precision Float (four-byte IEEE 754)
        {
            return get_number<float>();
        }

        case 0xfb: // Double-Precision Float (eight-byte IEEE 754)
        {
            return get_number<double>();
        }

        default: // anything else (0xFF is handled inside the other types)
        {
            std::string s;
            llvm::raw_string_ostream ss(s);
            ss << "error reading CBOR; last byte: ";
            ss << llvm::format_hex(current, 2);
            JSON_THROW(json::parse_error::create(112, chars_read, ss.str()));
        }
    }
}

std::string binary_reader::get_cbor_string()
{
    check_eof();

    switch (current)
    {
        // UTF-8 string (0x00..0x17 bytes follow)
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        {
            const auto len = static_cast<size_t>(current & 0x1f);
            return get_string(len);
        }

        case 0x78: // UTF-8 string (one-byte uint8_t for n follows)
        {
            const auto len = static_cast<size_t>(get_number<uint8_t>());
            return get_string(len);
        }

        case 0x79: // UTF-8 string (two-byte uint16_t for n follow)
        {
            const auto len = static_cast<size_t>(get_number<uint16_t>());
            return get_string(len);
        }

        case 0x7a: // UTF-8 string (four-byte uint32_t for n follow)
        {
            const auto len = static_cast<size_t>(get_number<uint32_t>());
            return get_string(len);
        }

        case 0x7b: // UTF-8 string (eight-byte uint64_t for n follow)
        {
            const auto len = static_cast<size_t>(get_number<uint64_t>());
            return get_string(len);
        }

        case 0x7f: // UTF-8 string (indefinite length)
        {
            std::string result;
            while (get() != 0xff)
            {
                check_eof();
                result.append(1, static_cast<char>(current));
            }
            return result;
        }

        default:
        {
            std::string s;
            llvm::raw_string_ostream ss(s);
            ss << "expected a CBOR string; last byte: ";
            ss << llvm::format_hex(current, 2);
            JSON_THROW(json::parse_error::create(113, chars_read, ss.str()));
        }
    }
}

json binary_reader::parse_msgpack()
{
    switch (get())
    {
        // EOF
        case std::char_traits<char>::eof():
        {
            JSON_THROW(json::parse_error::create(110, chars_read, "unexpected end of input"));
        }

        // positive fixint
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f:
        {
            return static_cast<std::uint64_t>(current);
        }

        // fixmap
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(current & 0x0f);
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_msgpack_string();
                result[key] = parse_msgpack();
            }
            return result;
        }

        // fixarray
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(current & 0x0f);
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_msgpack());
            }
            return result;
        }

        // fixstr
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:
        {
            return get_msgpack_string();
        }

        case 0xc0: // nil
        {
            return value_t::null;
        }

        case 0xc2: // false
        {
            return false;
        }

        case 0xc3: // true
        {
            return true;
        }

        case 0xca: // float 32
        {
            return get_number<float>();
        }

        case 0xcb: // float 64
        {
            return get_number<double>();
        }

        case 0xcc: // uint 8
        {
            return get_number<uint8_t>();
        }

        case 0xcd: // uint 16
        {
            return get_number<uint16_t>();
        }

        case 0xce: // uint 32
        {
            return get_number<uint32_t>();
        }

        case 0xcf: // uint 64
        {
            return get_number<uint64_t>();
        }

        case 0xd0: // int 8
        {
            return get_number<int8_t>();
        }

        case 0xd1: // int 16
        {
            return get_number<int16_t>();
        }

        case 0xd2: // int 32
        {
            return get_number<int32_t>();
        }

        case 0xd3: // int 64
        {
            return get_number<int64_t>();
        }

        case 0xd9: // str 8
        case 0xda: // str 16
        case 0xdb: // str 32
        {
            return get_msgpack_string();
        }

        case 0xdc: // array 16
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(get_number<uint16_t>());
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_msgpack());
            }
            return result;
        }

        case 0xdd: // array 32
        {
            json result = value_t::array;
            const auto len = static_cast<size_t>(get_number<uint32_t>());
            for (size_t i = 0; i < len; ++i)
            {
                result.push_back(parse_msgpack());
            }
            return result;
        }

        case 0xde: // map 16
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(get_number<uint16_t>());
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_msgpack_string();
                result[key] = parse_msgpack();
            }
            return result;
        }

        case 0xdf: // map 32
        {
            json result = value_t::object;
            const auto len = static_cast<size_t>(get_number<uint32_t>());
            for (size_t i = 0; i < len; ++i)
            {
                get();
                auto key = get_msgpack_string();
                result[key] = parse_msgpack();
            }
            return result;
        }

        // positive fixint
        case 0xe0:
        case 0xe1:
        case 0xe2:
        case 0xe3:
        case 0xe4:
        case 0xe5:
        case 0xe6:
        case 0xe7:
        case 0xe8:
        case 0xe9:
        case 0xea:
        case 0xeb:
        case 0xec:
        case 0xed:
        case 0xee:
        case 0xef:
        case 0xf0:
        case 0xf1:
        case 0xf2:
        case 0xf3:
        case 0xf4:
        case 0xf5:
        case 0xf6:
        case 0xf7:
        case 0xf8:
        case 0xf9:
        case 0xfa:
        case 0xfb:
        case 0xfc:
        case 0xfd:
        case 0xfe:
        case 0xff:
        {
            return static_cast<int8_t>(current);
        }

        default: // anything else
        {
            std::string s;
            llvm::raw_string_ostream ss(s);
            ss << "error reading MessagePack; last byte: ";
            ss << llvm::format_hex(current, 2);
            JSON_THROW(json::parse_error::create(112, chars_read, ss.str()));
        }
    }
}

std::string binary_reader::get_msgpack_string()
{
    check_eof();

    switch (current)
    {
        // fixstr
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:
        {
            const auto len = static_cast<size_t>(current & 0x1f);
            return get_string(len);
        }

        case 0xd9: // str 8
        {
            const auto len = static_cast<size_t>(get_number<uint8_t>());
            return get_string(len);
        }

        case 0xda: // str 16
        {
            const auto len = static_cast<size_t>(get_number<uint16_t>());
            return get_string(len);
        }

        case 0xdb: // str 32
        {
            const auto len = static_cast<size_t>(get_number<uint32_t>());
            return get_string(len);
        }

        default:
        {
            std::string s;
            llvm::raw_string_ostream ss(s);
            ss << "expected a MessagePack string; last byte: ";
            ss << llvm::format_hex(current, 2);
            JSON_THROW(json::parse_error::create(113, chars_read, ss.str()));
        }
    }
}

std::string binary_reader::get_string(const size_t len)
{
    std::string result;
    for (size_t i = 0; i < len; ++i)
    {
        get();
        check_eof();
        result.append(1, static_cast<char>(current));
    }
    return result;
}

json json::from_cbor(wpi::raw_istream& is)
{
    binary_reader br(is);
    return br.parse_cbor();
}

json json::from_cbor(llvm::StringRef s)
{
    wpi::raw_mem_istream is(s.data(), s.size());
    binary_reader br(is);
    return br.parse_cbor();
}

json json::from_msgpack(wpi::raw_istream& is)
{
    binary_reader br(is);
    return br.parse_msgpack();
}

json json::from_msgpack(llvm::StringRef s)
{
    wpi::raw_mem_istream is(s.data(), s.size());
    binary_reader br(is);
    return br.parse_msgpack();
}
