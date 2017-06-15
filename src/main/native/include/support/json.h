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

#ifndef WPI_SUPPORT_JSON_H_
#define WPI_SUPPORT_JSON_H_

#include <cassert> // assert
#include <cmath> // isfinite, labs, ldexp, signbit
#include <cstddef> // nullptr_t, ptrdiff_t, size_t
#include <cstdint> // int64_t, uint64_t
#include <cstdlib> // abort, strtod, strtof, strtold, strtoul, strtoll, strtoull
#include <cstring> // memcpy, strlen
#include <functional> // function, hash, less
#include <initializer_list> // initializer_list
#include <iterator> // advance, begin, back_inserter, bidirectional_iterator_tag, distance, end, inserter, iterator, iterator_traits, next, random_access_iterator_tag
#include <limits> // numeric_limits
#include <memory> // addressof, allocator, allocator_traits, unique_ptr
#include <string> // getline, stoi, string, to_string
#include <type_traits> // add_pointer, conditional, decay, enable_if, false_type, integral_constant, is_arithmetic, is_base_of, is_const, is_constructible, is_convertible, is_default_constructible, is_enum, is_floating_point, is_integral, is_nothrow_move_assignable, is_nothrow_move_constructible, is_pointer, is_reference, is_same, is_scalar, is_signed, remove_const, remove_cv, remove_pointer, remove_reference, true_type, underlying_type
#include <utility> // declval, forward, make_pair, move, pair, swap
#include <vector> // vector

#include "llvm/ArrayRef.h"
#include "llvm/raw_ostream.h"
#include "llvm/StringMap.h"
#include "llvm/StringRef.h"

// exclude unsupported compilers
#if defined(__clang__)
    #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
        #error "unsupported Clang version - see https://github.com/nlohmann/json#supported-compilers"
    #endif
#elif defined(__GNUC__)
    #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40800
        #error "unsupported GCC version - see https://github.com/nlohmann/json#supported-compilers"
    #endif
#endif

// disable float-equal warnings on GCC/clang
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
    #pragma GCC diagnostic ignored "-Wtype-limits"
#endif

// disable documentation warnings on clang
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdocumentation"
    #pragma GCC diagnostic ignored "-Wtautological-constant-out-of-range-compare"
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

// manual branch prediction
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define JSON_LIKELY(x)      __builtin_expect(!!(x), 1)
    #define JSON_UNLIKELY(x)    __builtin_expect(!!(x), 0)
#else
    #define JSON_LIKELY(x)      x
    #define JSON_UNLIKELY(x)    x
#endif

namespace wpi
{

class raw_istream;

/*!
@brief unnamed namespace with internal helper functions

This namespace collects some functions that could not be defined inside the
@ref json class.

@since version 2.1.0
*/
namespace detail
{
////////////////
// exceptions //
////////////////

/*!
@brief general exception of the @ref json class

Extension of std::exception objects with a member @a id for exception ids.

@note To have nothrow-copy-constructible exceptions, we internally use
      std::runtime_error which can cope with arbitrary-length error messages.
      Intermediate strings are built with static functions and then passed to
      the actual constructor.

@since version 3.0.0
*/
class exception : public std::exception
{
  public:
    /// returns the explanatory string
    virtual const char* what() const noexcept override
    {
        return m.what();
    }

    /// the id of the exception
    const int id;

  protected:
    exception(int id_, const char* what_arg)
        : id(id_), m(what_arg)
    {}

    static std::string name(const std::string& ename, int id);

  private:
    /// an exception object as storage for error messages
    std::runtime_error m;
};

/*!
@brief exception indicating a parse error

This excpetion is thrown by the library when a parse error occurs. Parse
errors can occur during the deserialization of JSON text as well as when
using JSON Patch.

Member @a byte holds the byte index of the last read character in the input
file.

@note For an input with n bytes, 1 is the index of the first character
      and n+1 is the index of the terminating null byte or the end of
      file. This also holds true when reading a byte vector (CBOR or
      MessagePack).

Exceptions have ids 1xx.

name / id                      | example massage | description
------------------------------ | --------------- | -------------------------
json.exception.parse_error.101 | parse error at 2: unexpected end of input; expected string literal | This error indicates a syntax error while deserializing a JSON text. The error message describes that an unexpected token (character) was encountered, and the member @a byte indicates the error position.
json.exception.parse_error.102 | parse error at 14: missing or wrong low surrogate | JSON uses the `\uxxxx` format to describe Unicode characters. Code points above above 0xFFFF are split into two `\uxxxx` entries ("surrogate pairs"). This error indicates that the surrogate pair is incomplete or contains an invalid code point.
json.exception.parse_error.103 | parse error: code points above 0x10FFFF are invalid | Unicode supports code points up to 0x10FFFF. Code points above 0x10FFFF are invalid.
json.exception.parse_error.104 | parse error: JSON patch must be an array of objects | [RFC 6902](https://tools.ietf.org/html/rfc6902) requires a JSON Patch document to be a JSON document that represents an array of objects.
json.exception.parse_error.105 | parse error: operation must have string member 'op' | An operation of a JSON Patch document must contain exactly one "op" member, whose value indicates the operation to perform. Its value must be one of "add", "remove", "replace", "move", "copy", or "test"; other values are errors.
json.exception.parse_error.106 | parse error: array index '01' must not begin with '0' | An array index in a JSON Pointer ([RFC 6901](https://tools.ietf.org/html/rfc6901)) may be `0` or any number wihtout a leading `0`.
json.exception.parse_error.107 | parse error: JSON pointer must be empty or begin with '/' - was: 'foo' | A JSON Pointer must be a Unicode string containing a sequence of zero or more reference tokens, each prefixed by a `/` character.
json.exception.parse_error.108 | parse error: escape character '~' must be followed with '0' or '1' | In a JSON Pointer, only `~0` and `~1` are valid escape sequences.
json.exception.parse_error.109 | parse error: array index 'one' is not a number | A JSON Pointer array index must be a number.
json.exception.parse_error.110 | parse error at 1: cannot read 2 bytes from vector | When parsing CBOR or MessagePack, the byte vector ends before the complete value has been read.
json.exception.parse_error.111 | parse error: bad input stream | Parsing CBOR or MessagePack from an input stream where the [`badbit` or `failbit`](http://en.cppreference.com/w/cpp/io/ios_base/iostate) is set.
json.exception.parse_error.112 | parse error at 1: error reading CBOR; last byte: 0xf8 | Not all types of CBOR or MessagePack are supported. This exception occurs if an unsupported byte was read.
json.exception.parse_error.113 | parse error at 2: expected a CBOR string; last byte: 0x98 | While parsing a map key, a value that is not a string has been read.

@since version 3.0.0
*/
class parse_error : public exception
{
  public:
    /*!
    @brief create a parse error exception
    @param[in] id         the id of the exception
    @param[in] byte_      the byte index where the error occured (or 0 if
                          the position cannot be determined)
    @param[in] what_arg   the explanatory string
    @return parse_error object
    */
    static parse_error create(int id, size_t byte_, const std::string& what_arg);

    /*!
    @brief byte index of the parse error

    The byte index of the last read character in the input file.

    @note For an input with n bytes, 1 is the index of the first character
          and n+1 is the index of the terminating null byte or the end of
          file. This also holds true when reading a byte vector (CBOR or
          MessagePack).
    */
    const size_t byte;

  private:
    parse_error(int id_, size_t byte_, const char* what_arg)
        : exception(id_, what_arg), byte(byte_)
    {}
};

/*!
@brief exception indicating errors with iterators

Exceptions have ids 2xx.

name / id                           | example massage | description
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
json.exception.invalid_iterator.213 | cannot compare order of object iterators | The order of object iterators cannot be compated, because JSON objects are unordered.
json.exception.invalid_iterator.214 | cannot get value | Cannot get value for iterator: Either the iterator belongs to a null value or it is an iterator to a primitive type (number, boolean, or string), but the iterator is different to @ref begin().

@since version 3.0.0
*/
class invalid_iterator : public exception
{
  public:
    static invalid_iterator create(int id, const std::string& what_arg);

  private:
    invalid_iterator(int id_, const char* what_arg)
        : exception(id_, what_arg)
    {}
};

/*!
@brief exception indicating executing a member function with a wrong type

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
json.exception.type_error.313 | invalid value to unflatten | The @ref unflatten function converts an object whose keys are JSON Pointers back into an arbitrary nested JSON value. The JSON Pointers must not overlap, because then the resulting value would not be well defined.
json.exception.type_error.314 | only objects can be unflattened | The @ref unflatten function only works for an object whose keys are JSON Pointers.
json.exception.type_error.315 | values in object must be primitive | The @ref unflatten function only works for an object whose keys are JSON Pointers and whose values are primitive.

@since version 3.0.0
*/
class type_error : public exception
{
  public:
    static type_error create(int id, const std::string& what_arg);

  private:
    type_error(int id_, const char* what_arg)
        : exception(id_, what_arg)
    {}
};

/*!
@brief exception indicating access out of the defined range

Exceptions have ids 4xx.

name / id                       | example message | description
------------------------------- | --------------- | -------------------------
json.exception.out_of_range.401 | array index 3 is out of range | The provided array index @a i is larger than @a size-1.
json.exception.out_of_range.402 | array index '-' (3) is out of range | The special array index `-` in a JSON Pointer never describes a valid element of the array, but the index past the end. That is, it can only be used to add elements at this position, but not to read it.
json.exception.out_of_range.403 | key 'foo' not found | The provided key was not found in the JSON object.
json.exception.out_of_range.404 | unresolved reference token 'foo' | A reference token in a JSON Pointer could not be resolved.
json.exception.out_of_range.405 | JSON pointer has no parent | The JSON Patch operations 'remove' and 'add' can not be applied to the root element of the JSON value.
json.exception.out_of_range.406 | number overflow parsing '10E1000' | A parsed number could not be stored as without changing it to NaN or INF.

@since version 3.0.0
*/
class out_of_range : public exception
{
  public:
    static out_of_range create(int id, const std::string& what_arg);

  private:
    out_of_range(int id_, const char* what_arg)
        : exception(id_, what_arg)
    {}
};

/*!
@brief exception indicating other errors

Exceptions have ids 5xx.

name / id                      | example message | description
------------------------------ | --------------- | -------------------------
json.exception.other_error.501 | unsuccessful: {"op":"test","path":"/baz", "value":"bar"} | A JSON Patch operation 'test' failed. The unsuccessful operation is also printed.
json.exception.other_error.502 | invalid object size for conversion | Some conversions to user-defined types impose constraints on the object size (e.g. std::pair)

@since version 3.0.0
*/
class other_error : public exception
{
  public:
    static other_error create(int id, const std::string& what_arg);

