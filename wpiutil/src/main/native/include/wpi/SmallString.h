//===- llvm/ADT/SmallString.h - 'Normally small' strings --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the SmallString class.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_SMALLSTRING_H
#define WPIUTIL_WPI_SMALLSTRING_H

#include "wpi/SmallVector.h"
#include <cstddef>
#include <string_view>

namespace wpi {

/// SmallString - A SmallString is just a SmallVector with methods and accessors
/// that make it work better as a string (e.g. operator+ etc).
template<unsigned InternalLen>
class SmallString : public SmallVector<char, InternalLen> {
public:
  /// Default ctor - Initialize to empty.
  SmallString() = default;

  /// Initialize from a std::string_view.
  SmallString(std::string_view S)
    : SmallVector<char, InternalLen>(S.begin(), S.end()) {}

  /// Initialize with a range.
  template<typename ItTy>
  SmallString(ItTy S, ItTy E) : SmallVector<char, InternalLen>(S, E) {}

  // Note that in order to add new overloads for append & assign, we have to
  // duplicate the inherited versions so as not to inadvertently hide them.

  /// @}
  /// @name String Assignment
  /// @{

  /// Assign from a repeated element.
  void assign(size_t NumElts, char Elt) {
    this->SmallVectorImpl<char>::assign(NumElts, Elt);
  }

  /// Assign from an iterator pair.
  template<typename in_iter>
  void assign(in_iter S, in_iter E) {
    this->clear();
    SmallVectorImpl<char>::append(S, E);
  }

  /// Assign from a std::string_view.
  void assign(std::string_view RHS) {
    this->clear();
    SmallVectorImpl<char>::append(RHS.begin(), RHS.end());
  }

  /// Assign from a SmallVector.
  void assign(const SmallVectorImpl<char> &RHS) {
    this->clear();
    SmallVectorImpl<char>::append(RHS.begin(), RHS.end());
  }

  /// @}
  /// @name String Concatenation
  /// @{

  /// Append from an iterator pair.
  template<typename in_iter>
  void append(in_iter S, in_iter E) {
    SmallVectorImpl<char>::append(S, E);
  }

  void append(size_t NumInputs, char Elt) {
    SmallVectorImpl<char>::append(NumInputs, Elt);
  }

  /// Append from a std::string_view.
  void append(std::string_view RHS) {
    SmallVectorImpl<char>::append(RHS.begin(), RHS.end());
  }

  /// Append from a SmallVector.
  void append(const SmallVectorImpl<char> &RHS) {
    SmallVectorImpl<char>::append(RHS.begin(), RHS.end());
  }

  /// @}
  /// @name String Comparison
  /// @{

  /// Compare two strings; the result is -1, 0, or 1 if this string is
  /// lexicographically less than, equal to, or greater than the \p RHS.
  int compare(std::string_view RHS) const {
    return str().compare(RHS);
  }

  /// @}
  /// @name String Searching
  /// @{

  /// find - Search for the first character \p C in the string.
  ///
  /// \return - The index of the first occurrence of \p C, or npos if not
  /// found.
  size_t find(char C, size_t From = 0) const {
    return str().find(C, From);
  }

  /// Search for the first string \p Str in the string.
  ///
  /// \returns The index of the first occurrence of \p Str, or npos if not
  /// found.
  size_t find(std::string_view Str, size_t From = 0) const {
    return str().find(Str, From);
  }

  /// Search for the last character \p C in the string.
  ///
  /// \returns The index of the last occurrence of \p C, or npos if not
  /// found.
  size_t rfind(char C, size_t From = std::string_view::npos) const {
    return str().rfind(C, From);
  }

  /// Search for the last string \p Str in the string.
  ///
  /// \returns The index of the last occurrence of \p Str, or npos if not
  /// found.
  size_t rfind(std::string_view Str) const {
    return str().rfind(Str);
  }

  /// Find the first character in the string that is \p C, or npos if not
  /// found. Same as find.
  size_t find_first_of(char C, size_t From = 0) const {
    return str().find_first_of(C, From);
  }

  /// Find the first character in the string that is in \p Chars, or npos if
  /// not found.
  ///
  /// Complexity: O(size() + Chars.size())
  size_t find_first_of(std::string_view Chars, size_t From = 0) const {
    return str().find_first_of(Chars, From);
  }

  /// Find the first character in the string that is not \p C or npos if not
  /// found.
  size_t find_first_not_of(char C, size_t From = 0) const {
    return str().find_first_not_of(C, From);
  }

  /// Find the first character in the string that is not in the string
  /// \p Chars, or npos if not found.
  ///
  /// Complexity: O(size() + Chars.size())
  size_t find_first_not_of(std::string_view Chars, size_t From = 0) const {
    return str().find_first_not_of(Chars, From);
  }

  /// Find the last character in the string that is \p C, or npos if not
  /// found.
  size_t find_last_of(char C, size_t From = std::string_view::npos) const {
    return str().find_last_of(C, From);
  }

  /// Find the last character in the string that is in \p C, or npos if not
  /// found.
  ///
  /// Complexity: O(size() + Chars.size())
  size_t find_last_of(
      std::string_view Chars, size_t From = std::string_view::npos) const {
    return str().find_last_of(Chars, From);
  }

  /// @}

  // Extra methods.

  /// Explicit conversion to std::string_view.
  std::string_view str() const { return {this->begin(), this->size()}; }

  /// Explicit conversion to std::string.
  std::string string() const { return {this->begin(), this->size()}; }

  // TODO: Make this const, if it's safe...
  const char* c_str() {
    this->push_back(0);
    this->pop_back();
    return this->data();
  }

  /// Implicit conversion to std::string_view.
  operator std::string_view() const { return str(); }

  /// Implicit conversion to std::string.
  operator std::string() const { return string(); }

  // Extra operators.
  const SmallString &operator=(std::string_view RHS) {
    this->clear();
    return *this += RHS;
  }

  SmallString &operator+=(std::string_view RHS) {
    this->append(RHS.begin(), RHS.end());
    return *this;
  }
  SmallString &operator+=(char C) {
    this->push_back(C);
    return *this;
  }
};

} // end namespace wpi

#endif // LLVM_ADT_SMALLSTRING_H
