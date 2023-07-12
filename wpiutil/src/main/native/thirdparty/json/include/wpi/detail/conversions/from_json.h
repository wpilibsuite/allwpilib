#pragma once

#include <algorithm> // transform
#include <array> // array
#include <ciso646> // and, not
#include <forward_list> // forward_list
#include <iterator> // inserter, front_inserter, end
#include <string> // string
#include <tuple> // tuple, make_tuple
#include <type_traits> // is_arithmetic, is_same, is_enum, underlying_type, is_convertible
#include <utility> // pair, declval
#include <valarray> // valarray

#include "wpi/detail/exceptions.h"
#include "wpi/detail/macro_scope.h"
#include "wpi/detail/meta.h"
#include "wpi/detail/value_t.h"

namespace wpi
{
namespace detail
{
// overloads for json template parameters
template<typename BasicJsonType, typename ArithmeticType,
         enable_if_t<std::is_arithmetic<ArithmeticType>::value and
                     not std::is_same<ArithmeticType, bool>::value,
                     int> = 0>
void get_arithmetic_value(const BasicJsonType& j, ArithmeticType& val)
{
    switch (static_cast<value_t>(j))
    {
        case value_t::number_unsigned:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const uint64_t*>());
            break;
        }
        case value_t::number_integer:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const int64_t*>());
            break;
        }
        case value_t::number_float:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const double*>());
            break;
        }

        default:
            JSON_THROW(type_error::create(302, "type must be number, but is", j.type_name()));
    }
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, bool& b)
{
    if (JSON_UNLIKELY(not j.is_boolean()))
    {
        JSON_THROW(type_error::create(302, "type must be boolean, but is", j.type_name()));
    }
    b = *j.template get_ptr<const bool*>();
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, std::string& s)
{
    if (JSON_UNLIKELY(not j.is_string()))
    {
        JSON_THROW(type_error::create(302, "type must be string, but is", j.type_name()));
    }
    s = *j.template get_ptr<const std::string*>();
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, double& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, uint64_t& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, int64_t& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void from_json(const BasicJsonType& j, EnumType& e)
{
    typename std::underlying_type<EnumType>::type val;
    get_arithmetic_value(j, val);
    e = static_cast<EnumType>(val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename BasicJsonType::array_t& arr)
{
    if (JSON_UNLIKELY(not j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is", j.type_name()));
    }
    arr = *j.template get_ptr<const typename BasicJsonType::array_t*>();
}

template<typename BasicJsonType, typename CompatibleArrayType>
void from_json_array_impl(const BasicJsonType& j, CompatibleArrayType& arr, priority_tag<0> /*unused*/)
{
    using std::end;

    std::transform(j.begin(), j.end(),
                   std::inserter(arr, end(arr)), [](const BasicJsonType & i)
    {
        // get<BasicJsonType>() returns *this, this won't call a from_json
        // method when value_type is BasicJsonType
        return i.template get<typename CompatibleArrayType::value_type>();
    });
}

template<typename BasicJsonType, typename CompatibleArrayType>
auto from_json_array_impl(const BasicJsonType& j, CompatibleArrayType& arr, priority_tag<1> /*unused*/)
-> decltype(
    arr.reserve(std::declval<typename CompatibleArrayType::size_type>()),
    void())
{
    using std::end;

    arr.reserve(j.size());
    std::transform(j.begin(), j.end(),
                   std::inserter(arr, end(arr)), [](const BasicJsonType & i)
    {
        // get<BasicJsonType>() returns *this, this won't call a from_json
        // method when value_type is BasicJsonType
        return i.template get<typename CompatibleArrayType::value_type>();
    });
}

template<typename BasicJsonType, typename T, std::size_t N>
void from_json_array_impl(const BasicJsonType& j, std::array<T, N>& arr, priority_tag<2> /*unused*/)
{
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = j.at(i).template get<T>();
    }
}

template <
    typename BasicJsonType, typename CompatibleArrayType,
    enable_if_t <
        is_compatible_array_type<BasicJsonType, CompatibleArrayType>::value and
        not std::is_same<typename BasicJsonType::array_t,
                         CompatibleArrayType>::value and
        std::is_constructible <
            BasicJsonType, typename CompatibleArrayType::value_type >::value,
        int > = 0 >
void from_json(const BasicJsonType& j, CompatibleArrayType& arr)
{
    if (JSON_UNLIKELY(not j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is", j.type_name()));
    }

    from_json_array_impl(j, arr, priority_tag<2> {});
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, typename BasicJsonType::object_t& obj)
{
    if (!j.is_object())
    {
        JSON_THROW(type_error::create(302, "type must be object, but is", j.type_name()));
    }

    auto inner_object = j.template get_ptr<const typename BasicJsonType::object_t*>();
    for (const auto& i : *inner_object) {
        obj.try_emplace(i.first(), i.second);
    }
}

template<typename BasicJsonType, typename CompatibleObjectType,
         enable_if_t<is_compatible_object_type<BasicJsonType, CompatibleObjectType>::value and
                     not std::is_same<typename BasicJsonType::object_t, CompatibleObjectType>::value, int> = 0>
void from_json(const BasicJsonType& j, CompatibleObjectType& obj)
{
    if (JSON_UNLIKELY(not j.is_object()))
    {
        JSON_THROW(type_error::create(302, "type must be object, but is", j.type_name()));
    }

    auto inner_object = j.template get_ptr<const typename BasicJsonType::object_t*>();
    using std::begin;
    using std::end;
    using value_type = typename CompatibleObjectType::value_type;
    std::vector<value_type> v;
    v.reserve(j.size());
    for (const auto& p : *inner_object)
    {
        v.emplace_back(
            p.first(),
            p.second
            .template get<typename CompatibleObjectType::mapped_type>());
    }
    // we could avoid the assignment, but this might require a for loop, which
    // might be less efficient than the container constructor for some
    // containers (would it?)
    obj = CompatibleObjectType(std::make_move_iterator(begin(v)),
                               std::make_move_iterator(end(v)));
}

// overload for arithmetic types, not chosen for json template arguments
// (BooleanType, etc..); note: Is it really necessary to provide explicit
// overloads for bool etc. in case of a custom BooleanType which is not
// an arithmetic type?
template<typename BasicJsonType, typename ArithmeticType,
         enable_if_t <
             std::is_arithmetic<ArithmeticType>::value and
             not std::is_same<ArithmeticType, uint64_t>::value and
             not std::is_same<ArithmeticType, int64_t>::value and
             not std::is_same<ArithmeticType, double>::value and
             not std::is_same<ArithmeticType, bool>::value,
             int> = 0>
void from_json(const BasicJsonType& j, ArithmeticType& val)
{
    switch (static_cast<value_t>(j))
    {
        case value_t::number_unsigned:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const uint64_t*>());
            break;
        }
        case value_t::number_integer:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const int64_t*>());
            break;
        }
        case value_t::number_float:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const double*>());
            break;
        }
        case value_t::boolean:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const bool*>());
            break;
        }

        default:
            JSON_THROW(type_error::create(302, "type must be number, but is", j.type_name()));
    }
}

