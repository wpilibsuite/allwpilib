#pragma once

#include <array> // array
#include <ciso646> // and
#include <cstddef> // size_t
#include <cstdint> // uint8_t

namespace wpi
{
namespace detail
{
///////////////////////////
// JSON type enumeration //
///////////////////////////

/*!
@brief the JSON type enumeration

This enumeration collects the different JSON types. It is internally used to
distinguish the stored values, and the functions @ref json::is_null(),
@ref json::is_object(), @ref json::is_array(),
@ref json::is_string(), @ref json::is_boolean(),
@ref json::is_number() (with @ref json::is_number_integer(),
@ref json::is_number_unsigned(), and @ref json::is_number_float()),
@ref json::is_discarded(), @ref json::is_primitive(), and
@ref json::is_structured() rely on it.

@note There are three enumeration entries (number_integer, number_unsigned, and
number_float), because the library distinguishes these three types for numbers:
uint64_t is used for unsigned integers,
int64_t is used for signed integers, and
double is used for floating-point numbers or to
approximate integers which do not fit in the limits of their respective type.

@sa @ref json::json(const value_t value_type) -- create a JSON
value with the default value for a given type

@since version 1.0.0
*/
enum class value_t : std::uint8_t
{
    null,             ///< null value
    object,           ///< object (unordered set of name/value pairs)
    array,            ///< array (ordered collection of values)
    string,           ///< string value
    boolean,          ///< boolean value
    number_integer,   ///< number value (signed integer)
    number_unsigned,  ///< number value (unsigned integer)
    number_float,     ///< number value (floating-point)
    discarded         ///< discarded by the the parser callback function
};

/*!
@brief comparison operator for JSON types

Returns an ordering that is similar to Python:
- order: null < boolean < number < object < array < string
- furthermore, each type is not smaller than itself
- discarded values are not comparable

@since version 1.0.0
*/
inline bool operator<(const value_t lhs, const value_t rhs) noexcept
{
    static constexpr std::array<std::uint8_t, 8> order = {{
            0 /* null */, 3 /* object */, 4 /* array */, 5 /* string */,
            1 /* boolean */, 2 /* integer */, 2 /* unsigned */, 2 /* float */
        }
    };

    const auto l_index = static_cast<std::size_t>(lhs);
    const auto r_index = static_cast<std::size_t>(rhs);
    return l_index < order.size() and r_index < order.size() and order[l_index] < order[r_index];
}
}
}
