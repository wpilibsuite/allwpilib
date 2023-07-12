#pragma once

#include <utility>

#include "wpi/detail/conversions/from_json.h"
#include "wpi/detail/conversions/to_json.h"

namespace wpi
{
template<typename, typename>
struct adl_serializer
{
    /*!
    @brief convert a JSON value to any value type

    This function is usually called by the `get()` function of the
    @ref json class (either explicit or via conversion operators).

    @param[in] j         JSON value to read from
    @param[in,out] val  value to write to
    */
    template<typename BasicJsonType, typename ValueType>
    static void from_json(BasicJsonType&& j, ValueType& val) noexcept(
        noexcept(::wpi::from_json(std::forward<BasicJsonType>(j), val)))
    {
        ::wpi::from_json(std::forward<BasicJsonType>(j), val);
    }

    /*!
    @brief convert any value type to a JSON value

    This function is usually called by the constructors of the @ref json
    class.

    @param[in,out] j  JSON value to write to
    @param[in] val     value to read from
    */
    template<typename BasicJsonType, typename ValueType>
    static void to_json(BasicJsonType& j, ValueType&& val) noexcept(
        noexcept(::wpi::to_json(j, std::forward<ValueType>(val))))
    {
        ::wpi::to_json(j, std::forward<ValueType>(val));
    }
};
}