  private:
    other_error(int id_, const char* what_arg)
        : exception(id_, what_arg)
    {}
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
std::uint64_t is used for unsigned integers,
std::int64_t is used for signed integers, and
double is used for floating-point numbers or to
approximate integers which do not fit in the limits of their respective type.

@sa @ref json::json(const value_t value_type) -- create a JSON
value with the default value for a given type

@since version 1.0.0
*/
enum class value_t : uint8_t
{
    null,            ///< null value
    object,          ///< object (unordered set of name/value pairs)
    array,           ///< array (ordered collection of values)
    string,          ///< string value
    boolean,         ///< boolean value
    number_integer,  ///< number value (signed integer)
    number_unsigned, ///< number value (unsigned integer)
    number_float,    ///< number value (floating-point)
    discarded        ///< discarded by the the parser callback function
};

/*!
@brief comparison operator for JSON types

Returns an ordering that is similar to Python:
- order: null < boolean < number < object < array < string
- furthermore, each type is not smaller than itself

@since version 1.0.0
*/
inline bool operator<(const value_t lhs, const value_t rhs) noexcept
{
    static constexpr uint8_t order[] = {
        0, // null
        3, // object
        4, // array
        5, // string
        1, // boolean
        2, // integer
        2, // unsigned
        2, // float
    };

    // discarded values are not comparable
    if (lhs == value_t::discarded || rhs == value_t::discarded)
    {
        return false;
    }

    return order[static_cast<std::size_t>(lhs)] <
           order[static_cast<std::size_t>(rhs)];
}


/////////////
// helpers //
/////////////

// alias templates to reduce boilerplate
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using uncvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

/*
Implementation of two C++17 constructs: conjunction, negation. This is needed
to avoid evaluating all the traits in a condition

For example: !std::is_same<void, T>::value && has_value_type<T>::value
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

template<class B> struct negation : std::integral_constant < bool, !B::value > {};

// dispatch utility (taken from ranges-v3)
template<unsigned N> struct priority_tag : priority_tag < N - 1 > {};
template<> struct priority_tag<0> {};


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
    static void construct(BasicJsonType& j, llvm::StringRef s)
    {
        j.m_type = value_t::string;
        j.m_value = s;
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
    static void construct(BasicJsonType& j, std::uint64_t val) noexcept
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
    static void construct(BasicJsonType& j, std::int64_t val) noexcept
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

    template<typename BasicJsonType, typename T>
    static void construct(BasicJsonType& j, llvm::ArrayRef<T> arr)
    {
        using std::begin;
        using std::end;
        j.m_type = value_t::array;
        j.m_value.array = j.template create<typename BasicJsonType::array_t>(begin(arr), end(arr));
        j.assert_invariant();
    }

    template<typename BasicJsonType, typename CompatibleArrayType,
             enable_if_t<!std::is_same<CompatibleArrayType,
                                       typename BasicJsonType::array_t>::value,
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
        for (bool x : arr)
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

    template<typename BasicJsonType, typename CompatibleObjectType,
             enable_if_t<!std::is_same<CompatibleObjectType,
                                       typename BasicJsonType::object_t>::value,
                         int> = 0>
    static void construct(BasicJsonType& j, const CompatibleObjectType& obj)
    {
        j.m_type = value_t::object;
        j.m_value = value_t::object;
        for (const auto& x : obj)
        {
            j.m_value.object->emplace_second(x.first, x.second);
        }
        j.assert_invariant();
    }
};


////////////////////////
// has_/is_ functions //
////////////////////////

/*!
@brief Helper to determine whether there's a key_type for T.

This helper is used to tell associative containers apart from other containers
such as sequence containers. For instance, `std::map` passes the test as it
contains a `mapped_type`, whereas `std::vector` fails the test.

@sa http://stackoverflow.com/a/7728728/266378
@since version 1.0.0, overworked in version 2.0.6
*/
#define WPI_JSON_HAS_HELPER(type)                                             \
    template<typename T> struct has_##type {                                  \
    private:                                                                  \
        template<typename U, typename = typename U::type>                     \
        static int detect(U &&);                                              \
        static void detect(...);                                              \
    public:                                                                   \
        static constexpr bool value =                                         \
                std::is_integral<decltype(detect(std::declval<T>()))>::value; \
    }

WPI_JSON_HAS_HELPER(mapped_type);
WPI_JSON_HAS_HELPER(key_type);
WPI_JSON_HAS_HELPER(value_type);
WPI_JSON_HAS_HELPER(iterator);

#undef WPI_JSON_HAS_HELPER


template<bool B, class RealType, class CompatibleObjectType>
struct is_compatible_object_type_impl : std::false_type {};

template<class RealType, class CompatibleObjectType>
struct is_compatible_object_type_impl<true, RealType, CompatibleObjectType>
{
    static constexpr auto value =
        std::is_constructible<llvm::StringRef,
        typename CompatibleObjectType::key_type>::value &&
        std::is_constructible<typename RealType::mapped_type,
        typename CompatibleObjectType::mapped_type>::value;
};

template<typename BasicJsonType, class CompatibleObjectType>
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
    static auto constexpr value = std::is_same<T, typename BasicJsonType::iterator>::value ||
                                  std::is_same<T, typename BasicJsonType::const_iterator>::value ||
                                  std::is_same<T, typename BasicJsonType::json_pointer>::value;
};

template<typename BasicJsonType, class CompatibleArrayType>
struct is_compatible_array_type
{
    static auto constexpr value =
        conjunction<negation<std::is_same<void, CompatibleArrayType>>,
        negation<is_compatible_object_type<BasicJsonType, CompatibleArrayType>>,
        negation<std::is_constructible<llvm::StringRef, CompatibleArrayType>>,
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
        std::is_constructible<RealIntegerType,
        CompatibleNumberIntegerType>::value &&
        CompatibleLimits::is_integer &&
        RealLimits::is_signed == CompatibleLimits::is_signed;
};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type
{
    static constexpr auto value =
        is_compatible_integer_type_impl <
        std::is_integral<CompatibleNumberIntegerType>::value &&
        !std::is_same<bool, CompatibleNumberIntegerType>::value,
        RealIntegerType, CompatibleNumberIntegerType > ::value;
};

/////////////
// to_json //
/////////////

template<typename BasicJsonType, typename T, enable_if_t<
             std::is_same<T, bool>::value, int> = 0>
inline
void to_json(BasicJsonType& j, T b) noexcept
{
    detail::external_constructor<value_t::boolean>::construct(j, b);
}

template<typename BasicJsonType, typename CompatibleString,
         enable_if_t<std::is_constructible<llvm::StringRef,
                     CompatibleString>::value, int> = 0>
inline
void to_json(BasicJsonType& j, const CompatibleString& s)
{
    external_constructor<value_t::string>::construct(j, s);
}

template<typename BasicJsonType, typename FloatType,
         enable_if_t<std::is_floating_point<FloatType>::value, int> = 0>
inline
void to_json(BasicJsonType& j, FloatType val) noexcept
{
    external_constructor<value_t::number_float>::construct(j, static_cast<double>(val));
}

template <
    typename BasicJsonType, typename CompatibleNumberUnsignedType,
    enable_if_t<is_compatible_integer_type<std::uint64_t,
                CompatibleNumberUnsignedType>::value, int> = 0 >
inline
void to_json(BasicJsonType& j, CompatibleNumberUnsignedType val) noexcept
{
    external_constructor<value_t::number_unsigned>::construct(j, static_cast<std::uint64_t>(val));
}

template <
    typename BasicJsonType, typename CompatibleNumberIntegerType,
    enable_if_t<is_compatible_integer_type<std::int64_t,
                CompatibleNumberIntegerType>::value, int> = 0 >
inline
void to_json(BasicJsonType& j, CompatibleNumberIntegerType val) noexcept
{
    external_constructor<value_t::number_integer>::construct(j, static_cast<std::int64_t>(val));
}

template<typename BasicJsonType, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
inline
void to_json(BasicJsonType& j, EnumType e) noexcept
{
    using underlying_type = typename std::underlying_type<EnumType>::type;
    external_constructor<value_t::number_integer>::construct(j, static_cast<underlying_type>(e));
}

template<typename BasicJsonType>
inline
void to_json(BasicJsonType& j, const std::vector<bool>& e)
{
    external_constructor<value_t::array>::construct(j, e);
}

template<typename BasicJsonType, typename T>
inline
void to_json(BasicJsonType& j, llvm::ArrayRef<T> arr)
{
    external_constructor<value_t::array>::construct(j, arr);
}

template <
    typename BasicJsonType, typename CompatibleArrayType,
    enable_if_t <
        is_compatible_array_type<BasicJsonType, CompatibleArrayType>::value ||
        std::is_same<typename BasicJsonType::array_t, CompatibleArrayType>::value,
        int > = 0 >
inline
void to_json(BasicJsonType& j, const  CompatibleArrayType& arr)
{
    external_constructor<value_t::array>::construct(j, arr);
}

template <
    typename BasicJsonType, typename CompatibleObjectType,
    enable_if_t<is_compatible_object_type<BasicJsonType, CompatibleObjectType>::value,
                int> = 0 >
inline
void to_json(BasicJsonType& j, const  CompatibleObjectType& arr)
{
    external_constructor<value_t::object>::construct(j, arr);
}

template <typename BasicJsonType, typename T, std::size_t N,
          enable_if_t<!std::is_constructible<
                          llvm::StringRef, T (&)[N]>::value,
                      int> = 0>
inline
void to_json(BasicJsonType& j, T (&arr)[N])
{
    external_constructor<value_t::array>::construct(j, arr);
}

template <typename BasicJsonType, typename CompatibleString, typename T,
          enable_if_t<std::is_constructible<llvm::StringRef,
                      CompatibleString>::value, int> = 0>
inline
void to_json(BasicJsonType& j, std::pair<CompatibleString, T> const& p)
{
    j[p.first] = p.second;
}

///////////////
// from_json //
///////////////

// overloads for json template parameters
template<typename BasicJsonType, typename ArithmeticType,
         enable_if_t<std::is_arithmetic<ArithmeticType>::value &&
                     !std::is_same<ArithmeticType, bool>::value,
                     int> = 0>
void get_arithmetic_value(const BasicJsonType& j, ArithmeticType& val)
{
    switch (static_cast<value_t>(j))
    {
        case value_t::number_unsigned:
        {
            val = static_cast<ArithmeticType>(
                      *j.template get_ptr<const std::uint64_t*>());
            break;
        }
        case value_t::number_integer:
        {
            val = static_cast<ArithmeticType>(
                      *j.template get_ptr<const std::int64_t*>());
            break;
        }
        case value_t::number_float:
        {
            val = static_cast<ArithmeticType>(
                      *j.template get_ptr<const double*>());
            break;
        }
        default:
        {
            JSON_THROW(type_error::create(302, "type must be number, but is " + j.type_name()));
        }
    }
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, bool& b)
{
    if (!j.is_boolean())
    {
        JSON_THROW(type_error::create(302, "type must be boolean, but is " + j.type_name()));
    }
    b = *j.template get_ptr<const bool*>();
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, std::string& s)
{
    if (!j.is_string())
    {
        JSON_THROW(detail::type_error::create(302, "type must be string, but is " + j.type_name()));
    }
    s = *j.template get_ptr<const std::string*>();
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, double& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, std::uint64_t& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, std::int64_t& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
inline
void from_json(const BasicJsonType& j, EnumType& e)
{
    typename std::underlying_type<EnumType>::type val;
    get_arithmetic_value(j, val);
    e = static_cast<EnumType>(val);
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, typename BasicJsonType::array_t& arr)
{
    if (!j.is_array())
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + j.type_name()));
    }
    arr = *j.template get_ptr<const typename BasicJsonType::array_t*>();
}

template<typename BasicJsonType, typename CompatibleArrayType>
void from_json_array_impl(const BasicJsonType& j, CompatibleArrayType& arr, detail::priority_tag<0>)
{
    using std::begin;
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
auto from_json_array_impl(const BasicJsonType& j, CompatibleArrayType& arr, detail::priority_tag<1>)
-> decltype(
    arr.reserve(std::declval<typename CompatibleArrayType::size_type>()),
    void())
{
    using std::begin;
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

template<typename BasicJsonType, typename CompatibleArrayType,
         enable_if_t<is_compatible_array_type<BasicJsonType, CompatibleArrayType>::value &&
                     std::is_convertible<BasicJsonType, typename CompatibleArrayType::value_type>::value &&
                     !std::is_same<typename BasicJsonType::array_t, CompatibleArrayType>::value, int> = 0>
void from_json(const BasicJsonType& j, CompatibleArrayType& arr)
{
    if (!j.is_array())
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + j.type_name()));
    }

    from_json_array_impl(j, arr, priority_tag<1> {});
}

template<typename BasicJsonType>
inline
void from_json(const BasicJsonType& j, typename BasicJsonType::object_t& obj)
{
    if (!j.is_object())
    {
        JSON_THROW(type_error::create(302, "type must be object, but is " + j.type_name()));
    }

    auto inner_object = j.template get_ptr<const typename BasicJsonType::object_t*>();
    for (const auto& i : *inner_object) {
        obj.emplace_second(i.first(), i.second);
    }
}

template<typename BasicJsonType, typename CompatibleObjectType,
         enable_if_t<is_compatible_object_type<BasicJsonType, CompatibleObjectType>::value &&
                     !std::is_same<typename BasicJsonType::object_t, CompatibleObjectType>::value, int> = 0>
void from_json(const BasicJsonType& j, CompatibleObjectType& obj)
{
    if (!j.is_object())
    {
        JSON_THROW(type_error::create(302, "type must be object, but is " + j.type_name()));
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

// overload for arithmetic types, not chosen for json template arguments;
template<typename BasicJsonType, typename ArithmeticType,
         enable_if_t <
             std::is_arithmetic<ArithmeticType>::value &&
             !std::is_same<ArithmeticType, std::uint64_t>::value &&
             !std::is_same<ArithmeticType, std::int64_t>::value &&
             !std::is_same<ArithmeticType, double>::value &&
             !std::is_same<ArithmeticType, bool>::value,
             int> = 0>
void from_json(const BasicJsonType& j, ArithmeticType& val)
{
    switch (static_cast<value_t>(j))
    {
        case value_t::number_unsigned:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const std::uint64_t*>());
            break;
        }
        case value_t::number_integer:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const std::int64_t*>());
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
        {
            JSON_THROW(type_error::create(302, "type must be number, but is " + j.type_name()));
        }
    }
}

template <typename BasicJsonType, typename CompatibleString, typename T,
          enable_if_t<std::is_constructible<llvm::StringRef,
                      CompatibleString>::value, int> = 0>
void from_json(const BasicJsonType& j, std::pair<CompatibleString, T>& p)
{
    if (!j.is_object())
    {
        JSON_THROW(type_error::create(302, "type must be object, but is " + j.type_name()));
    }

    auto const inner_object = j.template get_ptr<const typename BasicJsonType::object_t*>();
    auto const size = inner_object->size();
    if (size != 1)
    {
        JSON_THROW(other_error::create(502, "conversion to std::pair requires the object to have exactly one field, but it has " + std::to_string(size)));
    }
    auto const& obj = *inner_object->begin();
    // cannot use *inner_object, need to convert both members
    p = std::make_pair(CompatibleString(obj.first()), obj.second.template get<T>());
}

struct to_json_fn
{
  private:
    template<typename BasicJsonType, typename T>
    auto call(BasicJsonType& j, T&& val, priority_tag<1>) const noexcept(noexcept(to_json(j, std::forward<T>(val))))
    -> decltype(to_json(j, std::forward<T>(val)), void())
    {
        return to_json(j, std::forward<T>(val));
    }

    template<typename BasicJsonType, typename T>
    void call(BasicJsonType&, T&&, priority_tag<0>) const noexcept
    {
        static_assert(sizeof(BasicJsonType) == 0,
                      "could not find to_json() method in T's namespace");
    }

  public:
    template<typename BasicJsonType, typename T>
    void operator()(BasicJsonType& j, T&& val) const
    noexcept(noexcept(std::declval<to_json_fn>().call(j, std::forward<T>(val), priority_tag<1> {})))
    {
        return call(j, std::forward<T>(val), priority_tag<1> {});
    }
};

struct from_json_fn
{
  private:
    template<typename BasicJsonType, typename T>
    auto call(const BasicJsonType& j, T& val, priority_tag<1>) const
    noexcept(noexcept(from_json(j, val)))
    -> decltype(from_json(j, val), void())
    {
        return from_json(j, val);
    }

