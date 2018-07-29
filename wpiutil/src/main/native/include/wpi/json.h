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

#ifndef WPIUTIL_JSON_H
#define WPIUTIL_JSON_H

#define NLOHMANN_JSON_VERSION_MAJOR 3
#define NLOHMANN_JSON_VERSION_MINOR 1
#define NLOHMANN_JSON_VERSION_PATCH 2


#include <algorithm> // all_of, copy, find, for_each, generate_n, min, reverse, remove, fill, none_of, transform
#include <array> // array
#include <cassert> // assert
#include <ciso646> // and, not, or
#include <cstddef> // nullptr_t, ptrdiff_t, size_t
#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <exception> // exception
#include <functional> // function, hash, less
#include <initializer_list> // initializer_list
#include <iterator>
#include <limits> // numeric_limits
#include <memory> // allocator, shared_ptr, make_shared, addressof
#include <stdexcept> // runtime_error
#include <string> // string, char_traits, stoi, to_string
#include <tuple> // tuple, get, make_tuple
#include <type_traits>
#include <utility>
#include <vector> // vector

#include "wpi/ArrayRef.h"
#include "wpi/StringMap.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"

namespace wpi
{

class raw_istream;
class raw_ostream;

class JsonTest;

/*!
@brief default JSONSerializer template argument

This serializer ignores the template arguments and uses ADL
([argument-dependent lookup](http://en.cppreference.com/w/cpp/language/adl))
for serialization.
*/
template<typename = void, typename = void>
struct adl_serializer;

/*!
@brief JSON Pointer

A JSON pointer defines a string syntax for identifying a specific value
within a JSON document. It can be used with functions `at` and
`operator[]`. Furthermore, JSON pointers are the base for JSON patches.

@sa [RFC 6901](https://tools.ietf.org/html/rfc6901)

@since version 2.0.0
*/
class json_pointer;

/*!
@brief default JSON class

This type is the default specialization of the @ref json class which
uses the standard template types.

@since version 1.0.0
*/
class json;
}

// exclude unsupported compilers
#if defined(__clang__)
    #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
        #error "unsupported Clang version - see https://github.com/nlohmann/json#supported-compilers"
    #endif
#elif defined(__GNUC__) && !(defined(__ICC) || defined(__INTEL_COMPILER))
    #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
        #error "unsupported GCC version - see https://github.com/nlohmann/json#supported-compilers"
    #endif
#endif

// disable float-equal warnings on GCC/clang
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// disable documentation warnings on clang
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdocumentation"
#endif

// allow to disable exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(JSON_NOEXCEPTION)
    #define JSON_THROW(exception) throw exception
    #define JSON_TRY try
    #define JSON_CATCH(exception) catch(exception)
#else
    #define JSON_THROW(exception) std::abort()
    #define JSON_TRY if(true)
    #define JSON_CATCH(exception) if(false)
#endif

// override exception macros
#if defined(JSON_THROW_USER)
    #undef JSON_THROW
    #define JSON_THROW JSON_THROW_USER
#endif
#if defined(JSON_TRY_USER)
    #undef JSON_TRY
    #define JSON_TRY JSON_TRY_USER
#endif
#if defined(JSON_CATCH_USER)
    #undef JSON_CATCH
    #define JSON_CATCH JSON_CATCH_USER
#endif

// manual branch prediction
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define JSON_LIKELY(x)      __builtin_expect(!!(x), 1)
    #define JSON_UNLIKELY(x)    __builtin_expect(!!(x), 0)
#else
    #define JSON_LIKELY(x)      x
    #define JSON_UNLIKELY(x)    x
#endif

// C++ language standard detection
#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
    #define JSON_HAS_CPP_17
    #define JSON_HAS_CPP_14
#elif (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_HAS_CXX14) && _HAS_CXX14 == 1)
    #define JSON_HAS_CPP_14
#endif

/*!
@brief Helper to determine whether there's a key_type for T.

This helper is used to tell associative containers apart from other containers
such as sequence containers. For instance, `std::map` passes the test as it
contains a `mapped_type`, whereas `std::vector` fails the test.

@sa http://stackoverflow.com/a/7728728/266378
@since version 1.0.0, overworked in version 2.0.6
*/
#define NLOHMANN_JSON_HAS_HELPER(type)                                        \
    template<typename T> struct has_##type {                                  \
    private:                                                                  \
        template<typename U, typename = typename U::type>                     \
        static int detect(U &&);                                              \
        static void detect(...);                                              \
    public:                                                                   \
        static constexpr bool value =                                         \
                std::is_integral<decltype(detect(std::declval<T>()))>::value; \
    }

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

// implementation of C++14 index_sequence and affiliates
// source: https://stackoverflow.com/a/32223343
template<std::size_t... Ints>
struct index_sequence
{
    using type = index_sequence;
    using value_type = std::size_t;
    static constexpr std::size_t size() noexcept
    {
        return sizeof...(Ints);
    }
};

template<class Sequence1, class Sequence2>
struct merge_and_renumber;

template<std::size_t... I1, std::size_t... I2>
struct merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
        : index_sequence < I1..., (sizeof...(I1) + I2)... > {};

template<std::size_t N>
struct make_index_sequence
    : merge_and_renumber < typename make_index_sequence < N / 2 >::type,
      typename make_index_sequence < N - N / 2 >::type > {};

template<> struct make_index_sequence<0> : index_sequence<> {};
template<> struct make_index_sequence<1> : index_sequence<0> {};

template<typename... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

/*
Implementation of two C++17 constructs: conjunction, negation. This is needed
to avoid evaluating all the traits in a condition

For example: not std::is_same<void, T>::value and has_value_type<T>::value
will not compile when T = void (on MSVC at least). Whereas
conjunction<negation<std::is_same<void, T>>, has_value_type<T>>::value will
stop evaluating if negation<...>::value == false

Please note that those constructs must be used with caution, since symbols can
become very long quickly (which can slow down compilation and cause MSVC
internal compiler errors). Only use it when you have to (see example ahead).
*/
template<class...> struct conjunction : std::true_type {};
template<class B1> struct conjunction<B1> : B1 {};
template<class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

template<class B> struct negation : std::integral_constant<bool, not B::value> {};

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
        std::is_constructible<StringRef, typename CompatibleObjectType::key_type>::value and
        std::is_constructible<typename RealType::mapped_type, typename CompatibleObjectType::mapped_type>::value;
};

template<class BasicJsonType, class CompatibleObjectType>
struct is_compatible_object_type
{
    static auto constexpr value = is_compatible_object_type_impl <
                                  conjunction<negation<std::is_same<void, CompatibleObjectType>>,
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
        conjunction<negation<std::is_same<void, CompatibleArrayType>>,
        negation<is_compatible_object_type<
        BasicJsonType, CompatibleArrayType>>,
        negation<std::is_constructible<StringRef,
        CompatibleArrayType>>,
        negation<is_json_nested_type<BasicJsonType, CompatibleArrayType>>,
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
    : conjunction<is_complete_type<CompatibleType>,
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

////////////////
// exceptions //
////////////////

/*!
@brief general exception of the @ref json class

This class is an extension of `std::exception` objects with a member @a id for
exception ids. It is used as the base class for all exceptions thrown by the
@ref json class. This class can hence be used as "wildcard" to catch
exceptions.

Subclasses:
- @ref parse_error for exceptions indicating a parse error
- @ref invalid_iterator for exceptions indicating errors with iterators
- @ref type_error for exceptions indicating executing a member function with
                  a wrong type
- @ref out_of_range for exceptions indicating access out of the defined range
- @ref other_error for exceptions indicating other library errors

@internal
@note To have nothrow-copy-constructible exceptions, we internally use
      `std::runtime_error` which can cope with arbitrary-length error messages.
      Intermediate strings are built with static functions and then passed to
      the actual constructor.
@endinternal

@liveexample{The following code shows how arbitrary library exceptions can be
caught.,exception}

@since version 3.0.0
*/
class exception : public std::exception
{
  public:
    /// returns the explanatory string
    const char* what() const noexcept override
    {
        return m.what();
    }

    /// the id of the exception
    const int id;

  protected:
    exception(int id_, const Twine& what_arg);

  private:
    /// an exception object as storage for error messages
    std::runtime_error m;
};

/*!
@brief exception indicating a parse error

This exception is thrown by the library when a parse error occurs. Parse errors
can occur during the deserialization of JSON text, CBOR, MessagePack, as well
as when using JSON Patch.

Member @a byte holds the byte index of the last read character in the input
file.

Exceptions have ids 1xx.

name / id                      | example message | description
------------------------------ | --------------- | -------------------------
json.exception.parse_error.101 | parse error at 2: unexpected end of input; expected string literal | This error indicates a syntax error while deserializing a JSON text. The error message describes that an unexpected token (character) was encountered, and the member @a byte indicates the error position.
json.exception.parse_error.102 | parse error at 14: missing or wrong low surrogate | JSON uses the `\uxxxx` format to describe Unicode characters. Code points above above 0xFFFF are split into two `\uxxxx` entries ("surrogate pairs"). This error indicates that the surrogate pair is incomplete or contains an invalid code point.
json.exception.parse_error.103 | parse error: code points above 0x10FFFF are invalid | Unicode supports code points up to 0x10FFFF. Code points above 0x10FFFF are invalid.
json.exception.parse_error.104 | parse error: JSON patch must be an array of objects | [RFC 6902](https://tools.ietf.org/html/rfc6902) requires a JSON Patch document to be a JSON document that represents an array of objects.
json.exception.parse_error.105 | parse error: operation must have string member 'op' | An operation of a JSON Patch document must contain exactly one "op" member, whose value indicates the operation to perform. Its value must be one of "add", "remove", "replace", "move", "copy", or "test"; other values are errors.
json.exception.parse_error.106 | parse error: array index '01' must not begin with '0' | An array index in a JSON Pointer ([RFC 6901](https://tools.ietf.org/html/rfc6901)) may be `0` or any number without a leading `0`.
json.exception.parse_error.107 | parse error: JSON pointer must be empty or begin with '/' - was: 'foo' | A JSON Pointer must be a Unicode string containing a sequence of zero or more reference tokens, each prefixed by a `/` character.
json.exception.parse_error.108 | parse error: escape character '~' must be followed with '0' or '1' | In a JSON Pointer, only `~0` and `~1` are valid escape sequences.
json.exception.parse_error.109 | parse error: array index 'one' is not a number | A JSON Pointer array index must be a number.
json.exception.parse_error.110 | parse error at 1: cannot read 2 bytes from vector | When parsing CBOR or MessagePack, the byte vector ends before the complete value has been read.
json.exception.parse_error.112 | parse error at 1: error reading CBOR; last byte: 0xF8 | Not all types of CBOR or MessagePack are supported. This exception occurs if an unsupported byte was read.
json.exception.parse_error.113 | parse error at 2: expected a CBOR string; last byte: 0x98 | While parsing a map key, a value that is not a string has been read.

@note For an input with n bytes, 1 is the index of the first character and n+1
      is the index of the terminating null byte or the end of file. This also
      holds true when reading a byte vector (CBOR or MessagePack).

@liveexample{The following code shows how a `parse_error` exception can be
caught.,parse_error}

@sa @ref exception for the base class of the library exceptions
@sa @ref invalid_iterator for exceptions indicating errors with iterators
@sa @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa @ref out_of_range for exceptions indicating access out of the defined range
@sa @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class parse_error : public exception
{
  public:
    /*!
    @brief create a parse error exception
    @param[in] id_       the id of the exception
    @param[in] byte_     the byte index where the error occurred (or 0 if the
                         position cannot be determined)
    @param[in] what_arg  the explanatory string
    @return parse_error object
    */
    static parse_error create(int id_, std::size_t byte_, const Twine& what_arg);

    /*!
    @brief byte index of the parse error

    The byte index of the last read character in the input file.

    @note For an input with n bytes, 1 is the index of the first character and
          n+1 is the index of the terminating null byte or the end of file.
          This also holds true when reading a byte vector (CBOR or MessagePack).
    */
    const std::size_t byte;

  private:
    parse_error(int id_, std::size_t byte_, const Twine& what_arg)
        : exception(id_, what_arg), byte(byte_) {}
};

/*!
@brief exception indicating errors with iterators

This exception is thrown if iterators passed to a library function do not match
the expected semantics.

Exceptions have ids 2xx.

name / id                           | example message | description
----------------------------------- | --------------- | -------------------------
json.exception.invalid_iterator.201 | iterators are not compatible | The iterators passed to constructor @ref json(InputIT first, InputIT last) are not compatible, meaning they do not belong to the same container. Therefore, the range (@a first, @a last) is invalid.
json.exception.invalid_iterator.202 | iterator does not fit current value | In an erase or insert function, the passed iterator @a pos does not belong to the JSON value for which the function was called. It hence does not define a valid position for the deletion/insertion.
json.exception.invalid_iterator.203 | iterators do not fit current value | Either iterator passed to function @ref erase(IteratorType first, IteratorType last) does not belong to the JSON value from which values shall be erased. It hence does not define a valid range to delete values from.
json.exception.invalid_iterator.204 | iterators out of range | When an iterator range for a primitive type (number, boolean, or string) is passed to a constructor or an erase function, this range has to be exactly (@ref begin(), @ref end()), because this is the only way the single stored value is expressed. All other ranges are invalid.
json.exception.invalid_iterator.205 | iterator out of range | When an iterator for a primitive type (number, boolean, or string) is passed to an erase function, the iterator has to be the @ref begin() iterator, because it is the only way to address the stored value. All other iterators are invalid.
json.exception.invalid_iterator.206 | cannot construct with iterators from null | The iterators passed to constructor @ref json(InputIT first, InputIT last) belong to a JSON null value and hence to not define a valid range.
json.exception.invalid_iterator.207 | cannot use key() for non-object iterators | The key() member function can only be used on iterators belonging to a JSON object, because other types do not have a concept of a key.
json.exception.invalid_iterator.208 | cannot use operator[] for object iterators | The operator[] to specify a concrete offset cannot be used on iterators belonging to a JSON object, because JSON objects are unordered.
json.exception.invalid_iterator.209 | cannot use offsets with object iterators | The offset operators (+, -, +=, -=) cannot be used on iterators belonging to a JSON object, because JSON objects are unordered.
json.exception.invalid_iterator.210 | iterators do not fit | The iterator range passed to the insert function are not compatible, meaning they do not belong to the same container. Therefore, the range (@a first, @a last) is invalid.
json.exception.invalid_iterator.211 | passed iterators may not belong to container | The iterator range passed to the insert function must not be a subrange of the container to insert to.
json.exception.invalid_iterator.212 | cannot compare iterators of different containers | When two iterators are compared, they must belong to the same container.
json.exception.invalid_iterator.213 | cannot compare order of object iterators | The order of object iterators cannot be compared, because JSON objects are unordered.
json.exception.invalid_iterator.214 | cannot get value | Cannot get value for iterator: Either the iterator belongs to a null value or it is an iterator to a primitive type (number, boolean, or string), but the iterator is different to @ref begin().

@liveexample{The following code shows how an `invalid_iterator` exception can be
caught.,invalid_iterator}

@sa @ref exception for the base class of the library exceptions
@sa @ref parse_error for exceptions indicating a parse error
@sa @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa @ref out_of_range for exceptions indicating access out of the defined range
@sa @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class invalid_iterator : public exception
{
  public:
    static invalid_iterator create(int id_, const Twine& what_arg);

  private:
    invalid_iterator(int id_, const Twine& what_arg)
        : exception(id_, what_arg) {}
};

/*!
@brief exception indicating executing a member function with a wrong type

This exception is thrown in case of a type error; that is, a library function is
executed on a JSON value whose type does not match the expected semantics.

Exceptions have ids 3xx.

name / id                     | example message | description
----------------------------- | --------------- | -------------------------
json.exception.type_error.301 | cannot create object from initializer list | To create an object from an initializer list, the initializer list must consist only of a list of pairs whose first element is a string. When this constraint is violated, an array is created instead.
json.exception.type_error.302 | type must be object, but is array | During implicit or explicit value conversion, the JSON type must be compatible to the target type. For instance, a JSON string can only be converted into string types, but not into numbers or boolean types.
json.exception.type_error.303 | incompatible ReferenceType for get_ref, actual type is object | To retrieve a reference to a value stored in a @ref json object with @ref get_ref, the type of the reference must match the value type. For instance, for a JSON array, the @a ReferenceType must be @ref array_t&.
json.exception.type_error.304 | cannot use at() with string | The @ref at() member functions can only be executed for certain JSON types.
json.exception.type_error.305 | cannot use operator[] with string | The @ref operator[] member functions can only be executed for certain JSON types.
json.exception.type_error.306 | cannot use value() with string | The @ref value() member functions can only be executed for certain JSON types.
json.exception.type_error.307 | cannot use erase() with string | The @ref erase() member functions can only be executed for certain JSON types.
json.exception.type_error.308 | cannot use push_back() with string | The @ref push_back() and @ref operator+= member functions can only be executed for certain JSON types.
json.exception.type_error.309 | cannot use insert() with | The @ref insert() member functions can only be executed for certain JSON types.
json.exception.type_error.310 | cannot use swap() with number | The @ref swap() member functions can only be executed for certain JSON types.
json.exception.type_error.311 | cannot use emplace_back() with string | The @ref emplace_back() member function can only be executed for certain JSON types.
json.exception.type_error.312 | cannot use update() with string | The @ref update() member functions can only be executed for certain JSON types.
json.exception.type_error.313 | invalid value to unflatten | The @ref unflatten function converts an object whose keys are JSON Pointers back into an arbitrary nested JSON value. The JSON Pointers must not overlap, because then the resulting value would not be well defined.
json.exception.type_error.314 | only objects can be unflattened | The @ref unflatten function only works for an object whose keys are JSON Pointers.
json.exception.type_error.315 | values in object must be primitive | The @ref unflatten function only works for an object whose keys are JSON Pointers and whose values are primitive.
json.exception.type_error.316 | invalid UTF-8 byte at index 10: 0x7E | The @ref dump function only works with UTF-8 encoded strings; that is, if you assign a `std::string` to a JSON value, make sure it is UTF-8 encoded. |

@liveexample{The following code shows how a `type_error` exception can be
caught.,type_error}

@sa @ref exception for the base class of the library exceptions
@sa @ref parse_error for exceptions indicating a parse error
@sa @ref invalid_iterator for exceptions indicating errors with iterators
@sa @ref out_of_range for exceptions indicating access out of the defined range
@sa @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class type_error : public exception
{
  public:
    static type_error create(int id_, const Twine& what_arg);

  private:
    type_error(int id_, const Twine& what_arg) : exception(id_, what_arg) {}
};

/*!
@brief exception indicating access out of the defined range

This exception is thrown in case a library function is called on an input
parameter that exceeds the expected range, for instance in case of array
indices or nonexisting object keys.

Exceptions have ids 4xx.

name / id                       | example message | description
------------------------------- | --------------- | -------------------------
json.exception.out_of_range.401 | array index 3 is out of range | The provided array index @a i is larger than @a size-1.
json.exception.out_of_range.402 | array index '-' (3) is out of range | The special array index `-` in a JSON Pointer never describes a valid element of the array, but the index past the end. That is, it can only be used to add elements at this position, but not to read it.
json.exception.out_of_range.403 | key 'foo' not found | The provided key was not found in the JSON object.
json.exception.out_of_range.404 | unresolved reference token 'foo' | A reference token in a JSON Pointer could not be resolved.
json.exception.out_of_range.405 | JSON pointer has no parent | The JSON Patch operations 'remove' and 'add' can not be applied to the root element of the JSON value.
json.exception.out_of_range.406 | number overflow parsing '10E1000' | A parsed number could not be stored as without changing it to NaN or INF.
json.exception.out_of_range.407 | number overflow serializing '9223372036854775808' | UBJSON only supports integers numbers up to 9223372036854775807. |
json.exception.out_of_range.408 | excessive array size: 8658170730974374167 | The size (following `#`) of an UBJSON array or object exceeds the maximal capacity. |

@liveexample{The following code shows how an `out_of_range` exception can be
caught.,out_of_range}

@sa @ref exception for the base class of the library exceptions
@sa @ref parse_error for exceptions indicating a parse error
@sa @ref invalid_iterator for exceptions indicating errors with iterators
@sa @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class out_of_range : public exception
{
  public:
    static out_of_range create(int id_, const Twine& what_arg);

  private:
    out_of_range(int id_, const Twine& what_arg) : exception(id_, what_arg) {}
};

/*!
@brief exception indicating other library errors

This exception is thrown in case of errors that cannot be classified with the
other exception types.

Exceptions have ids 5xx.

name / id                      | example message | description
------------------------------ | --------------- | -------------------------
json.exception.other_error.501 | unsuccessful: {"op":"test","path":"/baz", "value":"bar"} | A JSON Patch operation 'test' failed. The unsuccessful operation is also printed.

@sa @ref exception for the base class of the library exceptions
@sa @ref parse_error for exceptions indicating a parse error
@sa @ref invalid_iterator for exceptions indicating errors with iterators
@sa @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa @ref out_of_range for exceptions indicating access out of the defined range

@liveexample{The following code shows how an `other_error` exception can be
caught.,other_error}

@since version 3.0.0
*/
class other_error : public exception
{
  public:
    static other_error create(int id_, const Twine& what_arg);

  private:
    other_error(int id_, const Twine& what_arg) : exception(id_, what_arg) {}
};

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
            JSON_THROW(type_error::create(302, "type must be number, but is " + Twine(j.type_name())));
    }
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, bool& b)
{
    if (JSON_UNLIKELY(not j.is_boolean()))
    {
        JSON_THROW(type_error::create(302, "type must be boolean, but is " + Twine(j.type_name())));
    }
    b = *j.template get_ptr<const bool*>();
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, std::string& s)
{
    if (JSON_UNLIKELY(not j.is_string()))
    {
        JSON_THROW(type_error::create(302, "type must be string, but is " + Twine(j.type_name())));
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
        JSON_THROW(type_error::create(302, "type must be array, but is " + Twine(j.type_name())));
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
        JSON_THROW(type_error::create(302, "type must be array, but is " +
                                      Twine(j.type_name())));
    }

    from_json_array_impl(j, arr, priority_tag<2> {});
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, typename BasicJsonType::object_t& obj)
{
    if (!j.is_object())
    {
        JSON_THROW(type_error::create(302, "type must be object, but is " + Twine(j.type_name())));
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
        JSON_THROW(type_error::create(302, "type must be object, but is " + Twine(j.type_name())));
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
            JSON_THROW(type_error::create(302, "type must be number, but is " + Twine(j.type_name())));
    }
}

template<typename BasicJsonType, typename A1, typename A2>
void from_json(const BasicJsonType& j, std::pair<A1, A2>& p)
{
    p = {j.at(0).template get<A1>(), j.at(1).template get<A2>()};
}

template<typename BasicJsonType, typename Tuple, std::size_t... Idx>
void from_json_tuple_impl(const BasicJsonType& j, Tuple& t, index_sequence<Idx...>)
{
    t = std::make_tuple(j.at(Idx).template get<typename std::tuple_element<Idx, Tuple>::type>()...);
}

template<typename BasicJsonType, typename... Args>
void from_json(const BasicJsonType& j, std::tuple<Args...>& t)
{
    from_json_tuple_impl(j, t, index_sequence_for<Args...> {});
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

// namespace to hold default `from_json` function
// to see why this is required:
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
namespace
{
constexpr const auto& from_json = detail::static_const<detail::from_json_fn>::value;
}

namespace detail
{
//////////////////
// constructors //
//////////////////

template<value_t> struct external_constructor;

template<>
struct external_constructor<value_t::boolean>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, bool b) noexcept
    {
        j.m_type = value_t::boolean;
        j.m_value = b;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::string>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, StringRef s)
    {
        j.m_type = value_t::string;
        j.m_value = s;
        j.assert_invariant();
    }

    template<typename BasicJsonType, typename T,
             enable_if_t<std::is_same<std::string, T>::value, int> = 0>
    static void construct(BasicJsonType& j, T&& s)
    {
        j.m_type = value_t::string;
        j.m_value = std::move(s);
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::number_float>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, double val) noexcept
    {
        j.m_type = value_t::number_float;
        j.m_value = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::number_unsigned>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, uint64_t val) noexcept
    {
        j.m_type = value_t::number_unsigned;
        j.m_value = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::number_integer>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, int64_t val) noexcept
    {
        j.m_type = value_t::number_integer;
        j.m_value = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::array>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const typename BasicJsonType::array_t& arr)
    {
        j.m_type = value_t::array;
        j.m_value = arr;
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::array_t&& arr)
    {
        j.m_type = value_t::array;
        j.m_value = std::move(arr);
        j.assert_invariant();
    }

    template<typename BasicJsonType, typename T>
    static void construct(BasicJsonType& j, ArrayRef<T> arr)
    {
        using std::begin;
        using std::end;
        j.m_type = value_t::array;
        j.m_value.array = j.template create<typename BasicJsonType::array_t>(begin(arr), end(arr));
        j.assert_invariant();
    }

    template<typename BasicJsonType, typename CompatibleArrayType,
             enable_if_t<not std::is_same<CompatibleArrayType, typename BasicJsonType::array_t>::value,
                         int> = 0>
    static void construct(BasicJsonType& j, const CompatibleArrayType& arr)
    {
        using std::begin;
        using std::end;
        j.m_type = value_t::array;
        j.m_value.array = j.template create<typename BasicJsonType::array_t>(begin(arr), end(arr));
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const std::vector<bool>& arr)
    {
        j.m_type = value_t::array;
        j.m_value = value_t::array;
        j.m_value.array->reserve(arr.size());
        for (const bool x : arr)
        {
            j.m_value.array->push_back(x);
        }
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::object>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const typename BasicJsonType::object_t& obj)
    {
        j.m_type = value_t::object;
        j.m_value = obj;
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::object_t&& obj)
    {
        j.m_type = value_t::object;
        j.m_value = std::move(obj);
        j.assert_invariant();
    }

    template<typename BasicJsonType, typename CompatibleObjectType,
             enable_if_t<not std::is_same<CompatibleObjectType, typename BasicJsonType::object_t>::value, int> = 0>
    static void construct(BasicJsonType& j, const CompatibleObjectType& obj)
    {
        j.m_type = value_t::object;
        j.m_value = value_t::object;
        for (const auto& x : obj)
        {
            j.m_value.object->try_emplace(x.first, x.second);
        }
        j.assert_invariant();
    }
};