template<typename BasicJsonType, typename A1, typename A2>
void from_json(const BasicJsonType& j, std::pair<A1, A2>& p)
{
    p = {j.at(0).template get<A1>(), j.at(1).template get<A2>()};
}

template<typename BasicJsonType, typename Tuple, std::size_t... Idx>
void from_json_tuple_impl(const BasicJsonType& j, Tuple& t, std::index_sequence<Idx...>)
{
    t = std::make_tuple(j.at(Idx).template get<typename std::tuple_element<Idx, Tuple>::type>()...);
}

template<typename BasicJsonType, typename... Args>
void from_json(const BasicJsonType& j, std::tuple<Args...>& t)
{
    from_json_tuple_impl(j, t, std::index_sequence_for<Args...> {});
}

struct from_json_fn
{
  private:
    template<typename BasicJsonType, typename T>
    auto call(const BasicJsonType& j, T& val, priority_tag<1> /*unused*/) const
    noexcept(noexcept(from_json(j, val)))
    -> decltype(from_json(j, val), void())
    {
        return from_json(j, val);
    }

    template<typename BasicJsonType, typename T>
    void call(const BasicJsonType& /*unused*/, T& /*unused*/, priority_tag<0> /*unused*/) const noexcept
    {
        static_assert(sizeof(BasicJsonType) == 0,
                      "could not find from_json() method in T's namespace");
#ifdef _MSC_VER
        // MSVC does not show a stacktrace for the above assert
        using decayed = uncvref_t<T>;
        static_assert(sizeof(typename decayed::force_msvc_stacktrace) == 0,
                      "forcing MSVC stacktrace to show which T we're talking about.");
#endif
    }

  public:
    template<typename BasicJsonType, typename T>
    void operator()(const BasicJsonType& j, T& val) const
    noexcept(noexcept(std::declval<from_json_fn>().call(j, val, priority_tag<1> {})))
    {
        return call(j, val, priority_tag<1> {});
    }
};
}

/// namespace to hold default `from_json` function
/// to see why this is required:
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
namespace
{
constexpr const auto& from_json = detail::static_const<detail::from_json_fn>::value;
}
}
