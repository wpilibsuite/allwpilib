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
#include "support/json.h"

#include <clocale> // lconv, localeconv
#include <locale> // locale

#include "llvm/raw_ostream.h"

namespace wpi {

/*!
@brief wrapper around the serialization functions
*/
class json::serializer
{
  public:
    serializer(const serializer&) = delete;
    serializer& operator=(const serializer&) = delete;

    /*!
    @param[in] s  output stream to serialize to
    @param[in] ichar  indentation character to use
    */
    explicit serializer(llvm::raw_ostream& s)
        : o(s), loc(std::localeconv()),
          thousands_sep(!loc->thousands_sep ? '\0' : loc->thousands_sep[0]),
          decimal_point(!loc->decimal_point ? '\0' : loc->decimal_point[0])
    {}

    /*!
    @brief internal implementation of the serialization function

    This function is called by the public member function dump and
    organizes the serialization internally. The indentation level is
    propagated as additional parameter. In case of arrays and objects, the
    function is called recursively.

    - strings and object keys are escaped using `escape_string()`
    - integer numbers are converted implicitly via `operator<<`
    - floating-point numbers are converted to a string using `"%g"` format

    @param[in] val             value to serialize
    @param[in] pretty_print    whether the output shall be pretty-printed
    @param[in] indent_step     the indent level
    @param[in] current_indent  the current indent level (only used internally)
    */
    void dump(const json& val,
              const bool pretty_print,
              const unsigned int indent_step,
              const unsigned int current_indent = 0);

    /*!
    @brief dump escaped string

    Escape a string by replacing certain special characters by a sequence
    of an escape character (backslash) and another character and other
    control characters by a sequence of "\u" followed by a four-digit hex
    representation. The escaped string is written to output stream @a o.

    @param[in] s  the string to escape

    @complexity Linear in the length of string @a s.
    */
    void dump_escaped(llvm::StringRef s) const;

    /*!
    @brief dump a floating-point number

    Dump a given floating-point number to output stream @a o. Works
    internally with @a number_buffer.

    @param[in] x  floating-point number to dump
    */
    void dump_float(double x);

  private:
    /// the output of the serializer
    llvm::raw_ostream& o;

    /// the locale
    const std::lconv* loc = nullptr;
    /// the locale's thousand separator character
    const char thousands_sep = '\0';
    /// the locale's decimal point character
    const char decimal_point = '\0';
};

}  // namespace wpi