/////////////
// to_json //
/////////////

template<typename BasicJsonType, typename T,
         enable_if_t<std::is_same<T, bool>::value, int> = 0>
void to_json(BasicJsonType& j, T b) noexcept
{
    external_constructor<value_t::boolean>::construct(j, b);
}

template<typename BasicJsonType, typename CompatibleString,
         enable_if_t<std::is_constructible<StringRef, CompatibleString>::value, int> = 0>
void to_json(BasicJsonType& j, const CompatibleString& s)
{
    external_constructor<value_t::string>::construct(j, s);
}

template<typename BasicJsonType, typename T,
         enable_if_t<std::is_same<std::string, T>::value, int> = 0>
void to_json(BasicJsonType& j, T&& s)
{
    external_constructor<value_t::string>::construct(j, std::move(s));
}

template<typename BasicJsonType, typename FloatType,
         enable_if_t<std::is_floating_point<FloatType>::value, int> = 0>
void to_json(BasicJsonType& j, FloatType val) noexcept
{
    external_constructor<value_t::number_float>::construct(j, static_cast<double>(val));
}

template<typename BasicJsonType, typename CompatibleNumberUnsignedType,
         enable_if_t<is_compatible_integer_type<uint64_t, CompatibleNumberUnsignedType>::value, int> = 0>
void to_json(BasicJsonType& j, CompatibleNumberUnsignedType val) noexcept
{
    external_constructor<value_t::number_unsigned>::construct(j, static_cast<uint64_t>(val));
}

template<typename BasicJsonType, typename CompatibleNumberIntegerType,
         enable_if_t<is_compatible_integer_type<int64_t, CompatibleNumberIntegerType>::value, int> = 0>
void to_json(BasicJsonType& j, CompatibleNumberIntegerType val) noexcept
{
    external_constructor<value_t::number_integer>::construct(j, static_cast<int64_t>(val));
}

template<typename BasicJsonType, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void to_json(BasicJsonType& j, EnumType e) noexcept
{
    using underlying_type = typename std::underlying_type<EnumType>::type;
    external_constructor<value_t::number_integer>::construct(j, static_cast<underlying_type>(e));
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, const std::vector<bool>& e)
{
    external_constructor<value_t::array>::construct(j, e);
}

template<typename BasicJsonType, typename CompatibleArrayType,
         enable_if_t<is_compatible_array_type<BasicJsonType, CompatibleArrayType>::value or
                     std::is_same<typename BasicJsonType::array_t, CompatibleArrayType>::value,
                     int> = 0>
void to_json(BasicJsonType& j, const CompatibleArrayType& arr)
{
    external_constructor<value_t::array>::construct(j, arr);
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, typename BasicJsonType::array_t&& arr)
{
    external_constructor<value_t::array>::construct(j, std::move(arr));
}

template<typename BasicJsonType, typename CompatibleObjectType,
         enable_if_t<is_compatible_object_type<BasicJsonType, CompatibleObjectType>::value, int> = 0>
void to_json(BasicJsonType& j, const CompatibleObjectType& obj)
{
    external_constructor<value_t::object>::construct(j, obj);
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, typename BasicJsonType::object_t&& obj)
{
    external_constructor<value_t::object>::construct(j, std::move(obj));
}

template<typename BasicJsonType, typename T, std::size_t N,
         enable_if_t<not std::is_constructible<StringRef, T (&)[N]>::value, int> = 0>
void to_json(BasicJsonType& j, T (&arr)[N])
{
    external_constructor<value_t::array>::construct(j, arr);
}

template<typename BasicJsonType, typename... Args>
void to_json(BasicJsonType& j, const std::pair<Args...>& p)
{
    j = {p.first, p.second};
}

template<typename BasicJsonType, typename Tuple, std::size_t... Idx>
void to_json_tuple_impl(BasicJsonType& j, const Tuple& t, index_sequence<Idx...>)
{
    j = {std::get<Idx>(t)...};
}

template<typename BasicJsonType, typename... Args>
void to_json(BasicJsonType& j, const std::tuple<Args...>& t)
{
    to_json_tuple_impl(j, t, index_sequence_for<Args...> {});
}

struct to_json_fn
{
  private:
    template<typename BasicJsonType, typename T>
    auto call(BasicJsonType& j, T&& val, priority_tag<1> /*unused*/) const noexcept(noexcept(to_json(j, std::forward<T>(val))))
    -> decltype(to_json(j, std::forward<T>(val)), void())
    {
        return to_json(j, std::forward<T>(val));
    }

    template<typename BasicJsonType, typename T>
    void call(BasicJsonType& /*unused*/, T&& /*unused*/, priority_tag<0> /*unused*/) const noexcept
    {
        static_assert(sizeof(BasicJsonType) == 0,
                      "could not find to_json() method in T's namespace");

#ifdef _MSC_VER
        // MSVC does not show a stacktrace for the above assert
        using decayed = uncvref_t<T>;
        static_assert(sizeof(typename decayed::force_msvc_stacktrace) == 0,
                      "forcing MSVC stacktrace to show which T we're talking about.");
#endif
    }

  public:
    template<typename BasicJsonType, typename T>
    void operator()(BasicJsonType& j, T&& val) const
    noexcept(noexcept(std::declval<to_json_fn>().call(j, std::forward<T>(val), priority_tag<1> {})))
    {
        return call(j, std::forward<T>(val), priority_tag<1> {});
    }
};
}

// namespace to hold default `to_json` function
namespace
{
constexpr const auto& to_json = detail::static_const<detail::to_json_fn>::value;
}

namespace detail
{
/*
@brief an iterator for primitive JSON types

This class models an iterator for primitive JSON types (boolean, number,
string). It's only purpose is to allow the iterator/const_iterator classes
to "iterate" over primitive values. Internally, the iterator is modeled by
a `difference_type` variable. Value begin_value (`0`) models the begin,
end_value (`1`) models past the end.
*/
class primitive_iterator_t
{
  private:
    using difference_type = std::ptrdiff_t;
    static constexpr difference_type begin_value = 0;
    static constexpr difference_type end_value = begin_value + 1;

    /// iterator as signed integer type
    difference_type m_it = (std::numeric_limits<std::ptrdiff_t>::min)();

  public:
    constexpr difference_type get_value() const noexcept
    {
        return m_it;
    }

    /// set iterator to a defined beginning
    void set_begin() noexcept
    {
        m_it = begin_value;
    }

    /// set iterator to a defined past the end
    void set_end() noexcept
    {
        m_it = end_value;
    }

    /// return whether the iterator can be dereferenced
    constexpr bool is_begin() const noexcept
    {
        return m_it == begin_value;
    }

    /// return whether the iterator is at end
    constexpr bool is_end() const noexcept
    {
        return m_it == end_value;
    }

    friend constexpr bool operator==(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
    {
        return lhs.m_it == rhs.m_it;
    }

    friend constexpr bool operator<(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
    {
        return lhs.m_it < rhs.m_it;
    }

    primitive_iterator_t operator+(difference_type n) noexcept
    {
        auto result = *this;
        result += n;
        return result;
    }

    friend constexpr difference_type operator-(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
    {
        return lhs.m_it - rhs.m_it;
    }

    primitive_iterator_t& operator++() noexcept
    {
        ++m_it;
        return *this;
    }

    primitive_iterator_t const operator++(int) noexcept
    {
        auto result = *this;
        m_it++;
        return result;
    }

    primitive_iterator_t& operator--() noexcept
    {
        --m_it;
        return *this;
    }

    primitive_iterator_t const operator--(int) noexcept
    {
        auto result = *this;
        m_it--;
        return result;
    }

    primitive_iterator_t& operator+=(difference_type n) noexcept
    {
        m_it += n;
        return *this;
    }

    primitive_iterator_t& operator-=(difference_type n) noexcept
    {
        m_it -= n;
        return *this;
    }
};

/*!
@brief an iterator value

@note This structure could easily be a union, but MSVC currently does not allow
unions members with complex constructors, see https://github.com/nlohmann/json/pull/105.
*/
template<typename BasicJsonType> struct internal_iterator
{
    /// iterator for JSON objects
    typename BasicJsonType::object_t::iterator object_iterator {};
    /// iterator for JSON arrays
    typename BasicJsonType::array_t::iterator array_iterator {};
    /// generic iterator for all other types
    primitive_iterator_t primitive_iterator {};
};

// forward declare, to be able to friend it later on
template<typename IteratorType> class iteration_proxy;

/*!
@brief a template for a bidirectional iterator for the @ref json class

This class implements a both iterators (iterator and const_iterator) for the
@ref json class.

@note An iterator is called *initialized* when a pointer to a JSON value has
      been set (e.g., by a constructor or a copy assignment). If the iterator is
      default-constructed, it is *uninitialized* and most methods are undefined.
      **The library uses assertions to detect calls on uninitialized iterators.**

@requirement The class satisfies the following concept requirements:
-
[BidirectionalIterator](http://en.cppreference.com/w/cpp/concept/BidirectionalIterator):
  The iterator that can be moved can be moved in both directions (i.e.
  incremented and decremented).

@since version 1.0.0, simplified in version 2.0.9, change to bidirectional
       iterators in version 3.0.0 (see https://github.com/nlohmann/json/issues/593)
*/
template<typename BasicJsonType>
class iter_impl
{
    /// allow json to access private members
    friend iter_impl<typename std::conditional<std::is_const<BasicJsonType>::value, typename std::remove_const<BasicJsonType>::type, const BasicJsonType>::type>;
    friend BasicJsonType;
    friend iteration_proxy<iter_impl>;
    friend class ::wpi::JsonTest;

    using object_t = typename BasicJsonType::object_t;
    using array_t = typename BasicJsonType::array_t;
    // make sure BasicJsonType is json or const json
    static_assert(is_json<typename std::remove_const<BasicJsonType>::type>::value,
                  "iter_impl only accepts (const) json");

  public:

    /// The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17.
    /// The C++ Standard has never required user-defined iterators to derive from std::iterator.
    /// A user-defined iterator should provide publicly accessible typedefs named
    /// iterator_category, value_type, difference_type, pointer, and reference.
    /// Note that value_type is required to be non-const, even for constant iterators.
    using iterator_category = std::bidirectional_iterator_tag;

    /// the type of the values when the iterator is dereferenced
    using value_type = typename BasicJsonType::value_type;
    /// a type to represent differences between iterators
    using difference_type = typename BasicJsonType::difference_type;
    /// defines a pointer to the type iterated over (value_type)
    using pointer = typename std::conditional<std::is_const<BasicJsonType>::value,
          typename BasicJsonType::const_pointer,
          typename BasicJsonType::pointer>::type;
    /// defines a reference to the type iterated over (value_type)
    using reference =
        typename std::conditional<std::is_const<BasicJsonType>::value,
        typename BasicJsonType::const_reference,
        typename BasicJsonType::reference>::type;

    /// default constructor
    iter_impl() = default;

    /*!
    @brief constructor for a given JSON instance
    @param[in] object  pointer to a JSON object for this iterator
    @pre object != nullptr
    @post The iterator is initialized; i.e. `m_object != nullptr`.
    */
    explicit iter_impl(pointer object) noexcept : m_object(object)
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                m_it.object_iterator = typename object_t::iterator();
                break;
            }

            case value_t::array:
            {
                m_it.array_iterator = typename array_t::iterator();
                break;
            }

            default:
            {
                m_it.primitive_iterator = primitive_iterator_t();
                break;
            }
        }
    }

    /*!
    @note The conventional copy constructor and copy assignment are implicitly
          defined. Combined with the following converting constructor and
          assignment, they support: (1) copy from iterator to iterator, (2)
          copy from const iterator to const iterator, and (3) conversion from
          iterator to const iterator. However conversion from const iterator
          to iterator is not defined.
    */

    /*!
    @brief converting constructor
    @param[in] other  non-const iterator to copy from
    @note It is not checked whether @a other is initialized.
    */
    iter_impl(const iter_impl<typename std::remove_const<BasicJsonType>::type>& other) noexcept
        : m_object(other.m_object), m_it(other.m_it) {}

    /*!
    @brief converting assignment
    @param[in,out] other  non-const iterator to copy from
    @return const/non-const iterator
    @note It is not checked whether @a other is initialized.
    */
    iter_impl& operator=(const iter_impl<typename std::remove_const<BasicJsonType>::type>& other) noexcept
    {
        m_object = other.m_object;
        m_it = other.m_it;
        return *this;
    }

  private:
    /*!
    @brief set the iterator to the first value
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    void set_begin() noexcept
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                m_it.object_iterator = m_object->m_value.object->begin();
                break;
            }

            case value_t::array:
            {
                m_it.array_iterator = m_object->m_value.array->begin();
                break;
            }

            case value_t::null:
            {
                // set to end so begin()==end() is true: null is empty
                m_it.primitive_iterator.set_end();
                break;
            }

            default:
            {
                m_it.primitive_iterator.set_begin();
                break;
            }
        }
    }

    /*!
    @brief set the iterator past the last value
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    void set_end() noexcept
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                m_it.object_iterator = m_object->m_value.object->end();
                break;
            }

            case value_t::array:
            {
                m_it.array_iterator = m_object->m_value.array->end();
                break;
            }

            default:
            {
                m_it.primitive_iterator.set_end();
                break;
            }
        }
    }

  public:
    /*!
    @brief return a reference to the value pointed to by the iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference operator*() const
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                assert(m_it.object_iterator != m_object->m_value.object->end());
                return m_it.object_iterator->second;
            }

            case value_t::array:
            {
                assert(m_it.array_iterator != m_object->m_value.array->end());
                return *m_it.array_iterator;
            }

            case value_t::null:
                JSON_THROW(invalid_iterator::create(214, "cannot get value"));

            default:
            {
                if (JSON_LIKELY(m_it.primitive_iterator.is_begin()))
                {
                    return *m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value"));
            }
        }
    }

    /*!
    @brief dereference the iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    pointer operator->() const
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                assert(m_it.object_iterator != m_object->m_value.object->end());
                return &(m_it.object_iterator->second);
            }

            case value_t::array:
            {
                assert(m_it.array_iterator != m_object->m_value.array->end());
                return &*m_it.array_iterator;
            }

            default:
            {
                if (JSON_LIKELY(m_it.primitive_iterator.is_begin()))
                {
                    return m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value"));
            }
        }
    }

    /*!
    @brief post-increment (it++)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl const operator++(int)
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    /*!
    @brief pre-increment (++it)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator++()
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                ++m_it.object_iterator;
                break;
            }

            case value_t::array:
            {
                std::advance(m_it.array_iterator, 1);
                break;
            }

            default:
            {
                ++m_it.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief post-decrement (it--)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl const operator--(int)
    {
        auto result = *this;
        --(*this);
        return result;
    }

    /*!
    @brief pre-decrement (--it)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator--()
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                --m_it.object_iterator;
                break;
            }

            case value_t::array:
            {
                std::advance(m_it.array_iterator, -1);
                break;
            }

            default:
            {
                --m_it.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief  comparison: equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator==(const iter_impl& other) const
    {
        // if objects are not the same, the comparison is undefined
        if (JSON_UNLIKELY(m_object != other.m_object))
        {
            JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
        }

        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                return (m_it.object_iterator == other.m_it.object_iterator);

            case value_t::array:
                return (m_it.array_iterator == other.m_it.array_iterator);

            default:
                return (m_it.primitive_iterator == other.m_it.primitive_iterator);
        }
    }

    /*!
    @brief  comparison: not equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator!=(const iter_impl& other) const
    {
        return not operator==(other);
    }

    /*!
    @brief  comparison: smaller
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator<(const iter_impl& other) const
    {
        // if objects are not the same, the comparison is undefined
        if (JSON_UNLIKELY(m_object != other.m_object))
        {
            JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
        }

        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(213, "cannot compare order of object iterators"));

            case value_t::array:
                return (m_it.array_iterator < other.m_it.array_iterator);

            default:
                return (m_it.primitive_iterator < other.m_it.primitive_iterator);
        }
    }

    /*!
    @brief  comparison: less than or equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator<=(const iter_impl& other) const
    {
        return not other.operator < (*this);
    }

    /*!
    @brief  comparison: greater than
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator>(const iter_impl& other) const
    {
        return not operator<=(other);
    }

    /*!
    @brief  comparison: greater than or equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator>=(const iter_impl& other) const
    {
        return not operator<(other);
    }

    /*!
    @brief  add to iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator+=(difference_type i)
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators"));

            case value_t::array:
            {
                std::advance(m_it.array_iterator, i);
                break;
            }

            default:
            {
                m_it.primitive_iterator += i;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief  subtract from iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator-=(difference_type i)
    {
        return operator+=(-i);
    }

    /*!
    @brief  add to iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl operator+(difference_type i) const
    {
        auto result = *this;
        result += i;
        return result;
    }

    /*!
    @brief  addition of distance and iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    friend iter_impl operator+(difference_type i, const iter_impl& it)
    {
        auto result = it;
        result += i;
        return result;
    }

    /*!
    @brief  subtract from iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl operator-(difference_type i) const
    {
        auto result = *this;
        result -= i;
        return result;
    }

    /*!
    @brief  return difference
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    difference_type operator-(const iter_impl& other) const
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators"));

            case value_t::array:
                return m_it.array_iterator - other.m_it.array_iterator;

            default:
                return m_it.primitive_iterator - other.m_it.primitive_iterator;
        }
    }

    /*!
    @brief  access to successor
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference operator[](difference_type n) const
    {
        assert(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(208, "cannot use operator[] for object iterators"));

            case value_t::array:
                return *std::next(m_it.array_iterator, n);

            case value_t::null:
                JSON_THROW(invalid_iterator::create(214, "cannot get value"));

            default:
            {
                if (JSON_LIKELY(m_it.primitive_iterator.get_value() == -n))
                {
                    return *m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value"));
            }
        }
    }

    /*!
    @brief  return the key of an object iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    StringRef key() const
    {
        assert(m_object != nullptr);

        if (JSON_LIKELY(m_object->is_object()))
        {
            return m_it.object_iterator->first();
        }

        JSON_THROW(invalid_iterator::create(207, "cannot use key() for non-object iterators"));
    }

    /*!
    @brief  return the value of an iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference value() const
    {
        return operator*();
    }

  private:
    /// associated JSON instance
    pointer m_object = nullptr;
    /// the actual iterator of the associated instance
    internal_iterator<typename std::remove_const<BasicJsonType>::type> m_it;
};

/// proxy class for the items() function
template<typename IteratorType> class iteration_proxy
{
  private:
    /// helper class for iteration
    class iteration_proxy_internal
    {
      private:
        /// the iterator
        IteratorType anchor;
        /// an index for arrays (used to create key names)
        std::size_t array_index = 0;

      public:
        explicit iteration_proxy_internal(IteratorType it) noexcept : anchor(it) {}

        /// dereference operator (needed for range-based for)
        iteration_proxy_internal& operator*()
        {
            return *this;
        }

        /// increment operator (needed for range-based for)
        iteration_proxy_internal& operator++()
        {
            ++anchor;
            ++array_index;

            return *this;
        }

        /// inequality operator (needed for range-based for)
        bool operator!=(const iteration_proxy_internal& o) const noexcept
        {
            return anchor != o.anchor;
        }

        /// return key of the iterator
        std::string key() const
        {
            assert(anchor.m_object != nullptr);

            switch (anchor.m_object->type())
            {
                // use integer array index as key
                case value_t::array:
                    return std::to_string(array_index);

                // use key from the object
                case value_t::object:
                    return anchor.key();

                // use an empty key for all primitive types
                default:
                    return "";
            }
        }

        /// return value of the iterator
        typename IteratorType::reference value() const
        {
            return anchor.value();
        }
    };

    /// the container to iterate
    typename IteratorType::reference container;

  public:
    /// construct iteration proxy from a container
    explicit iteration_proxy(typename IteratorType::reference cont) noexcept
        : container(cont) {}

    /// return iterator begin (needed for range-based for)
    iteration_proxy_internal begin() noexcept
    {
        return iteration_proxy_internal(container.begin());
    }

    /// return iterator end (needed for range-based for)
    iteration_proxy_internal end() noexcept
    {
        return iteration_proxy_internal(container.end());
    }
};

//////////////////////
// reverse_iterator //
//////////////////////

/*!
@brief a template for a reverse iterator class

@tparam Base the base iterator type to reverse. Valid types are @ref
iterator (to create @ref reverse_iterator) and @ref const_iterator (to
create @ref const_reverse_iterator).

@requirement The class satisfies the following concept requirements:
-
[BidirectionalIterator](http://en.cppreference.com/w/cpp/concept/BidirectionalIterator):
  The iterator that can be moved can be moved in both directions (i.e.
  incremented and decremented).
- [OutputIterator](http://en.cppreference.com/w/cpp/concept/OutputIterator):
  It is possible to write to the pointed-to element (only if @a Base is
  @ref iterator).

@since version 1.0.0
*/
template<typename Base>
class json_reverse_iterator : public std::reverse_iterator<Base>
{
  public:
    using difference_type = std::ptrdiff_t;
    /// shortcut to the reverse iterator adapter
    using base_iterator = std::reverse_iterator<Base>;
    /// the reference type for the pointed-to element
    using reference = typename Base::reference;

    /// create reverse iterator from iterator
    json_reverse_iterator(const typename base_iterator::iterator_type& it) noexcept
        : base_iterator(it) {}

    /// create reverse iterator from base class
    json_reverse_iterator(const base_iterator& it) noexcept : base_iterator(it) {}

    /// post-increment (it++)
    json_reverse_iterator const operator++(int)
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator++(1));
    }

    /// pre-increment (++it)
    json_reverse_iterator& operator++()
    {
        return static_cast<json_reverse_iterator&>(base_iterator::operator++());
    }

    /// post-decrement (it--)
    json_reverse_iterator const operator--(int)
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator--(1));
    }

    /// pre-decrement (--it)
    json_reverse_iterator& operator--()
    {
        return static_cast<json_reverse_iterator&>(base_iterator::operator--());
    }

    /// add to iterator
    json_reverse_iterator& operator+=(difference_type i)
    {
        return static_cast<json_reverse_iterator&>(base_iterator::operator+=(i));
    }

    /// add to iterator
    json_reverse_iterator operator+(difference_type i) const
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator+(i));
    }

    /// subtract from iterator
    json_reverse_iterator operator-(difference_type i) const
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator-(i));
    }

    /// return difference
    difference_type operator-(const json_reverse_iterator& other) const
    {
        return base_iterator(*this) - base_iterator(other);
    }

    /// access to successor
    reference operator[](difference_type n) const
    {
        return *(this->operator+(n));
    }

    /// return the key of an object iterator
    auto key() const -> decltype(std::declval<Base>().key())
    {
        auto it = --this->base();
        return it.key();
    }

    /// return the value of an iterator
    reference value() const
    {
        auto it = --this->base();
        return it.operator * ();
    }
};

template<typename BasicJsonType>
class json_ref
{
  public:
    using value_type = BasicJsonType;

    json_ref(value_type&& value)
        : owned_value(std::move(value)), value_ref(&owned_value), is_rvalue(true)
    {}

    json_ref(const value_type& value)
        : value_ref(const_cast<value_type*>(&value)), is_rvalue(false)
    {}

    json_ref(std::initializer_list<json_ref> init)
        : owned_value(init), value_ref(&owned_value), is_rvalue(true)
    {}

    template<class... Args>
    json_ref(Args&& ... args)
        : owned_value(std::forward<Args>(args)...), value_ref(&owned_value), is_rvalue(true)
    {}

    // class should be movable only
    json_ref(json_ref&&) = default;
    json_ref(const json_ref&) = delete;
    json_ref& operator=(const json_ref&) = delete;

    value_type moved_or_copied() const
    {
        if (is_rvalue)
        {
            return std::move(*value_ref);
        }
        return *value_ref;
    }

    value_type const& operator*() const
    {
        return *static_cast<value_type const*>(value_ref);
    }

    value_type const* operator->() const
    {
        return static_cast<value_type const*>(value_ref);
    }

  private:
    mutable value_type owned_value = nullptr;
    value_type* value_ref = nullptr;
    const bool is_rvalue;
};
}  // namespace detail

class json_pointer
{
    // allow json to access private members
    friend class json;
    friend class JsonTest;

  public:
    /*!
    @brief create JSON pointer

    Create a JSON pointer according to the syntax described in
    [Section 3 of RFC6901](https://tools.ietf.org/html/rfc6901#section-3).

    @param[in] s  string representing the JSON pointer; if omitted, the empty
                  string is assumed which references the whole JSON value

    @throw parse_error.107 if the given JSON pointer @a s is nonempty and does
                           not begin with a slash (`/`); see example below

    @throw parse_error.108 if a tilde (`~`) in the given JSON pointer @a s is
    not followed by `0` (representing `~`) or `1` (representing `/`); see
    example below

    @liveexample{The example shows the construction several valid JSON pointers
    as well as the exceptional behavior.,json_pointer}

    @since version 2.0.0
    */
    explicit json_pointer(const Twine& s = {})
        : reference_tokens(split(s))
    {}

    /*!
    @brief return a string representation of the JSON pointer

    @invariant For each JSON pointer `ptr`, it holds:
    @code {.cpp}
    ptr == json_pointer(ptr.to_string());
    @endcode

    @return a string representation of the JSON pointer

    @liveexample{The example shows the result of `to_string`.,
    json_pointer__to_string}

    @since version 2.0.0
    */
    std::string to_string() const noexcept;

    /// @copydoc to_string()
    operator std::string() const
    {
        return to_string();
    }

    /*!
    @param[in] s  reference token to be converted into an array index

    @return integer representation of @a s

    @throw out_of_range.404 if string @a s could not be converted to an integer
    */
    static int array_index(const Twine& s);

