#pragma once

#include "wpi/json.h"

#include "fmt/format.h"
#include "wpi/raw_ostream.h"

namespace wpi {
///////////////////
// binary writer //
///////////////////

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
    void write_cbor_string(std::string_view str);

    void write_msgpack_string(std::string_view str);

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

}  // namespace wpi