    template<typename BasicJsonType, typename T>
    void call(const BasicJsonType&, T&, priority_tag<0>) const noexcept
    {
        static_assert(sizeof(BasicJsonType) == 0,
                      "could not find from_json() method in T's namespace");
    }

  public:
    template<typename BasicJsonType, typename T>
    void operator()(const BasicJsonType& j, T& val) const
    noexcept(noexcept(std::declval<from_json_fn>().call(j, val, priority_tag<1> {})))
    {
        return call(j, val, priority_tag<1> {});
    }
};

// taken from ranges-v3
template<typename T>
struct static_const
{
    static constexpr T value{};
};

template<typename T>
constexpr T static_const<T>::value;
} // namespace detail


/// namespace to hold default `to_json` / `from_json` functions
namespace
{
constexpr const auto& to_json = detail::static_const<detail::to_json_fn>::value;
constexpr const auto& from_json = detail::static_const<detail::from_json_fn>::value;
}

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

@see [RFC 7159: The JavaScript Object Notation (JSON) Data Interchange
Format](http://rfc7159.net/rfc7159)

@since version 1.0.0

@nosubgrouping
*/
class json
{
  private:
    template<detail::value_t> friend struct detail::external_constructor;
    friend class JsonTest;

  public:
    using value_t = detail::value_t;
    // forward declarations
    template<typename U> class iter_impl;
    class json_pointer;

    class parser;
    class serializer;
    class binary_writer;

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

    /// the type of an element pointer
    using pointer = json*;
    /// the type of an element const pointer
    using const_pointer = const json*;

    /// an iterator for a json container
    using iterator = iter_impl<json>;
    /// a const iterator for a json container
    using const_iterator = iter_impl<const json>;

    /// @}

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
    - When the names within an object are not unique, later stored name/value
      pairs overwrite previously stored name/value pairs, leaving the used
      names unique. For instance, `{"key": 1}` and `{"key": 2, "key": 1}` will
      be treated as equal and both stored as `{"key": 1}`.
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

    In this class, the object's limit of nesting is not constraint explicitly.
    However, a maximum depth of nesting may be introduced by the compiler or
    runtime environment. A theoretical limit can be queried by calling the
    @ref max_size function of a JSON object.

    #### Storage

    Objects are stored as pointers in a @ref json type. That is, for any
    access to object values, a pointer of type `object_t*` must be
    dereferenced.

    @sa @ref array_t -- type for an array value

    @since version 1.0.0

    @note The order name/value pairs are added to the object is *not*
    preserved by the library. Therefore, iterating an object may return
    name/value pairs in a different order than they were originally stored. In
    fact, keys will be traversed in alphabetical order as `std::map` with
    `std::less` is used by default. Please note this behavior conforms to [RFC
    7159](http://rfc7159.net/rfc7159), because any order implements the
    specified "unordered" nature of JSON objects.
    */
    using object_t = llvm::StringMap<json>;

    /*!
    @brief a type for an array

    [RFC 7159](http://rfc7159.net/rfc7159) describes JSON arrays as follows:
    > An array is an ordered sequence of zero or more values.

    To store objects in C++, a type is defined by the template parameters
    explained below.

    #### Default type

    The default value for @a array_t is:

    @code {.cpp}
    std::vector<
      json // value_type
    >
    @endcode

    #### Limits

    [RFC 7159](http://rfc7159.net/rfc7159) specifies:
    > An implementation may set limits on the maximum depth of nesting.

    In this class, the array's limit of nesting is not constraint explicitly.
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

    /*!
    @brief a type for a string

    [RFC 7159](http://rfc7159.net/rfc7159) describes JSON strings as follows:
    > A string is a sequence of zero or more Unicode characters.

    To store objects in C++, a type is defined by the template parameter
    described below. Unicode values are split by the JSON class into
    byte-sized characters during deserialization.

    #### Default type

    The default value for @a string_t is:

    @code {.cpp}
    std::string
    @endcode

    #### Encoding

    Strings are stored in UTF-8 encoding. Therefore, functions like
    `std::string::size()` or `std::string::length()` return the number of
    bytes in the string rather than the number of characters or glyphs.

    #### String comparison

    [RFC 7159](http://rfc7159.net/rfc7159) states:
    > Software implementations are typically required to test names of object
    > members for equality. Implementations that transform the textual
    > representation into sequences of Unicode code units and then perform the
    > comparison numerically, code unit by code unit, are interoperable in the
    > sense that implementations will agree in all cases on equality or
    > inequality of two strings. For example, implementations that compare
    > strings with escaped characters unconverted may incorrectly find that
    > `"a\\b"` and `"a\u005Cb"` are not equal.

    This implementation is interoperable as it does compare strings code unit
    by code unit.

    #### Storage

    String values are stored as pointers in a @ref basic_json type. That is,
    for any access to string values, a pointer of type `string_t*` must be
    dereferenced.

    @since version 1.0.0
    */
    using string_t = std::string;

    /*!
    @brief a type for a boolean

    [RFC 7159](http://rfc7159.net/rfc7159) implicitly describes a boolean as a
    type which differentiates the two literals `true` and `false`.

    #### Default type

    The default value for @a boolean_t is:

    @code {.cpp}
    bool
    @endcode

    #### Storage

    Boolean values are stored directly inside a @ref basic_json type.

    @since version 1.0.0
    */
    using boolean_t = bool;

    /*!
    @brief a type for a number (integer)

    [RFC 7159](http://rfc7159.net/rfc7159) describes numbers as follows:
    > The representation of numbers is similar to that used in most
    > programming languages. A number is represented in base 10 using decimal
    > digits. It contains an integer component that may be prefixed with an
    > optional minus sign, which may be followed by a fraction part and/or an
    > exponent part. Leading zeros are not allowed. (...) Numeric values that
    > cannot be represented in the grammar below (such as Infinity and NaN)
    > are not permitted.

    This description includes both integer and floating-point numbers.
    However, C++ allows more precise storage if it is known whether the number
    is a signed integer, an unsigned integer or a floating-point number.
    Therefore, three different types, @ref number_integer_t, @ref
    number_unsigned_t and @ref number_float_t are used.

    #### Default type

    The default value for @a number_integer_t is:

    @code {.cpp}
    int64_t
    @endcode

    #### Default behavior

    - The restrictions about leading zeros is not enforced in C++. Instead,
      leading zeros in integer literals lead to an interpretation as octal
      number. Internally, the value will be stored as decimal number. For
      instance, the C++ integer literal `010` will be serialized to `8`.
      During deserialization, leading zeros yield an error.
    - Not-a-number (NaN) values will be serialized to `null`.

    #### Limits

    [RFC 7159](http://rfc7159.net/rfc7159) specifies:
    > An implementation may set limits on the range and precision of numbers.

    When the default type is used, the maximal integer number that can be
    stored is `9223372036854775807` (INT64_MAX) and the minimal integer number
    that can be stored is `-9223372036854775808` (INT64_MIN). Integer numbers
    that are out of range will yield over/underflow when used in a
    constructor. During deserialization, too large or small integer numbers
    will be automatically be stored as @ref number_unsigned_t or @ref
    number_float_t.

    [RFC 7159](http://rfc7159.net/rfc7159) further states:
    > Note that when such software is used, numbers that are integers and are
    > in the range \f$[-2^{53}+1, 2^{53}-1]\f$ are interoperable in the sense
    > that implementations will agree exactly on their numeric values.

    As this range is a subrange of the exactly supported range [INT64_MIN,
    INT64_MAX], this class's integer type is interoperable.

    #### Storage

    Integer number values are stored directly inside a @ref basic_json type.

    @sa @ref number_float_t -- type for number values (floating-point)

    @sa @ref number_unsigned_t -- type for number values (unsigned integer)

    @since version 1.0.0
    */
    using number_integer_t = int64_t;

    /*!
    @brief a type for a number (unsigned)

    [RFC 7159](http://rfc7159.net/rfc7159) describes numbers as follows:
    > The representation of numbers is similar to that used in most
    > programming languages. A number is represented in base 10 using decimal
    > digits. It contains an integer component that may be prefixed with an
    > optional minus sign, which may be followed by a fraction part and/or an
    > exponent part. Leading zeros are not allowed. (...) Numeric values that
    > cannot be represented in the grammar below (such as Infinity and NaN)
    > are not permitted.

    This description includes both integer and floating-point numbers.
    However, C++ allows more precise storage if it is known whether the number
    is a signed integer, an unsigned integer or a floating-point number.
    Therefore, three different types, @ref number_integer_t, @ref
    number_unsigned_t and @ref number_float_t are used.

    #### Default type

    The default value for @a number_unsigned_t is:

    @code {.cpp}
    uint64_t
    @endcode

    #### Default behavior

    - The restrictions about leading zeros is not enforced in C++. Instead,
      leading zeros in integer literals lead to an interpretation as octal
      number. Internally, the value will be stored as decimal number. For
      instance, the C++ integer literal `010` will be serialized to `8`.
      During deserialization, leading zeros yield an error.
    - Not-a-number (NaN) values will be serialized to `null`.

    #### Limits

    [RFC 7159](http://rfc7159.net/rfc7159) specifies:
    > An implementation may set limits on the range and precision of numbers.

    When the default type is used, the maximal integer number that can be
    stored is `18446744073709551615` (UINT64_MAX) and the minimal integer
    number that can be stored is `0`. Integer numbers that are out of range
    will yield over/underflow when used in a constructor. During
    deserialization, too large or small integer numbers will be automatically
    be stored as @ref number_integer_t or @ref number_float_t.

    [RFC 7159](http://rfc7159.net/rfc7159) further states:
    > Note that when such software is used, numbers that are integers and are
    > in the range \f$[-2^{53}+1, 2^{53}-1]\f$ are interoperable in the sense
    > that implementations will agree exactly on their numeric values.

    As this range is a subrange (when considered in conjunction with the
    number_integer_t type) of the exactly supported range [0, UINT64_MAX],
    this class's integer type is interoperable.

    #### Storage

    Integer number values are stored directly inside a @ref basic_json type.

    @sa @ref number_float_t -- type for number values (floating-point)
    @sa @ref number_integer_t -- type for number values (integer)

    @since version 2.0.0
    */
    using number_unsigned_t = uint64_t;

    /*!
    @brief a type for a number (floating-point)

    [RFC 7159](http://rfc7159.net/rfc7159) describes numbers as follows:
    > The representation of numbers is similar to that used in most
    > programming languages. A number is represented in base 10 using decimal
    > digits. It contains an integer component that may be prefixed with an
    > optional minus sign, which may be followed by a fraction part and/or an
    > exponent part. Leading zeros are not allowed. (...) Numeric values that
    > cannot be represented in the grammar below (such as Infinity and NaN)
    > are not permitted.

    This description includes both integer and floating-point numbers.
    However, C++ allows more precise storage if it is known whether the number
    is a signed integer, an unsigned integer or a floating-point number.
    Therefore, three different types, @ref number_integer_t, @ref
    number_unsigned_t and @ref number_float_t are used.

    #### Default type

    The default value for @a number_float_t is:

    @code {.cpp}
    double
    @endcode

    #### Default behavior

    - The restrictions about leading zeros is not enforced in C++. Instead,
      leading zeros in floating-point literals will be ignored. Internally,
      the value will be stored as decimal number. For instance, the C++
      floating-point literal `01.2` will be serialized to `1.2`. During
      deserialization, leading zeros yield an error.
    - Not-a-number (NaN) values will be serialized to `null`.

    #### Limits

    [RFC 7159](http://rfc7159.net/rfc7159) states:
    > This specification allows implementations to set limits on the range and
    > precision of numbers accepted. Since software that implements IEEE
    > 754-2008 binary64 (double precision) numbers is generally available and
    > widely used, good interoperability can be achieved by implementations
    > that expect no more precision or range than these provide, in the sense
    > that implementations will approximate JSON numbers within the expected
    > precision.

    This implementation does exactly follow this approach, as it uses double
    precision floating-point numbers. Note values smaller than
    `-1.79769313486232e+308` and values greater than `1.79769313486232e+308`
    will be stored as NaN internally and be serialized to `null`.

    #### Storage

    Floating-point number values are stored directly inside a @ref basic_json
    type.

    @sa @ref number_integer_t -- type for number values (integer)

    @sa @ref number_unsigned_t -- type for number values (unsigned integer)

    @since version 1.0.0
    */
    using number_float_t = double;

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

  public:
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
    number    | number_integer  | std::int64_t
    number    | number_unsigned | std::uint64_t
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
        std::int64_t number_integer;
        /// number (unsigned integer)
        std::uint64_t number_unsigned;
        /// number (floating-point)
        double number_float;

        /// default constructor (for null values)
        json_value() = default;
        /// constructor for booleans
        json_value(bool v) noexcept : boolean(v) {}
        /// constructor for numbers (integer)
        json_value(std::int64_t v) noexcept : number_integer(v) {}
        /// constructor for numbers (unsigned)
        json_value(std::uint64_t v) noexcept : number_unsigned(v) {}
        /// constructor for numbers (floating-point)
        json_value(double v) noexcept : number_float(v) {}
        /// constructor for empty values of a given type
        json_value(value_t t);

        /// constructor for strings
        json_value(llvm::StringRef value);
        json_value(const std::string& value);

        /// constructor for objects
        json_value(const object_t& value);

        /// constructor for arrays
        json_value(const array_t& value);
    };

  private:
    /*!
    @brief checks the class invariants

    This function asserts the class invariants. It needs to be called at the
    end of every constructor to make sure that created objects respect the
    invariant. Furthermore, it has to be called each time the type of a JSON
    value is changed, because the invariant expresses a relationship between
    @a m_type and @a m_value.
    */
    void assert_invariant() const
    {
        assert(m_type != value_t::object || m_value.object != nullptr);
        assert(m_type != value_t::array || m_value.array != nullptr);
        assert(m_type != value_t::string || m_value.string != nullptr);
    }

  public:
    //////////////////////////
    // JSON parser callback //
    //////////////////////////

    /*!
    @brief JSON callback events

    This enumeration lists the parser events that can trigger calling a
    callback function of type @ref parser_callback_t during parsing.

    @image html callback_events.png "Example when certain parse events are triggered"

    @since version 1.0.0
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
    influenced. When passed to @ref parse(wpi::raw_istream&, const
    parser_callback_t) or @ref parse(const CharT, const parser_callback_t),
    it is called on certain events (passed as @ref parse_event_t via parameter
    @a event) with a set recursion depth @a depth and context JSON value
    @a parsed. The return value of the callback function is a boolean
    indicating whether the element that emitted the callback shall be kept or
    not.

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

    @sa @ref parse(wpi::raw_istream&, parser_callback_t) or
    @ref parse(const CharT, const parser_callback_t) for examples

    @since version 1.0.0
    */
    using parser_callback_t = std::function<bool(int depth,
                              parse_event_t event,
                              json& parsed)>;


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

    @param[in] value_type  the type of the value to create

    @complexity Constant.

    @liveexample{The following code shows the constructor for different @ref
    value_t values,json__value_t}

    @since version 1.0.0
    */
    json(const value_t value_type)
        : m_type(value_type), m_value(value_type)
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
    types for which a `to_json()` method exsits. The constructor forwards the
    parameter @a val to that method (to `json_serializer<U>::to_json` method
    with `U = uncvref_t<CompatibleType>`, to be exact).

    Template type @a CompatibleType includes, but is not limited to, the
    following types:
    - **arrays**: @ref array_t and all kinds of compatible containers such as
      `std::vector`, `std::deque`, `std::list`, `std::forward_list`,
      `std::array`, `std::set`, `std::unordered_set`, `std::multiset`, and
      `unordered_multiset` with a `value_type` from which a @ref json
      value can be constructed.
    - **objects**: @ref object_t and all kinds of compatible associative
      containers such as `std::map`, `std::unordered_map`, `std::multimap`,
      and `std::unordered_multimap` with a `key_type` compatible to
      std::string and a `value_type` from which a @ref json value can
      be constructed.
    - **strings**: std::string, string literals, and all compatible string
      containers can be used.
    - **numbers**: std::int64_t, std::uint64_t,
      double, and all convertible number types such as `int`,
      `size_t`, `int64_t`, `float` or `double` can be used.
    - **boolean**: `bool` can be used.

    See the examples below.

    @tparam CompatibleType a type such that:
    - @a CompatibleType is not derived from `wpi::raw_istream`,
    - @a CompatibleType is not @ref json (to avoid hijacking copy/move
         constructors),
    - @a CompatibleType is not a @ref json nested type (e.g.,
         @ref json_pointer, @ref iterator, etc ...)
    - @ref @ref json_serializer<U> has a
         `to_json(json&, CompatibleType&&)` method

    @tparam U = `uncvref_t<CompatibleType>`

    @param[in] val the value to be forwarded

    @complexity Usually linear in the size of the passed @a val, also
                depending on the implementation of the called `to_json()`
                method.

    @throw what `json_serializer<U>::to_json()` throws

    @liveexample{The following code shows the constructor with several
    compatible types.,json__CompatibleType}

    @since version 2.1.0
    */
    template<typename CompatibleType, typename U = detail::uncvref_t<CompatibleType>,
             detail::enable_if_t<!std::is_base_of<wpi::raw_istream, U>::value &&
                                 !std::is_same<U, json>::value &&
                                 !detail::is_json_nested_type<json, U>::value,
                                 int> = 0>
    json(CompatibleType && val)
    {
        to_json(*this, std::forward<CompatibleType>(val));
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
    2. C++ has now way of describing mapped types other than to list a list of
       pairs. As JSON requires that keys must be of type string, rule 2 is the
       weakest constraint one can pose on initializer lists to interpret them
       as an object.
    3. In all other cases, the initializer list could not be interpreted as
       JSON object type, so interpreting it as JSON array type is safe.

    With the rules described above, the following JSON values cannot be
    expressed by an initializer list:

    - the empty array (`[]`): use @ref array(std::initializer_list<json>)
      with an empty initializer list in this case
    - arrays whose elements satisfy rule 2: use @ref
      array(std::initializer_list<json>) with the same initializer list
      in this case

    @note When used without parentheses around an empty initializer list, @ref
    json() is called instead of this function, yielding the JSON null
    value.

    @param[in] init  initializer list with JSON values

    @param[in] type_deduction internal parameter; when set to `true`, the type
    of the JSON value is deducted from the initializer list @a init; when set
    to `false`, the type provided via @a manual_type is forced. This mode is
    used by the functions @ref array(std::initializer_list<json>) and
    @ref object(std::initializer_list<json>).

    @param[in] manual_type internal parameter; when @a type_deduction is set
    to `false`, the created JSON value will use the provided type (only @ref
    value_t::array and @ref value_t::object are valid); when @a type_deduction
    is set to `true`, this parameter has no effect

    @throw type_error.301 if @a type_deduction is `false`, @a manual_type is
    `value_t::object`, but @a init contains an element which is not a pair
    whose first element is a string. In this case, the constructor could not
    create an object. If @a type_deduction would have be `true`, an array
    would have been created. See @ref object(std::initializer_list<json>)
    for an example.

    @complexity Linear in the size of the initializer list @a init.

    @liveexample{The example below shows how JSON values are created from
    initializer lists.,json__list_init_t}

    @sa @ref array(std::initializer_list<json>) -- create a JSON array
    value from an initializer list
    @sa @ref object(std::initializer_list<json>) -- create a JSON object
    value from an initializer list

    @since version 1.0.0
    */
    json(std::initializer_list<json> init,
               bool type_deduction = true,
               value_t manual_type = value_t::array);

    /*!
    @brief explicitly create an array from an initializer list

    Creates a JSON array value from a given initializer list. That is, given a
    list of values `a, b, c`, creates the JSON value `[a, b, c]`. If the
    initializer list is empty, the empty array `[]` is created.

    @note This function is only needed to express two edge cases that cannot
    be realized with the initializer list constructor (@ref
    json(std::initializer_list<json>, bool, value_t)). These cases
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

    @liveexample{The following code shows an example for the `array`
    function.,array}

    @sa @ref json(std::initializer_list<json>, bool, value_t) --
    create a JSON value from an initializer list
    @sa @ref object(std::initializer_list<json>) -- create a JSON object
    value from an initializer list

    @since version 1.0.0
    */
    static json array(std::initializer_list<json> init =
                                std::initializer_list<json>())
    {
        return json(init, false, value_t::array);
    }

    /*!
    @brief explicitly create an object from an initializer list

    Creates a JSON object value from a given initializer list. The initializer
    lists elements must be pairs, and their first elements must be strings. If
    the initializer list is empty, the empty object `{}` is created.

    @note This function is only added for symmetry reasons. In contrast to the
    related function @ref array(std::initializer_list<json>), there are
    no cases which can only be expressed by this function. That is, any
    initializer list @a init can also be passed to the initializer list
    constructor @ref json(std::initializer_list<json>, bool, value_t).

    @param[in] init  initializer list to create an object from (optional)

    @return JSON object value

    @throw type_error.301 if @a init is not a list of pairs whose first
    elements are strings. In this case, no object can be created. When such a
    value is passed to @ref json(std::initializer_list<json>, bool, value_t),
    an array would have been created from the passed initializer list @a init.
    See example below.

    @complexity Linear in the size of @a init.

    @liveexample{The following code shows an example for the `object`
    function.,object}

    @sa @ref json(std::initializer_list<json>, bool, value_t) --
    create a JSON value from an initializer list
    @sa @ref array(std::initializer_list<json>) -- create a JSON array
    value from an initializer list

    @since version 1.0.0
    */
    static json object(std::initializer_list<json> init =
                                 std::initializer_list<json>())
    {
        return json(init, false, value_t::object);
    }

    /*!
    @brief construct an array with count copies of given value

    Constructs a JSON array value by creating @a cnt copies of a passed value.
    In case @a cnt is `0`, an empty array is created. As postcondition,
    `std::distance(begin(),end()) == cnt` holds.

    @param[in] cnt  the number of JSON copies of @a val to create
    @param[in] val  the JSON value to copy

    @complexity Linear in @a cnt.

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
    - In case of primitive types (number, boolean, or string), @a first must
      be `begin()` and @a last must be `end()`. In this case, the value is
      copied. Otherwise, invalid_iterator.204 is thrown.
    - In case of structured types (array, object), the constructor behaves as
      similar versions for `std::vector`.
    - In case of a null type, invalid_iterator.206 is thrown.

    @tparam InputIT an input iterator type (@ref iterator or @ref
    const_iterator)

    @param[in] first begin of the range to copy from (included)
    @param[in] last end of the range to copy from (excluded)

    @pre Iterators @a first and @a last must be initialized. **This
         precondition is enforced with an assertion.**

    @pre Range `[first, last)` is valid. Usually, this precondition cannot be
         checked efficiently. Only certain edge cases are detected; see the
         description of the exceptions below.

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

    @liveexample{The example below shows several ways to create JSON values by
    specifying a subrange with iterators.,json__InputIt_InputIt}

    @since version 1.0.0
    */
    template<class InputIT, typename std::enable_if<
                 std::is_same<InputIT, json::iterator>::value ||
                 std::is_same<InputIT, json::const_iterator>::value, int>::type = 0>
    json(InputIT first, InputIT last)
    {
        assert(first.m_object != nullptr);
        assert(last.m_object != nullptr);

        // make sure iterator fits the current value
        if (first.m_object != last.m_object)
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
                if (!first.m_it.primitive_iterator.is_begin() || !last.m_it.primitive_iterator.is_end())
                {
                    JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
                }
                break;
            }

            default:
            {
                break;
            }
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
            {
                JSON_THROW(invalid_iterator::create(206, "cannot construct with iterators from " +
                                                    first.m_object->type_name()));
            }
        }

        assert_invariant();
    }


    ///////////////////////////////////////
    // other constructors and destructor //
    ///////////////////////////////////////

    /*!
    @brief copy constructor

    Creates a copy of a given JSON value.

    @param[in] other  the JSON value to copy

    @complexity Linear in the size of @a other.

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

    @post @a other is a JSON null value

    @complexity Constant.

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
    and the swap() member function.

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
        std::is_nothrow_move_constructible<value_t>::value &&
        std::is_nothrow_move_assignable<value_t>::value &&
        std::is_nothrow_move_constructible<json_value>::value &&
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
    ~json();

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
    parameter.

    @param[in] indent If indent is nonnegative, then array elements and object
    members will be pretty-printed with that indent level. An indent level of
    `0` will only insert newlines. `-1` (the default) selects the most compact
    representation.

    @return string containing the serialization of the JSON value

    @complexity Linear.

    @liveexample{The following example shows the effect of different @a indent
    parameters to the result of the serialization.,dump}

    @see https://docs.python.org/2/library/json.html#json.dump

    @since version 1.0.0; indentaction character added in version 3.0.0
    */
    std::string dump(const int indent = -1) const;

    /*!
    @brief serialization

    Serialization function for JSON values. The function tries to mimic
    Python's `json.dumps()` function, and currently supports its @a indent
    parameter.

    @param[in,out] os Output stream
    @param[in] indent If indent is nonnegative, then array elements and object
    members will be pretty-printed with that indent level. An indent level of
    `0` will only insert newlines. `-1` (the default) selects the most compact
    representation.

    @complexity Linear.

    @liveexample{The following example shows the effect of different @a indent
    parameters to the result of the serialization.,dump}

    @see https://docs.python.org/2/library/json.html#json.dump

    @since version 1.0.0; indentaction character added in version 3.0.0
    */
    void dump(llvm::raw_ostream& os, int indent = -1) const;

    /*!
    @brief return the type of the JSON value (explicit)

    Return the type of the JSON value as a value from the @ref value_t
    enumeration.

    @return the type of the JSON value

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @liveexample{The following code exemplifies `type()` for all JSON
    types.,type}

    @since version 1.0.0
    */
    value_t type() const noexcept
    {
        return m_type;
    }

    /*!
    @brief return whether type is primitive

    This function returns true iff the JSON type is primitive (string, number,
    boolean, or null).

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
        return is_null() || is_string() || is_boolean() || is_number();
    }

    /*!
    @brief return whether type is structured

    This function returns true iff the JSON type is structured (array or
    object).

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
        return is_array() || is_object();
    }

    /*!
    @brief return whether value is null

    This function returns true iff the JSON value is null.

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
        return m_type == value_t::null;
    }

    /*!
    @brief return whether value is a boolean

    This function returns true iff the JSON value is a boolean.

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
        return m_type == value_t::boolean;
    }

    /*!
    @brief return whether value is a number

    This function returns true iff the JSON value is a number. This includes
    both integer and floating-point values.

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
        return is_number_integer() || is_number_float();
    }

    /*!
    @brief return whether value is an integer number

    This function returns true iff the JSON value is an integer or unsigned
    integer number. This excludes floating-point values.

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
        return m_type == value_t::number_integer || m_type == value_t::number_unsigned;
    }

    /*!
    @brief return whether value is an unsigned integer number

    This function returns true iff the JSON value is an unsigned integer
    number. This excludes floating-point and (signed) integer values.

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
        return m_type == value_t::number_unsigned;
    }

    /*!
    @brief return whether value is a floating-point number

    This function returns true iff the JSON value is a floating-point number.
    This excludes integer and unsigned integer values.

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
        return m_type == value_t::number_float;
    }

    /*!
    @brief return whether value is an object

    This function returns true iff the JSON value is an object.

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
        return m_type == value_t::object;
    }

    /*!
    @brief return whether value is an array

    This function returns true iff the JSON value is an array.

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
        return m_type == value_t::array;
    }

    /*!
    @brief return whether value is a string

    This function returns true iff the JSON value is a string.

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
        return m_type == value_t::string;
    }

    /*!
    @brief return whether value is discarded

    This function returns true iff the JSON value was discarded during parsing
    with a callback function (see @ref parser_callback_t).

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
        return m_type == value_t::discarded;
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
        if (is_boolean())
        {
            return m_value.boolean;
        }

        JSON_THROW(type_error::create(302, "type must be boolean, but is " + type_name()));
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
    std::int64_t* get_impl_ptr(std::int64_t* /*unused*/) noexcept
    {
        return is_number_integer() ? &m_value.number_integer : nullptr;
    }

    /// get a pointer to the value (integer number)
    const std::int64_t* get_impl_ptr(const std::int64_t* /*unused*/) const noexcept
    {
        return is_number_integer() ? &m_value.number_integer : nullptr;
    }

    /// get a pointer to the value (unsigned number)
    std::uint64_t* get_impl_ptr(std::uint64_t* /*unused*/) noexcept
    {
        return is_number_unsigned() ? &m_value.number_unsigned : nullptr;
    }

    /// get a pointer to the value (unsigned number)
    const std::uint64_t* get_impl_ptr(const std::uint64_t* /*unused*/) const noexcept
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

    This funcion helps to implement get_ref() without code duplication for
    const and non-const overloads

    @tparam ThisType will be deduced as `json` or `const json`

    @throw type_error.303 if ReferenceType does not match underlying value
    type of the current JSON
    */
    template<typename ReferenceType, typename ThisType>
    static ReferenceType get_ref_impl(ThisType& obj)
    {
        // helper type
        using PointerType = typename std::add_pointer<ReferenceType>::type;

        // delegate the call to get_ptr<>()
        auto ptr = obj.template get_ptr<PointerType>();

        if (ptr != nullptr)
        {
            return *ptr;
        }

        JSON_THROW(type_error::create(303, "incompatible ReferenceType for get_ref, actual type is " + obj.type_name()));
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
    template <
        typename BasicJsonType,
        detail::enable_if_t<std::is_same<typename std::remove_const<BasicJsonType>::type,
                                         json>::value,
                            int> = 0 >
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
    template <
        typename ValueTypeCV,
        typename ValueType = detail::uncvref_t<ValueTypeCV>,
        detail::enable_if_t < !std::is_same<json, ValueType>::value, int > = 0 >
    ValueType get() const
    {
        // we cannot static_assert on ValueTypeCV being non-const, because
        // there is support for get<const json>(), which is why we
        // still need the uncvref
        static_assert(!std::is_reference<ValueTypeCV>::value,
                      "get() cannot be used with reference types, you might want to use get_ref()");
        static_assert(std::is_default_constructible<ValueType>::value,
                      "types must be DefaultConstructible when used with get()");

        ValueType ret;
        from_json(*this, ret);
        return ret;
    }

    /*!
    @brief get a pointer value (explicit)

    Explicit pointer access to the internally stored JSON value. No copies are
    made.

    @warning The pointer becomes invalid if the underlying JSON object
    changes.

    @tparam PointerType pointer type; must be a pointer to @ref array_t, @ref
    object_t, std::string, bool, std::int64_t,
    std::uint64_t, or double.

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
    object_t, std::string, bool, std::int64_t,
    std::uint64_t, or double. Enforced by a static
    assertion.

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
            || std::is_same<array_t, pointee_t>::value
            || std::is_same<std::string, pointee_t>::value
            || std::is_same<bool, pointee_t>::value
            || std::is_same<std::int64_t, pointee_t>::value
            || std::is_same<std::uint64_t, pointee_t>::value
            || std::is_same<double, pointee_t>::value
            , "incompatible pointer type");

        // delegate the call to get_impl_ptr<>()
        return get_impl_ptr(static_cast<PointerType>(nullptr));
    }

    /*!
    @brief get a pointer value (implicit)
    @copydoc get_ptr()
    */
    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value &&
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
            || std::is_same<array_t, pointee_t>::value
            || std::is_same<std::string, pointee_t>::value
            || std::is_same<bool, pointee_t>::value
            || std::is_same<std::int64_t, pointee_t>::value
            || std::is_same<std::uint64_t, pointee_t>::value
            || std::is_same<double, pointee_t>::value
            , "incompatible pointer type");

        // delegate the call to get_impl_ptr<>() const
        return get_impl_ptr(static_cast<const PointerType>(nullptr));
    }

    /*!
    @brief get a reference value (implicit)

    Implicit reference access to the internally stored JSON value. No copies
    are made.

    @warning Writing data to the referee of the result yields an undefined
    state.

    @tparam ReferenceType reference type; must be a reference to @ref array_t,
    @ref object_t, std::string, bool, std::int64_t, or
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
                 std::is_reference<ReferenceType>::value &&
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
    `std::vector` types for JSON arrays. The character type of std::string
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
                   !std::is_pointer<ValueType>::value &&
                   !std::is_same<ValueType, std::string::value_type>::value
#ifndef _MSC_VER  // fix for issue #167 operator<< ambiguity under VS2015
                   && !std::is_same<ValueType, std::initializer_list<std::string::value_type>>::value
#endif
#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_MSC_VER) && _MSC_VER >1900 && defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
                   && !std::is_same<ValueType, typename std::string_view>::value
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

    @sa @ref operator[](llvm::StringRef) for unchecked
    access by reference
    @sa @ref value() for access by value with a default value

    @since version 1.0.0

    @liveexample{The example below shows how object elements can be read and
    written using `at()`. It also demonstrates the different exceptions that
    can be thrown.,at__object_t_key_type}
    */
    reference at(llvm::StringRef key);

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

    @sa @ref operator[](llvm::StringRef) for unchecked
    access by reference
    @sa @ref value() for access by value with a default value

    @since version 1.0.0

    @liveexample{The example below shows how object elements can be read using
    `at()`. It also demonstrates the different exceptions that can be thrown.,
    at__object_t_key_type_const}
    */
    const_reference at(llvm::StringRef key) const;

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

    @throw type_error.305 if the JSON value is not an array; in that cases,
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

    @sa @ref at(llvm::StringRef) for access by reference
    with range checking
    @sa @ref value() for access by value with a default value

    @since version 1.0.0
    */
    reference operator[](llvm::StringRef key);

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

    @throw type_error.305 if the JSON value is not an object; in that cases,
    using the [] operator with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be read using
    the `[]` operator.,operatorarray__key_type_const}

    @sa @ref at(llvm::StringRef) for access by reference
    with range checking
    @sa @ref value() for access by value with a default value

    @since version 1.0.0
    */
    const_reference operator[](llvm::StringRef key) const;

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
    template<typename T, std::size_t n>
    reference operator[](T * (&key)[n])
    {
        return operator[](static_cast<const T>(key));
    }

    /*!
    @brief read-only access specified object element

    Returns a const reference to the element at with specified key @a key. No
    bounds checking is performed.

    @warning If the element with key @a key does not exist, the behavior is
    undefined.

    @note This function is required for compatibility reasons with Clang.

    @param[in] key  key of the element to access

    @return const reference to the element at key @a key

    @throw type_error.305 if the JSON value is not an object; in that cases,
    using the [] operator with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be read using
    the `[]` operator.,operatorarray__key_type_const}

    @sa @ref at(const typename object_t::key_type&) for access by reference
    with range checking
    @sa @ref value() for access by value with a default value

    @since version 1.0.0
    */
    template<typename T, std::size_t n>
    const_reference operator[](T * (&key)[n]) const
    {
        return operator[](static_cast<const T>(key));
    }

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
        return this->operator[](llvm::StringRef(key));
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

    @throw type_error.305 if the JSON value is not an object; in that cases,
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
        return this->operator[](llvm::StringRef(key));
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

    @note Unlike @ref at(llvm::StringRef), this function
    does not throw if the given key @a key was not found.

    @note Unlike @ref operator[](llvm::StringRef key), this
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

    @throw type_error.306 if the JSON value is not an objec; in that cases,
    using `value()` with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be queried
    with a default value.,json__value}

    @sa @ref at(llvm::StringRef) for access by reference
    with range checking
    @sa @ref operator[](llvm::StringRef) for unchecked
    access by reference

    @since version 1.0.0
    */
    template<class ValueType, typename std::enable_if<
                 std::is_convertible<json, ValueType>::value, int>::type = 0>
    ValueType value(llvm::StringRef key, ValueType default_value) const
    {
        // at only works for objects
        if (is_object())
        {
            // if key is found, return value and given default value otherwise
            const auto it = find(key);
            if (it != end())
            {
                return *it;
            }

            return default_value;
        }
        else
        {
            JSON_THROW(type_error::create(306, "cannot use value() with " + type_name()));
        }
    }

    /*!
    @brief overload for a default value of type const char*
    @copydoc json::value(llvm::StringRef, ValueType) const
    */
    std::string value(llvm::StringRef key, const char* default_value) const
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

    @throw type_error.306 if the JSON value is not an objec; in that cases,
    using `value()` with a key makes no sense.

    @complexity Logarithmic in the size of the container.

    @liveexample{The example below shows how object elements can be queried
    with a default value.,json__value_ptr}

    @sa @ref operator[](const json_pointer&) for unchecked access by reference

    @since version 2.0.2
    */
    template<class ValueType, typename std::enable_if<
                 std::is_convertible<json, ValueType>::value, int>::type = 0>
    ValueType value(const json_pointer& ptr, ValueType default_value) const
    {
        // at only works for objects
        if (is_object())
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

        JSON_THROW(type_error::create(306, "cannot use value() with " + type_name()));
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
    reference back();

    /*!
    @copydoc json::back()
    */
    const_reference back() const;

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
    @sa @ref erase(llvm::StringRef) -- removes the element
    from an object at the given key
    @sa @ref erase(const size_type) -- removes the element from an array at
    the given index

    @since version 1.0.0
    */
    template<class IteratorType, typename std::enable_if<
                 std::is_same<IteratorType, json::iterator>::value ||
                 std::is_same<IteratorType, json::const_iterator>::value, int>::type
             = 0>
    void erase(IteratorType pos)
    {
        // make sure iterator fits the current value
        if (this != pos.m_object)
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
                if (!pos.m_it.primitive_iterator.is_begin())
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
            {
                JSON_THROW(type_error::create(307, "cannot use erase() with " + type_name()));
            }
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
    @sa @ref erase(llvm::StringRef) -- removes the element
    from an object at the given key
    @sa @ref erase(const size_type) -- removes the element from an array at
    the given index

    @since version 1.0.0
    */
    template<class IteratorType, typename std::enable_if<
                 std::is_same<IteratorType, json::iterator>::value ||
                 std::is_same<IteratorType, json::const_iterator>::value, int>::type
             = 0>
    void erase(IteratorType first, IteratorType last)
    {
        // make sure iterator fits the current value
        if (this != first.m_object || this != last.m_object)
        {
            JSON_THROW(invalid_iterator::create(203, "iterators do not fit current value"));
        }

        switch (m_type)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (!first.m_it.primitive_iterator.is_begin() || !last.m_it.primitive_iterator.is_end())
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
                m_value.array->erase(first.m_it.array_iterator,
                                     last.m_it.array_iterator);
                break;
            }

            default:
            {
                JSON_THROW(type_error::create(307, "cannot use erase() with " + type_name()));
            }
        }
    }

    /*!
    @brief remove element from a JSON object given a key

    Removes elements from a JSON object with the key value @a key.

    @param[in] key value of the elements to remove

    @return Number of elements removed.
    The return value will always be `0` (@a key was not
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
    size_type erase(llvm::StringRef key);

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
    @sa @ref erase(llvm::StringRef) -- removes the element
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

    @param[in] key key value of the element to search for

    @return Iterator to an element with key equivalent to @a key. If no such
    element is found or the JSON value is not an object, past-the-end (see
    @ref end()) iterator is returned.

    @complexity Logarithmic in the size of the JSON object.

    @liveexample{The example shows how `find()` is used.,find__key_type}

    @since version 1.0.0
    */
    iterator find(llvm::StringRef key);

    /*!
    @brief find an element in a JSON object
    @copydoc find(llvm::StringRef)
    */
    const_iterator find(llvm::StringRef key) const;

    /*!
    @brief returns the number of occurrences of a key in a JSON object

    Returns the number of elements with key @a key.
    The return value will always be `0` (@a key was
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
    size_type count(llvm::StringRef key) const
    {
        // return 0 for all nonobject types
        return is_object() ? m_value.object->count(key) : 0;
    }

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

  private:
    // forward declaration
    template<typename IteratorType> class iteration_proxy;

  public:
    /*!
    @brief wrapper to access iterator member functions in range-based for

    This function allows to access @ref iterator::key() and @ref
    iterator::value() during range-based for loops. In these loops, a
    reference to the JSON values is returned, so there is no access to the
    underlying iterator.

    @liveexample{The following code shows how the wrapper is used,iterator_wrapper}

    @note The name of this function is not yet final and may change in the
    future.
    */
    static iteration_proxy<iterator> iterator_wrapper(reference cont)
    {
        return iteration_proxy<iterator>(cont);
    }

    /*!
    @copydoc iterator_wrapper(reference)
    */
    static iteration_proxy<const_iterator> iterator_wrapper(const_reference cont)
    {
        return iteration_proxy<const_iterator>(cont);
    }

    /// @}


    //////////////
    // capacity //
    //////////////

    /// @name capacity
    /// @{

    /*!
    @brief checks whether the container is empty

    Checks if a JSON value has no elements.

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

    @note This function does not return whether a string stored as JSON value
    is empty - it returns whether the JSON container itself is empty which is
    false in the case of a string.

    @complexity Constant, as long as @ref array_t and @ref object_t satisfy
    the Container concept; that is, their `empty()` functions have constant
    complexity.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of `begin() == end()`.

    @liveexample{The following code uses `empty()` to check if a JSON
    object contains any elements.,empty}

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

    @note This function does not return the length of a string stored as JSON
    value - it returns the number of elements in the JSON value which is 1 in
    the case of a string.

    @complexity Constant, as long as @ref array_t and @ref object_t satisfy
    the Container concept; that is, their size() functions have constant
    complexity.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of `std::distance(begin(), end())`.

    @liveexample{The following code calls `size()` on the different value
    types.,size}

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

    @complexity Constant, as long as @ref array_t and @ref object_t satisfy
    the Container concept; that is, their `max_size()` functions have constant
    complexity.

    @requirement This function helps `json` satisfying the
    [Container](http://en.cppreference.com/w/cpp/concept/Container)
    requirements:
    - The complexity is constant.
    - Has the semantics of returning `b.size()` where `b` is the largest
      possible JSON value.

    @liveexample{The following code calls `max_size()` on the different value
    types. Note the output is implementation specific.,max_size}

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
    if @ref json(value_t) would have been called:

    Value type  | initial value
    ----------- | -------------
    null        | `null`
    boolean     | `false`
    string      | `""`
    number      | `0`
    object      | `{}`
    array       | `[]`

    @complexity Linear in the size of the JSON value.

    @liveexample{The example below shows the effect of `clear()` to different
    JSON types.,clear}

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
    void push_back(const std::pair<llvm::StringRef, json>& val);

    /*!
    @brief add an object to an object
    @copydoc push_back(const typename object_t::value_type&)
    */
    reference operator+=(const std::pair<llvm::StringRef, json>& val)
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
    void push_back(std::initializer_list<json> init);

    /*!
    @brief add an object to an object
    @copydoc push_back(std::initializer_list<json>)
    */
    reference operator+=(std::initializer_list<json> init)
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
        if (!(is_null() || is_array()))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace_back() with " + type_name()));
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
    std::pair<iterator, bool> emplace(llvm::StringRef key, Args&& ... args)
    {
        // emplace only works for null objects or arrays
        if (!(is_null() || is_object()))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace() with " + type_name()));
        }

        // transform null object into an object
        if (is_null())
        {
            m_type = value_t::object;
            m_value = value_t::object;
            assert_invariant();
        }

        // add element to array (perfect forwarding)
        auto res = m_value.object->emplace_second(key, std::forward<Args>(args)...);
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
    iterator insert(const_iterator pos, std::initializer_list<json> ilist);

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
        std::is_nothrow_move_constructible<value_t>::value &&
        std::is_nothrow_move_assignable<value_t>::value &&
        std::is_nothrow_move_constructible<json_value>::value &&
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
    `swap()`.,swap__array_t}

    @since version 1.0.0
    */
    void swap(array_t& other)
    {
        // swap only works for arrays
        if (is_array())
        {
            std::swap(*(m_value.array), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + type_name()));
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
        if (is_object())
        {
            std::swap(*(m_value.object), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + type_name()));
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
    `swap()`.,swap__std::string}

    @since version 1.0.0
    */
    void swap(std::string& other)
    {
        // swap only works for strings
        if (is_string())
        {
            std::swap(*(m_value.string), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + type_name()));
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
      comparison. Floating-point numbers are compared indirectly: two
      floating-point numbers `f1` and `f2` are considered equal if neither
      `f1 > f2` nor `f2 > f1` holds. Note than two NaN values are always
      treated as unequal.
    - Two JSON null values are equal.

    @note NaN values never compare equal to themselves or to other NaN values.

    @param[in] lhs  first JSON value to consider
    @param[in] rhs  second JSON value to consider
    @return whether the values @a lhs and @a rhs are equal

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

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__notequal}

    @since version 1.0.0
    */
    friend bool operator!=(const_reference lhs, const_reference rhs) noexcept
    {
        return !(lhs == rhs);
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

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__greater}

    @since version 1.0.0
    */
    friend bool operator<=(const_reference lhs, const_reference rhs) noexcept
    {
        return !(rhs < lhs);
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

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__lessequal}

    @since version 1.0.0
    */
    friend bool operator>(const_reference lhs, const_reference rhs) noexcept
    {
        return !(lhs <= rhs);
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

    @liveexample{The example demonstrates comparing several JSON
    types.,operator__greaterequal}

    @since version 1.0.0
    */
    friend bool operator>=(const_reference lhs, const_reference rhs) noexcept
    {
        return !(lhs < rhs);
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

  public:
    /*!
    @brief serialize to stream

    Serialize the given JSON value @a j to the output stream @a o. The JSON
    value will be serialized using the @ref dump member function.

    - The indentation of the output can be controlled with the member variable
      `width` of the output stream @a o. For instance, using the manipulator
      `std::setw(4)` on @a o sets the indentation level to `4` and the
      serialization result is the same as calling `dump(4)`.

    - The indentation characrer can be controlled with the member variable
      `fill` of the output stream @a o. For instance, the manipulator
      `std::setfill('\\t')` sets indentation to use a tab character rather than
      the default space character.

    @param[in,out] o  stream to serialize to
    @param[in] j  JSON value to serialize

    @return the stream @a o

    @complexity Linear.

    @liveexample{The example below shows the serialization with different
    parameters to `width` to adjust the indentation level.,operator_serialize}

    @since version 1.0.0; indentaction character added in version 3.0.0
    */
    friend llvm::raw_ostream& operator<<(llvm::raw_ostream& o, const json& j);

    /// @}


    /////////////////////
    // deserialization //
    /////////////////////

    /// @name deserialization
    /// @{

    /*!
    @brief deserialize from string literal

    @tparam CharT character/literal type with size of 1 byte
    @param[in] s  string literal to read a serialized JSON value from
    @param[in] cb a parser callback function of type @ref parser_callback_t
    which is used to control the deserialization by filtering unwanted values
    (optional)

    @return result of the deserialization

    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails

    @complexity Linear in the length of the input. The parser is a predictive
    LL(1) parser. The complexity can be higher if the parser callback function
    @a cb has a super-linear complexity.

    @note A UTF-8 byte order mark is silently ignored.
    @note String containers like `std::string` can be parsed
          with @ref parse(const ContiguousContainer&, const parser_callback_t)

    @liveexample{The example below demonstrates the `parse()` function with
    and without callback function.,parse__string__parser_callback_t}

    @sa @ref parse(wpi::raw_istream&, const parser_callback_t) for a version
    that reads from an input stream

    @since version 1.0.0 (originally for std::string)
    */
    static json parse(llvm::StringRef s,
                            const parser_callback_t cb = nullptr);

    /*!
    @brief deserialize from stream

    @param[in,out] i  stream to read a serialized JSON value from
    @param[in] cb a parser callback function of type @ref parser_callback_t
    which is used to control the deserialization by filtering unwanted values
    (optional)

    @return result of the deserialization

    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails
    @throw parse_error.111 if input stream is in a bad state

    @complexity Linear in the length of the input. The parser is a predictive
    LL(1) parser. The complexity can be higher if the parser callback function
    @a cb has a super-linear complexity.

    @note A UTF-8 byte order mark is silently ignored.

    @liveexample{The example below demonstrates the `parse()` function with
    and without callback function.,parse__raw_istream__parser_callback_t}

    @sa @ref parse(const CharT, const parser_callback_t) for a version
    that reads from a string

    @since version 1.0.0
    */
    static json parse(wpi::raw_istream& i,
                            const parser_callback_t cb = nullptr);

    /*!
    @brief deserialize from stream

    Deserializes an input stream to a JSON value.

    @param[in,out] i  input stream to read a serialized JSON value from
    @param[in,out] j  JSON value to write the deserialized input to

    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails
    @throw parse_error.111 if input stream is in a bad state

    @complexity Linear in the length of the input. The parser is a predictive
    LL(1) parser.

    @note A UTF-8 byte order mark is silently ignored.

    @liveexample{The example below shows how a JSON value is constructed by
    reading a serialization from a stream.,operator_deserialize}

    @sa parse(wpi::raw_istream&, const parser_callback_t) for a variant with a
    parser callback function to filter values while parsing

    @since version 1.0.0
    */
    friend wpi::raw_istream& operator>>(wpi::raw_istream& i, json& j);

    /// @}

    ///////////////////////////
    // convenience functions //
    ///////////////////////////

    /*!
    @brief return the type as string

    Returns the type name as string to be used in error messages - usually to
    indicate that a function was called on a wrong JSON type.

    @return basically a string representation of a the @a m_type member

    @complexity Constant.

    @liveexample{The following code exemplifies `type_name()` for all JSON
    types.,type_name}

    @since version 1.0.0, public since 2.1.0
    */
    std::string type_name() const;

  private:
    //////////////////////
    // member variables //
    //////////////////////

    /// the type of the current element
    value_t m_type = value_t::null;

    /// the value of the current element
    json_value m_value = {};


  private:
    ///////////////
    // iterators //
    ///////////////

    /*!
    @brief an iterator for primitive JSON types

    This class models an iterator for primitive JSON types (boolean, number,
    string). It's only purpose is to allow the iterator/const_iterator classes
    to "iterate" over primitive values. Internally, the iterator is modeled by
    a `difference_type` variable. Value begin_value (`0`) models the begin,
    end_value (`1`) models past the end.
    */
    class primitive_iterator_t
    {
      public:

        difference_type get_value() const noexcept
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
            return (m_it == begin_value);
        }

        /// return whether the iterator is at end
        constexpr bool is_end() const noexcept
        {
            return (m_it == end_value);
        }

        friend constexpr bool operator==(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
        {
            return lhs.m_it == rhs.m_it;
        }

        friend constexpr bool operator!=(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
        {
            return !(lhs == rhs);
        }

        friend constexpr bool operator<(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
        {
            return lhs.m_it < rhs.m_it;
        }

        friend constexpr bool operator<=(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
        {
            return lhs.m_it <= rhs.m_it;
        }

        friend constexpr bool operator>(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
        {
            return lhs.m_it > rhs.m_it;
        }

        friend constexpr bool operator>=(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
        {
            return lhs.m_it >= rhs.m_it;
        }

        primitive_iterator_t operator+(difference_type i)
        {
            auto result = *this;
            result += i;
            return result;
        }

        friend constexpr difference_type operator-(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
        {
            return lhs.m_it - rhs.m_it;
        }

        friend llvm::raw_ostream& operator<<(llvm::raw_ostream& os, primitive_iterator_t it)
        {
            return os << it.m_it;
        }

        primitive_iterator_t& operator++()
        {
            ++m_it;
            return *this;
        }

        primitive_iterator_t operator++(int)
        {
            auto result = *this;
            m_it++;
            return result;
        }

        primitive_iterator_t& operator--()
        {
            --m_it;
            return *this;
        }

        primitive_iterator_t operator--(int)
        {
            auto result = *this;
            m_it--;
            return result;
        }

        primitive_iterator_t& operator+=(difference_type n)
        {
            m_it += n;
            return *this;
        }

        primitive_iterator_t& operator-=(difference_type n)
        {
            m_it -= n;
            return *this;
        }

      private:
        static constexpr difference_type begin_value = 0;
        static constexpr difference_type end_value = begin_value + 1;

        /// iterator as signed integer type
        difference_type m_it = std::numeric_limits<std::ptrdiff_t>::denorm_min();
    };

    /*!
    @brief an iterator value

    @note This structure could easily be a union, but MSVC currently does not
    allow unions members with complex constructors, see
    https://github.com/nlohmann/json/pull/105.
    */
    struct internal_iterator
    {
        /// iterator for JSON objects
        typename object_t::iterator object_iterator;
        /// iterator for JSON arrays
        typename array_t::iterator array_iterator;
        /// generic iterator for all other types
        primitive_iterator_t primitive_iterator;

        /// create an uninitialized internal_iterator
        internal_iterator() noexcept
            : object_iterator(), array_iterator(), primitive_iterator()
        {}
    };

    /// proxy class for the iterator_wrapper functions
    template<typename IteratorType>
    class iteration_proxy
    {
      private:
        /// helper class for iteration
        class iteration_proxy_internal
        {
          private:
            /// the iterator
            IteratorType anchor;
            /// an index for arrays (used to create key names)
            size_t array_index = 0;

          public:
            explicit iteration_proxy_internal(IteratorType it) noexcept
                : anchor(it)
            {}

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
            bool operator!= (const iteration_proxy_internal& o) const
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
                    {
                        return std::to_string(array_index);
                    }

                    // use key from the object
                    case value_t::object:
                    {
                        return anchor.key();
                    }

                    // use an empty key for all primitive types
                    default:
                    {
                        return "";
                    }
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
        explicit iteration_proxy(typename IteratorType::reference cont)
            : container(cont)
        {}

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

  public:
    /*!
    @brief a template for a random access iterator for the @ref json class

    This class implements a both iterators (iterator and const_iterator) for the
    @ref json class.

    @note An iterator is called *initialized* when a pointer to a JSON value
          has been set (e.g., by a constructor or a copy assignment). If the
          iterator is default-constructed, it is *uninitialized* and most
          methods are undefined. **The library uses assertions to detect calls
          on uninitialized iterators.**

    @requirement The class satisfies the following concept requirements:
    - [RandomAccessIterator](http://en.cppreference.com/w/cpp/concept/RandomAccessIterator):
      The iterator that can be moved to point (forward and backward) to any
      element in constant time.

    @since version 1.0.0, simplified in version 2.0.9
    */
    template<typename U>
    class iter_impl : public std::iterator<std::random_access_iterator_tag, U>
    {
        /// allow json to access private members
        friend class json;
        friend class JsonTest;

        // make sure U is json or const json
        static_assert(std::is_same<U, json>::value
                      || std::is_same<U, const json>::value,
                      "iter_impl only accepts (const) json");

      public:
        /// the type of the values when the iterator is dereferenced
        using value_type = typename json::value_type;
        /// a type to represent differences between iterators
        using difference_type = typename json::difference_type;
        /// defines a pointer to the type iterated over (value_type)
        using pointer = typename std::conditional<std::is_const<U>::value,
              typename json::const_pointer,
              typename json::pointer>::type;
        /// defines a reference to the type iterated over (value_type)
        using reference = typename std::conditional<std::is_const<U>::value,
              typename json::const_reference,
              typename json::reference>::type;
        /// the category of the iterator
        using iterator_category = std::bidirectional_iterator_tag;

        /// default constructor
        iter_impl() noexcept : m_object(nullptr) {}

        /*!
        @brief constructor for a given JSON instance
        @param[in] object  pointer to a JSON object for this iterator
        @pre object != nullptr
        @post The iterator is initialized; i.e. `m_object != nullptr`.
        */
        explicit iter_impl(pointer object) noexcept
            : m_object(object)
        {
            assert(m_object != nullptr);

            switch (m_object->m_type)
            {
                case json::value_t::object:
                {
                    m_it.object_iterator = typename object_t::iterator();
                    break;
                }

                case json::value_t::array:
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
        @note The conventional copy constructor and copy assignment are
              implicitly defined.
              Combined with the following converting constructor and assigment,
              they support: copy from iterator to iterator,
                            copy from const iterator to const iterator,
                            and conversion from iterator to const iterator.
              However conversion from const iterator to iterator is not defined.
        */

        /*!
        @brief converting constructor
        @param[in] other  non-const iterator to copy from
        @note It is not checked whether @a other is initialized.
        */
        iter_impl(const iter_impl<json>& other) noexcept
            : m_object(other.m_object), m_it(other.m_it)
        {}

        /*!
        @brief converting assignment
        @param[in,out] other  non-const iterator to copy from
        @return const/non-const iterator
        @note It is not checked whether @a other is initialized.
        */
        iter_impl& operator=(const iter_impl<json>& other) noexcept
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
                case json::value_t::object:
                {
                    m_it.object_iterator = m_object->m_value.object->begin();
                    break;
                }

                case json::value_t::array:
                {
                    m_it.array_iterator = m_object->m_value.array->begin();
                    break;
                }

                case json::value_t::null:
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
                case json::value_t::object:
                {
                    m_it.object_iterator = m_object->m_value.object->end();
                    break;
                }

                case json::value_t::array:
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
                case json::value_t::object:
                {
                    assert(m_it.object_iterator != m_object->m_value.object->end());
                    return m_it.object_iterator->second;
                }

                case json::value_t::array:
                {
                    assert(m_it.array_iterator != m_object->m_value.array->end());
                    return *m_it.array_iterator;
                }

                case json::value_t::null:
                {
                    JSON_THROW(invalid_iterator::create(214, "cannot get value"));
                }

                default:
                {
                    if (m_it.primitive_iterator.is_begin())
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
                case json::value_t::object:
                {
                    assert(m_it.object_iterator != m_object->m_value.object->end());
                    return &(m_it.object_iterator->second);
                }

                case json::value_t::array:
                {
                    assert(m_it.array_iterator != m_object->m_value.array->end());
                    return &*m_it.array_iterator;
                }

                default:
                {
                    if (m_it.primitive_iterator.is_begin())
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
        iter_impl operator++(int)
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
                case json::value_t::object:
                {
                    ++m_it.object_iterator;
                    break;
                }

                case json::value_t::array:
                {
                    ++m_it.array_iterator;
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
        iter_impl operator--(int)
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
                case json::value_t::object:
                {
                    JSON_THROW(invalid_iterator::create(209, "cannot decrement object iterators"));
                    break;
                }

                case json::value_t::array:
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
            if (m_object != other.m_object)
            {
                JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
            }

            assert(m_object != nullptr);

            switch (m_object->m_type)
            {
                case json::value_t::object:
                {
                    return (m_it.object_iterator == other.m_it.object_iterator);
                }

                case json::value_t::array:
                {
                    return (m_it.array_iterator == other.m_it.array_iterator);
                }

                default:
                {
                    return (m_it.primitive_iterator == other.m_it.primitive_iterator);
                }
            }
        }

        /*!
        @brief  comparison: not equal
        @pre The iterator is initialized; i.e. `m_object != nullptr`.
        */
        bool operator!=(const iter_impl& other) const
        {
            return !operator==(other);
        }

        /*!
        @brief  comparison: smaller
        @pre The iterator is initialized; i.e. `m_object != nullptr`.
        */
        bool operator<(const iter_impl& other) const
        {
            // if objects are not the same, the comparison is undefined
            if (m_object != other.m_object)
            {
                JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
            }

            assert(m_object != nullptr);

            switch (m_object->m_type)
            {
                case json::value_t::object:
                {
                    JSON_THROW(invalid_iterator::create(213, "cannot compare order of object iterators"));
                }

                case json::value_t::array:
                {
                    return (m_it.array_iterator < other.m_it.array_iterator);
                }

                default:
                {
                    return (m_it.primitive_iterator < other.m_it.primitive_iterator);
                }
            }
        }

        /*!
        @brief  comparison: less than or equal
        @pre The iterator is initialized; i.e. `m_object != nullptr`.
        */
        bool operator<=(const iter_impl& other) const
        {
            return !(other.operator < (*this));
        }

        /*!
        @brief  comparison: greater than
        @pre The iterator is initialized; i.e. `m_object != nullptr`.
        */
        bool operator>(const iter_impl& other) const
        {
            return !operator<=(other);
        }

        /*!
        @brief  comparison: greater than or equal
        @pre The iterator is initialized; i.e. `m_object != nullptr`.
        */
        bool operator>=(const iter_impl& other) const
        {
            return !operator<(other);
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
                case json::value_t::object:
                {
                    JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators"));
                }

                case json::value_t::array:
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
                case json::value_t::object:
                {
                    JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators"));
                }

                case json::value_t::array:
                {
                    return m_it.array_iterator - other.m_it.array_iterator;
                }

                default:
                {
                    return m_it.primitive_iterator - other.m_it.primitive_iterator;
                }
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
                case json::value_t::object:
                {
                    JSON_THROW(invalid_iterator::create(208, "cannot use operator[] for object iterators"));
                }

                case json::value_t::array:
                {
                    return *std::next(m_it.array_iterator, n);
                }

                case json::value_t::null:
                {
                    JSON_THROW(invalid_iterator::create(214, "cannot get value"));
                }

                default:
                {
                    if (m_it.primitive_iterator.get_value() == -n)
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
        llvm::StringRef key() const
        {
            assert(m_object != nullptr);

            if (m_object->is_object())
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
        pointer m_object;
        /// the actual iterator of the associated instance
        struct internal_iterator m_it = internal_iterator();
    };

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
    null            | `null`                                     | Null                               | 0xf6
    boolean         | `true`                                     | True                               | 0xf5
    boolean         | `false`                                    | False                              | 0xf4
    number_integer  | -9223372036854775808..-2147483649          | Negative integer (8 bytes follow)  | 0x3b
    number_integer  | -2147483648..-32769                        | Negative integer (4 bytes follow)  | 0x3a
    number_integer  | -32768..-129                               | Negative integer (2 bytes follow)  | 0x39
    number_integer  | -128..-25                                  | Negative integer (1 byte follow)   | 0x38
    number_integer  | -24..-1                                    | Negative integer                   | 0x20..0x37
    number_integer  | 0..23                                      | Integer                            | 0x00..0x17
    number_integer  | 24..255                                    | Unsigned integer (1 byte follow)   | 0x18
    number_integer  | 256..65535                                 | Unsigned integer (2 bytes follow)  | 0x19
    number_integer  | 65536..4294967295                          | Unsigned integer (4 bytes follow)  | 0x1a
    number_integer  | 4294967296..18446744073709551615           | Unsigned integer (8 bytes follow)  | 0x1b
    number_unsigned | 0..23                                      | Integer                            | 0x00..0x17
    number_unsigned | 24..255                                    | Unsigned integer (1 byte follow)   | 0x18
    number_unsigned | 256..65535                                 | Unsigned integer (2 bytes follow)  | 0x19
    number_unsigned | 65536..4294967295                          | Unsigned integer (4 bytes follow)  | 0x1a
    number_unsigned | 4294967296..18446744073709551615           | Unsigned integer (8 bytes follow)  | 0x1b
    number_float    | *any value*                                | Double-Precision Float             | 0xfb
    string          | *length*: 0..23                            | UTF-8 string                       | 0x60..0x77
    string          | *length*: 23..255                          | UTF-8 string (1 byte follow)       | 0x78
    string          | *length*: 256..65535                       | UTF-8 string (2 bytes follow)      | 0x79
    string          | *length*: 65536..4294967295                | UTF-8 string (4 bytes follow)      | 0x7a
    string          | *length*: 4294967296..18446744073709551615 | UTF-8 string (8 bytes follow)      | 0x7b
    array           | *size*: 0..23                              | array                              | 0x80..0x97
    array           | *size*: 23..255                            | array (1 byte follow)              | 0x98
    array           | *size*: 256..65535                         | array (2 bytes follow)             | 0x99
    array           | *size*: 65536..4294967295                  | array (4 bytes follow)             | 0x9a
    array           | *size*: 4294967296..18446744073709551615   | array (8 bytes follow)             | 0x9b
    object          | *size*: 0..23                              | map                                | 0xa0..0xb7
    object          | *size*: 23..255                            | map (1 byte follow)                | 0xb8
    object          | *size*: 256..65535                         | map (2 bytes follow)               | 0xb9
    object          | *size*: 65536..4294967295                  | map (4 bytes follow)               | 0xba
    object          | *size*: 4294967296..18446744073709551615   | map (8 bytes follow)               | 0xbb

    @note The mapping is **complete** in the sense that any JSON value type
          can be converted to a CBOR value.

    @note The following CBOR types are not used in the conversion:
          - byte strings (0x40..0x5f)
          - UTF-8 strings terminated by "break" (0x7f)
          - arrays terminated by "break" (0x9f)
          - maps terminated by "break" (0xbf)
          - date/time (0xc0..0xc1)
          - bignum (0xc2..0xc3)
          - decimal fraction (0xc4)
          - bigfloat (0xc5)
          - tagged items (0xc6..0xd4, 0xd8..0xdb)
          - expected conversions (0xd5..0xd7)
          - simple values (0xe0..0xf3, 0xf8)
          - undefined (0xf7)
          - half and single-precision floats (0xf9-0xfa)
          - break (0xff)

    @param[in,out] os  output stream
    @param[in] j  JSON value to serialize

    @complexity Linear in the size of the JSON value @a j.

    @liveexample{The example shows the serialization of a JSON value to a byte
    vector in CBOR format.,to_cbor}

    @sa http://cbor.io
    @sa @ref from_cbor(const std::vector<uint8_t>&, const size_t) for the
        analogous deserialization
    @sa @ref to_msgpack(const json& for the related MessagePack format

    @since version 2.0.9
    */
    static void to_cbor(llvm::raw_ostream& os, const json& j);
    static llvm::StringRef to_cbor(const json& j, llvm::SmallVectorImpl<char> buf);
    static std::string to_cbor(const json& j);

    /*!
    @brief create a MessagePack serialization of a given JSON value

    Serializes a given JSON value @a j to a byte vector using the MessagePack
    serialization format. MessagePack is a binary serialization format which
    aims to be more compact than JSON itself, yet more efficient to parse.

    The library uses the following mapping from JSON values types to
    MessagePack types according to the MessagePack specification:

    JSON value type | value/range                       | MessagePack type | first byte
    --------------- | --------------------------------- | ---------------- | ----------
    null            | `null`                            | nil              | 0xc0
    boolean         | `true`                            | true             | 0xc3
    boolean         | `false`                           | false            | 0xc2
    number_integer  | -9223372036854775808..-2147483649 | int64            | 0xd3
    number_integer  | -2147483648..-32769               | int32            | 0xd2
    number_integer  | -32768..-129                      | int16            | 0xd1
    number_integer  | -128..-33                         | int8             | 0xd0
    number_integer  | -32..-1                           | negative fixint  | 0xe0..0xff
    number_integer  | 0..127                            | positive fixint  | 0x00..0x7f
    number_integer  | 128..255                          | uint 8           | 0xcc
    number_integer  | 256..65535                        | uint 16          | 0xcd
    number_integer  | 65536..4294967295                 | uint 32          | 0xce
    number_integer  | 4294967296..18446744073709551615  | uint 64          | 0xcf
    number_unsigned | 0..127                            | positive fixint  | 0x00..0x7f
    number_unsigned | 128..255                          | uint 8           | 0xcc
    number_unsigned | 256..65535                        | uint 16          | 0xcd
    number_unsigned | 65536..4294967295                 | uint 32          | 0xce
    number_unsigned | 4294967296..18446744073709551615  | uint 64          | 0xcf
    number_float    | *any value*                       | float 64         | 0xcb
    string          | *length*: 0..31                   | fixstr           | 0xa0..0xbf
    string          | *length*: 32..255                 | str 8            | 0xd9
    string          | *length*: 256..65535              | str 16           | 0xda
    string          | *length*: 65536..4294967295       | str 32           | 0xdb
    array           | *size*: 0..15                     | fixarray         | 0x90..0x9f
    array           | *size*: 16..65535                 | array 16         | 0xdc
    array           | *size*: 65536..4294967295         | array 32         | 0xdd
    object          | *size*: 0..15                     | fix map          | 0x80..0x8f
    object          | *size*: 16..65535                 | map 16           | 0xde
    object          | *size*: 65536..4294967295         | map 32           | 0xdf

    @note The mapping is **complete** in the sense that any JSON value type
          can be converted to a MessagePack value.

    @note The following values can **not** be converted to a MessagePack value:
          - strings with more than 4294967295 bytes
          - arrays with more than 4294967295 elements
          - objects with more than 4294967295 elements

    @note The following MessagePack types are not used in the conversion:
          - bin 8 - bin 32 (0xc4..0xc6)
          - ext 8 - ext 32 (0xc7..0xc9)
          - float 32 (0xca)
          - fixext 1 - fixext 16 (0xd4..0xd8)

    @note Any MessagePack output created @ref to_msgpack can be successfully
          parsed by @ref from_msgpack.

    @param[in,out] os  output stream
    @param[in] j  JSON value to serialize

    @complexity Linear in the size of the JSON value @a j.

    @liveexample{The example shows the serialization of a JSON value to a byte
    vector in MessagePack format.,to_msgpack}

    @sa http://msgpack.org
    @sa @ref from_msgpack(const std::vector<uint8_t>&, const size_t) for the
        analogous deserialization
    @sa @ref to_cbor(const json& for the related CBOR format

    @since version 2.0.9
    */
    static void to_msgpack(llvm::raw_ostream& os, const json& j);
    static llvm::StringRef to_msgpack(const json& j, llvm::SmallVectorImpl<char> buf);
    static std::string to_msgpack(const json& j);

    /*!
    @brief create a JSON value from a byte vector in CBOR format

    Deserializes a given byte vector @a v to a JSON value using the CBOR
    (Concise Binary Object Representation) serialization format.

    The library maps CBOR types to JSON value types as follows:

    CBOR type              | JSON value type | first byte
    ---------------------- | --------------- | ----------
    Integer                | number_unsigned | 0x00..0x17
    Unsigned integer       | number_unsigned | 0x18
    Unsigned integer       | number_unsigned | 0x19
    Unsigned integer       | number_unsigned | 0x1a
    Unsigned integer       | number_unsigned | 0x1b
    Negative integer       | number_integer  | 0x20..0x37
    Negative integer       | number_integer  | 0x38
    Negative integer       | number_integer  | 0x39
    Negative integer       | number_integer  | 0x3a
    Negative integer       | number_integer  | 0x3b
    Negative integer       | number_integer  | 0x40..0x57
    UTF-8 string           | string          | 0x60..0x77
    UTF-8 string           | string          | 0x78
    UTF-8 string           | string          | 0x79
    UTF-8 string           | string          | 0x7a
    UTF-8 string           | string          | 0x7b
    UTF-8 string           | string          | 0x7f
    array                  | array           | 0x80..0x97
    array                  | array           | 0x98
    array                  | array           | 0x99
    array                  | array           | 0x9a
    array                  | array           | 0x9b
    array                  | array           | 0x9f
    map                    | object          | 0xa0..0xb7
    map                    | object          | 0xb8
    map                    | object          | 0xb9
    map                    | object          | 0xba
    map                    | object          | 0xbb
    map                    | object          | 0xbf
    False                  | `false`         | 0xf4
    True                   | `true`          | 0xf5
    Nill                   | `null`          | 0xf6
    Half-Precision Float   | number_float    | 0xf9
    Single-Precision Float | number_float    | 0xfa
    Double-Precision Float | number_float    | 0xfb

    @warning The mapping is **incomplete** in the sense that not all CBOR
             types can be converted to a JSON value. The following CBOR types
             are not supported and will yield parse errors (parse_error.112):
             - byte strings (0x40..0x5f)
             - date/time (0xc0..0xc1)
             - bignum (0xc2..0xc3)
             - decimal fraction (0xc4)
             - bigfloat (0xc5)
             - tagged items (0xc6..0xd4, 0xd8..0xdb)
             - expected conversions (0xd5..0xd7)
             - simple values (0xe0..0xf3, 0xf8)
             - undefined (0xf7)

    @warning CBOR allows map keys of any type, whereas JSON only allows
             strings as keys in object values. Therefore, CBOR maps with keys
             other than UTF-8 strings are rejected (parse_error.113).

    @note Any CBOR output created @ref to_cbor can be successfully parsed by
          @ref from_cbor.

    @param[in] is an input stream in CBOR format
    @return deserialized JSON value

    @throw parse_error.110 if the given vector ends prematurely
    @throw parse_error.112 if unsupported features from CBOR were
    used in the given vector @a v or if the input is not valid CBOR
    @throw parse_error.113 if a string was expected as map key, but not found

    @complexity Linear in the size of the byte vector @a v.

    @liveexample{The example shows the deserialization of a byte vector in CBOR
    format to a JSON value.,from_cbor}

    @sa http://cbor.io
    @sa @ref to_cbor(const json&) for the analogous serialization
    @sa @ref from_msgpack(const std::vector<uint8_t>&, const size_t) for the
        related MessagePack format

    @since version 2.0.9, parameter @a start_index since 2.1.1
    */
    static json from_cbor(wpi::raw_istream& is);
    static json from_cbor(llvm::StringRef s);

    /*!
    @brief create a JSON value from a byte vector in MessagePack format

    Deserializes a given byte vector @a v to a JSON value using the MessagePack
    serialization format.

    The library maps MessagePack types to JSON value types as follows:

    MessagePack type | JSON value type | first byte
    ---------------- | --------------- | ----------
    positive fixint  | number_unsigned | 0x00..0x7f
    fixmap           | object          | 0x80..0x8f
    fixarray         | array           | 0x90..0x9f
    fixstr           | string          | 0xa0..0xbf
    nil              | `null`          | 0xc0
    false            | `false`         | 0xc2
    true             | `true`          | 0xc3
    float 32         | number_float    | 0xca
    float 64         | number_float    | 0xcb
    uint 8           | number_unsigned | 0xcc
    uint 16          | number_unsigned | 0xcd
    uint 32          | number_unsigned | 0xce
    uint 64          | number_unsigned | 0xcf
    int 8            | number_integer  | 0xd0
    int 16           | number_integer  | 0xd1
    int 32           | number_integer  | 0xd2
    int 64           | number_integer  | 0xd3
    str 8            | string          | 0xd9
    str 16           | string          | 0xda
    str 32           | string          | 0xdb
    array 16         | array           | 0xdc
    array 32         | array           | 0xdd
    map 16           | object          | 0xde
    map 32           | object          | 0xdf
    negative fixint  | number_integer  | 0xe0-0xff

    @warning The mapping is **incomplete** in the sense that not all
             MessagePack types can be converted to a JSON value. The following
             MessagePack types are not supported and will yield parse errors:
              - bin 8 - bin 32 (0xc4..0xc6)
              - ext 8 - ext 32 (0xc7..0xc9)
              - fixext 1 - fixext 16 (0xd4..0xd8)

    @note Any MessagePack output created @ref to_msgpack can be successfully
          parsed by @ref from_msgpack.

    @param[in] is an input stream in MessagePack format
    @return deserialized JSON value

    @throw parse_error.110 if the given vector ends prematurely
    @throw parse_error.112 if unsupported features from MessagePack were
    used in the given vector @a v or if the input is not valid MessagePack
    @throw parse_error.113 if a string was expected as map key, but not found

    @complexity Linear in the size of the byte vector @a v.

    @liveexample{The example shows the deserialization of a byte vector in
    MessagePack format to a JSON value.,from_msgpack}

    @sa http://msgpack.org
    @sa @ref to_msgpack(const json&) for the analogous serialization
    @sa @ref from_cbor(const std::vector<uint8_t>&, const size_t) for the
        related CBOR format

    @since version 2.0.9, parameter @a start_index since 2.1.1
    */
    static json from_msgpack(wpi::raw_istream& is);
    static json from_msgpack(llvm::StringRef s);

    /// @}

  public:
    /*!
    @brief JSON Pointer

    A JSON pointer defines a string syntax for identifying a specific value
    within a JSON document. It can be used with functions `at` and
    `operator[]`. Furthermore, JSON pointers are the base for JSON patches.

    @sa [RFC 6901](https://tools.ietf.org/html/rfc6901)

    @since version 2.0.0
    */
    class json_pointer
    {
        /// allow json to access private members
        friend class json;
        friend class JsonTest;

      public:
        /*!
        @brief create JSON pointer

        Create a JSON pointer according to the syntax described in
        [Section 3 of RFC6901](https://tools.ietf.org/html/rfc6901#section-3).

        @param[in] s  string representing the JSON pointer; if omitted, the
                      empty string is assumed which references the whole JSON
                      value

        @throw parse_error.107 if the given JSON pointer @a s is nonempty and
        does not begin with a slash (`/`); see example below

        @throw parse_error.108 if a tilde (`~`) in the given JSON pointer @a s
        is not followed by `0` (representing `~`) or `1` (representing `/`);
        see example below

        @liveexample{The example shows the construction several valid JSON
        pointers as well as the exceptional behavior.,json_pointer}

        @since version 2.0.0
        */
        explicit json_pointer(const std::string& s = "")
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

      private:
        /*!
        @brief remove and return last reference pointer
        @throw out_of_range.405 if JSON pointer has no parent
        */
        std::string pop_back()
        {
            if (is_root())
            {
                JSON_THROW(out_of_range::create(405, "JSON pointer has no parent"));
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
            if (is_root())
            {
                JSON_THROW(out_of_range::create(405, "JSON pointer has no parent"));
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
        reference get_and_create(reference j) const;

        /*!
        @brief return a reference to the pointed to value

        @note This version does not throw if a value is not present, but tries
        to create nested values instead. For instance, calling this function
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
        reference get_unchecked(pointer ptr) const;

        /*!
        @throw parse_error.106   if an array index begins with '0'
        @throw parse_error.109   if an array index was not a number
        @throw out_of_range.402  if the array index '-' is used
        @throw out_of_range.404  if the JSON pointer can not be resolved
        */
        reference get_checked(pointer ptr) const;

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
        const_reference get_unchecked(const_pointer ptr) const;

        /*!
        @throw parse_error.106   if an array index begins with '0'
        @throw parse_error.109   if an array index was not a number
        @throw out_of_range.402  if the array index '-' is used
        @throw out_of_range.404  if the JSON pointer can not be resolved
        */
        const_reference get_checked(const_pointer ptr) const;

        /*!
        @brief split the string input to reference tokens

        @note This function is only called by the json_pointer constructor.
              All exceptions below are documented there.

        @throw parse_error.107  if the pointer is not empty or begins with '/'
        @throw parse_error.108  if character '~' is not followed by '0' or '1'
        */
        static std::vector<std::string> split(const std::string& reference_string);

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
        static void replace_substring(std::string& s,
                                      const std::string& f,
                                      const std::string& t);

        /// escape tilde and slash
        static std::string escape(std::string s);

        /// unescape tilde and slash
        static void unescape(std::string& s);

        /*!
        @param[in] reference_string  the reference string to the current value
        @param[in] value             the value to consider
        @param[in,out] result        the result object to insert values to

        @note Empty objects or arrays are flattened to `null`.
        */
        static void flatten(const std::string& reference_string,
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
        static json unflatten(const json& value);

        friend bool operator==(json_pointer const& lhs,
                               json_pointer const& rhs) noexcept
        {
            return lhs.reference_tokens == rhs.reference_tokens;
        }

        friend bool operator!=(json_pointer const& lhs,
                               json_pointer const& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        /// the reference tokens
        std::vector<std::string> reference_tokens {};
    };

    //////////////////////////
    // JSON Pointer support //
    //////////////////////////

    /// @name JSON Pointer functions
    /// @{

    /*!
    @brief access specified element via JSON Pointer

    Uses a JSON pointer to retrieve a reference to the respective JSON value.
    No bound checking is performed. Similar to @ref operator[](
    llvm::StringRef), `null` values are created in arrays and objects if
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
    @throw type_error.315  if object values are not primitve

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
                     is_nothrow_move_constructible<wpi::json>::value &&
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
    std::size_t operator()(const wpi::json& j) const;
};

/// specialization for std::less<value_t>
template <>
struct less<::wpi::detail::value_t>
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
    return wpi::json::parse(llvm::StringRef(s, n));
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
    return wpi::json::json_pointer(std::string(s, n));
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

#endif  // WPI_JSON_IMPLEMENTATION

#endif