  private:
    /*!
    @brief remove and return last reference pointer
    @throw out_of_range.405 if JSON pointer has no parent
    */
    std::string pop_back()
    {
        if (JSON_UNLIKELY(is_root()))
        {
            JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
        }

        auto last = reference_tokens.back();
        reference_tokens.pop_back();
        return last;
    }

    /// return whether pointer points to the root document
    bool is_root() const
    {
        return reference_tokens.empty();
    }

    json_pointer top() const
    {
        if (JSON_UNLIKELY(is_root()))
        {
            JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
        }

        json_pointer result = *this;
        result.reference_tokens = {reference_tokens[0]};
        return result;
    }

    /*!
    @brief create and return a reference to the pointed to value

    @complexity Linear in the number of reference tokens.

    @throw parse_error.109 if array index is not a number
    @throw type_error.313 if value cannot be unflattened
    */
    json& get_and_create(json& j) const;

    /*!
    @brief return a reference to the pointed to value

    @note This version does not throw if a value is not present, but tries to
          create nested values instead. For instance, calling this function
          with pointer `"/this/that"` on a null value is equivalent to calling
          `operator[]("this").operator[]("that")` on that value, effectively
          changing the null value to an object.

    @param[in] ptr  a JSON value

    @return reference to the JSON value pointed to by the JSON pointer

    @complexity Linear in the length of the JSON pointer.

    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    json& get_unchecked(json* ptr) const;

    /*!
    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.402  if the array index '-' is used
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    json& get_checked(json* ptr) const;

    /*!
    @brief return a const reference to the pointed to value

    @param[in] ptr  a JSON value

    @return const reference to the JSON value pointed to by the JSON
    pointer

    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.402  if the array index '-' is used
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    const json& get_unchecked(const json* ptr) const;

    /*!
    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.402  if the array index '-' is used
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    const json& get_checked(const json* ptr) const;

    /*!
    @brief split the string input to reference tokens

    @note This function is only called by the json_pointer constructor.
          All exceptions below are documented there.

    @throw parse_error.107  if the pointer is not empty or begins with '/'
    @throw parse_error.108  if character '~' is not followed by '0' or '1'
    */
    static std::vector<std::string> split(const Twine& reference_string);

    /*!
    @brief replace all occurrences of a substring by another string

    @param[in,out] s  the string to manipulate; changed so that all
                   occurrences of @a f are replaced with @a t
    @param[in]     f  the substring to replace with @a t
    @param[in]     t  the string to replace @a f

    @pre The search string @a f must not be empty. **This precondition is
    enforced with an assertion.**

    @since version 2.0.0
    */
    static void replace_substring(std::string& s, const std::string& f,
                                  const std::string& t);

    /// escape "~"" to "~0" and "/" to "~1"
    static std::string escape(std::string s);

    /// unescape "~1" to tilde and "~0" to slash (order is important!)
    static void unescape(std::string& s);

    /*!
    @param[in] reference_string  the reference string to the current value
    @param[in] value             the value to consider
    @param[in,out] result        the result object to insert values to

    @note Empty objects or arrays are flattened to `null`.
    */
    static void flatten(const Twine& reference_string,
                        const json& value,
                        json& result);

    /*!
    @param[in] value  flattened JSON

    @return unflattened JSON

    @throw parse_error.109 if array index is not a number
    @throw type_error.314  if value is not an object
    @throw type_error.315  if object values are not primitive
    @throw type_error.313  if value cannot be unflattened
    */
    static json
    unflatten(const json& value);

    friend bool operator==(json_pointer const& lhs,
                           json_pointer const& rhs) noexcept
    {
        return (lhs.reference_tokens == rhs.reference_tokens);
    }

    friend bool operator!=(json_pointer const& lhs,
                           json_pointer const& rhs) noexcept
    {
        return not (lhs == rhs);
    }

    /// the reference tokens
    std::vector<std::string> reference_tokens;
};

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

/*!
@brief a class to store JSON values

@requirement The class satisfies the following concept requirements:
- Basic
 - [DefaultConstructible](http://en.cppreference.com/w/cpp/concept/DefaultConstructible):
   JSON values can be default constructed. The result will be a JSON null
   value.
 - [MoveConstructible](http://en.cppreference.com/w/cpp/concept/MoveConstructible):
   A JSON value can be constructed from an rvalue argument.
 - [CopyConstructible](http://en.cppreference.com/w/cpp/concept/CopyConstructible):
   A JSON value can be copy-constructed from an lvalue expression.
 - [MoveAssignable](http://en.cppreference.com/w/cpp/concept/MoveAssignable):
   A JSON value van be assigned from an rvalue argument.
 - [CopyAssignable](http://en.cppreference.com/w/cpp/concept/CopyAssignable):
   A JSON value can be copy-assigned from an lvalue expression.
 - [Destructible](http://en.cppreference.com/w/cpp/concept/Destructible):
   JSON values can be destructed.
- Layout
 - [StandardLayoutType](http://en.cppreference.com/w/cpp/concept/StandardLayoutType):
   JSON values have
   [standard layout](http://en.cppreference.com/w/cpp/language/data_members#Standard_layout):
   All non-static data members are private and standard layout types, the
   class has no virtual functions or (virtual) base classes.
- Library-wide
 - [EqualityComparable](http://en.cppreference.com/w/cpp/concept/EqualityComparable):
   JSON values can be compared with `==`, see @ref
   operator==(const_reference,const_reference).
 - [LessThanComparable](http://en.cppreference.com/w/cpp/concept/LessThanComparable):
   JSON values can be compared with `<`, see @ref
   operator<(const_reference,const_reference).
 - [Swappable](http://en.cppreference.com/w/cpp/concept/Swappable):
   Any JSON lvalue or rvalue of can be swapped with any lvalue or rvalue of
   other compatible types, using unqualified function call @ref swap().
 - [NullablePointer](http://en.cppreference.com/w/cpp/concept/NullablePointer):
   JSON values can be compared against `std::nullptr_t` objects which are used
   to model the `null` value.
- Container
 - [Container](http://en.cppreference.com/w/cpp/concept/Container):
   JSON values can be used like STL containers and provide iterator access.
 - [ReversibleContainer](http://en.cppreference.com/w/cpp/concept/ReversibleContainer);
   JSON values can be used like STL containers and provide reverse iterator
   access.

@invariant The member variables @a m_value and @a m_type have the following
relationship:
- If `m_type == value_t::object`, then `m_value.object != nullptr`.
- If `m_type == value_t::array`, then `m_value.array != nullptr`.
- If `m_type == value_t::string`, then `m_value.string != nullptr`.
The invariants are checked by member function assert_invariant().

@internal
@note ObjectType trick from http://stackoverflow.com/a/9860911
@endinternal

@see [RFC 7159: The JavaScript Object Notation (JSON) Data Interchange
Format](http://rfc7159.net/rfc7159)

@since version 1.0.0

@nosubgrouping
*/
class json
{
  private:
    template<detail::value_t> friend struct detail::external_constructor;
    friend ::wpi::json_pointer;
    template<typename BasicJsonType>
    friend class ::wpi::detail::iter_impl;
    friend class JsonTest;

    /// workaround type for MSVC
    using json_t = json;

    // convenience aliases for types residing in namespace detail;
    using primitive_iterator_t = ::wpi::detail::primitive_iterator_t;
    template<typename BasicJsonType>
    using internal_iterator = ::wpi::detail::internal_iterator<BasicJsonType>;
    template<typename BasicJsonType>
    using iter_impl = ::wpi::detail::iter_impl<BasicJsonType>;
    template<typename Iterator>
    using iteration_proxy = ::wpi::detail::iteration_proxy<Iterator>;
    template<typename Base> using json_reverse_iterator = ::wpi::detail::json_reverse_iterator<Base>;

    class binary_reader;
    class binary_writer;
    class lexer;
    class parser;
    class serializer;

  public:
    using value_t = detail::value_t;
    /// @copydoc wpi::json_pointer
    using json_pointer = ::wpi::json_pointer;
    template<typename T, typename SFINAE>
    using json_serializer = adl_serializer<T, SFINAE>;
    /// helper type for initializer lists of json values
    using initializer_list_t = std::initializer_list<detail::json_ref<json>>;

    ////////////////
    // exceptions //
    ////////////////

    /// @name exceptions
    /// Classes to implement user-defined exceptions.
    /// @{

    /// @copydoc detail::exception
    using exception = detail::exception;
    /// @copydoc detail::parse_error
    using parse_error = detail::parse_error;
    /// @copydoc detail::invalid_iterator
    using invalid_iterator = detail::invalid_iterator;
    /// @copydoc detail::type_error
    using type_error = detail::type_error;
    /// @copydoc detail::out_of_range
    using out_of_range = detail::out_of_range;
    /// @copydoc detail::other_error
    using other_error = detail::other_error;

    /// @}


    /////////////////////
    // container types //
    /////////////////////

    /// @name container types
    /// The canonic container types to use @ref json like any other STL
    /// container.
    /// @{

    /// the type of elements in a json container
    using value_type = json;

    /// the type of an element reference
    using reference = value_type&;
    /// the type of an element const reference
    using const_reference = const value_type&;

    /// a type to represent differences between iterators
    using difference_type = std::ptrdiff_t;
    /// a type to represent container sizes
    using size_type = std::size_t;

    /// the allocator type
    using allocator_type = std::allocator<json>;

    /// the type of an element pointer
    using pointer = json*;
    /// the type of an element const pointer
    using const_pointer = const json*;

    /// an iterator for a json container
    using iterator = iter_impl<json>;
    /// a const iterator for a json container
    using const_iterator = iter_impl<const json>;
    /// a reverse iterator for a json container
    using reverse_iterator = json_reverse_iterator<typename json::iterator>;
    /// a const reverse iterator for a json container
    using const_reverse_iterator = json_reverse_iterator<typename json::const_iterator>;

    /// @}


    /*!
    @brief returns the allocator associated with the container
    */
    static allocator_type get_allocator()
    {
        return allocator_type();
    }

    /*!
    @brief returns version information on the library

    This function returns a JSON object with information about the library,
    including the version number and information on the platform and compiler.

    @return JSON object holding version information
    key         | description
    ----------- | ---------------
    `compiler`  | Information on the used compiler. It is an object with the following keys: `c++` (the used C++ standard), `family` (the compiler family; possible values are `clang`, `icc`, `gcc`, `ilecpp`, `msvc`, `pgcpp`, `sunpro`, and `unknown`), and `version` (the compiler version).
    `copyright` | The copyright line for the library as string.
    `name`      | The name of the library as string.
    `platform`  | The used platform as string. Possible values are `win32`, `linux`, `apple`, `unix`, and `unknown`.
    `url`       | The URL of the project as string.
    `version`   | The version of the library. It is an object with the following keys: `major`, `minor`, and `patch` as defined by [Semantic Versioning](http://semver.org), and `string` (the version string).

    @liveexample{The following code shows an example output of the `meta()`
    function.,meta}

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @complexity Constant.

    @since 2.1.0
    */
    static json meta();


    ///////////////////////////
    // JSON value data types //
    ///////////////////////////

    /// @name JSON value data types
    /// The data types to store a JSON value. These types are derived from
    /// the template arguments passed to class @ref json.
    /// @{

#if defined(JSON_HAS_CPP_14)
    // Use transparent comparator if possible, combined with perfect forwarding
    // on find() and count() calls prevents unnecessary string construction.
    using object_comparator_t = std::less<>;
#else
    using object_comparator_t = std::less<std::string>;
#endif

    /*!
    @brief a type for an object

    [RFC 7159](http://rfc7159.net/rfc7159) describes JSON objects as follows:
    > An object is an unordered collection of zero or more name/value pairs,
    > where a name is a string and a value is a string, number, boolean, null,
    > object, or array.

    #### Behavior

    The choice of @a object_t influences the behavior of the JSON class. With
    the default type, objects have the following behavior:

    - When all names are unique, objects will be interoperable in the sense
      that all software implementations receiving that object will agree on
      the name-value mappings.
    - When the names within an object are not unique, it is unspecified which
      one of the values for a given key will be chosen. For instance,
      `{"key": 2, "key": 1}` could be equal to either `{"key": 1}` or
      `{"key": 2}`.
    - Internally, name/value pairs are stored in lexicographical order of the
      names. Objects will also be serialized (see @ref dump) in this order.
      For instance, `{"b": 1, "a": 2}` and `{"a": 2, "b": 1}` will be stored
      and serialized as `{"a": 2, "b": 1}`.
    - When comparing objects, the order of the name/value pairs is irrelevant.
      This makes objects interoperable in the sense that they will not be
      affected by these differences. For instance, `{"b": 1, "a": 2}` and
      `{"a": 2, "b": 1}` will be treated as equal.

    #### Limits

    [RFC 7159](http://rfc7159.net/rfc7159) specifies:
    > An implementation may set limits on the maximum depth of nesting.

    In this class, the object's limit of nesting is not explicitly constrained.
    However, a maximum depth of nesting may be introduced by the compiler or
    runtime environment. A theoretical limit can be queried by calling the
    @ref max_size function of a JSON object.

    #### Storage

    Objects are stored as pointers in a @ref json type. That is, for any
    access to object values, a pointer of type `object_t*` must be
    dereferenced.

    @since version 1.0.0

    @note The order name/value pairs are added to the object is *not*
    preserved by the library. Therefore, iterating an object may return
    name/value pairs in a different order than they were originally stored. In
    fact, keys will be traversed in alphabetical order as `std::map` with
    `std::less` is used by default. Please note this behavior conforms to [RFC
    7159](http://rfc7159.net/rfc7159), because any order implements the
    specified "unordered" nature of JSON objects.
    */
    using object_t = StringMap<json>;

    /*!
    @brief a type for an array

    [RFC 7159](http://rfc7159.net/rfc7159) describes JSON arrays as follows:
    > An array is an ordered sequence of zero or more values.

    #### Limits

    [RFC 7159](http://rfc7159.net/rfc7159) specifies:
    > An implementation may set limits on the maximum depth of nesting.

    In this class, the array's limit of nesting is not explicitly constrained.
    However, a maximum depth of nesting may be introduced by the compiler or
    runtime environment. A theoretical limit can be queried by calling the
    @ref max_size function of a JSON array.

    #### Storage

    Arrays are stored as pointers in a @ref json type. That is, for any
    access to array values, a pointer of type `array_t*` must be dereferenced.

    @sa @ref object_t -- type for an object value

    @since version 1.0.0
    */
    using array_t = std::vector<json>;

    /// @}

  private:

    /// helper for exception-safe object creation
    template<typename T, typename... Args>
    static T* create(Args&& ... args)
    {
        std::allocator<T> alloc;

        auto deleter = [&](T * object)
        {
            alloc.deallocate(object, 1);
        };
        std::unique_ptr<T, decltype(deleter)> object(alloc.allocate(1), deleter);
        alloc.construct(object.get(), std::forward<Args>(args)...);
        assert(object != nullptr);
        return object.release();
    }

    ////////////////////////
    // JSON value storage //
    ////////////////////////

    /*!
    @brief a JSON value

    The actual storage for a JSON value of the @ref json class. This
    union combines the different storage types for the JSON value types
    defined in @ref value_t.

    JSON type | value_t type    | used type
    --------- | --------------- | ------------------------
    object    | object          | pointer to @ref object_t
    array     | array           | pointer to @ref array_t
    string    | string          | pointer to std::string
    boolean   | boolean         | bool
    number    | number_integer  | int64_t
    number    | number_unsigned | uint64_t
    number    | number_float    | double
    null      | null            | *no value is stored*

    @note Variable-length types (objects, arrays, and strings) are stored as
    pointers. The size of the union should not exceed 64 bits if the default
    value types are used.

    @since version 1.0.0
    */
    union json_value
    {
        /// object (stored with pointer to save storage)
        object_t* object;
        /// array (stored with pointer to save storage)
        array_t* array;
        /// string (stored with pointer to save storage)
        std::string* string;
        /// boolean
        bool boolean;
        /// number (integer)
        int64_t number_integer;
        /// number (unsigned integer)
        uint64_t number_unsigned;
        /// number (floating-point)
        double number_float;

        /// default constructor (for null values)
        json_value() = default;
        /// constructor for booleans
        json_value(bool v) noexcept : boolean(v) {}
        /// constructor for numbers (integer)
        json_value(int64_t v) noexcept : number_integer(v) {}
        /// constructor for numbers (unsigned)
        json_value(uint64_t v) noexcept : number_unsigned(v) {}
        /// constructor for numbers (floating-point)
        json_value(double v) noexcept : number_float(v) {}
        /// constructor for empty values of a given type
        json_value(value_t t);

        /// constructor for strings
        json_value(StringRef value)
        {
            string = create<std::string>(value);
        }

        /// constructor for strings
        json_value(const std::string& value)
        {
            string = create<std::string>(value);
        }

        /// constructor for rvalue strings
        json_value(std::string&& value)
        {
            string = create<std::string>(std::move(value));
        }

        /// constructor for objects
        json_value(const object_t& value)
        {
            object = create<object_t>(value);
        }

        /// constructor for rvalue objects
        json_value(object_t&& value)
        {
            object = create<object_t>(std::move(value));
        }

        /// constructor for arrays
        json_value(const array_t& value)
        {
            array = create<array_t>(value);
        }

        /// constructor for rvalue arrays
        json_value(array_t&& value)
        {
            array = create<array_t>(std::move(value));
        }

        void destroy(value_t t) noexcept;
    };

    /*!
    @brief checks the class invariants

    This function asserts the class invariants. It needs to be called at the
    end of every constructor to make sure that created objects respect the
    invariant. Furthermore, it has to be called each time the type of a JSON
    value is changed, because the invariant expresses a relationship between
    @a m_type and @a m_value.
    */
    void assert_invariant() const noexcept
    {
        assert(m_type != value_t::object or m_value.object != nullptr);
        assert(m_type != value_t::array or m_value.array != nullptr);
        assert(m_type != value_t::string or m_value.string != nullptr);
    }

  public:
    //////////////////////////
    // JSON parser callback //
    //////////////////////////

    /*!
    @brief parser event types

    The parser callback distinguishes the following events:
    - `object_start`: the parser read `{` and started to process a JSON object
    - `key`: the parser read a key of a value in an object
    - `object_end`: the parser read `}` and finished processing a JSON object
    - `array_start`: the parser read `[` and started to process a JSON array
    - `array_end`: the parser read `]` and finished processing a JSON array
    - `value`: the parser finished reading a JSON value

    @image html callback_events.png "Example when certain parse events are triggered"

    @sa @ref parser_callback_t for more information and examples
    */
    enum class parse_event_t : uint8_t
    {
        /// the parser read `{` and started to process a JSON object
        object_start,
        /// the parser read `}` and finished processing a JSON object
        object_end,
        /// the parser read `[` and started to process a JSON array
        array_start,
        /// the parser read `]` and finished processing a JSON array
        array_end,
        /// the parser read a key of a value in an object
        key,
        /// the parser finished reading a JSON value
        value
    };

    /*!
    @brief per-element parser callback type

    With a parser callback function, the result of parsing a JSON text can be
    influenced. When passed to @ref parse, it is called on certain events
    (passed as @ref parse_event_t via parameter @a event) with a set recursion
    depth @a depth and context JSON value @a parsed. The return value of the
    callback function is a boolean indicating whether the element that emitted
    the callback shall be kept or not.

    We distinguish six scenarios (determined by the event type) in which the
    callback function can be called. The following table describes the values
    of the parameters @a depth, @a event, and @a parsed.

    parameter @a event | description | parameter @a depth | parameter @a parsed
    ------------------ | ----------- | ------------------ | -------------------
    parse_event_t::object_start | the parser read `{` and started to process a JSON object | depth of the parent of the JSON object | a JSON value with type discarded
    parse_event_t::key | the parser read a key of a value in an object | depth of the currently parsed JSON object | a JSON string containing the key
    parse_event_t::object_end | the parser read `}` and finished processing a JSON object | depth of the parent of the JSON object | the parsed JSON object
    parse_event_t::array_start | the parser read `[` and started to process a JSON array | depth of the parent of the JSON array | a JSON value with type discarded
    parse_event_t::array_end | the parser read `]` and finished processing a JSON array | depth of the parent of the JSON array | the parsed JSON array
    parse_event_t::value | the parser finished reading a JSON value | depth of the value | the parsed JSON value

    @image html callback_events.png "Example when certain parse events are triggered"

    Discarding a value (i.e., returning `false`) has different effects
    depending on the context in which function was called:

    - Discarded values in structured types are skipped. That is, the parser
      will behave as if the discarded value was never read.
    - In case a value outside a structured type is skipped, it is replaced
      with `null`. This case happens if the top-level element is skipped.

    @param[in] depth  the depth of the recursion during parsing

    @param[in] event  an event of type parse_event_t indicating the context in
    the callback function has been called

    @param[in,out] parsed  the current intermediate parse result; note that
    writing to this value has no effect for parse_event_t::key events

    @return Whether the JSON value which called the function during parsing
    should be kept (`true`) or not (`false`). In the latter case, it is either
    skipped completely or replaced by an empty discarded object.

    @sa @ref parse for examples

    @since version 1.0.0
    */
    using parser_callback_t =
        std::function<bool(int depth, parse_event_t event, json& parsed)>;


    //////////////////
    // constructors //
    //////////////////

    /// @name constructors and destructors
    /// Constructors of class @ref json, copy/move constructor, copy
    /// assignment, static functions creating objects, and the destructor.
    /// @{

    /*!
    @brief create an empty value with a given type

    Create an empty JSON value with a given type. The value will be default
    initialized with an empty value which depends on the type:

    Value type  | initial value
    ----------- | -------------
    null        | `null`
    boolean     | `false`
    string      | `""`
    number      | `0`
    object      | `{}`
    array       | `[]`

    @param[in] v  the type of the value to create

    @complexity Constant.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @liveexample{The following code shows the constructor for different @ref
    value_t values,json__value_t}

    @sa @ref clear() -- restores the postcondition of this constructor

    @since version 1.0.0
    */
    json(const value_t v)
        : m_type(v), m_value(v)
    {
        assert_invariant();
    }

    /*!
    @brief create a null object

    Create a `null` JSON value. It either takes a null pointer as parameter
    (explicitly creating `null`) or no parameter (implicitly creating `null`).
    The passed null pointer itself is not read -- it is only used to choose
    the right constructor.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this constructor never throws
    exceptions.

    @liveexample{The following code shows the constructor with and without a
    null pointer parameter.,json__nullptr_t}

    @since version 1.0.0
    */
    json(std::nullptr_t = nullptr) noexcept
        : json(value_t::null)
    {
        assert_invariant();
    }

    /*!
    @brief create a JSON value

    This is a "catch all" constructor for all compatible JSON types; that is,
    types for which a `to_json()` method exists. The constructor forwards the
    parameter @a val to that method (to `json_serializer<U>::to_json` method
    with `U = uncvref_t<CompatibleType>`, to be exact).

    Template type @a CompatibleType includes, but is not limited to, the
    following types:
    - **arrays**: @ref array_t and all kinds of compatible containers such as
      `std::vector`, `std::deque`, `std::list`,
      `std::array`, `std::set`, `std::unordered_set`,
      `std::multiset`, and `std::unordered_multiset` with a `value_type` from
      which a @ref json value can be constructed.
    - **objects**: @ref object_t and all kinds of compatible associative
      containers such as `std::map`, `std::unordered_map`, `std::multimap`,
      and `std::unordered_multimap` with a `key_type` compatible to
      `std::string` and a `value_type` from which a @ref json value can
      be constructed.
    - **strings**: `std::string`, string literals, and all compatible string
      containers can be used.
    - **numbers**: int64_t, uint64_t,
      double, and all convertible number types such as `int`,
      `size_t`, `int64_t`, `float` or `double` can be used.
    - **boolean**: `bool` can be used.

    See the examples below.

    @tparam CompatibleType a type such that:
    - @a CompatibleType is not derived from `std::istream`,
    - @a CompatibleType is not @ref json (to avoid hijacking copy/move
         constructors),
    - @a CompatibleType is not a different @ref json type (i.e. with different template arguments)
    - @a CompatibleType is not a @ref json nested type (e.g.,
         @ref json_pointer, @ref iterator, etc ...)
    - @ref @ref json_serializer<U> has a
         `to_json(json_t&, CompatibleType&&)` method

    @tparam U = `uncvref_t<CompatibleType>`

    @param[in] val the value to be forwarded to the respective constructor

    @complexity Usually linear in the size of the passed @a val, also
                depending on the implementation of the called `to_json()`
                method.

    @exceptionsafety Depends on the called constructor. For types directly
    supported by the library (i.e., all types for which no `to_json()` function
    was provided), strong guarantee holds: if an exception is thrown, there are
    no changes to any JSON value.

    @liveexample{The following code shows the constructor with several
    compatible types.,json__CompatibleType}

    @since version 2.1.0
    */
    template <typename CompatibleType,
              typename U = detail::uncvref_t<CompatibleType>,
              detail::enable_if_t<
                  detail::is_compatible_type<json_t, U>::value, int> = 0>
    json(CompatibleType && val) noexcept(noexcept(
                adl_serializer<U, void>::to_json(std::declval<json_t&>(),
                                           std::forward<CompatibleType>(val))))
    {
        adl_serializer<U, void>::to_json(*this, std::forward<CompatibleType>(val));
        assert_invariant();
    }

