#pragma once

#include <cassert> // assert
#include <numeric> // accumulate
#include <string> // string
#include <vector> // vector

#include "wpi/detail/macro_scope.h"
#include "wpi/detail/exceptions.h"
#include "wpi/detail/value_t.h"

namespace wpi
{
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
    explicit json_pointer(std::string_view s = {})
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
    static int array_index(std::string_view s);

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
    static std::vector<std::string> split(std::string_view reference_string);

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
    static void flatten(std::string_view reference_string,
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
}

