#pragma once

#include "wpi/json.h"

#include <cmath>  // ldexp

#include "fmt/format.h"
#include "wpi/raw_istream.h"

namespace wpi {
///////////////////
// binary reader //
///////////////////

/*!
@brief deserialization of CBOR and MessagePack values
*/
class json::binary_reader
{
  public:
    /*!
    @brief create a binary reader

    @param[in] adapter  input adapter to read from
    */
    explicit binary_reader(raw_istream& s) : is(s)
    {
    }

    /*!
    @brief create a JSON value from CBOR input

    @param[in] strict  whether to expect the input to be consumed completed
    @return JSON value created from CBOR input

    @throw parse_error.110 if input ended unexpectedly or the end of file was
                           not reached when @a strict was set to true
    @throw parse_error.112 if unsupported byte was read
    */
    json parse_cbor(const bool strict)
    {
        const auto res = parse_cbor_internal();
        if (strict)
        {
            get();
            expect_eof();
        }
        return res;
    }

    /*!
    @brief create a JSON value from MessagePack input

    @param[in] strict  whether to expect the input to be consumed completed
    @return JSON value created from MessagePack input

    @throw parse_error.110 if input ended unexpectedly or the end of file was
                           not reached when @a strict was set to true
    @throw parse_error.112 if unsupported byte was read
    */
    json parse_msgpack(const bool strict)
    {
        const auto res = parse_msgpack_internal();
        if (strict)
        {
            get();
            expect_eof();
        }
        return res;
    }

    /*!
    @brief create a JSON value from UBJSON input

    @param[in] strict  whether to expect the input to be consumed completed
    @return JSON value created from UBJSON input

    @throw parse_error.110 if input ended unexpectedly or the end of file was
                           not reached when @a strict was set to true
    @throw parse_error.112 if unsupported byte was read
    */
    json parse_ubjson(const bool strict)
    {
        const auto res = parse_ubjson_internal();
        if (strict)
        {
            get_ignore_noop();
            expect_eof();
        }
        return res;
    }

    /*!
    @brief determine system byte order

    @return true if and only if system's byte order is little endian

    @note from http://stackoverflow.com/a/1001328/266378
    */
    static bool little_endianess(int num = 1) noexcept
    {
        return (*reinterpret_cast<char*>(&num) == 1);
    }

  private:
    /*!
    @param[in] get_char  whether a new character should be retrieved from the
                         input (true, default) or whether the last read
                         character should be considered instead
    */
    json parse_cbor_internal(const bool get_char = true);

    json parse_msgpack_internal();

    /*!
    @param[in] get_char  whether a new character should be retrieved from the
                         input (true, default) or whether the last read
                         character should be considered instead
    */
    json parse_ubjson_internal(const bool get_char = true)
    {
        return get_ubjson_value(get_char ? get_ignore_noop() : current);
    }

    /*!
    @brief get next character from the input

    This function provides the interface to the used input adapter. It does
    not throw in case the input reached EOF, but returns a -'ve valued
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

    /*!
    @return character read from the input after ignoring all 'N' entries
    */
    int get_ignore_noop()
    {
        do
        {
            get();
        }
        while (current == 'N');

        return current;
    }

    /*
    @brief read a number from the input

    @tparam NumberType the type of the number

    @return number of type @a NumberType

    @note This function needs to respect the system's endianess, because
          bytes in CBOR and MessagePack are stored in network order (big
          endian) and therefore need reordering on little endian systems.

    @throw parse_error.110 if input has less than `sizeof(NumberType)` bytes
    */
    template<typename NumberType> NumberType get_number()
    {
        // step 1: read input into array with system's byte order
        std::array<uint8_t, sizeof(NumberType)> vec;
        for (std::size_t i = 0; i < sizeof(NumberType); ++i)
        {
            get();
            unexpect_eof();

            // reverse byte order prior to conversion if necessary
            if (is_little_endian)
            {
                vec[sizeof(NumberType) - i - 1] = static_cast<uint8_t>(current);
            }
            else
            {
                vec[i] = static_cast<uint8_t>(current); // LCOV_EXCL_LINE
            }
        }

        // step 2: convert array into number of type T and return
        NumberType result;
        std::memcpy(&result, vec.data(), sizeof(NumberType));
        return result;
    }

