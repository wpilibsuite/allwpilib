#ifndef NLOHMANN_JSON_FWD_HPP
#define NLOHMANN_JSON_FWD_HPP

#include <cstdint> // int64_t, uint64_t
#include <map> // map
#include <memory> // allocator
#include <string> // string
#include <vector> // vector

/*!
@brief namespace for Niels Lohmann
@see https://github.com/nlohmann
@since version 1.0.0
*/
namespace wpi
{
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

#endif
