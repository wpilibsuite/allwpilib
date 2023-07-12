#pragma once

#include <ciso646> // not
#include <cstddef> // size_t
#include <limits> // numeric_limits
#include <type_traits> // conditional, enable_if, false_type, integral_constant, is_constructible, is_integral, is_same, remove_cv, remove_reference, true_type
#include <utility> // declval

#include "wpi/json_fwd.h"
#include "wpi/detail/macro_scope.h"

namespace wpi
{
/*!
@brief detail namespace with internal helper functions

This namespace collects functions that should not be exposed,
implementations of some @ref json methods, and meta-programming helpers.

@since version 2.1.0
*/
namespace detail
{
/////////////
// helpers //
/////////////

template<typename> struct is_json : std::false_type {};

template<> struct is_json<json> : std::true_type {};

// alias templates to reduce boilerplate
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using uncvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

// dispatch utility (taken from ranges-v3)
template<unsigned N> struct priority_tag : priority_tag < N - 1 > {};
template<> struct priority_tag<0> {};

////////////////////////
// has_/is_ functions //
////////////////////////

// source: https://stackoverflow.com/a/37193089/4116453

template <typename T, typename = void>
struct is_complete_type : std::false_type {};

template <typename T>
struct is_complete_type<T, decltype(void(sizeof(T)))> : std::true_type {};

NLOHMANN_JSON_HAS_HELPER(mapped_type);
NLOHMANN_JSON_HAS_HELPER(key_type);
NLOHMANN_JSON_HAS_HELPER(value_type);
NLOHMANN_JSON_HAS_HELPER(iterator);

template<bool B, class RealType, class CompatibleObjectType>
struct is_compatible_object_type_impl : std::false_type {};

template<class RealType, class CompatibleObjectType>
struct is_compatible_object_type_impl<true, RealType, CompatibleObjectType>
{
    static constexpr auto value =
        std::is_constructible<std::string_view, typename CompatibleObjectType::key_type>::value and
        std::is_constructible<typename RealType::mapped_type, typename CompatibleObjectType::mapped_type>::value;
};

template<class BasicJsonType, class CompatibleObjectType>
struct is_compatible_object_type
{
    static auto constexpr value = is_compatible_object_type_impl <
                                  std::conjunction<std::negation<std::is_same<void, CompatibleObjectType>>,
                                  has_mapped_type<CompatibleObjectType>,
                                  has_key_type<CompatibleObjectType>>::value,
                                  typename BasicJsonType::object_t, CompatibleObjectType >::value;
};

template<typename BasicJsonType, typename T>
struct is_json_nested_type
{
    static auto constexpr value = std::is_same<T, typename BasicJsonType::iterator>::value or
                                  std::is_same<T, typename BasicJsonType::const_iterator>::value or
                                  std::is_same<T, typename BasicJsonType::reverse_iterator>::value or
                                  std::is_same<T, typename BasicJsonType::const_reverse_iterator>::value;
};

template<class BasicJsonType, class CompatibleArrayType>
struct is_compatible_array_type
{
    static auto constexpr value =
        std::conjunction<std::negation<std::is_same<void, CompatibleArrayType>>,
        std::negation<is_compatible_object_type<
        BasicJsonType, CompatibleArrayType>>,
        std::negation<std::is_constructible<std::string_view,
        CompatibleArrayType>>,
        std::negation<is_json_nested_type<BasicJsonType, CompatibleArrayType>>,
        has_value_type<CompatibleArrayType>,
        has_iterator<CompatibleArrayType>>::value;
};

template<bool, typename, typename>
struct is_compatible_integer_type_impl : std::false_type {};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type_impl<true, RealIntegerType, CompatibleNumberIntegerType>
{
    // is there an assert somewhere on overflows?
    using RealLimits = std::numeric_limits<RealIntegerType>;
    using CompatibleLimits = std::numeric_limits<CompatibleNumberIntegerType>;

    static constexpr auto value =
        std::is_constructible<RealIntegerType, CompatibleNumberIntegerType>::value and
        CompatibleLimits::is_integer and
        RealLimits::is_signed == CompatibleLimits::is_signed;
};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type
{
    static constexpr auto value =
        is_compatible_integer_type_impl <
        std::is_integral<CompatibleNumberIntegerType>::value and
        not std::is_same<bool, CompatibleNumberIntegerType>::value,
        RealIntegerType, CompatibleNumberIntegerType > ::value;
};

// trait checking if JSONSerializer<T>::from_json(json const&, udt&) exists
template<typename BasicJsonType, typename T>
struct has_from_json
{
  private:
    // also check the return type of from_json
    template<typename U, typename = enable_if_t<std::is_same<void, decltype(uncvref_t<U>::from_json(
                 std::declval<BasicJsonType>(), std::declval<T&>()))>::value>>
    static int detect(U&&);
    static void detect(...);

  public:
    static constexpr bool value = std::is_integral<decltype(
                                      detect(std::declval<typename BasicJsonType::template json_serializer<T, void>>()))>::value;
};

// This trait checks if JSONSerializer<T>::from_json(json const&) exists
// this overload is used for non-default-constructible user-defined-types
template<typename BasicJsonType, typename T>
struct has_non_default_from_json
{
  private:
    template <
        typename U,
        typename = enable_if_t<std::is_same<
                                   T, decltype(uncvref_t<U>::from_json(std::declval<BasicJsonType>()))>::value >>
    static int detect(U&&);
    static void detect(...);

  public:
    static constexpr bool value = std::is_integral<decltype(detect(
                                      std::declval<typename BasicJsonType::template json_serializer<T, void>>()))>::value;
};

// This trait checks if BasicJsonType::json_serializer<T>::to_json exists
template<typename BasicJsonType, typename T>
struct has_to_json
{
  private:
    template<typename U, typename = decltype(uncvref_t<U>::to_json(
                 std::declval<BasicJsonType&>(), std::declval<T>()))>
    static int detect(U&&);
    static void detect(...);

  public:
    static constexpr bool value = std::is_integral<decltype(detect(
                                      std::declval<typename BasicJsonType::template json_serializer<T, void>>()))>::value;
};

template <typename BasicJsonType, typename CompatibleCompleteType>
struct is_compatible_complete_type
{
    static constexpr bool value =
        not std::is_base_of<std::istream, CompatibleCompleteType>::value and
        not is_json<CompatibleCompleteType>::value and
        not is_json_nested_type<BasicJsonType, CompatibleCompleteType>::value and
        has_to_json<BasicJsonType, CompatibleCompleteType>::value;
};

template <typename BasicJsonType, typename CompatibleType>
struct is_compatible_type
    : std::conjunction<is_complete_type<CompatibleType>,
      is_compatible_complete_type<BasicJsonType, CompatibleType>>
{
};

// taken from ranges-v3
template<typename T>
struct static_const
{
    static constexpr T value{};
};

template<typename T>
constexpr T static_const<T>::value;
}
}