    /*!
    @brief create a string by reading characters from the input

    @param[in] len number of bytes to read

    @note We can not reserve @a len bytes for the result, because @a len
          may be too large. Usually, @ref unexpect_eof() detects the end of
          the input before we run out of string memory.

    @return string created by reading @a len bytes

    @throw parse_error.110 if input has less than @a len bytes
    */
    template<typename NumberType>
    std::string get_string(const NumberType len)
    {
        std::string result;
        std::generate_n(std::back_inserter(result), len, [this]()
        {
            get();
            unexpect_eof();
            return static_cast<char>(current);
        });
        return result;
    }

    /*!
    @brief reads a CBOR string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.
    Additionally, CBOR's strings with indefinite lengths are supported.

    @return string

    @throw parse_error.110 if input ended
    @throw parse_error.113 if an unexpected byte is read
    */
    std::string get_cbor_string();

    template<typename NumberType>
    json get_cbor_array(const NumberType len)
    {
        json result = value_t::array;
        std::generate_n(std::back_inserter(*result.m_value.array), len, [this]()
        {
            return parse_cbor_internal();
        });
        return result;
    }

    template<typename NumberType>
    json get_cbor_object(const NumberType len)
    {
        json result = value_t::object;
        for (NumberType i = 0; i < len; ++i)
        {
            get();
            auto key = get_cbor_string();
            (*result.m_value.object)[key] = parse_cbor_internal();
        }
        return result;
    }

    /*!
    @brief reads a MessagePack string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.

    @return string

    @throw parse_error.110 if input ended
    @throw parse_error.113 if an unexpected byte is read
    */
    std::string get_msgpack_string();

    template<typename NumberType>
    json get_msgpack_array(const NumberType len)
    {
        json result = value_t::array;
        std::generate_n(std::back_inserter(*result.m_value.array), len, [this]()
        {
            return parse_msgpack_internal();
        });
        return result;
    }

    template<typename NumberType>
    json get_msgpack_object(const NumberType len)
    {
        json result = value_t::object;
        for (NumberType i = 0; i < len; ++i)
        {
            get();
            auto key = get_msgpack_string();
            (*result.m_value.object)[key] = parse_msgpack_internal();
        }
        return result;
    }

    /*!
    @brief reads a UBJSON string

    This function is either called after reading the 'S' byte explicitly
    indicating a string, or in case of an object key where the 'S' byte can be
    left out.

    @param[in] get_char  whether a new character should be retrieved from the
                         input (true, default) or whether the last read
                         character should be considered instead

    @return string

    @throw parse_error.110 if input ended
    @throw parse_error.113 if an unexpected byte is read
    */
    std::string get_ubjson_string(const bool get_char = true);

    /*!
    @brief determine the type and size for a container

    In the optimized UBJSON format, a type and a size can be provided to allow
    for a more compact representation.

    @return pair of the size and the type
    */
    std::pair<std::size_t, int> get_ubjson_size_type();

    json get_ubjson_value(const int prefix);

    json get_ubjson_array();

    json get_ubjson_object();

    /*!
    @brief throw if end of input is not reached
    @throw parse_error.110 if input not ended
    */
    void expect_eof() const
    {
        if (JSON_UNLIKELY(current != std::char_traits<char>::eof()))
        {
            JSON_THROW(parse_error::create(110, chars_read, "expected end of input"));
        }
    }

    /*!
    @briefthrow if end of input is reached
    @throw parse_error.110 if input ended
    */
    void unexpect_eof() const
    {
        if (JSON_UNLIKELY(current == std::char_traits<char>::eof()))
        {
            JSON_THROW(parse_error::create(110, chars_read, "unexpected end of input"));
        }
    }

  private:
    /// input adapter
    raw_istream& is;

    /// the current character
    int current = std::char_traits<char>::eof();

    /// the number of characters read
    std::size_t chars_read = 0;

    /// whether we can assume little endianess
    const bool is_little_endian = little_endianess();
};

}  // namespace wpi