    /*!
    @brief create a container (array or object) from an initializer list

    Creates a JSON value of type array or object from the passed initializer
    list @a init. In case @a type_deduction is `true` (default), the type of
    the JSON value to be created is deducted from the initializer list @a init
    according to the following rules:

    1. If the list is empty, an empty JSON object value `{}` is created.
    2. If the list consists of pairs whose first element is a string, a JSON
       object value is created where the first elements of the pairs are
       treated as keys and the second elements are as values.
    3. In all other cases, an array is created.

    The rules aim to create the best fit between a C++ initializer list and
    JSON values. The rationale is as follows:

    1. The empty initializer list is written as `{}` which is exactly an empty
       JSON object.
    2. C++ has no way of describing mapped types other than to list a list of
       pairs. As JSON requires that keys must be of type string, rule 2 is the
       weakest constraint one can pose on initializer lists to interpret them
       as an object.
    3. In all other cases, the initializer list could not be interpreted as
       JSON object type, so interpreting it as JSON array type is safe.

    With the rules described above, the following JSON values cannot be
    expressed by an initializer list:

    - the empty array (`[]`): use @ref array(initializer_list_t)
      with an empty initializer list in this case
    - arrays whose elements satisfy rule 2: use @ref
      array(initializer_list_t) with the same initializer list
      in this case

    @note When used without parentheses around an empty initializer list, @ref
    json() is called instead of this function, yielding the JSON null
    value.

    @param[in] init  initializer list with JSON values

    @param[in] type_deduction internal parameter; when set to `true`, the type
    of the JSON value is deducted from the initializer list @a init; when set
    to `false`, the type provided via @a manual_type is forced. This mode is
    used by the functions @ref array(initializer_list_t) and
    @ref object(initializer_list_t).

    @param[in] manual_type internal parameter; when @a type_deduction is set
    to `false`, the created JSON value will use the provided type (only @ref
    value_t::array and @ref value_t::object are valid); when @a type_deduction
    is set to `true`, this parameter has no effect

    @throw type_error.301 if @a type_deduction is `false`, @a manual_type is
    `value_t::object`, but @a init contains an element which is not a pair
    whose first element is a string. In this case, the constructor could not
    create an object. If @a type_deduction would have be `true`, an array
    would have been created. See @ref object(initializer_list_t)
    for an example.

    @complexity Linear in the size of the initializer list @a init.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @liveexample{The example below shows how JSON values are created from
    initializer lists.,json__list_init_t}

    @sa @ref array(initializer_list_t) -- create a JSON array
    value from an initializer list
    @sa @ref object(initializer_list_t) -- create a JSON object
    value from an initializer list

    @since version 1.0.0
    */
    json(initializer_list_t init,
               bool type_deduction = true,
               value_t manual_type = value_t::array);

    /*!
    @brief explicitly create an array from an initializer list

    Creates a JSON array value from a given initializer list. That is, given a
    list of values `a, b, c`, creates the JSON value `[a, b, c]`. If the
    initializer list is empty, the empty array `[]` is created.

    @note This function is only needed to express two edge cases that cannot
    be realized with the initializer list constructor (@ref
    json(initializer_list_t, bool, value_t)). These cases
    are:
    1. creating an array whose elements are all pairs whose first element is a
    string -- in this case, the initializer list constructor would create an
    object, taking the first elements as keys
    2. creating an empty array -- passing the empty initializer list to the
    initializer list constructor yields an empty object

    @param[in] init  initializer list with JSON values to create an array from
    (optional)

    @return JSON array value

    @complexity Linear in the size of @a init.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @liveexample{The following code shows an example for the `array`
    function.,array}

    @sa @ref json(initializer_list_t, bool, value_t) --
    create a JSON value from an initializer list
    @sa @ref object(initializer_list_t) -- create a JSON object
    value from an initializer list

    @since version 1.0.0
    */
    static json array(initializer_list_t init = {})
    {
        return json(init, false, value_t::array);
    }

    /*!
    @brief explicitly create an object from an initializer list

    Creates a JSON object value from a given initializer list. The initializer
    lists elements must be pairs, and their first elements must be strings. If
    the initializer list is empty, the empty object `{}` is created.

    @note This function is only added for symmetry reasons. In contrast to the
    related function @ref array(initializer_list_t), there are
    no cases which can only be expressed by this function. That is, any
    initializer list @a init can also be passed to the initializer list
    constructor @ref json(initializer_list_t, bool, value_t).

    @param[in] init  initializer list to create an object from (optional)

    @return JSON object value

    @throw type_error.301 if @a init is not a list of pairs whose first
    elements are strings. In this case, no object can be created. When such a
    value is passed to @ref json(initializer_list_t, bool, value_t),
    an array would have been created from the passed initializer list @a init.
    See example below.

    @complexity Linear in the size of @a init.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @liveexample{The following code shows an example for the `object`
    function.,object}

    @sa @ref json(initializer_list_t, bool, value_t) --
    create a JSON value from an initializer list
    @sa @ref array(initializer_list_t) -- create a JSON array
    value from an initializer list

    @since version 1.0.0
    */
    static json object(initializer_list_t init = {})
    {
        return json(init, false, value_t::object);
    }

    /*!
    @brief construct an array with count copies of given value

    Constructs a JSON array value by creating @a cnt copies of a passed value.
    In case @a cnt is `0`, an empty array is created.

    @param[in] cnt  the number of JSON copies of @a val to create
    @param[in] val  the JSON value to copy

    @post `std::distance(begin(),end()) == cnt` holds.

    @complexity Linear in @a cnt.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @liveexample{The following code shows examples for the @ref
    json(size_type\, const json&)
    constructor.,json__size_type_json}

    @since version 1.0.0
    */
    json(size_type cnt, const json& val);

    /*!
    @brief construct a JSON container given an iterator range

    Constructs the JSON value with the contents of the range `[first, last)`.
    The semantics depends on the different types a JSON value can have:
    - In case of a null type, invalid_iterator.206 is thrown.
    - In case of other primitive types (number, boolean, or string), @a first
      must be `begin()` and @a last must be `end()`. In this case, the value is
      copied. Otherwise, invalid_iterator.204 is thrown.
    - In case of structured types (array, object), the constructor behaves as
      similar versions for `std::vector` or `std::map`; that is, a JSON array
      or object is constructed from the values in the range.

    @tparam InputIT an input iterator type (@ref iterator or @ref
    const_iterator)

    @param[in] first begin of the range to copy from (included)
    @param[in] last end of the range to copy from (excluded)

    @pre Iterators @a first and @a last must be initialized. **This
         precondition is enforced with an assertion (see warning).** If
         assertions are switched off, a violation of this precondition yields
         undefined behavior.

    @pre Range `[first, last)` is valid. Usually, this precondition cannot be
         checked efficiently. Only certain edge cases are detected; see the
         description of the exceptions below. A violation of this precondition
         yields undefined behavior.

    @warning A precondition is enforced with a runtime assertion that will
             result in calling `std::abort` if this precondition is not met.
             Assertions can be disabled by defining `NDEBUG` at compile time.
             See http://en.cppreference.com/w/cpp/error/assert for more
             information.

    @throw invalid_iterator.201 if iterators @a first and @a last are not
    compatible (i.e., do not belong to the same JSON value). In this case,
    the range `[first, last)` is undefined.
    @throw invalid_iterator.204 if iterators @a first and @a last belong to a
    primitive type (number, boolean, or string), but @a first does not point
    to the first element any more. In this case, the range `[first, last)` is
    undefined. See example code below.
    @throw invalid_iterator.206 if iterators @a first and @a last belong to a
    null value. In this case, the range `[first, last)` is undefined.

    @complexity Linear in distance between @a first and @a last.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @liveexample{The example below shows several ways to create JSON values by
    specifying a subrange with iterators.,json__InputIt_InputIt}

    @since version 1.0.0
    */
    template<class InputIT, typename std::enable_if<
                 std::is_same<InputIT, typename json_t::iterator>::value or
                 std::is_same<InputIT, typename json_t::const_iterator>::value, int>::type = 0>
    json(InputIT first, InputIT last)
    {
        assert(first.m_object != nullptr);
        assert(last.m_object != nullptr);

        // make sure iterator fits the current value
        if (JSON_UNLIKELY(first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(201, "iterators are not compatible"));
        }

        // copy type from first iterator
        m_type = first.m_object->m_type;

        // check if iterator range is complete for primitive values
        switch (m_type)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (JSON_UNLIKELY(not first.m_it.primitive_iterator.is_begin()
                                  or not last.m_it.primitive_iterator.is_end()))
                {
                    JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
                }
                break;
            }

            default:
                break;
        }

        switch (m_type)
        {
            case value_t::number_integer:
            {
                m_value.number_integer = first.m_object->m_value.number_integer;
                break;
            }

            case value_t::number_unsigned:
            {
                m_value.number_unsigned = first.m_object->m_value.number_unsigned;
                break;
            }

            case value_t::number_float:
            {
                m_value.number_float = first.m_object->m_value.number_float;
                break;
            }

            case value_t::boolean:
            {
                m_value.boolean = first.m_object->m_value.boolean;
                break;
            }

            case value_t::string:
            {
                m_value = *first.m_object->m_value.string;
                break;
            }

            case value_t::array:
            {
                m_value.array = create<array_t>(first.m_it.array_iterator,
                                                last.m_it.array_iterator);
                break;
            }

            default:
                JSON_THROW(invalid_iterator::create(206, "cannot construct with iterators from " +
                                                    Twine(first.m_object->type_name())));
        }

        assert_invariant();
    }


    ///////////////////////////////////////
    // other constructors and destructor //
    ///////////////////////////////////////

    /// @private
    json(const detail::json_ref<json>& ref)
        : json(ref.moved_or_copied())
    {}

    /*!
    @brief copy constructor

    Creates a copy of a given JSON value.

    @param[in] other  the JSON value to copy

    @post `*this == other`

    @complexity Linear in the size of @a other.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes to any JSON value.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is linear.
    - As postcondition, it holds: `other == json(other)`.

    @liveexample{The following code shows an example for the copy
    constructor.,json__json}

    @since version 1.0.0
    */
    json(const json& other);

    /*!
    @brief move constructor

    Move constructor. Constructs a JSON value with the contents of the given
    value @a other using move semantics. It "steals" the resources from @a
    other and leaves it as JSON null value.

    @param[in,out] other  value to move to this object

    @post `*this` has the same value as @a other before the call.
    @post @a other is a JSON null value.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this constructor never throws
    exceptions.

    @requirement This function helps `json` satisfying the
    [MoveConstructible](http://en.cppreference.com/w/cpp/concept/MoveConstructible)
    requirements.

    @liveexample{The code below shows the move constructor explicitly called
    via std::move.,json__moveconstructor}

    @since version 1.0.0
    */
    json(json&& other) noexcept
        : m_type(std::move(other.m_type)),
          m_value(std::move(other.m_value))
    {
        // check that passed value is valid
        other.assert_invariant();

        // invalidate payload
        other.m_type = value_t::null;
        other.m_value = {};

        assert_invariant();
    }

    /*!
    @brief copy assignment

    Copy assignment operator. Copies a JSON value via the "copy and swap"
    strategy: It is expressed in terms of the copy constructor, destructor,
    and the `swap()` member function.

    @param[in] other  value to copy from

    @complexity Linear.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is linear.

    @liveexample{The code below shows and example for the copy assignment. It
    creates a copy of value `a` which is then swapped with `b`. Finally\, the
    copy of `a` (which is the null value after the swap) is
    destroyed.,json__copyassignment}

    @since version 1.0.0
    */
    reference& operator=(json other) noexcept (
        std::is_nothrow_move_constructible<value_t>::value and
        std::is_nothrow_move_assignable<value_t>::value and
        std::is_nothrow_move_constructible<json_value>::value and
        std::is_nothrow_move_assignable<json_value>::value
    )
    {
        // check that passed value is valid
        other.assert_invariant();

        using std::swap;
        swap(m_type, other.m_type);
        swap(m_value, other.m_value);

        assert_invariant();
        return *this;
    }

    /*!
    @brief destructor

    Destroys the JSON value and frees all allocated memory.

    @complexity Linear.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is linear.
    - All stored elements are destroyed and all memory is freed.

    @since version 1.0.0
    */
    ~json() noexcept
    {
        assert_invariant();
        m_value.destroy(m_type);
    }

    /// @}

  public:
    ///////////////////////
    // object inspection //
    ///////////////////////

    /// @name object inspection
    /// Functions to inspect the type of a JSON value.
    /// @{

    /*!
    @brief serialization

    Serialization function for JSON values. The function tries to mimic
    Python's `json.dumps()` function, and currently supports its @a indent
    and @a ensure_ascii parameters.

    @param[in] indent If indent is nonnegative, then array elements and object
    members will be pretty-printed with that indent level. An indent level of
    `0` will only insert newlines. `-1` (the default) selects the most compact
    representation.
    @param[in] indent_char The character to use for indentation if @a indent is
    greater than `0`. The default is ` ` (space).
    @param[in] ensure_ascii If @a ensure_ascii is true, all non-ASCII characters
    in the output are escaped with `\uXXXX` sequences, and the result consists
    of ASCII characters only.

    @return string containing the serialization of the JSON value

    @throw type_error.316 if a string stored inside the JSON value is not
                          UTF-8 encoded

    @complexity Linear.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @liveexample{The following example shows the effect of different @a indent\,
    @a indent_char\, and @a ensure_ascii parameters to the result of the
    serialization.,dump}

    @see https://docs.python.org/2/library/json.html#json.dump

    @since version 1.0.0; indentation character @a indent_char, option
           @a ensure_ascii and exceptions added in version 3.0.0
    */
    std::string dump(const int indent = -1, const char indent_char = ' ',
                     const bool ensure_ascii = false) const;

    void dump(raw_ostream& os, int indent = -1, const char indent_char = ' ',
              const bool ensure_ascii = false) const;

    /*!
    @brief return the type of the JSON value (explicit)

    Return the type of the JSON value as a value from the @ref value_t
    enumeration.

    @return the type of the JSON value
            Value type                | return value
            ------------------------- | -------------------------
            null                      | value_t::null
            boolean                   | value_t::boolean
            string                    | value_t::string
            number (integer)          | value_t::number_integer
            number (unsigned integer) | value_t::number_unsigned
            number (floating-point)   | value_t::number_float
            object                    | value_t::object
            array                     | value_t::array
            discarded                 | value_t::discarded

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `type()` for all JSON
    types.,type}

    @sa @ref operator value_t() -- return the type of the JSON value (implicit)
    @sa @ref type_name() -- return the type as string

    @since version 1.0.0
    */
    value_t type() const noexcept
    {
        return m_type;
    }

    /*!
    @brief return whether type is primitive

    This function returns true if and only if the JSON type is primitive
    (string, number, boolean, or null).

    @return `true` if type is primitive (string, number, boolean, or null),
    `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_primitive()` for all JSON
    types.,is_primitive}

    @sa @ref is_structured() -- returns whether JSON value is structured
    @sa @ref is_null() -- returns whether JSON value is `null`
    @sa @ref is_string() -- returns whether JSON value is a string
    @sa @ref is_boolean() -- returns whether JSON value is a boolean
    @sa @ref is_number() -- returns whether JSON value is a number

    @since version 1.0.0
    */
    bool is_primitive() const noexcept
    {
        return is_null() or is_string() or is_boolean() or is_number();
    }

    /*!
    @brief return whether type is structured

    This function returns true if and only if the JSON type is structured
    (array or object).

    @return `true` if type is structured (array or object), `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_structured()` for all JSON
    types.,is_structured}

    @sa @ref is_primitive() -- returns whether value is primitive
    @sa @ref is_array() -- returns whether value is an array
    @sa @ref is_object() -- returns whether value is an object

    @since version 1.0.0
    */
    bool is_structured() const noexcept
    {
        return is_array() or is_object();
    }

    /*!
    @brief return whether value is null

    This function returns true if and only if the JSON value is null.

    @return `true` if type is null, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_null()` for all JSON
    types.,is_null}

    @since version 1.0.0
    */
    bool is_null() const noexcept
    {
        return (m_type == value_t::null);
    }

    /*!
    @brief return whether value is a boolean

    This function returns true if and only if the JSON value is a boolean.

    @return `true` if type is boolean, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_boolean()` for all JSON
    types.,is_boolean}

    @since version 1.0.0
    */
    bool is_boolean() const noexcept
    {
        return (m_type == value_t::boolean);
    }

    /*!
    @brief return whether value is a number

    This function returns true if and only if the JSON value is a number. This
    includes both integer (signed and unsigned) and floating-point values.

    @return `true` if type is number (regardless whether integer, unsigned
    integer or floating-type), `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_number()` for all JSON
    types.,is_number}

    @sa @ref is_number_integer() -- check if value is an integer or unsigned
    integer number
    @sa @ref is_number_unsigned() -- check if value is an unsigned integer
    number
    @sa @ref is_number_float() -- check if value is a floating-point number

    @since version 1.0.0
    */
    bool is_number() const noexcept
    {
        return is_number_integer() or is_number_float();
    }

    /*!
    @brief return whether value is an integer number

    This function returns true if and only if the JSON value is a signed or
    unsigned integer number. This excludes floating-point values.

    @return `true` if type is an integer or unsigned integer number, `false`
    otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_number_integer()` for all
    JSON types.,is_number_integer}

    @sa @ref is_number() -- check if value is a number
    @sa @ref is_number_unsigned() -- check if value is an unsigned integer
    number
    @sa @ref is_number_float() -- check if value is a floating-point number

    @since version 1.0.0
    */
    bool is_number_integer() const noexcept
    {
        return (m_type == value_t::number_integer or m_type == value_t::number_unsigned);
    }

    /*!
    @brief return whether value is an unsigned integer number

    This function returns true if and only if the JSON value is an unsigned
    integer number. This excludes floating-point and signed integer values.

    @return `true` if type is an unsigned integer number, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_number_unsigned()` for all
    JSON types.,is_number_unsigned}

    @sa @ref is_number() -- check if value is a number
    @sa @ref is_number_integer() -- check if value is an integer or unsigned
    integer number
    @sa @ref is_number_float() -- check if value is a floating-point number

    @since version 2.0.0
    */
    bool is_number_unsigned() const noexcept
    {
        return (m_type == value_t::number_unsigned);
    }

    /*!
    @brief return whether value is a floating-point number

    This function returns true if and only if the JSON value is a
    floating-point number. This excludes signed and unsigned integer values.

    @return `true` if type is a floating-point number, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_number_float()` for all
    JSON types.,is_number_float}

    @sa @ref is_number() -- check if value is number
    @sa @ref is_number_integer() -- check if value is an integer number
    @sa @ref is_number_unsigned() -- check if value is an unsigned integer
    number

    @since version 1.0.0
    */
    bool is_number_float() const noexcept
    {
        return (m_type == value_t::number_float);
    }

    /*!
    @brief return whether value is an object

    This function returns true if and only if the JSON value is an object.

    @return `true` if type is object, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_object()` for all JSON
    types.,is_object}

    @since version 1.0.0
    */
    bool is_object() const noexcept
    {
        return (m_type == value_t::object);
    }

    /*!
    @brief return whether value is an array

    This function returns true if and only if the JSON value is an array.

    @return `true` if type is array, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_array()` for all JSON
    types.,is_array}

    @since version 1.0.0
    */
    bool is_array() const noexcept
    {
        return (m_type == value_t::array);
    }

    /*!
    @brief return whether value is a string

    This function returns true if and only if the JSON value is a string.

    @return `true` if type is string, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_string()` for all JSON
    types.,is_string}

    @since version 1.0.0
    */
    bool is_string() const noexcept
    {
        return (m_type == value_t::string);
    }

    /*!
    @brief return whether value is discarded

    This function returns true if and only if the JSON value was discarded
    during parsing with a callback function (see @ref parser_callback_t).

    @note This function will always be `false` for JSON values after parsing.
    That is, discarded values can only occur during parsing, but will be
    removed when inside a structured value or replaced by null in other cases.

    @return `true` if type is discarded, `false` otherwise.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `is_discarded()` for all JSON
    types.,is_discarded}

    @since version 1.0.0
    */
    bool is_discarded() const noexcept
    {
        return (m_type == value_t::discarded);
    }

    /*!
    @brief return the type of the JSON value (implicit)

    Implicitly return the type of the JSON value as a value from the @ref
    value_t enumeration.

    @return the type of the JSON value

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies the @ref value_t operator for
    all JSON types.,operator__value_t}

    @sa @ref type() -- return the type of the JSON value (explicit)
    @sa @ref type_name() -- return the type as string

    @since version 1.0.0
    */
    operator value_t() const noexcept
    {
        return m_type;
    }

    /// @}

  private:
    //////////////////
    // value access //
    //////////////////

    /// get a boolean (explicit)
    bool get_impl(bool* /*unused*/) const
    {
        if (JSON_LIKELY(is_boolean()))
        {
            return m_value.boolean;
        }

        JSON_THROW(type_error::create(302, "type must be boolean, but is " + Twine(type_name())));
    }

    /// get a pointer to the value (object)
    object_t* get_impl_ptr(object_t* /*unused*/) noexcept
    {
        return is_object() ? m_value.object : nullptr;
    }

    /// get a pointer to the value (object)
    const object_t* get_impl_ptr(const object_t* /*unused*/) const noexcept
    {
        return is_object() ? m_value.object : nullptr;
    }

    /// get a pointer to the value (array)
    array_t* get_impl_ptr(array_t* /*unused*/) noexcept
    {
        return is_array() ? m_value.array : nullptr;
    }

    /// get a pointer to the value (array)
    const array_t* get_impl_ptr(const array_t* /*unused*/) const noexcept
    {
        return is_array() ? m_value.array : nullptr;
    }

    /// get a pointer to the value (string)
    std::string* get_impl_ptr(std::string* /*unused*/) noexcept
    {
        return is_string() ? m_value.string : nullptr;
    }

    /// get a pointer to the value (string)
    const std::string* get_impl_ptr(const std::string* /*unused*/) const noexcept
    {
        return is_string() ? m_value.string : nullptr;
    }

    /// get a pointer to the value (boolean)
    bool* get_impl_ptr(bool* /*unused*/) noexcept
    {
        return is_boolean() ? &m_value.boolean : nullptr;
    }

    /// get a pointer to the value (boolean)
    const bool* get_impl_ptr(const bool* /*unused*/) const noexcept
    {
        return is_boolean() ? &m_value.boolean : nullptr;
    }

    /// get a pointer to the value (integer number)
    int64_t* get_impl_ptr(int64_t* /*unused*/) noexcept
    {
        return is_number_integer() ? &m_value.number_integer : nullptr;
    }

    /// get a pointer to the value (integer number)
    const int64_t* get_impl_ptr(const int64_t* /*unused*/) const noexcept
    {
        return is_number_integer() ? &m_value.number_integer : nullptr;
    }

    /// get a pointer to the value (unsigned number)
    uint64_t* get_impl_ptr(uint64_t* /*unused*/) noexcept
    {
        return is_number_unsigned() ? &m_value.number_unsigned : nullptr;
    }

    /// get a pointer to the value (unsigned number)
    const uint64_t* get_impl_ptr(const uint64_t* /*unused*/) const noexcept
    {
        return is_number_unsigned() ? &m_value.number_unsigned : nullptr;
    }

    /// get a pointer to the value (floating-point number)
    double* get_impl_ptr(double* /*unused*/) noexcept
    {
        return is_number_float() ? &m_value.number_float : nullptr;
    }

    /// get a pointer to the value (floating-point number)
    const double* get_impl_ptr(const double* /*unused*/) const noexcept
    {
        return is_number_float() ? &m_value.number_float : nullptr;
    }

    /*!
    @brief helper function to implement get_ref()

    This function helps to implement get_ref() without code duplication for
    const and non-const overloads

    @tparam ThisType will be deduced as `json` or `const json`

    @throw type_error.303 if ReferenceType does not match underlying value
    type of the current JSON
    */
    template<typename ReferenceType, typename ThisType>
    static ReferenceType get_ref_impl(ThisType& obj)
    {
        // delegate the call to get_ptr<>()
        auto ptr = obj.template get_ptr<typename std::add_pointer<ReferenceType>::type>();

        if (JSON_LIKELY(ptr != nullptr))
        {
            return *ptr;
        }

        JSON_THROW(type_error::create(303, "incompatible ReferenceType for get_ref, actual type is " + Twine(obj.type_name())));
    }

  public:
    /// @name value access
    /// Direct access to the stored value of a JSON value.
    /// @{

    /*!
    @brief get special-case overload

    This overloads avoids a lot of template boilerplate, it can be seen as the
    identity method

    @tparam BasicJsonType == @ref json

    @return a copy of *this

    @complexity Constant.

    @since version 2.1.0
    */
    template<typename BasicJsonType, detail::enable_if_t<
                 std::is_same<typename std::remove_const<BasicJsonType>::type, json_t>::value,
                 int> = 0>
    json get() const
    {
        return *this;
    }

    /*!
    @brief get a value (explicit)

    Explicit type conversion between the JSON value and a compatible value
    which is [CopyConstructible](http://en.cppreference.com/w/cpp/concept/CopyConstructible)
    and [DefaultConstructible](http://en.cppreference.com/w/cpp/concept/DefaultConstructible).
    The value is converted by calling the @ref json_serializer<ValueType>
    `from_json()` method.

    The function is equivalent to executing
    @code {.cpp}
    ValueType ret;
    adl_serializer<ValueType, void>::from_json(*this, ret);
    return ret;
    @endcode

    This overloads is chosen if:
    - @a ValueType is not @ref json,
    - @ref json_serializer<ValueType> has a `from_json()` method of the form
      `void from_json(const json&, ValueType&)`, and
    - @ref json_serializer<ValueType> does not have a `from_json()` method of
      the form `ValueType from_json(const json&)`

    @tparam ValueTypeCV the provided value type
    @tparam ValueType the returned value type

    @return copy of the JSON value, converted to @a ValueType

    @throw what @ref json_serializer<ValueType> `from_json()` method throws

    @liveexample{The example below shows several conversions from JSON values
    to other types. There a few things to note: (1) Floating-point numbers can
    be converted to integers\, (2) A JSON array can be converted to a standard
    `std::vector<short>`\, (3) A JSON object can be converted to C++
    associative containers such as `std::unordered_map<std::string\,
    json>`.,get__ValueType_const}

    @since version 2.1.0
    */
    template<typename ValueTypeCV, typename ValueType = detail::uncvref_t<ValueTypeCV>,
             detail::enable_if_t <
                 not detail::is_json<ValueType>::value and
                 detail::has_from_json<json_t, ValueType>::value and
                 not detail::has_non_default_from_json<json_t, ValueType>::value,
                 int> = 0>
    ValueType get() const noexcept(noexcept(
                                       adl_serializer<ValueType, void>::from_json(std::declval<const json_t&>(), std::declval<ValueType&>())))
    {
        // we cannot static_assert on ValueTypeCV being non-const, because
        // there is support for get<const json_t>(), which is why we
        // still need the uncvref
        static_assert(not std::is_reference<ValueTypeCV>::value,
                      "get() cannot be used with reference types, you might want to use get_ref()");
        static_assert(std::is_default_constructible<ValueType>::value,
                      "types must be DefaultConstructible when used with get()");

        ValueType ret;
        adl_serializer<ValueType, void>::from_json(*this, ret);
        return ret;
    }

    /*!
    @brief get a value (explicit); special case

    Explicit type conversion between the JSON value and a compatible value
    which is **not** [CopyConstructible](http://en.cppreference.com/w/cpp/concept/CopyConstructible)
    and **not** [DefaultConstructible](http://en.cppreference.com/w/cpp/concept/DefaultConstructible).
    The value is converted by calling the @ref json_serializer<ValueType>
    `from_json()` method.

    The function is equivalent to executing
    @code {.cpp}
    return adl_serializer<ValueTypeCV, void>::from_json(*this);
    @endcode

    This overloads is chosen if:
    - @a ValueType is not @ref json and
    - @ref json_serializer<ValueType> has a `from_json()` method of the form
      `ValueType from_json(const json&)`

    @note If @ref json_serializer<ValueType> has both overloads of
    `from_json()`, this one is chosen.

    @tparam ValueTypeCV the provided value type
    @tparam ValueType the returned value type

    @return copy of the JSON value, converted to @a ValueType

    @throw what @ref json_serializer<ValueType> `from_json()` method throws

    @since version 2.1.0
    */
    template<typename ValueTypeCV, typename ValueType = detail::uncvref_t<ValueTypeCV>,
             detail::enable_if_t<not std::is_same<json_t, ValueType>::value and
                                 detail::has_non_default_from_json<json_t, ValueType>::value,
                                 int> = 0>
    ValueType get() const noexcept(noexcept(
                                       adl_serializer<ValueTypeCV, void>::from_json(std::declval<const json_t&>())))
    {
        static_assert(not std::is_reference<ValueTypeCV>::value,
                      "get() cannot be used with reference types, you might want to use get_ref()");
        return adl_serializer<ValueTypeCV, void>::from_json(*this);
    }

    /*!
    @brief get a pointer value (explicit)

    Explicit pointer access to the internally stored JSON value. No copies are
    made.

    @warning The pointer becomes invalid if the underlying JSON object
    changes.

    @tparam PointerType pointer type; must be a pointer to @ref array_t, @ref
    object_t, `std::string`, bool, int64_t, uint64_t, or double.

    @return pointer to the internally stored JSON value if the requested
    pointer type @a PointerType fits to the JSON value; `nullptr` otherwise

    @complexity Constant.

    @liveexample{The example below shows how pointers to internal values of a
    JSON value can be requested. Note that no type conversions are made and a
    `nullptr` is returned if the value and the requested pointer type does not
    match.,get__PointerType}

    @sa @ref get_ptr() for explicit pointer-member access

    @since version 1.0.0
    */
    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    PointerType get() noexcept
    {
        // delegate the call to get_ptr
        return get_ptr<PointerType>();
    }

    /*!
    @brief get a pointer value (explicit)
    @copydoc get()
    */
    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    const PointerType get() const noexcept
    {
        // delegate the call to get_ptr
        return get_ptr<PointerType>();
    }

    /*!
    @brief get a pointer value (implicit)

    Implicit pointer access to the internally stored JSON value. No copies are
    made.

    @warning Writing data to the pointee of the result yields an undefined
    state.

    @tparam PointerType pointer type; must be a pointer to @ref array_t, @ref
    object_t, `std::string`, bool, int64_t,
    uint64_t, or double. Enforced by a static assertion.

    @return pointer to the internally stored JSON value if the requested
    pointer type @a PointerType fits to the JSON value; `nullptr` otherwise

    @complexity Constant.

    @liveexample{The example below shows how pointers to internal values of a
    JSON value can be requested. Note that no type conversions are made and a
    `nullptr` is returned if the value and the requested pointer type does not
    match.,get_ptr}

    @since version 1.0.0
    */
    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    PointerType get_ptr() noexcept
    {
        // get the type of the PointerType (remove pointer and const)
        using pointee_t = typename std::remove_const<typename
                          std::remove_pointer<typename
                          std::remove_const<PointerType>::type>::type>::type;
        // make sure the type matches the allowed types
        static_assert(
            std::is_same<object_t, pointee_t>::value
            or std::is_same<array_t, pointee_t>::value
            or std::is_same<std::string, pointee_t>::value
            or std::is_same<bool, pointee_t>::value
            or std::is_same<int64_t, pointee_t>::value
            or std::is_same<uint64_t, pointee_t>::value
            or std::is_same<double, pointee_t>::value
            , "incompatible pointer type");

        // delegate the call to get_impl_ptr<>()
        return get_impl_ptr(static_cast<PointerType>(nullptr));
    }

    /*!
    @brief get a pointer value (implicit)
    @copydoc get_ptr()
    */
    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value and
                 std::is_const<typename std::remove_pointer<PointerType>::type>::value, int>::type = 0>
    const PointerType get_ptr() const noexcept
    {
        // get the type of the PointerType (remove pointer and const)
        using pointee_t = typename std::remove_const<typename
                          std::remove_pointer<typename
                          std::remove_const<PointerType>::type>::type>::type;
        // make sure the type matches the allowed types
        static_assert(
            std::is_same<object_t, pointee_t>::value
            or std::is_same<array_t, pointee_t>::value
            or std::is_same<std::string, pointee_t>::value
            or std::is_same<bool, pointee_t>::value
            or std::is_same<int64_t, pointee_t>::value
            or std::is_same<uint64_t, pointee_t>::value
            or std::is_same<double, pointee_t>::value
            , "incompatible pointer type");

        // delegate the call to get_impl_ptr<>() const
        return get_impl_ptr(static_cast<PointerType>(nullptr));
    }

    /*!
    @brief get a reference value (implicit)

    Implicit reference access to the internally stored JSON value. No copies
    are made.

    @warning Writing data to the referee of the result yields an undefined
    state.

    @tparam ReferenceType reference type; must be a reference to @ref array_t,
    @ref object_t, std::string, bool, int64_t, or
    double. Enforced by static assertion.

    @return reference to the internally stored JSON value if the requested
    reference type @a ReferenceType fits to the JSON value; throws
    type_error.303 otherwise

    @throw type_error.303 in case passed type @a ReferenceType is incompatible
    with the stored JSON value; see example below

    @complexity Constant.

    @liveexample{The example shows several calls to `get_ref()`.,get_ref}

    @since version 1.1.0
    */
    template<typename ReferenceType, typename std::enable_if<
                 std::is_reference<ReferenceType>::value, int>::type = 0>
    ReferenceType get_ref()
    {
        // delegate call to get_ref_impl
        return get_ref_impl<ReferenceType>(*this);
    }

    /*!
    @brief get a reference value (implicit)
    @copydoc get_ref()
    */
    template<typename ReferenceType, typename std::enable_if<
                 std::is_reference<ReferenceType>::value and
                 std::is_const<typename std::remove_reference<ReferenceType>::type>::value, int>::type = 0>
    ReferenceType get_ref() const
    {
        // delegate call to get_ref_impl
        return get_ref_impl<ReferenceType>(*this);
    }

    /*!
    @brief get a value (implicit)

    Implicit type conversion between the JSON value and a compatible value.
    The call is realized by calling @ref get() const.

    @tparam ValueType non-pointer type compatible to the JSON value, for
    instance `int` for JSON integer numbers, `bool` for JSON booleans, or
    `std::vector` types for JSON arrays. The character type of `std::string`
    as well as an initializer list of this type is excluded to avoid
    ambiguities as these types implicitly convert to `std::string`.

    @return copy of the JSON value, converted to type @a ValueType

    @throw type_error.302 in case passed type @a ValueType is incompatible
    to the JSON value type (e.g., the JSON value is of type boolean, but a
    string is requested); see example below

    @complexity Linear in the size of the JSON value.

    @liveexample{The example below shows several conversions from JSON values
    to other types. There a few things to note: (1) Floating-point numbers can
    be converted to integers\, (2) A JSON array can be converted to a standard
    `std::vector<short>`\, (3) A JSON object can be converted to C++
    associative containers such as `std::unordered_map<std::string\,
    json>`.,operator__ValueType}

    @since version 1.0.0
    */
    template < typename ValueType, typename std::enable_if <
                   not std::is_pointer<ValueType>::value and
                   not std::is_same<ValueType, detail::json_ref<json>>::value and
                   not std::is_same<ValueType, std::string::value_type>::value and
                   not detail::is_json<ValueType>::value
#ifndef _MSC_VER  // fix for issue #167 operator<< ambiguity under VS2015
                   and not std::is_same<ValueType, std::initializer_list<std::string::value_type>>::value
#endif
#if defined(JSON_HAS_CPP_17)
                   and not std::is_same<ValueType, typename std::string_view>::value
#endif
                   , int >::type = 0 >
    operator ValueType() const
    {
        // delegate the call to get<>() const
        return get<ValueType>();
    }

    /// @}


    ////////////////////
    // element access //
    ////////////////////

    /// @name element access
    /// Access to the JSON value.
    /// @{

    /*!
    @brief access specified array element with bounds checking

    Returns a reference to the element at specified location @a idx, with
    bounds checking.

    @param[in] idx  index of the element to access

    @return reference to the element at index @a idx

    @throw type_error.304 if the JSON value is not an array; in this case,
    calling `at` with an index makes no sense. See example below.
    @throw out_of_range.401 if the index @a idx is out of range of the array;
    that is, `idx >= size()`. See example below.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @complexity Constant.

    @since version 1.0.0

    @liveexample{The example below shows how array elements can be read and
    written using `at()`. It also demonstrates the different exceptions that
    can be thrown.,at__size_type}
    */
    reference at(size_type idx);

    /*!
    @brief access specified array element with bounds checking

    Returns a const reference to the element at specified location @a idx,
    with bounds checking.

    @param[in] idx  index of the element to access

    @return const reference to the element at index @a idx

    @throw type_error.304 if the JSON value is not an array; in this case,
    calling `at` with an index makes no sense. See example below.
    @throw out_of_range.401 if the index @a idx is out of range of the array;
    that is, `idx >= size()`. See example below.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @complexity Constant.

    @since version 1.0.0

    @liveexample{The example below shows how array elements can be read using
    `at()`. It also demonstrates the different exceptions that can be thrown.,
    at__size_type_const}
    */
    const_reference at(size_type idx) const;

    /*!
    @brief access specified object element with bounds checking

    Returns a reference to the element at with specified key @a key, with
    bounds checking.

    @param[in] key  key of the element to access

    @return reference to the element at key @a key

    @throw type_error.304 if the JSON value is not an object; in this case,
    calling `at` with a key makes no sense. See example below.
    @throw out_of_range.403 if the key @a key is is not stored in the object;
    that is, `find(key) == end()`. See example below.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @complexity Logarithmic in the size of the container.

    @sa @ref operator[](const typename object_t::key_type&) for unchecked
    access by reference
    @sa @ref value() for access by value with a default value

    @since version 1.0.0

    @liveexample{The example below shows how object elements can be read and
    written using `at()`. It also demonstrates the different exceptions that
    can be thrown.,at__object_t_key_type}
    */
    reference at(StringRef key);

    /*!
    @brief access specified object element with bounds checking

    Returns a const reference to the element at with specified key @a key,
    with bounds checking.

    @param[in] key  key of the element to access

    @return const reference to the element at key @a key

    @throw type_error.304 if the JSON value is not an object; in this case,
    calling `at` with a key makes no sense. See example below.
    @throw out_of_range.403 if the key @a key is is not stored in the object;
    that is, `find(key) == end()`. See example below.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @complexity Logarithmic in the size of the container.

    @sa @ref operator[](const typename object_t::key_type&) for unchecked
    access by reference
    @sa @ref value() for access by value with a default value

    @since version 1.0.0

    @liveexample{The example below shows how object elements can be read using
    `at()`. It also demonstrates the different exceptions that can be thrown.,
    at__object_t_key_type_const}
    */
    const_reference at(StringRef key) const;

    /*!
    @brief access specified array element

    Returns a reference to the element at specified location @a idx.

    @note If @a idx is beyond the range of the array (i.e., `idx >= size()`),
    then the array is silently filled up with `null` values to make `idx` a
    valid reference to the last stored element.

    @param[in] idx  index of the element to access

    @return reference to the element at index @a idx

    @throw type_error.305 if the JSON value is not an array or null; in that
    cases, using the [] operator with an index makes no sense.

    @complexity Constant if @a idx is in the range of the array. Otherwise
    linear in `idx - size()`.

    @liveexample{The example below shows how array elements can be read and
    written using `[]` operator. Note the addition of `null`
    values.,operatorarray__size_type}

    @since version 1.0.0
    */
    reference operator[](size_type idx);

    /*!
    @brief access specified array element

    Returns a const reference to the element at specified location @a idx.

    @param[in] idx  index of the element to access

    @return const reference to the element at index @a idx

    @throw type_error.305 if the JSON value is not an array; in that case,
    using the [] operator with an index makes no sense.

    @complexity Constant.

    @liveexample{The example below shows how array elements can be read using
    the `[]` operator.,operatorarray__size_type_const}

    @since version 1.0.0
    */
    const_reference operator[](size_type idx) const;

    /*!
    @brief access specified object element

    Returns a reference to the element at with specified key @a key.

    @note If @a key is not found in the object, then it is silently added to
    the object and filled with a `null` value to make `key` a valid reference.
    In case the value was `null` before, it is converted to an object.

    @param[in] key  key of the element to access

    @return reference to the element at key @a key

    @throw type_error.305 if the JSON value is not an object or null; in that
    cases, using the [] operator with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be read and
    written using the `[]` operator.,operatorarray__key_type}

    @sa @ref at(const typename object_t::key_type&) for access by reference
    with range checking
    @sa @ref value() for access by value with a default value

    @since version 1.0.0
    */
    reference operator[](StringRef key);

    /*!
    @brief read-only access specified object element

    Returns a const reference to the element at with specified key @a key. No
    bounds checking is performed.

    @warning If the element with key @a key does not exist, the behavior is
    undefined.

    @param[in] key  key of the element to access

    @return const reference to the element at key @a key

    @pre The element with key @a key must exist. **This precondition is
         enforced with an assertion.**

    @throw type_error.305 if the JSON value is not an object; in that case,
    using the [] operator with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be read using
    the `[]` operator.,operatorarray__key_type_const}

    @sa @ref at(const typename object_t::key_type&) for access by reference
    with range checking
    @sa @ref value() for access by value with a default value

    @since version 1.0.0
    */
    const_reference operator[](StringRef key) const;

    /*!
    @brief access specified object element

    Returns a reference to the element at with specified key @a key.

    @note If @a key is not found in the object, then it is silently added to
    the object and filled with a `null` value to make `key` a valid reference.
    In case the value was `null` before, it is converted to an object.

    @param[in] key  key of the element to access

    @return reference to the element at key @a key

    @throw type_error.305 if the JSON value is not an object or null; in that
    cases, using the [] operator with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be read and
    written using the `[]` operator.,operatorarray__key_type}

    @sa @ref at(const typename object_t::key_type&) for access by reference
    with range checking
    @sa @ref value() for access by value with a default value

    @since version 1.1.0
    */
    template<typename T>
    reference operator[](T* key)
    {
        // implicitly convert null to object
        if (is_null())
        {
            m_type = value_t::object;
            m_value = value_t::object;
            assert_invariant();
        }

        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            return m_value.object->operator[](key);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with " + Twine(type_name())));
    }

    /*!
    @brief read-only access specified object element

    Returns a const reference to the element at with specified key @a key. No
    bounds checking is performed.

    @warning If the element with key @a key does not exist, the behavior is
    undefined.

    @param[in] key  key of the element to access

    @return const reference to the element at key @a key

    @pre The element with key @a key must exist. **This precondition is
         enforced with an assertion.**

    @throw type_error.305 if the JSON value is not an object; in that case,
    using the [] operator with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be read using
    the `[]` operator.,operatorarray__key_type_const}

    @sa @ref at(const typename object_t::key_type&) for access by reference
    with range checking
    @sa @ref value() for access by value with a default value

    @since version 1.1.0
    */
    template<typename T>
    const_reference operator[](T* key) const
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            assert(m_value.object->find(key) != m_value.object->end());
            return m_value.object->find(key)->second;
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with " + Twine(type_name())));
    }

    /*!
    @brief access specified object element with default value

    Returns either a copy of an object's element at the specified key @a key
    or a given default value if no element with key @a key exists.

    The function is basically equivalent to executing
    @code {.cpp}
    try {
        return at(key);
    } catch(out_of_range) {
        return default_value;
    }
    @endcode

    @note Unlike @ref at(const typename object_t::key_type&), this function
    does not throw if the given key @a key was not found.

    @note Unlike @ref operator[](const typename object_t::key_type& key), this
    function does not implicitly add an element to the position defined by @a
    key. This function is furthermore also applicable to const objects.

    @param[in] key  key of the element to access
    @param[in] default_value  the value to return if @a key is not found

    @tparam ValueType type compatible to JSON values, for instance `int` for
    JSON integer numbers, `bool` for JSON booleans, or `std::vector` types for
    JSON arrays. Note the type of the expected value at @a key and the default
    value @a default_value must be compatible.

    @return copy of the element at key @a key or @a default_value if @a key
    is not found

    @throw type_error.306 if the JSON value is not an object; in that case,
    using `value()` with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be queried
    with a default value.,json__value}

    @sa @ref at(const typename object_t::key_type&) for access by reference
    with range checking
    @sa @ref operator[](const typename object_t::key_type&) for unchecked
    access by reference

    @since version 1.0.0
    */
    template<class ValueType, typename std::enable_if<
                 std::is_convertible<json_t, ValueType>::value, int>::type = 0>
    ValueType value(StringRef key, const ValueType& default_value) const
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            // if key is found, return value and given default value otherwise
            const auto it = find(key);
            if (it != end())
            {
                return *it;
            }

            return default_value;
        }

        JSON_THROW(type_error::create(306, "cannot use value() with " + Twine(type_name())));
    }

    /*!
    @brief overload for a default value of type const char*
    @copydoc json::value(const typename object_t::key_type&, ValueType) const
    */
    std::string value(StringRef key, const char* default_value) const
    {
        return value(key, std::string(default_value));
    }

    /*!
    @brief access specified object element via JSON Pointer with default value

    Returns either a copy of an object's element at the specified key @a key
    or a given default value if no element with key @a key exists.

    The function is basically equivalent to executing
    @code {.cpp}
    try {
        return at(ptr);
    } catch(out_of_range) {
        return default_value;
    }
    @endcode

    @note Unlike @ref at(const json_pointer&), this function does not throw
    if the given key @a key was not found.

    @param[in] ptr  a JSON pointer to the element to access
    @param[in] default_value  the value to return if @a ptr found no value

    @tparam ValueType type compatible to JSON values, for instance `int` for
    JSON integer numbers, `bool` for JSON booleans, or `std::vector` types for
    JSON arrays. Note the type of the expected value at @a key and the default
    value @a default_value must be compatible.

    @return copy of the element at key @a key or @a default_value if @a key
    is not found

    @throw type_error.306 if the JSON value is not an object; in that case,
    using `value()` with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be queried
    with a default value.,json__value_ptr}

    @sa @ref operator[](const json_pointer&) for unchecked access by reference

    @since version 2.0.2
    */
    template<class ValueType, typename std::enable_if<
                 std::is_convertible<json_t, ValueType>::value, int>::type = 0>
    ValueType value(const json_pointer& ptr, const ValueType& default_value) const
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            // if pointer resolves a value, return it or use default value
            JSON_TRY
            {
                return ptr.get_checked(this);
            }
            JSON_CATCH (out_of_range&)
            {
                return default_value;
            }
        }

        JSON_THROW(type_error::create(306, "cannot use value() with " + Twine(type_name())));
    }

    /*!
    @brief overload for a default value of type const char*
    @copydoc json::value(const json_pointer&, ValueType) const
    */
    std::string value(const json_pointer& ptr, const char* default_value) const
    {
        return value(ptr, std::string(default_value));
    }

    /*!
    @brief access the first element

    Returns a reference to the first element in the container. For a JSON
    container `c`, the expression `c.front()` is equivalent to `*c.begin()`.

    @return In case of a structured type (array or object), a reference to the
    first element is returned. In case of number, string, or boolean values, a
    reference to the value is returned.

    @complexity Constant.

    @pre The JSON value must not be `null` (would throw `std::out_of_range`)
    or an empty array or object (undefined behavior, **guarded by
    assertions**).
    @post The JSON value remains unchanged.

    @throw invalid_iterator.214 when called on `null` value

    @liveexample{The following code shows an example for `front()`.,front}

    @sa @ref back() -- access the last element

    @since version 1.0.0
    */
    reference front()
    {
        return *begin();
    }

    /*!
    @copydoc json::front()
    */
    const_reference front() const
    {
        return *cbegin();
    }

    /*!
    @brief access the last element

    Returns a reference to the last element in the container. For a JSON
    container `c`, the expression `c.back()` is equivalent to
    @code {.cpp}
    auto tmp = c.end();
    --tmp;
    return *tmp;
    @endcode

    @return In case of a structured type (array or object), a reference to the
    last element is returned. In case of number, string, or boolean values, a
    reference to the value is returned.

    @complexity Constant.

    @pre The JSON value must not be `null` (would throw `std::out_of_range`)
    or an empty array or object (undefined behavior, **guarded by
    assertions**).
    @post The JSON value remains unchanged.

    @throw invalid_iterator.214 when called on a `null` value. See example
    below.

    @liveexample{The following code shows an example for `back()`.,back}

    @sa @ref front() -- access the first element

    @since version 1.0.0
    */
    reference back()
    {
        auto tmp = end();
        --tmp;
        return *tmp;
    }

    /*!
    @copydoc json::back()
    */
    const_reference back() const
    {
        auto tmp = cend();
        --tmp;
        return *tmp;
    }

    /*!
    @brief remove element given an iterator

    Removes the element specified by iterator @a pos. The iterator @a pos must
    be valid and dereferenceable. Thus the `end()` iterator (which is valid,
    but is not dereferenceable) cannot be used as a value for @a pos.

    If called on a primitive type other than `null`, the resulting JSON value
    will be `null`.

    @param[in] pos iterator to the element to remove

    @tparam IteratorType an @ref iterator or @ref const_iterator

    @post Invalidates iterators and references at or after the point of the
    erase, including the `end()` iterator.

    @throw type_error.307 if called on a `null` value; example: `"cannot use
    erase() with null"`
    @throw invalid_iterator.202 if called on an iterator which does not belong
    to the current JSON value; example: `"iterator does not fit current
    value"`
    @throw invalid_iterator.205 if called on a primitive type with invalid
    iterator (i.e., any iterator which is not `begin()`); example: `"iterator
    out of range"`

    @complexity The complexity depends on the type:
    - objects: amortized constant
    - arrays: linear in distance between @a pos and the end of the container
    - strings: linear in the length of the string
    - other types: constant

    @liveexample{The example shows the result of `erase()` for different JSON
    types.,erase__IteratorType}

    @sa @ref erase(IteratorType, IteratorType) -- removes the elements in
    the given range
    @sa @ref erase(StringRef) -- removes the element
    from an object at the given key
    @sa @ref erase(const size_type) -- removes the element from an array at
    the given index

    @since version 1.0.0
    */
    template<class IteratorType, typename std::enable_if<
                 std::is_same<IteratorType, typename json_t::iterator>::value or
                 std::is_same<IteratorType, typename json_t::const_iterator>::value, int>::type
             = 0>
    void erase(IteratorType pos)
    {
        // make sure iterator fits the current value
        if (JSON_UNLIKELY(this != pos.m_object))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        switch (m_type)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (JSON_UNLIKELY(not pos.m_it.primitive_iterator.is_begin()))
                {
                    JSON_THROW(invalid_iterator::create(205, "iterator out of range"));
                }

                if (is_string())
                {
                    std::allocator<std::string> alloc;
                    alloc.destroy(m_value.string);
                    alloc.deallocate(m_value.string, 1);
                    m_value.string = nullptr;
                }

                m_type = value_t::null;
                assert_invariant();
                break;
            }

            case value_t::object:
            {
                m_value.object->erase(pos.m_it.object_iterator);
                break;
            }

            case value_t::array:
            {
                m_value.array->erase(pos.m_it.array_iterator);
                break;
            }

            default:
                JSON_THROW(type_error::create(307, "cannot use erase() with " + Twine(type_name())));
        }
    }

    /*!
    @brief remove elements given an iterator range

    Removes the element specified by the range `[first; last)`. The iterator
    @a first does not need to be dereferenceable if `first == last`: erasing
    an empty range is a no-op.

    If called on a primitive type other than `null`, the resulting JSON value
    will be `null`.

    @param[in] first iterator to the beginning of the range to remove
    @param[in] last iterator past the end of the range to remove
    @return Iterator following the last removed element. If the iterator @a
    second refers to the last element, the `end()` iterator is returned.

    @tparam IteratorType an @ref iterator or @ref const_iterator

    @post Invalidates iterators and references at or after the point of the
    erase, including the `end()` iterator.

    @throw type_error.307 if called on a `null` value; example: `"cannot use
    erase() with null"`
    @throw invalid_iterator.203 if called on iterators which does not belong
    to the current JSON value; example: `"iterators do not fit current value"`
    @throw invalid_iterator.204 if called on a primitive type with invalid
    iterators (i.e., if `first != begin()` and `last != end()`); example:
    `"iterators out of range"`

    @complexity The complexity depends on the type:
    - objects: `log(size()) + std::distance(first, last)`
    - arrays: linear in the distance between @a first and @a last, plus linear
      in the distance between @a last and end of the container
    - strings: linear in the length of the string
    - other types: constant

    @liveexample{The example shows the result of `erase()` for different JSON
    types.,erase__IteratorType_IteratorType}

    @sa @ref erase(IteratorType) -- removes the element at a given position
    @sa @ref erase(const typename object_t::key_type&) -- removes the element
    from an object at the given key
    @sa @ref erase(const size_type) -- removes the element from an array at
    the given index

    @since version 1.0.0
    */
    template<class IteratorType, typename std::enable_if<
                 std::is_same<IteratorType, typename json_t::iterator>::value or
                 std::is_same<IteratorType, typename json_t::const_iterator>::value, int>::type
             = 0>
    IteratorType erase(IteratorType first, IteratorType last)
    {
        // make sure iterator fits the current value
        if (JSON_UNLIKELY(this != first.m_object or this != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(203, "iterators do not fit current value"));
        }

        IteratorType result = end();

        switch (m_type)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (JSON_LIKELY(not first.m_it.primitive_iterator.is_begin()
                                or not last.m_it.primitive_iterator.is_end()))
                {
                    JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
                }

                if (is_string())
                {
                    std::allocator<std::string> alloc;
                    alloc.destroy(m_value.string);
                    alloc.deallocate(m_value.string, 1);
                    m_value.string = nullptr;
                }

                m_type = value_t::null;
                assert_invariant();
                break;
            }

            case value_t::array:
            {
                result.m_it.array_iterator = m_value.array->erase(first.m_it.array_iterator,
                                             last.m_it.array_iterator);
                break;
            }

            default:
                JSON_THROW(type_error::create(307, "cannot use erase() with " + Twine(type_name())));
        }

        return result;
    }

    /*!
    @brief remove element from a JSON object given a key

    Removes elements from a JSON object with the key value @a key.

    @param[in] key value of the elements to remove

    @return Number of elements removed. If @a ObjectType is the default
    `std::map` type, the return value will always be `0` (@a key was not
    found) or `1` (@a key was found).

    @post References and iterators to the erased elements are invalidated.
    Other references and iterators are not affected.

    @throw type_error.307 when called on a type other than JSON object;
    example: `"cannot use erase() with null"`

    @complexity `log(size()) + count(key)`

    @liveexample{The example shows the effect of `erase()`.,erase__key_type}

    @sa @ref erase(IteratorType) -- removes the element at a given position
    @sa @ref erase(IteratorType, IteratorType) -- removes the elements in
    the given range
    @sa @ref erase(const size_type) -- removes the element from an array at
    the given index

    @since version 1.0.0
    */
    size_type erase(StringRef key);

    /*!
    @brief remove element from a JSON array given an index

    Removes element from a JSON array at the index @a idx.

    @param[in] idx index of the element to remove

    @throw type_error.307 when called on a type other than JSON object;
    example: `"cannot use erase() with null"`
    @throw out_of_range.401 when `idx >= size()`; example: `"array index 17
    is out of range"`

    @complexity Linear in distance between @a idx and the end of the container.

    @liveexample{The example shows the effect of `erase()`.,erase__size_type}

    @sa @ref erase(IteratorType) -- removes the element at a given position
    @sa @ref erase(IteratorType, IteratorType) -- removes the elements in
    the given range
    @sa @ref erase(const typename object_t::key_type&) -- removes the element
    from an object at the given key

    @since version 1.0.0
    */
    void erase(const size_type idx);

    /// @}


    ////////////
    // lookup //
    ////////////

    /// @name lookup
    /// @{

    /*!
    @brief find an element in a JSON object

    Finds an element in a JSON object with key equivalent to @a key. If the
    element is not found or the JSON value is not an object, end() is
    returned.

    @note This method always returns @ref end() when executed on a JSON type
          that is not an object.

    @param[in] key key value of the element to search for.

    @return Iterator to an element with key equivalent to @a key. If no such
    element is found or the JSON value is not an object, past-the-end (see
    @ref end()) iterator is returned.

    @complexity Logarithmic in the size of the JSON object.

    @liveexample{The example shows how `find()` is used.,find__key_type}

    @since version 1.0.0
    */
    iterator find(StringRef key);

    /*!
    @brief find an element in a JSON object
    @copydoc find(KeyT&&)
    */
    const_iterator find(StringRef key) const;

    /*!
    @brief returns the number of occurrences of a key in a JSON object

    Returns the number of elements with key @a key. If ObjectType is the
    default `std::map` type, the return value will always be `0` (@a key was
    not found) or `1` (@a key was found).

    @note This method always returns `0` when executed on a JSON type that is
          not an object.

    @param[in] key key value of the element to count

    @return Number of elements with key @a key. If the JSON value is not an
    object, the return value will be `0`.

    @complexity Logarithmic in the size of the JSON object.

    @liveexample{The example shows how `count()` is used.,count}

    @since version 1.0.0
    */
    size_type count(StringRef key) const;

    /// @}


    ///////////////
    // iterators //
    ///////////////

    /// @name iterators
    /// @{

    /*!
    @brief returns an iterator to the first element

    Returns an iterator to the first element.

    @image html range-begin-end.svg "Illustration from cppreference.com"

    @return iterator to the first element

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.

    @liveexample{The following code shows an example for `begin()`.,begin}

    @sa @ref cbegin() -- returns a const iterator to the beginning
    @sa @ref end() -- returns an iterator to the end
    @sa @ref cend() -- returns a const iterator to the end

    @since version 1.0.0
    */
    iterator begin() noexcept
    {
        iterator result(this);
        result.set_begin();
        return result;
    }

    /*!
    @copydoc json::cbegin()
    */
    const_iterator begin() const noexcept
    {
        return cbegin();
    }

    /*!
    @brief returns a const iterator to the first element

    Returns a const iterator to the first element.

    @image html range-begin-end.svg "Illustration from cppreference.com"

    @return const iterator to the first element

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of `const_cast<const json&>(*this).begin()`.

    @liveexample{The following code shows an example for `cbegin()`.,cbegin}

    @sa @ref begin() -- returns an iterator to the beginning
    @sa @ref end() -- returns an iterator to the end
    @sa @ref cend() -- returns a const iterator to the end

    @since version 1.0.0
    */
    const_iterator cbegin() const noexcept
    {
        const_iterator result(this);
        result.set_begin();
        return result;
    }

    /*!
    @brief returns an iterator to one past the last element

    Returns an iterator to one past the last element.

    @image html range-begin-end.svg "Illustration from cppreference.com"

    @return iterator one past the last element

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.

    @liveexample{The following code shows an example for `end()`.,end}

    @sa @ref cend() -- returns a const iterator to the end
    @sa @ref begin() -- returns an iterator to the beginning
    @sa @ref cbegin() -- returns a const iterator to the beginning

    @since version 1.0.0
    */
    iterator end() noexcept
    {
        iterator result(this);
        result.set_end();
        return result;
    }

    /*!
    @copydoc json::cend()
    */
    const_iterator end() const noexcept
    {
        return cend();
    }

    /*!
    @brief returns a const iterator to one past the last element

    Returns a const iterator to one past the last element.

    @image html range-begin-end.svg "Illustration from cppreference.com"

    @return const iterator one past the last element

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of `const_cast<const json&>(*this).end()`.

    @liveexample{The following code shows an example for `cend()`.,cend}

    @sa @ref end() -- returns an iterator to the end
    @sa @ref begin() -- returns an iterator to the beginning
    @sa @ref cbegin() -- returns a const iterator to the beginning

    @since version 1.0.0
    */
    const_iterator cend() const noexcept
    {
        const_iterator result(this);
        result.set_end();
        return result;
    }

    /*!
    @brief returns an iterator to the reverse-beginning

    Returns an iterator to the reverse-beginning; that is, the last element.

    @image html range-rbegin-rend.svg "Illustration from cppreference.com"

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [ReversibleContainer](http://en.cppreference.com/w/cpp/concept/ReversibleContainer)
    requirements:
    - The complexity is constant.
    - Has the semantics of `reverse_iterator(end())`.

    @liveexample{The following code shows an example for `rbegin()`.,rbegin}

    @sa @ref crbegin() -- returns a const reverse iterator to the beginning
    @sa @ref rend() -- returns a reverse iterator to the end
    @sa @ref crend() -- returns a const reverse iterator to the end

    @since version 1.0.0
    */
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    /*!
    @copydoc json::crbegin()
    */
    const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    /*!
    @brief returns an iterator to the reverse-end

    Returns an iterator to the reverse-end; that is, one before the first
    element.

    @image html range-rbegin-rend.svg "Illustration from cppreference.com"

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [ReversibleContainer](http://en.cppreference.com/w/cpp/concept/ReversibleContainer)
    requirements:
    - The complexity is constant.
    - Has the semantics of `reverse_iterator(begin())`.

    @liveexample{The following code shows an example for `rend()`.,rend}

    @sa @ref crend() -- returns a const reverse iterator to the end
    @sa @ref rbegin() -- returns a reverse iterator to the beginning
    @sa @ref crbegin() -- returns a const reverse iterator to the beginning

    @since version 1.0.0
    */
    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    /*!
    @copydoc json::crend()
    */
    const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    /*!
    @brief returns a const reverse iterator to the last element

    Returns a const iterator to the reverse-beginning; that is, the last
    element.

    @image html range-rbegin-rend.svg "Illustration from cppreference.com"

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [ReversibleContainer](http://en.cppreference.com/w/cpp/concept/ReversibleContainer)
    requirements:
    - The complexity is constant.
    - Has the semantics of `const_cast<const json&>(*this).rbegin()`.

    @liveexample{The following code shows an example for `crbegin()`.,crbegin}

    @sa @ref rbegin() -- returns a reverse iterator to the beginning
    @sa @ref rend() -- returns a reverse iterator to the end
    @sa @ref crend() -- returns a const reverse iterator to the end

    @since version 1.0.0
    */
    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    /*!
    @brief returns a const reverse iterator to one before the first

    Returns a const reverse iterator to the reverse-end; that is, one before
    the first element.

    @image html range-rbegin-rend.svg "Illustration from cppreference.com"

    @complexity Constant.

    @requirement This function helps `json` satisfying the
    [ReversibleContainer](http://en.cppreference.com/w/cpp/concept/ReversibleContainer)
    requirements:
    - The complexity is constant.
    - Has the semantics of `const_cast<const json&>(*this).rend()`.

    @liveexample{The following code shows an example for `crend()`.,crend}

    @sa @ref rend() -- returns a reverse iterator to the end
    @sa @ref rbegin() -- returns a reverse iterator to the beginning
    @sa @ref crbegin() -- returns a const reverse iterator to the beginning

    @since version 1.0.0
    */
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

  public:
    /*!
    @brief helper to access iterator member functions in range-based for

    This function allows to access @ref iterator::key() and @ref
    iterator::value() during range-based for loops. In these loops, a
    reference to the JSON values is returned, so there is no access to the
    underlying iterator.

    For loop without `items()` function:

    @code{cpp}
    for (auto it = j_object.begin(); it != j_object.end(); ++it)
    {
        std::cout << "key: " << it.key() << ", value:" << it.value() << '\n';
    }
    @endcode

    Range-based for loop without `items()` function:

    @code{cpp}
    for (auto it : j_object)
    {
        // "it" is of type json::reference and has no key() member
        std::cout << "value: " << it << '\n';
    }
    @endcode

    Range-based for loop with `items()` function:

    @code{cpp}
    for (auto it : j_object.items())
    {
        std::cout << "key: " << it.key() << ", value:" << it.value() << '\n';
    }
    @endcode

    @note When iterating over an array, `key()` will return the index of the
          element as string (see example). For primitive types (e.g., numbers),
          `key()` returns an empty string.

    @return iteration proxy object wrapping @a ref with an interface to use in
            range-based for loops

    @liveexample{The following code shows how the function is used.,items}

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @complexity Constant.

    @since version 3.x.x.
    */
    iteration_proxy<iterator> items() noexcept
    {
        return iteration_proxy<iterator>(*this);
    }

    /*!
    @copydoc items()
    */
    iteration_proxy<const_iterator> items() const noexcept
    {
        return iteration_proxy<const_iterator>(*this);
    }

    /// @}


    //////////////
    // capacity //
    //////////////

    /// @name capacity
    /// @{

    /*!
    @brief checks whether the container is empty.

    Checks if a JSON value has no elements (i.e. whether its @ref size is `0`).

    @return The return value depends on the different types and is
            defined as follows:
            Value type  | return value
            ----------- | -------------
            null        | `true`
            boolean     | `false`
            string      | `false`
            number      | `false`
            object      | result of function `object_t::empty()`
            array       | result of function `array_t::empty()`

    @liveexample{The following code uses `empty()` to check if a JSON
    object contains any elements.,empty}

    @complexity Constant, as long as @ref array_t and @ref object_t satisfy
    the Container concept; that is, their `empty()` functions have constant
    complexity.

    @iterators No changes.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @note This function does not return whether a string stored as JSON value
    is empty - it returns whether the JSON container itself is empty which is
    false in the case of a string.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of `begin() == end()`.

    @sa @ref size() -- returns the number of elements

    @since version 1.0.0
    */
    bool empty() const noexcept;

    /*!
    @brief returns the number of elements

    Returns the number of elements in a JSON value.

    @return The return value depends on the different types and is
            defined as follows:
            Value type  | return value
            ----------- | -------------
            null        | `0`
            boolean     | `1`
            string      | `1`
            number      | `1`
            object      | result of function object_t::size()
            array       | result of function array_t::size()

    @liveexample{The following code calls `size()` on the different value
    types.,size}

    @complexity Constant, as long as @ref array_t and @ref object_t satisfy
    the Container concept; that is, their size() functions have constant
    complexity.

    @iterators No changes.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @note This function does not return the length of a string stored as JSON
    value - it returns the number of elements in the JSON value which is 1 in
    the case of a string.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of `std::distance(begin(), end())`.

    @sa @ref empty() -- checks whether the container is empty
    @sa @ref max_size() -- returns the maximal number of elements

    @since version 1.0.0
    */
    size_type size() const noexcept;

    /*!
    @brief returns the maximum possible number of elements

    Returns the maximum number of elements a JSON value is able to hold due to
    system or library implementation limitations, i.e. `std::distance(begin(),
    end())` for the JSON value.

    @return The return value depends on the different types and is
            defined as follows:
            Value type  | return value
            ----------- | -------------
            null        | `0` (same as `size()`)
            boolean     | `1` (same as `size()`)
            string      | `1` (same as `size()`)
            number      | `1` (same as `size()`)
            object      | result of function `object_t::max_size()`
            array       | result of function `array_t::max_size()`

    @liveexample{The following code calls `max_size()` on the different value
    types. Note the output is implementation specific.,max_size}

    @complexity Constant, as long as @ref array_t and @ref object_t satisfy
    the Container concept; that is, their `max_size()` functions have constant
    complexity.

    @iterators No changes.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of returning `b.size()` where `b` is the largest
      possible JSON value.

    @sa @ref size() -- returns the number of elements

    @since version 1.0.0
    */
    size_type max_size() const noexcept;

    /// @}


    ///////////////
    // modifiers //
    ///////////////

    /// @name modifiers
    /// @{

    /*!
    @brief clears the contents

    Clears the content of a JSON value and resets it to the default value as
    if @ref json(value_t) would have been called with the current value
    type from @ref type():

    Value type  | initial value
    ----------- | -------------
    null        | `null`
    boolean     | `false`
    string      | `""`
    number      | `0`
    object      | `{}`
    array       | `[]`

    @post Has the same effect as calling
    @code {.cpp}
    *this = json(type());
    @endcode

    @liveexample{The example below shows the effect of `clear()` to different
    JSON types.,clear}

    @complexity Linear in the size of the JSON value.

    @iterators All iterators, pointers and references related to this container
               are invalidated.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @sa @ref json(value_t) -- constructor that creates an object with the
        same value than calling `clear()`

    @since version 1.0.0
    */
    void clear() noexcept;

    /*!
    @brief add an object to an array

    Appends the given element @a val to the end of the JSON value. If the
    function is called on a JSON null value, an empty array is created before
    appending @a val.

    @param[in] val the value to add to the JSON array

    @throw type_error.308 when called on a type other than JSON array or
    null; example: `"cannot use push_back() with number"`

    @complexity Amortized constant.

    @liveexample{The example shows how `push_back()` and `+=` can be used to
    add elements to a JSON array. Note how the `null` value was silently
    converted to a JSON array.,push_back}

    @since version 1.0.0
    */
    void push_back(json&& val);

    /*!
    @brief add an object to an array
    @copydoc push_back(json&&)
    */
    reference operator+=(json&& val)
    {
        push_back(std::move(val));
        return *this;
    }

    /*!
    @brief add an object to an array
    @copydoc push_back(json&&)
    */
    void push_back(const json& val);

    /*!
    @brief add an object to an array
    @copydoc push_back(json&&)
    */
    reference operator+=(const json& val)
    {
        push_back(val);
        return *this;
    }

    /*!
    @brief add an object to an object

    Inserts the given element @a val to the JSON object. If the function is
    called on a JSON null value, an empty object is created before inserting
    @a val.

    @param[in] val the value to add to the JSON object

    @throw type_error.308 when called on a type other than JSON object or
    null; example: `"cannot use push_back() with number"`

    @complexity Logarithmic in the size of the container, O(log(`size()`)).

    @liveexample{The example shows how `push_back()` and `+=` can be used to
    add elements to a JSON object. Note how the `null` value was silently
    converted to a JSON object.,push_back__object_t__value}

    @since version 1.0.0
    */
    template<typename T, typename U>
    void push_back(const std::pair<T, U>& val)
    {
        // push_back only works for null objects or objects
        if (JSON_UNLIKELY(not(is_null() or is_object())))
        {
            JSON_THROW(type_error::create(308, "cannot use push_back() with " + Twine(type_name())));
        }

        // transform null object into an object
        if (is_null())
        {
            m_type = value_t::object;
            m_value = value_t::object;
            assert_invariant();
        }

        // add element to array
        m_value.object->try_emplace(val.first, std::move(val.second));
    }

    /*!
    @brief add an object to an object
    @copydoc push_back(const typename object_t::value_type&)
    */
    template<typename T, typename U>
    reference operator+=(const std::pair<T, U>& val)
    {
        push_back(val);
        return *this;
    }

    /*!
    @brief add an object to an object

    This function allows to use `push_back` with an initializer list. In case

    1. the current value is an object,
    2. the initializer list @a init contains only two elements, and
    3. the first element of @a init is a string,

    @a init is converted into an object element and added using
    @ref push_back(const typename object_t::value_type&). Otherwise, @a init
    is converted to a JSON value and added using @ref push_back(json&&).

    @param[in] init  an initializer list

    @complexity Linear in the size of the initializer list @a init.

    @note This function is required to resolve an ambiguous overload error,
          because pairs like `{"key", "value"}` can be both interpreted as
          `object_t::value_type` or `std::initializer_list<json>`, see
          https://github.com/nlohmann/json/issues/235 for more information.

    @liveexample{The example shows how initializer lists are treated as
    objects when possible.,push_back__initializer_list}
    */
    void push_back(initializer_list_t init);

    /*!
    @brief add an object to an object
    @copydoc push_back(initializer_list_t)
    */
    reference operator+=(initializer_list_t init)
    {
        push_back(init);
        return *this;
    }

    /*!
    @brief add an object to an array

    Creates a JSON value from the passed parameters @a args to the end of the
    JSON value. If the function is called on a JSON null value, an empty array
    is created before appending the value created from @a args.

    @param[in] args arguments to forward to a constructor of @ref json
    @tparam Args compatible types to create a @ref json object

    @throw type_error.311 when called on a type other than JSON array or
    null; example: `"cannot use emplace_back() with number"`

    @complexity Amortized constant.

    @liveexample{The example shows how `push_back()` can be used to add
    elements to a JSON array. Note how the `null` value was silently converted
    to a JSON array.,emplace_back}

    @since version 2.0.8
    */
    template<class... Args>
    void emplace_back(Args&& ... args)
    {
        // emplace_back only works for null objects or arrays
        if (JSON_UNLIKELY(not(is_null() or is_array())))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace_back() with " + Twine(type_name())));
        }

        // transform null object into an array
        if (is_null())
        {
            m_type = value_t::array;
            m_value = value_t::array;
            assert_invariant();
        }

        // add element to array (perfect forwarding)
        m_value.array->emplace_back(std::forward<Args>(args)...);
    }

    /*!
    @brief add an object to an object if key does not exist

    Inserts a new element into a JSON object constructed in-place with the
    given @a args if there is no element with the key in the container. If the
    function is called on a JSON null value, an empty object is created before
    appending the value created from @a args.

    @param[in] args arguments to forward to a constructor of @ref json
    @tparam Args compatible types to create a @ref json object

    @return a pair consisting of an iterator to the inserted element, or the
            already-existing element if no insertion happened, and a bool
            denoting whether the insertion took place.

    @throw type_error.311 when called on a type other than JSON object or
    null; example: `"cannot use emplace() with number"`

    @complexity Logarithmic in the size of the container, O(log(`size()`)).

    @liveexample{The example shows how `emplace()` can be used to add elements
    to a JSON object. Note how the `null` value was silently converted to a
    JSON object. Further note how no value is added if there was already one
    value stored with the same key.,emplace}

    @since version 2.0.8
    */
    template<class... Args>
    std::pair<iterator, bool> emplace(StringRef key, Args&& ... args)
    {
        // emplace only works for null objects or arrays
        if (JSON_UNLIKELY(not(is_null() or is_object())))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace() with " + Twine(type_name())));
        }

        // transform null object into an object
        if (is_null())
        {
            m_type = value_t::object;
            m_value = value_t::object;
            assert_invariant();
        }

        // add element to array (perfect forwarding)
        auto res = m_value.object->try_emplace(key, std::forward<Args>(args)...);
        // create result iterator and set iterator to the result of emplace
        auto it = begin();
        it.m_it.object_iterator = res.first;

        // return pair of iterator and boolean
        return {it, res.second};
    }

    /*!
    @brief inserts element

    Inserts element @a val before iterator @a pos.

    @param[in] pos iterator before which the content will be inserted; may be
    the end() iterator
    @param[in] val element to insert
    @return iterator pointing to the inserted @a val.

    @throw type_error.309 if called on JSON values other than arrays;
    example: `"cannot use insert() with string"`
    @throw invalid_iterator.202 if @a pos is not an iterator of *this;
    example: `"iterator does not fit current value"`

    @complexity Constant plus linear in the distance between @a pos and end of
    the container.

    @liveexample{The example shows how `insert()` is used.,insert}

    @since version 1.0.0
    */
    iterator insert(const_iterator pos, const json& val);

    /*!
    @brief inserts element
    @copydoc insert(const_iterator, const json&)
    */
    iterator insert(const_iterator pos, json&& val)
    {
        return insert(pos, val);
    }

    /*!
    @brief inserts elements

    Inserts @a cnt copies of @a val before iterator @a pos.

    @param[in] pos iterator before which the content will be inserted; may be
    the end() iterator
    @param[in] cnt number of copies of @a val to insert
    @param[in] val element to insert
    @return iterator pointing to the first element inserted, or @a pos if
    `cnt==0`

    @throw type_error.309 if called on JSON values other than arrays; example:
    `"cannot use insert() with string"`
    @throw invalid_iterator.202 if @a pos is not an iterator of *this;
    example: `"iterator does not fit current value"`

    @complexity Linear in @a cnt plus linear in the distance between @a pos
    and end of the container.

    @liveexample{The example shows how `insert()` is used.,insert__count}

    @since version 1.0.0
    */
    iterator insert(const_iterator pos, size_type cnt, const json& val);

    /*!
    @brief inserts elements

    Inserts elements from range `[first, last)` before iterator @a pos.

    @param[in] pos iterator before which the content will be inserted; may be
    the end() iterator
    @param[in] first begin of the range of elements to insert
    @param[in] last end of the range of elements to insert

    @throw type_error.309 if called on JSON values other than arrays; example:
    `"cannot use insert() with string"`
    @throw invalid_iterator.202 if @a pos is not an iterator of *this;
    example: `"iterator does not fit current value"`
    @throw invalid_iterator.210 if @a first and @a last do not belong to the
    same JSON value; example: `"iterators do not fit"`
    @throw invalid_iterator.211 if @a first or @a last are iterators into
    container for which insert is called; example: `"passed iterators may not
    belong to container"`

    @return iterator pointing to the first element inserted, or @a pos if
    `first==last`

    @complexity Linear in `std::distance(first, last)` plus linear in the
    distance between @a pos and end of the container.

    @liveexample{The example shows how `insert()` is used.,insert__range}

    @since version 1.0.0
    */
    iterator insert(const_iterator pos, const_iterator first, const_iterator last);

    /*!
    @brief inserts elements

    Inserts elements from initializer list @a ilist before iterator @a pos.

    @param[in] pos iterator before which the content will be inserted; may be
    the end() iterator
    @param[in] ilist initializer list to insert the values from

    @throw type_error.309 if called on JSON values other than arrays; example:
    `"cannot use insert() with string"`
    @throw invalid_iterator.202 if @a pos is not an iterator of *this;
    example: `"iterator does not fit current value"`

    @return iterator pointing to the first element inserted, or @a pos if
    `ilist` is empty

    @complexity Linear in `ilist.size()` plus linear in the distance between
    @a pos and end of the container.

    @liveexample{The example shows how `insert()` is used.,insert__ilist}

    @since version 1.0.0
    */
    iterator insert(const_iterator pos, initializer_list_t ilist);

    /*!
    @brief inserts elements

    Inserts elements from range `[first, last)`.

    @param[in] first begin of the range of elements to insert
    @param[in] last end of the range of elements to insert

    @throw type_error.309 if called on JSON values other than objects; example:
    `"cannot use insert() with string"`
    @throw invalid_iterator.202 if iterator @a first or @a last does does not
    point to an object; example: `"iterators first and last must point to
    objects"`
    @throw invalid_iterator.210 if @a first and @a last do not belong to the
    same JSON value; example: `"iterators do not fit"`

    @complexity Logarithmic: `O(N*log(size() + N))`, where `N` is the number
    of elements to insert.

    @liveexample{The example shows how `insert()` is used.,insert__range_object}

    @since version 3.0.0
    */
    void insert(const_iterator first, const_iterator last);

    /*!
    @brief updates a JSON object from another object, overwriting existing keys

    Inserts all values from JSON object @a j and overwrites existing keys.

    @param[in] j  JSON object to read values from

    @throw type_error.312 if called on JSON values other than objects; example:
    `"cannot use update() with string"`

    @complexity O(N*log(size() + N)), where N is the number of elements to
                insert.

    @liveexample{The example shows how `update()` is used.,update}

    @sa https://docs.python.org/3.6/library/stdtypes.html#dict.update

    @since version 3.0.0
    */
    void update(const_reference j);

    /*!
    @brief updates a JSON object from another object, overwriting existing keys

    Inserts all values from from range `[first, last)` and overwrites existing
    keys.

    @param[in] first begin of the range of elements to insert
    @param[in] last end of the range of elements to insert

    @throw type_error.312 if called on JSON values other than objects; example:
    `"cannot use update() with string"`
    @throw invalid_iterator.202 if iterator @a first or @a last does does not
    point to an object; example: `"iterators first and last must point to
    objects"`
    @throw invalid_iterator.210 if @a first and @a last do not belong to the
    same JSON value; example: `"iterators do not fit"`

    @complexity O(N*log(size() + N)), where N is the number of elements to
                insert.

    @liveexample{The example shows how `update()` is used__range.,update}

    @sa https://docs.python.org/3.6/library/stdtypes.html#dict.update

    @since version 3.0.0
    */
    void update(const_iterator first, const_iterator last);

    /*!
    @brief exchanges the values

    Exchanges the contents of the JSON value with those of @a other. Does not
    invoke any move, copy, or swap operations on individual elements. All
    iterators and references remain valid. The past-the-end iterator is
    invalidated.

    @param[in,out] other JSON value to exchange the contents with

    @complexity Constant.

    @liveexample{The example below shows how JSON values can be swapped with
    `swap()`.,swap__reference}

    @since version 1.0.0
    */
    void swap(reference other) noexcept (
        std::is_nothrow_move_constructible<value_t>::value and
        std::is_nothrow_move_assignable<value_t>::value and
        std::is_nothrow_move_constructible<json_value>::value and
        std::is_nothrow_move_assignable<json_value>::value
    )
    {
        std::swap(m_type, other.m_type);
        std::swap(m_value, other.m_value);
        assert_invariant();
    }

    /*!
    @brief exchanges the values

    Exchanges the contents of a JSON array with those of @a other. Does not
    invoke any move, copy, or swap operations on individual elements. All
    iterators and references remain valid. The past-the-end iterator is
    invalidated.

    @param[in,out] other array to exchange the contents with

    @throw type_error.310 when JSON value is not an array; example: `"cannot
    use swap() with string"`

    @complexity Constant.

    @liveexample{The example below shows how arrays can be swapped with
    `swap()`.,swap__std_vector_json}

    @since version 1.0.0
    */
    void swap(array_t& other)
    {
        // swap only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            std::swap(*(m_value.array), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + Twine(type_name())));
        }
    }

    /*!
    @brief exchanges the values

    Exchanges the contents of a JSON object with those of @a other. Does not
    invoke any move, copy, or swap operations on individual elements. All
    iterators and references remain valid. The past-the-end iterator is
    invalidated.

    @param[in,out] other object to exchange the contents with

    @throw type_error.310 when JSON value is not an object; example:
    `"cannot use swap() with string"`

    @complexity Constant.

    @liveexample{The example below shows how objects can be swapped with
    `swap()`.,swap__object_t}

    @since version 1.0.0
    */
    void swap(object_t& other)
    {
        // swap only works for objects
        if (JSON_LIKELY(is_object()))
        {
            std::swap(*(m_value.object), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + Twine(type_name())));
        }
    }

    /*!
    @brief exchanges the values

    Exchanges the contents of a JSON string with those of @a other. Does not
    invoke any move, copy, or swap operations on individual elements. All
    iterators and references remain valid. The past-the-end iterator is
    invalidated.

    @param[in,out] other string to exchange the contents with

    @throw type_error.310 when JSON value is not a string; example: `"cannot
    use swap() with boolean"`

    @complexity Constant.

    @liveexample{The example below shows how strings can be swapped with
    `swap()`.,swap__std_string}

    @since version 1.0.0
    */
    void swap(std::string& other)
    {
        // swap only works for strings
        if (JSON_LIKELY(is_string()))
        {
            std::swap(*(m_value.string), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + Twine(type_name())));
        }
    }

    /// @}

  public:
    //////////////////////////////////////////
    // lexicographical comparison operators //
    //////////////////////////////////////////

    /// @name lexicographical comparison operators
    /// @{

    /*!
    @brief comparison: equal

    Compares two JSON values for equality according to the following rules:
    - Two JSON values are equal if (1) they are from the same type and (2)
      their stored values are the same according to their respective
      `operator==`.
    - Integer and floating-point numbers are automatically converted before
      comparison. Note than two NaN values are always treated as unequal.
    - Two JSON null values are equal.

    @note Floating-point inside JSON values numbers are compared with
    `double::operator==`.
    To compare floating-point while respecting an epsilon, an alternative
    [comparison function](https://github.com/mariokonrad/marnav/blob/master/src/marnav/math/floatingpoint.hpp#L34-#L39)
    could be used, for instance
    @code {.cpp}
    template<typename T, typename = typename std::enable_if<std::is_floating_point<T>::value, T>::type>
    inline bool is_same(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) noexcept
    {
        return std::abs(a - b) <= epsilon;
    }
    @endcode

    @note NaN values never compare equal to themselves or to other NaN values.

    @param[in] lhs  first JSON value to consider
    @param[in] rhs  second JSON value to consider
    @return whether the values @a lhs and @a rhs are equal

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @complexity Linear.

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__equal}

    @since version 1.0.0
    */
    friend bool operator==(const_reference lhs, const_reference rhs) noexcept;

    /*!
    @brief comparison: equal
    @copydoc operator==(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator==(const_reference lhs, const ScalarType rhs) noexcept
    {
        return (lhs == json(rhs));
    }

    /*!
    @brief comparison: equal
    @copydoc operator==(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator==(const ScalarType lhs, const_reference rhs) noexcept
    {
        return (json(lhs) == rhs);
    }

    /*!
    @brief comparison: not equal

    Compares two JSON values for inequality by calculating `not (lhs == rhs)`.

    @param[in] lhs  first JSON value to consider
    @param[in] rhs  second JSON value to consider
    @return whether the values @a lhs and @a rhs are not equal

    @complexity Linear.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__notequal}

    @since version 1.0.0
    */
    friend bool operator!=(const_reference lhs, const_reference rhs) noexcept
    {
        return not (lhs == rhs);
    }

    /*!
    @brief comparison: not equal
    @copydoc operator!=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator!=(const_reference lhs, const ScalarType rhs) noexcept
    {
        return (lhs != json(rhs));
    }

    /*!
    @brief comparison: not equal
    @copydoc operator!=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator!=(const ScalarType lhs, const_reference rhs) noexcept
    {
        return (json(lhs) != rhs);
    }

    /*!
    @brief comparison: less than

    Compares whether one JSON value @a lhs is less than another JSON value @a
    rhs according to the following rules:
    - If @a lhs and @a rhs have the same type, the values are compared using
      the default `<` operator.
    - Integer and floating-point numbers are automatically converted before
      comparison
    - In case @a lhs and @a rhs have different types, the values are ignored
      and the order of the types is considered, see
      @ref operator<(const value_t, const value_t).

    @param[in] lhs  first JSON value to consider
    @param[in] rhs  second JSON value to consider
    @return whether @a lhs is less than @a rhs

    @complexity Linear.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__less}

    @since version 1.0.0
    */
    friend bool operator<(const_reference lhs, const_reference rhs) noexcept;

    /*!
    @brief comparison: less than
    @copydoc operator<(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<(const_reference lhs, const ScalarType rhs) noexcept
    {
        return (lhs < json(rhs));
    }

    /*!
    @brief comparison: less than
    @copydoc operator<(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<(const ScalarType lhs, const_reference rhs) noexcept
    {
        return (json(lhs) < rhs);
    }

    /*!
    @brief comparison: less than or equal

    Compares whether one JSON value @a lhs is less than or equal to another
    JSON value by calculating `not (rhs < lhs)`.

    @param[in] lhs  first JSON value to consider
    @param[in] rhs  second JSON value to consider
    @return whether @a lhs is less than or equal to @a rhs

    @complexity Linear.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__greater}

    @since version 1.0.0
    */
    friend bool operator<=(const_reference lhs, const_reference rhs) noexcept
    {
        return not (rhs < lhs);
    }

    /*!
    @brief comparison: less than or equal
    @copydoc operator<=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<=(const_reference lhs, const ScalarType rhs) noexcept
    {
        return (lhs <= json(rhs));
    }

    /*!
    @brief comparison: less than or equal
    @copydoc operator<=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<=(const ScalarType lhs, const_reference rhs) noexcept
    {
        return (json(lhs) <= rhs);
    }

    /*!
    @brief comparison: greater than

    Compares whether one JSON value @a lhs is greater than another
    JSON value by calculating `not (lhs <= rhs)`.

    @param[in] lhs  first JSON value to consider
    @param[in] rhs  second JSON value to consider
    @return whether @a lhs is greater than to @a rhs

    @complexity Linear.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__lessequal}

    @since version 1.0.0
    */
    friend bool operator>(const_reference lhs, const_reference rhs) noexcept
    {
        return not (lhs <= rhs);
    }

    /*!
    @brief comparison: greater than
    @copydoc operator>(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>(const_reference lhs, const ScalarType rhs) noexcept
    {
        return (lhs > json(rhs));
    }

    /*!
    @brief comparison: greater than
    @copydoc operator>(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>(const ScalarType lhs, const_reference rhs) noexcept
    {
        return (json(lhs) > rhs);
    }

    /*!
    @brief comparison: greater than or equal

    Compares whether one JSON value @a lhs is greater than or equal to another
    JSON value by calculating `not (lhs < rhs)`.

    @param[in] lhs  first JSON value to consider
    @param[in] rhs  second JSON value to consider
    @return whether @a lhs is greater than or equal to @a rhs

    @complexity Linear.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__greaterequal}

    @since version 1.0.0
    */
    friend bool operator>=(const_reference lhs, const_reference rhs) noexcept
    {
        return not (lhs < rhs);
    }

    /*!
    @brief comparison: greater than or equal
    @copydoc operator>=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>=(const_reference lhs, const ScalarType rhs) noexcept
    {
        return (lhs >= json(rhs));
    }

    /*!
    @brief comparison: greater than or equal
    @copydoc operator>=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>=(const ScalarType lhs, const_reference rhs) noexcept
    {
        return (json(lhs) >= rhs);
    }

    /// @}

    ///////////////////
    // serialization //
    ///////////////////

    /// @name serialization
    /// @{

    /*!
    @brief serialize to stream

    Serialize the given JSON value @a j to the output stream @a o. The JSON
    value will be serialized using the @ref dump member function.

    - The indentation of the output can be controlled with the member variable
      `width` of the output stream @a o. For instance, using the manipulator
      `std::setw(4)` on @a o sets the indentation level to `4` and the
      serialization result is the same as calling `dump(4)`.

    - The indentation character can be controlled with the member variable
      `fill` of the output stream @a o. For instance, the manipulator
      `std::setfill('\\t')` sets indentation to use a tab character rather than
      the default space character.

    @param[in,out] o  stream to serialize to
    @param[in] j  JSON value to serialize

    @return the stream @a o

    @throw type_error.316 if a string stored inside the JSON value is not
                          UTF-8 encoded

    @complexity Linear.

    @liveexample{The example below shows the serialization with different
    parameters to `width` to adjust the indentation level.,operator_serialize}

    @since version 1.0.0; indentation character added in version 3.0.0
    */
    friend raw_ostream& operator<<(raw_ostream& o, const json& j);

    /// @}


    /////////////////////
    // deserialization //
    /////////////////////

    /// @name deserialization
    /// @{

    /*!
    @brief deserialize from a compatible input

    This function reads from a compatible input. Examples are:
    - an array of 1-byte values
    - strings with character/literal type with size of 1 byte
    - input streams
    - container with contiguous storage of 1-byte values. Compatible container
      types include `std::vector`, `std::string`, `std::array`,
      and `std::initializer_list`. Furthermore, C-style
      arrays can be used with `std::begin()`/`std::end()`. User-defined
      containers can be used as long as they implement random-access iterators
      and a contiguous storage.

    @pre Each element of the container has a size of 1 byte. Violating this
    precondition yields undefined behavior. **This precondition is enforced
    with a static assertion.**

    @pre The container storage is contiguous. Violating this precondition
    yields undefined behavior. **This precondition is enforced with an
    assertion.**
    @pre Each element of the container has a size of 1 byte. Violating this
    precondition yields undefined behavior. **This precondition is enforced
    with a static assertion.**

    @warning There is no way to enforce all preconditions at compile-time. If
             the function is called with a noncompliant container and with
             assertions switched off, the behavior is undefined and will most
             likely yield segmentation violation.

    @param[in] i  input to read from
    @param[in] cb  a parser callback function of type @ref parser_callback_t
    which is used to control the deserialization by filtering unwanted values
    (optional)

    @return result of the deserialization

    @throw parse_error.101 if a parse error occurs; example: `""unexpected end
    of input; expected string literal""`
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails

    @complexity Linear in the length of the input. The parser is a predictive
    LL(1) parser. The complexity can be higher if the parser callback function
    @a cb has a super-linear complexity.

    @note A UTF-8 byte order mark is silently ignored.

    @liveexample{The example below demonstrates the `parse()` function reading
    from an array.,parse__array__parser_callback_t}

    @liveexample{The example below demonstrates the `parse()` function with
    and without callback function.,parse__string__parser_callback_t}

    @liveexample{The example below demonstrates the `parse()` function with
    and without callback function.,parse__istream__parser_callback_t}

    @liveexample{The example below demonstrates the `parse()` function reading
    from a contiguous container.,parse__contiguouscontainer__parser_callback_t}

    @since version 2.0.3 (contiguous containers)
    */
    static json parse(StringRef s,
                            const parser_callback_t cb = nullptr,
                            const bool allow_exceptions = true);

    static json parse(ArrayRef<uint8_t> arr,
                            const parser_callback_t cb = nullptr,
                            const bool allow_exceptions = true);

    /*!
    @copydoc json parse(raw_istream&, const parser_callback_t)
    */
    static json parse(raw_istream& i,
                            const parser_callback_t cb = nullptr,
                            const bool allow_exceptions = true);

    static bool accept(StringRef s);

    static bool accept(ArrayRef<uint8_t> arr);

    static bool accept(raw_istream& i);

    /*!
    @brief deserialize from stream

    Deserializes an input stream to a JSON value.

    @param[in,out] i  input stream to read a serialized JSON value from
    @param[in,out] j  JSON value to write the deserialized input to

    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails

    @complexity Linear in the length of the input. The parser is a predictive
    LL(1) parser.

    @note A UTF-8 byte order mark is silently ignored.

    @liveexample{The example below shows how a JSON value is constructed by
    reading a serialization from a stream.,operator_deserialize}

    @sa parse(std::istream&, const parser_callback_t) for a variant with a
    parser callback function to filter values while parsing

    @since version 1.0.0
    */
    friend raw_istream& operator>>(raw_istream& i, json& j);

    /// @}

    ///////////////////////////
    // convenience functions //
    ///////////////////////////

    /*!
    @brief return the type as string

    Returns the type name as string to be used in error messages - usually to
    indicate that a function was called on a wrong JSON type.

    @return a string representation of a the @a m_type member:
            Value type  | return value
            ----------- | -------------
            null        | `"null"`
            boolean     | `"boolean"`
            string      | `"string"`
            number      | `"number"` (for all number types)
            object      | `"object"`
            array       | `"array"`
            discarded   | `"discarded"`

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @complexity Constant.

    @liveexample{The following code exemplifies `type_name()` for all JSON
    types.,type_name}

    @sa @ref type() -- return the type of the JSON value
    @sa @ref operator value_t() -- return the type of the JSON value (implicit)

    @since version 1.0.0, public since 2.1.0, `const char*` and `noexcept`
    since 3.0.0
    */
    const char* type_name() const noexcept;


  private:
    //////////////////////
    // member variables //
    //////////////////////

    /// the type of the current element
    value_t m_type = value_t::null;

    /// the value of the current element
    json_value m_value = {};

    //////////////////////////////////////////
    // binary serialization/deserialization //
    //////////////////////////////////////////

    /// @name binary serialization/deserialization support
    /// @{

  public:
    /*!
    @brief create a CBOR serialization of a given JSON value

    Serializes a given JSON value @a j to a byte vector using the CBOR (Concise
    Binary Object Representation) serialization format. CBOR is a binary
    serialization format which aims to be more compact than JSON itself, yet
    more efficient to parse.

    The library uses the following mapping from JSON values types to
    CBOR types according to the CBOR specification (RFC 7049):

    JSON value type | value/range                                | CBOR type                          | first byte
    --------------- | ------------------------------------------ | ---------------------------------- | ---------------
    null            | `null`                                     | Null                               | 0xF6
    boolean         | `true`                                     | True                               | 0xF5
    boolean         | `false`                                    | False                              | 0xF4
    number_integer  | -9223372036854775808..-2147483649          | Negative integer (8 bytes follow)  | 0x3B
    number_integer  | -2147483648..-32769                        | Negative integer (4 bytes follow)  | 0x3A
    number_integer  | -32768..-129                               | Negative integer (2 bytes follow)  | 0x39
    number_integer  | -128..-25                                  | Negative integer (1 byte follow)   | 0x38
    number_integer  | -24..-1                                    | Negative integer                   | 0x20..0x37
    number_integer  | 0..23                                      | Integer                            | 0x00..0x17
    number_integer  | 24..255                                    | Unsigned integer (1 byte follow)   | 0x18
    number_integer  | 256..65535                                 | Unsigned integer (2 bytes follow)  | 0x19
    number_integer  | 65536..4294967295                          | Unsigned integer (4 bytes follow)  | 0x1A
    number_integer  | 4294967296..18446744073709551615           | Unsigned integer (8 bytes follow)  | 0x1B
    number_unsigned | 0..23                                      | Integer                            | 0x00..0x17
    number_unsigned | 24..255                                    | Unsigned integer (1 byte follow)   | 0x18
    number_unsigned | 256..65535                                 | Unsigned integer (2 bytes follow)  | 0x19
    number_unsigned | 65536..4294967295                          | Unsigned integer (4 bytes follow)  | 0x1A
    number_unsigned | 4294967296..18446744073709551615           | Unsigned integer (8 bytes follow)  | 0x1B
    number_float    | *any value*                                | Double-Precision Float             | 0xFB
    string          | *length*: 0..23                            | UTF-8 string                       | 0x60..0x77
    string          | *length*: 23..255                          | UTF-8 string (1 byte follow)       | 0x78
    string          | *length*: 256..65535                       | UTF-8 string (2 bytes follow)      | 0x79
    string          | *length*: 65536..4294967295                | UTF-8 string (4 bytes follow)      | 0x7A
    string          | *length*: 4294967296..18446744073709551615 | UTF-8 string (8 bytes follow)      | 0x7B
    array           | *size*: 0..23                              | array                              | 0x80..0x97
    array           | *size*: 23..255                            | array (1 byte follow)              | 0x98
    array           | *size*: 256..65535                         | array (2 bytes follow)             | 0x99
    array           | *size*: 65536..4294967295                  | array (4 bytes follow)             | 0x9A
    array           | *size*: 4294967296..18446744073709551615   | array (8 bytes follow)             | 0x9B
    object          | *size*: 0..23                              | map                                | 0xA0..0xB7
    object          | *size*: 23..255                            | map (1 byte follow)                | 0xB8
    object          | *size*: 256..65535                         | map (2 bytes follow)               | 0xB9
    object          | *size*: 65536..4294967295                  | map (4 bytes follow)               | 0xBA
    object          | *size*: 4294967296..18446744073709551615   | map (8 bytes follow)               | 0xBB

    @note The mapping is **complete** in the sense that any JSON value type
          can be converted to a CBOR value.

    @note If NaN or Infinity are stored inside a JSON number, they are
          serialized properly. This behavior differs from the @ref dump()
          function which serializes NaN or Infinity to `null`.

    @note The following CBOR types are not used in the conversion:
          - byte strings (0x40..0x5F)
          - UTF-8 strings terminated by "break" (0x7F)
          - arrays terminated by "break" (0x9F)
          - maps terminated by "break" (0xBF)
          - date/time (0xC0..0xC1)
          - bignum (0xC2..0xC3)
          - decimal fraction (0xC4)
          - bigfloat (0xC5)
          - tagged items (0xC6..0xD4, 0xD8..0xDB)
          - expected conversions (0xD5..0xD7)
          - simple values (0xE0..0xF3, 0xF8)
          - undefined (0xF7)
          - half and single-precision floats (0xF9-0xFA)
          - break (0xFF)

    @param[in] j  JSON value to serialize
    @return MessagePack serialization as byte vector

    @complexity Linear in the size of the JSON value @a j.

    @liveexample{The example shows the serialization of a JSON value to a byte
    vector in CBOR format.,to_cbor}

    @sa http://cbor.io
    @sa @ref from_cbor(raw_istream&, const bool strict) for the
        analogous deserialization
    @sa @ref to_msgpack(const json&) for the related MessagePack format
    @sa @ref to_ubjson(const json&, const bool, const bool) for the
             related UBJSON format

    @since version 2.0.9
    */
    static std::vector<uint8_t> to_cbor(const json& j);
    static ArrayRef<uint8_t> to_cbor(const json& j, std::vector<uint8_t>& buf);
    static ArrayRef<uint8_t> to_cbor(const json& j, SmallVectorImpl<uint8_t>& buf);
    static void to_cbor(raw_ostream& os, const json& j);

    /*!
    @brief create a MessagePack serialization of a given JSON value

    Serializes a given JSON value @a j to a byte vector using the MessagePack
    serialization format. MessagePack is a binary serialization format which
    aims to be more compact than JSON itself, yet more efficient to parse.

    The library uses the following mapping from JSON values types to
    MessagePack types according to the MessagePack specification:

    JSON value type | value/range                       | MessagePack type | first byte
    --------------- | --------------------------------- | ---------------- | ----------
    null            | `null`                            | nil              | 0xC0
    boolean         | `true`                            | true             | 0xC3
    boolean         | `false`                           | false            | 0xC2
    number_integer  | -9223372036854775808..-2147483649 | int64            | 0xD3
    number_integer  | -2147483648..-32769               | int32            | 0xD2
    number_integer  | -32768..-129                      | int16            | 0xD1
    number_integer  | -128..-33                         | int8             | 0xD0
    number_integer  | -32..-1                           | negative fixint  | 0xE0..0xFF
    number_integer  | 0..127                            | positive fixint  | 0x00..0x7F
    number_integer  | 128..255                          | uint 8           | 0xCC
    number_integer  | 256..65535                        | uint 16          | 0xCD
    number_integer  | 65536..4294967295                 | uint 32          | 0xCE
    number_integer  | 4294967296..18446744073709551615  | uint 64          | 0xCF
    number_unsigned | 0..127                            | positive fixint  | 0x00..0x7F
    number_unsigned | 128..255                          | uint 8           | 0xCC
    number_unsigned | 256..65535                        | uint 16          | 0xCD
    number_unsigned | 65536..4294967295                 | uint 32          | 0xCE
    number_unsigned | 4294967296..18446744073709551615  | uint 64          | 0xCF
    number_float    | *any value*                       | float 64         | 0xCB
    string          | *length*: 0..31                   | fixstr           | 0xA0..0xBF
    string          | *length*: 32..255                 | str 8            | 0xD9
    string          | *length*: 256..65535              | str 16           | 0xDA
    string          | *length*: 65536..4294967295       | str 32           | 0xDB
    array           | *size*: 0..15                     | fixarray         | 0x90..0x9F
    array           | *size*: 16..65535                 | array 16         | 0xDC
    array           | *size*: 65536..4294967295         | array 32         | 0xDD
    object          | *size*: 0..15                     | fix map          | 0x80..0x8F
    object          | *size*: 16..65535                 | map 16           | 0xDE
    object          | *size*: 65536..4294967295         | map 32           | 0xDF

    @note The mapping is **complete** in the sense that any JSON value type
          can be converted to a MessagePack value.

    @note The following values can **not** be converted to a MessagePack value:
          - strings with more than 4294967295 bytes
          - arrays with more than 4294967295 elements
          - objects with more than 4294967295 elements

    @note The following MessagePack types are not used in the conversion:
          - bin 8 - bin 32 (0xC4..0xC6)
          - ext 8 - ext 32 (0xC7..0xC9)
          - float 32 (0xCA)
          - fixext 1 - fixext 16 (0xD4..0xD8)

    @note Any MessagePack output created @ref to_msgpack can be successfully
          parsed by @ref from_msgpack.

    @note If NaN or Infinity are stored inside a JSON number, they are
          serialized properly. This behavior differs from the @ref dump()
          function which serializes NaN or Infinity to `null`.

    @param[in] j  JSON value to serialize
    @return MessagePack serialization as byte vector

    @complexity Linear in the size of the JSON value @a j.

    @liveexample{The example shows the serialization of a JSON value to a byte
    vector in MessagePack format.,to_msgpack}

    @sa http://msgpack.org
    @sa @ref from_msgpack(const std::vector<uint8_t>&, const size_t) for the
        analogous deserialization
    @sa @ref to_cbor(const json& for the related CBOR format
    @sa @ref to_ubjson(const json&, const bool, const bool) for the
             related UBJSON format

    @since version 2.0.9
    */
    static std::vector<uint8_t> to_msgpack(const json& j);
    static ArrayRef<uint8_t> to_msgpack(const json& j, std::vector<uint8_t>& buf);
    static ArrayRef<uint8_t> to_msgpack(const json& j, SmallVectorImpl<uint8_t>& buf);
    static void to_msgpack(raw_ostream& os, const json& j);

    /*!
    @brief create a UBJSON serialization of a given JSON value

    Serializes a given JSON value @a j to a byte vector using the UBJSON
    (Universal Binary JSON) serialization format. UBJSON aims to be more compact
    than JSON itself, yet more efficient to parse.

    The library uses the following mapping from JSON values types to
    UBJSON types according to the UBJSON specification:

    JSON value type | value/range                       | UBJSON type | marker
    --------------- | --------------------------------- | ----------- | ------
    null            | `null`                            | null        | `Z`
    boolean         | `true`                            | true        | `T`
    boolean         | `false`                           | false       | `F`
    number_integer  | -9223372036854775808..-2147483649 | int64       | `L`
    number_integer  | -2147483648..-32769               | int32       | `l`
    number_integer  | -32768..-129                      | int16       | `I`
    number_integer  | -128..127                         | int8        | `i`
    number_integer  | 128..255                          | uint8       | `U`
    number_integer  | 256..32767                        | int16       | `I`
    number_integer  | 32768..2147483647                 | int32       | `l`
    number_integer  | 2147483648..9223372036854775807   | int64       | `L`
    number_unsigned | 0..127                            | int8        | `i`
    number_unsigned | 128..255                          | uint8       | `U`
    number_unsigned | 256..32767                        | int16       | `I`
    number_unsigned | 32768..2147483647                 | int32       | `l`
    number_unsigned | 2147483648..9223372036854775807   | int64       | `L`
    number_float    | *any value*                       | float64     | `D`
    string          | *with shortest length indicator*  | string      | `S`
    array           | *see notes on optimized format*   | array       | `[`
    object          | *see notes on optimized format*   | map         | `{`

    @note The mapping is **complete** in the sense that any JSON value type
          can be converted to a UBJSON value.

    @note The following values can **not** be converted to a UBJSON value:
          - strings with more than 9223372036854775807 bytes (theoretical)
          - unsigned integer numbers above 9223372036854775807

    @note The following markers are not used in the conversion:
          - `Z`: no-op values are not created.
          - `C`: single-byte strings are serialized with `S` markers.

    @note Any UBJSON output created @ref to_ubjson can be successfully parsed
          by @ref from_ubjson.

    @note If NaN or Infinity are stored inside a JSON number, they are
          serialized properly. This behavior differs from the @ref dump()
          function which serializes NaN or Infinity to `null`.

    @note The optimized formats for containers are supported: Parameter
          @a use_size adds size information to the beginning of a container and
          removes the closing marker. Parameter @a use_type further checks
          whether all elements of a container have the same type and adds the
          type marker to the beginning of the container. The @a use_type
          parameter must only be used together with @a use_size = true. Note
          that @a use_size = true alone may result in larger representations -
          the benefit of this parameter is that the receiving side is
          immediately informed on the number of elements of the container.

    @param[in] j  JSON value to serialize
    @param[in] use_size  whether to add size annotations to container types
    @param[in] use_type  whether to add type annotations to container types
                         (must be combined with @a use_size = true)
    @return UBJSON serialization as byte vector

    @complexity Linear in the size of the JSON value @a j.

    @liveexample{The example shows the serialization of a JSON value to a byte
    vector in UBJSON format.,to_ubjson}

    @sa http://ubjson.org
    @sa @ref from_ubjson(raw_istream&, const bool strict) for the
        analogous deserialization
    @sa @ref to_cbor(const json& for the related CBOR format
    @sa @ref to_msgpack(const json&) for the related MessagePack format

    @since version 3.1.0
    */
    static std::vector<uint8_t> to_ubjson(const json& j,
                                          const bool use_size = false,
                                          const bool use_type = false);
    static ArrayRef<uint8_t> to_ubjson(const json& j, std::vector<uint8_t>& buf,
                                       const bool use_size = false, const bool use_type = false);
    static ArrayRef<uint8_t> to_ubjson(const json& j, SmallVectorImpl<uint8_t>& buf,
                                       const bool use_size = false, const bool use_type = false);
    static void to_ubjson(raw_ostream& os, const json& j,
                          const bool use_size = false, const bool use_type = false);

    /*!
    @brief create a JSON value from an input in CBOR format

    Deserializes a given input @a i to a JSON value using the CBOR (Concise
    Binary Object Representation) serialization format.

    The library maps CBOR types to JSON value types as follows:

    CBOR type              | JSON value type | first byte
    ---------------------- | --------------- | ----------
    Integer                | number_unsigned | 0x00..0x17
    Unsigned integer       | number_unsigned | 0x18
    Unsigned integer       | number_unsigned | 0x19
    Unsigned integer       | number_unsigned | 0x1A
    Unsigned integer       | number_unsigned | 0x1B
    Negative integer       | number_integer  | 0x20..0x37
    Negative integer       | number_integer  | 0x38
    Negative integer       | number_integer  | 0x39
    Negative integer       | number_integer  | 0x3A
    Negative integer       | number_integer  | 0x3B
    Negative integer       | number_integer  | 0x40..0x57
    UTF-8 string           | string          | 0x60..0x77
    UTF-8 string           | string          | 0x78
    UTF-8 string           | string          | 0x79
    UTF-8 string           | string          | 0x7A
    UTF-8 string           | string          | 0x7B
    UTF-8 string           | string          | 0x7F
    array                  | array           | 0x80..0x97
    array                  | array           | 0x98
    array                  | array           | 0x99
    array                  | array           | 0x9A
    array                  | array           | 0x9B
    array                  | array           | 0x9F
    map                    | object          | 0xA0..0xB7
    map                    | object          | 0xB8
    map                    | object          | 0xB9
    map                    | object          | 0xBA
    map                    | object          | 0xBB
    map                    | object          | 0xBF
    False                  | `false`         | 0xF4
    True                   | `true`          | 0xF5
    Nill                   | `null`          | 0xF6
    Half-Precision Float   | number_float    | 0xF9
    Single-Precision Float | number_float    | 0xFA
    Double-Precision Float | number_float    | 0xFB

    @warning The mapping is **incomplete** in the sense that not all CBOR
             types can be converted to a JSON value. The following CBOR types
             are not supported and will yield parse errors (parse_error.112):
             - byte strings (0x40..0x5F)
             - date/time (0xC0..0xC1)
             - bignum (0xC2..0xC3)
             - decimal fraction (0xC4)
             - bigfloat (0xC5)
             - tagged items (0xC6..0xD4, 0xD8..0xDB)
             - expected conversions (0xD5..0xD7)
             - simple values (0xE0..0xF3, 0xF8)
             - undefined (0xF7)

    @warning CBOR allows map keys of any type, whereas JSON only allows
             strings as keys in object values. Therefore, CBOR maps with keys
             other than UTF-8 strings are rejected (parse_error.113).

    @note Any CBOR output created @ref to_cbor can be successfully parsed by
          @ref from_cbor.

    @param[in] i  an input in CBOR format convertible to an input adapter
    @param[in] strict  whether to expect the input to be consumed until EOF
                       (true by default)
    @return deserialized JSON value

    @throw parse_error.110 if the given input ends prematurely or the end of
    file was not reached when @a strict was set to true
    @throw parse_error.112 if unsupported features from CBOR were
    used in the given input @a v or if the input is not valid CBOR
    @throw parse_error.113 if a string was expected as map key, but not found

    @complexity Linear in the size of the input @a i.

    @liveexample{The example shows the deserialization of a byte vector in CBOR
    format to a JSON value.,from_cbor}

    @sa http://cbor.io
    @sa @ref to_cbor(const json&) for the analogous serialization
    @sa @ref from_msgpack(raw_istream&, const bool) for the
        related MessagePack format
    @sa @ref from_ubjson(raw_istream&, const bool) for the related
        UBJSON format

    @since version 2.0.9; parameter @a start_index since 2.1.1; changed to
           consume input adapters, removed start_index parameter, and added
           @a strict parameter since 3.0.0
    */
    static json from_cbor(raw_istream& is,
                                const bool strict = true);

    /*!
    @copydoc from_cbor(raw_istream&, const bool)
    */
    static json from_cbor(ArrayRef<uint8_t> arr, const bool strict = true);

    /*!
    @brief create a JSON value from an input in MessagePack format

    Deserializes a given input @a i to a JSON value using the MessagePack
    serialization format.

    The library maps MessagePack types to JSON value types as follows:

    MessagePack type | JSON value type | first byte
    ---------------- | --------------- | ----------
    positive fixint  | number_unsigned | 0x00..0x7F
    fixmap           | object          | 0x80..0x8F
    fixarray         | array           | 0x90..0x9F
    fixstr           | string          | 0xA0..0xBF
    nil              | `null`          | 0xC0
    false            | `false`         | 0xC2
    true             | `true`          | 0xC3
    float 32         | number_float    | 0xCA
    float 64         | number_float    | 0xCB
    uint 8           | number_unsigned | 0xCC
    uint 16          | number_unsigned | 0xCD
    uint 32          | number_unsigned | 0xCE
    uint 64          | number_unsigned | 0xCF
    int 8            | number_integer  | 0xD0
    int 16           | number_integer  | 0xD1
    int 32           | number_integer  | 0xD2
    int 64           | number_integer  | 0xD3
    str 8            | string          | 0xD9
    str 16           | string          | 0xDA
    str 32           | string          | 0xDB
    array 16         | array           | 0xDC
    array 32         | array           | 0xDD
    map 16           | object          | 0xDE
    map 32           | object          | 0xDF
    negative fixint  | number_integer  | 0xE0-0xFF

    @warning The mapping is **incomplete** in the sense that not all
             MessagePack types can be converted to a JSON value. The following
             MessagePack types are not supported and will yield parse errors:
              - bin 8 - bin 32 (0xC4..0xC6)
              - ext 8 - ext 32 (0xC7..0xC9)
              - fixext 1 - fixext 16 (0xD4..0xD8)

    @note Any MessagePack output created @ref to_msgpack can be successfully
          parsed by @ref from_msgpack.

    @param[in] i  an input in MessagePack format convertible to an input
                  adapter
    @param[in] strict  whether to expect the input to be consumed until EOF
                       (true by default)

    @throw parse_error.110 if the given input ends prematurely or the end of
    file was not reached when @a strict was set to true
    @throw parse_error.112 if unsupported features from MessagePack were
    used in the given input @a i or if the input is not valid MessagePack
    @throw parse_error.113 if a string was expected as map key, but not found

    @complexity Linear in the size of the input @a i.

    @liveexample{The example shows the deserialization of a byte vector in
    MessagePack format to a JSON value.,from_msgpack}

    @sa http://msgpack.org
    @sa @ref to_msgpack(const json&) for the analogous serialization
    @sa @ref from_cbor(raw_istream&, const bool) for the related CBOR
        format
    @sa @ref from_ubjson(raw_istream&, const bool) for the related
        UBJSON format

    @since version 2.0.9; parameter @a start_index since 2.1.1; changed to
           consume input adapters, removed start_index parameter, and added
           @a strict parameter since 3.0.0
    */
    static json from_msgpack(raw_istream& is,
                                   const bool strict = true);

    /*!
    @copydoc from_msgpack(raw_istream, const bool)
    */
    static json from_msgpack(ArrayRef<uint8_t> arr, const bool strict = true);

    /*!
    @brief create a JSON value from an input in UBJSON format

    Deserializes a given input @a i to a JSON value using the UBJSON (Universal
    Binary JSON) serialization format.

    The library maps UBJSON types to JSON value types as follows:

    UBJSON type | JSON value type                         | marker
    ----------- | --------------------------------------- | ------
    no-op       | *no value, next value is read*          | `N`
    null        | `null`                                  | `Z`
    false       | `false`                                 | `F`
    true        | `true`                                  | `T`
    float32     | number_float                            | `d`
    float64     | number_float                            | `D`
    uint8       | number_unsigned                         | `U`
    int8        | number_integer                          | `i`
    int16       | number_integer                          | `I`
    int32       | number_integer                          | `l`
    int64       | number_integer                          | `L`
    string      | string                                  | `S`
    char        | string                                  | `C`
    array       | array (optimized values are supported)  | `[`
    object      | object (optimized values are supported) | `{`

    @note The mapping is **complete** in the sense that any UBJSON value can
          be converted to a JSON value.

    @param[in] i  an input in UBJSON format convertible to an input adapter
    @param[in] strict  whether to expect the input to be consumed until EOF
                       (true by default)

    @throw parse_error.110 if the given input ends prematurely or the end of
    file was not reached when @a strict was set to true
    @throw parse_error.112 if a parse error occurs
    @throw parse_error.113 if a string could not be parsed successfully

    @complexity Linear in the size of the input @a i.

    @liveexample{The example shows the deserialization of a byte vector in
    UBJSON format to a JSON value.,from_ubjson}

    @sa http://ubjson.org
    @sa @ref to_ubjson(const json&, const bool, const bool) for the
             analogous serialization
    @sa @ref from_cbor(raw_istream&, const bool) for the related CBOR
        format
    @sa @ref from_msgpack(raw_istream&, const bool) for the related
        MessagePack format

    @since version 3.1.0
    */
    static json from_ubjson(raw_istream& is,
                                  const bool strict = true);

    static json from_ubjson(ArrayRef<uint8_t> arr, const bool strict = true);

    /// @}

    //////////////////////////
    // JSON Pointer support //
    //////////////////////////

    /// @name JSON Pointer functions
    /// @{

    /*!
    @brief access specified element via JSON Pointer

    Uses a JSON pointer to retrieve a reference to the respective JSON value.
    No bound checking is performed. Similar to @ref operator[](const typename
    object_t::key_type&), `null` values are created in arrays and objects if
    necessary.

    In particular:
    - If the JSON pointer points to an object key that does not exist, it
      is created an filled with a `null` value before a reference to it
      is returned.
    - If the JSON pointer points to an array index that does not exist, it
      is created an filled with a `null` value before a reference to it
      is returned. All indices between the current maximum and the given
      index are also filled with `null`.
    - The special value `-` is treated as a synonym for the index past the
      end.

    @param[in] ptr  a JSON pointer

    @return reference to the element pointed to by @a ptr

    @complexity Constant.

    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.404  if the JSON pointer can not be resolved

    @liveexample{The behavior is shown in the example.,operatorjson_pointer}

    @since version 2.0.0
    */
    reference operator[](const json_pointer& ptr)
    {
        return ptr.get_unchecked(this);
    }

    /*!
    @brief access specified element via JSON Pointer

    Uses a JSON pointer to retrieve a reference to the respective JSON value.
    No bound checking is performed. The function does not change the JSON
    value; no `null` values are created. In particular, the the special value
    `-` yields an exception.

    @param[in] ptr  JSON pointer to the desired element

    @return const reference to the element pointed to by @a ptr

    @complexity Constant.

    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.402  if the array index '-' is used
    @throw out_of_range.404  if the JSON pointer can not be resolved

    @liveexample{The behavior is shown in the example.,operatorjson_pointer_const}

    @since version 2.0.0
    */
    const_reference operator[](const json_pointer& ptr) const
    {
        return ptr.get_unchecked(this);
    }

    /*!
    @brief access specified element via JSON Pointer

    Returns a reference to the element at with specified JSON pointer @a ptr,
    with bounds checking.

    @param[in] ptr  JSON pointer to the desired element

    @return reference to the element pointed to by @a ptr

    @throw parse_error.106 if an array index in the passed JSON pointer @a ptr
    begins with '0'. See example below.

    @throw parse_error.109 if an array index in the passed JSON pointer @a ptr
    is not a number. See example below.

    @throw out_of_range.401 if an array index in the passed JSON pointer @a ptr
    is out of range. See example below.

    @throw out_of_range.402 if the array index '-' is used in the passed JSON
    pointer @a ptr. As `at` provides checked access (and no elements are
    implicitly inserted), the index '-' is always invalid. See example below.

    @throw out_of_range.403 if the JSON pointer describes a key of an object
    which cannot be found. See example below.

    @throw out_of_range.404 if the JSON pointer @a ptr can not be resolved.
    See example below.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @complexity Constant.

    @since version 2.0.0

    @liveexample{The behavior is shown in the example.,at_json_pointer}
    */
    reference at(const json_pointer& ptr)
    {
        return ptr.get_checked(this);
    }

    /*!
    @brief access specified element via JSON Pointer

    Returns a const reference to the element at with specified JSON pointer @a
    ptr, with bounds checking.

    @param[in] ptr  JSON pointer to the desired element

    @return reference to the element pointed to by @a ptr

    @throw parse_error.106 if an array index in the passed JSON pointer @a ptr
    begins with '0'. See example below.

    @throw parse_error.109 if an array index in the passed JSON pointer @a ptr
    is not a number. See example below.

    @throw out_of_range.401 if an array index in the passed JSON pointer @a ptr
    is out of range. See example below.

    @throw out_of_range.402 if the array index '-' is used in the passed JSON
    pointer @a ptr. As `at` provides checked access (and no elements are
    implicitly inserted), the index '-' is always invalid. See example below.

    @throw out_of_range.403 if the JSON pointer describes a key of an object
    which cannot be found. See example below.

    @throw out_of_range.404 if the JSON pointer @a ptr can not be resolved.
    See example below.

    @exceptionsafety Strong guarantee: if an exception is thrown, there are no
    changes in the JSON value.

    @complexity Constant.

    @since version 2.0.0

    @liveexample{The behavior is shown in the example.,at_json_pointer_const}
    */
    const_reference at(const json_pointer& ptr) const
    {
        return ptr.get_checked(this);
    }

    /*!
    @brief return flattened JSON value

    The function creates a JSON object whose keys are JSON pointers (see [RFC
    6901](https://tools.ietf.org/html/rfc6901)) and whose values are all
    primitive. The original JSON value can be restored using the @ref
    unflatten() function.

    @return an object that maps JSON pointers to primitive values

    @note Empty objects and arrays are flattened to `null` and will not be
          reconstructed correctly by the @ref unflatten() function.

    @complexity Linear in the size the JSON value.

    @liveexample{The following code shows how a JSON object is flattened to an
    object whose keys consist of JSON pointers.,flatten}

    @sa @ref unflatten() for the reverse function

    @since version 2.0.0
    */
    json flatten() const
    {
        json result(value_t::object);
        json_pointer::flatten("", *this, result);
        return result;
    }

    /*!
    @brief unflatten a previously flattened JSON value

    The function restores the arbitrary nesting of a JSON value that has been
    flattened before using the @ref flatten() function. The JSON value must
    meet certain constraints:
    1. The value must be an object.
    2. The keys must be JSON pointers (see
       [RFC 6901](https://tools.ietf.org/html/rfc6901))
    3. The mapped values must be primitive JSON types.

    @return the original JSON from a flattened version

    @note Empty objects and arrays are flattened by @ref flatten() to `null`
          values and can not unflattened to their original type. Apart from
          this example, for a JSON value `j`, the following is always true:
          `j == j.flatten().unflatten()`.

    @complexity Linear in the size the JSON value.

    @throw type_error.314  if value is not an object
    @throw type_error.315  if object values are not primitive

    @liveexample{The following code shows how a flattened JSON object is
    unflattened into the original nested JSON object.,unflatten}

    @sa @ref flatten() for the reverse function

    @since version 2.0.0
    */
    json unflatten() const
    {
        return json_pointer::unflatten(*this);
    }

    /// @}

    //////////////////////////
    // JSON Patch functions //
    //////////////////////////

    /// @name JSON Patch functions
    /// @{

    /*!
    @brief applies a JSON patch

    [JSON Patch](http://jsonpatch.com) defines a JSON document structure for
    expressing a sequence of operations to apply to a JSON) document. With
    this function, a JSON Patch is applied to the current JSON value by
    executing all operations from the patch.

    @param[in] json_patch  JSON patch document
    @return patched document

    @note The application of a patch is atomic: Either all operations succeed
          and the patched document is returned or an exception is thrown. In
          any case, the original value is not changed: the patch is applied
          to a copy of the value.

    @throw parse_error.104 if the JSON patch does not consist of an array of
    objects

    @throw parse_error.105 if the JSON patch is malformed (e.g., mandatory
    attributes are missing); example: `"operation add must have member path"`

    @throw out_of_range.401 if an array index is out of range.

    @throw out_of_range.403 if a JSON pointer inside the patch could not be
    resolved successfully in the current JSON value; example: `"key baz not
    found"`

    @throw out_of_range.405 if JSON pointer has no parent ("add", "remove",
    "move")

    @throw other_error.501 if "test" operation was unsuccessful

    @complexity Linear in the size of the JSON value and the length of the
    JSON patch. As usually only a fraction of the JSON value is affected by
    the patch, the complexity can usually be neglected.

    @liveexample{The following code shows how a JSON patch is applied to a
    value.,patch}

    @sa @ref diff -- create a JSON patch by comparing two JSON values

    @sa [RFC 6902 (JSON Patch)](https://tools.ietf.org/html/rfc6902)
    @sa [RFC 6901 (JSON Pointer)](https://tools.ietf.org/html/rfc6901)

    @since version 2.0.0
    */
    json patch(const json& json_patch) const;

    /*!
    @brief creates a diff as a JSON patch

    Creates a [JSON Patch](http://jsonpatch.com) so that value @a source can
    be changed into the value @a target by calling @ref patch function.

    @invariant For two JSON values @a source and @a target, the following code
    yields always `true`:
    @code {.cpp}
    source.patch(diff(source, target)) == target;
    @endcode

    @note Currently, only `remove`, `add`, and `replace` operations are
          generated.

    @param[in] source  JSON value to compare from
    @param[in] target  JSON value to compare against
    @param[in] path    helper value to create JSON pointers

    @return a JSON patch to convert the @a source to @a target

    @complexity Linear in the lengths of @a source and @a target.

    @liveexample{The following code shows how a JSON patch is created as a
    diff for two JSON values.,diff}

    @sa @ref patch -- apply a JSON patch
    @sa @ref merge_patch -- apply a JSON Merge Patch

    @sa [RFC 6902 (JSON Patch)](https://tools.ietf.org/html/rfc6902)

    @since version 2.0.0
    */
    static json diff(const json& source, const json& target,
                           const std::string& path = "");

    /// @}

    ////////////////////////////////
    // JSON Merge Patch functions //
    ////////////////////////////////

    /// @name JSON Merge Patch functions
    /// @{

    /*!
    @brief applies a JSON Merge Patch

    The merge patch format is primarily intended for use with the HTTP PATCH
    method as a means of describing a set of modifications to a target
    resource's content. This function applies a merge patch to the current
    JSON value.

    The function implements the following algorithm from Section 2 of
    [RFC 7396 (JSON Merge Patch)](https://tools.ietf.org/html/rfc7396):

    ```
    define MergePatch(Target, Patch):
      if Patch is an Object:
        if Target is not an Object:
          Target = {} // Ignore the contents and set it to an empty Object
        for each Name/Value pair in Patch:
          if Value is null:
            if Name exists in Target:
              remove the Name/Value pair from Target
          else:
            Target[Name] = MergePatch(Target[Name], Value)
        return Target
      else:
        return Patch
    ```

    Thereby, `Target` is the current object; that is, the patch is applied to
    the current value.

    @param[in] patch  the patch to apply

    @complexity Linear in the lengths of @a patch.

    @liveexample{The following code shows how a JSON Merge Patch is applied to
    a JSON document.,merge_patch}

    @sa @ref patch -- apply a JSON patch
    @sa [RFC 7396 (JSON Merge Patch)](https://tools.ietf.org/html/rfc7396)

    @since version 3.0.0
    */
    void merge_patch(const json& patch);

    /// @}
};
} // namespace wpi

///////////////////////
// nonmember support //
///////////////////////

// specialization of std::swap, and std::hash
namespace std
{
/*!
@brief exchanges the values of two JSON objects

@since version 1.0.0
*/
template<>
inline void swap(wpi::json& j1,
                 wpi::json& j2) noexcept(
                     is_nothrow_move_constructible<wpi::json>::value and
                     is_nothrow_move_assignable<wpi::json>::value
                 )
{
    j1.swap(j2);
}

/// hash value for JSON objects
template<>
struct hash<wpi::json>
{
    /*!
    @brief return a hash value for a JSON object

    @since version 1.0.0
    */
    std::size_t operator()(const wpi::json& j) const
    {
        // a naive hashing via the string representation
        const auto& h = hash<std::string>();
        return h(j.dump());
    }
};

/// specialization for std::less<value_t>
/// @note: do not remove the space after '<',
///        see https://github.com/nlohmann/json/pull/679
template<>
struct less< ::wpi::detail::value_t>
{
    /*!
    @brief compare two value_t enum values
    @since version 3.0.0
    */
    bool operator()(wpi::detail::value_t lhs,
                    wpi::detail::value_t rhs) const noexcept
    {
        return wpi::detail::operator<(lhs, rhs);
    }
};

} // namespace std

/*!
@brief user-defined string literal for JSON values

This operator implements a user-defined string literal for JSON objects. It
can be used by adding `"_json"` to a string literal and returns a JSON object
if no parse error occurred.

@param[in] s  a string representation of a JSON object
@param[in] n  the length of string @a s
@return a JSON object

@since version 1.0.0
*/
inline wpi::json operator "" _json(const char* s, std::size_t n)
{
    return wpi::json::parse(wpi::StringRef(s, n));
}

/*!
@brief user-defined string literal for JSON pointer

This operator implements a user-defined string literal for JSON Pointers. It
can be used by adding `"_json_pointer"` to a string literal and returns a JSON pointer
object if no parse error occurred.

@param[in] s  a string representation of a JSON Pointer
@param[in] n  the length of string @a s
@return a JSON pointer object

@since version 2.0.0
*/
inline wpi::json::json_pointer operator "" _json_pointer(const char* s, std::size_t n)
{
    return wpi::json::json_pointer(wpi::StringRef(s, n));
}

#ifndef WPI_JSON_IMPLEMENTATION

// restore GCC/clang diagnostic settings
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif

// clean up
#undef JSON_CATCH
#undef JSON_THROW
#undef JSON_TRY
#undef JSON_LIKELY
#undef JSON_UNLIKELY
#undef JSON_HAS_CPP_14
#undef JSON_HAS_CPP_17
#undef NLOHMANN_BASIC_JSON_TPL_DECLARATION
#undef NLOHMANN_BASIC_JSON_TPL
#undef NLOHMANN_JSON_HAS_HELPER

#endif  // WPI_JSON_IMPLEMENTATION

#endif
