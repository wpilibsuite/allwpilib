//===- llvm/ADT/SmallString.h - 'Normally small' strings --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the SmallString class.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_SMALLSTRING_H
#define WPIUTIL_WPI_SMALLSTRING_H

#include "wpi/SmallVector.h"
#include <string_view>
#include <cstddef>

namespace wpi {

/// SmallString - A SmallString is just a SmallVector with methods and accessors
/// that make it work better as a string (e.g. operator+ etc).
template<unsigned InternalLen>
class SmallString : public SmallVector<char, InternalLen> {
public:
  /// Default ctor - Initialize to empty.
  SmallString() = default;

  /// Initialize from a std::string_view.
  SmallString(std::string_view S) : SmallVector<char, InternalLen>(S.begin(), S.end()) {}

  /// Initialize by concatenating a list of StringRefs.
  SmallString(std::initializer_list<std::string_view> Refs)
      : SmallVector<char, InternalLen>() {
    this->append(Refs);
  }

  /// Initialize with a range.
  template<typename ItTy>
  SmallString(ItTy S, ItTy E) : SmallVector<char, InternalLen>(S, E) {}

  /// @}
  /// @name String Assignment
  /// @{

  using SmallVector<char, InternalLen>::assign;

  /// Assign from a std::string_view.
  void assign(std::string_view RHS) {
    SmallVectorImpl<char>::assign(RHS.begin(), RHS.end());
  }

  /// Assign from a list of StringRefs.
  void assign(std::initializer_list<std::string_view> Refs) {
    this->clear();
    append(Refs);
  }

  /// @}
  /// @name String Concatenation
  /// @{

  using SmallVector<char, InternalLen>::append;

  /// Append from a std::string_view.
  void append(std::string_view RHS) {
    SmallVectorImpl<char>::append(RHS.begin(), RHS.end());
  }

  /// Append from a list of StringRefs.
  void append(std::initializer_list<std::string_view> Refs) {
    size_t SizeNeeded = this->size();
    for (const std::string_view &Ref : Refs)
      SizeNeeded += Ref.size();
    this->reserve(SizeNeeded);
    auto CurEnd = this->end();
    for (const std::string_view &Ref : Refs) {
      this->uninitialized_copy(Ref.begin(), Ref.end(), CurEnd);
      CurEnd += Ref.size();
    }
    this->set_size(SizeNeeded);
  }

  /// @}
  /// @name String Comparison
  /// @{

  /// Check for string equality.  This is more efficient than compare() when
  /// the relative ordering of inequal strings isn't needed.
  bool equals(std::string_view RHS) const {
    return str().equals(RHS);
  }

  /// Check for string equality, ignoring case.
  bool equals_insensitive(std::string_view RHS) const {
    return str().equals_insensitive(RHS);
  }

  /// Compare two strings; the result is -1, 0, or 1 if this string is
  /// lexicographically less than, equal to, or greater than the \p RHS.
  int compare(std::string_view RHS) const {
    return str().compare(RHS);
  }

  /// compare_insensitive - Compare two strings, ignoring case.
  int compare_insensitive(std::string_view RHS) const {
    return str().compare_insensitive(RHS);
  }

  /// compare_numeric - Compare two strings, treating sequences of digits as
  /// numbers.
  int compare_numeric(std::string_view RHS) const {
    return str().compare_numeric(RHS);
  }

  /// @}
  /// @name String Predicates
  /// @{

  /// startswith - Check if this string starts with the given \p Prefix.
  bool startswith(std::string_view Prefix) const {
    return str().startswith(Prefix);
  }

  /// endswith - Check if this string ends with the given \p Suffix.
  bool endswith(std::string_view Suffix) const {
    return str().endswith(Suffix);
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
  /// @name Helpful Algorithms
  /// @{

  /// Return the number of occurrences of \p C in the string.
  size_t count(char C) const {
    return str().count(C);
  }

  /// Return the number of non-overlapped occurrences of \p Str in the
  /// string.
  size_t count(std::string_view Str) const {
    return str().count(Str);
  }

  /// @}
  /// @name Substring Operations
  /// @{

  /// Return a reference to the substring from [Start, Start + N).
  ///
  /// \param Start The index of the starting character in the substring; if
  /// the index is npos or greater than the length of the string then the
  /// empty substring will be returned.
  ///
  /// \param N The number of characters to included in the substring. If \p N
  /// exceeds the number of characters remaining in the string, the string
  /// suffix (starting with \p Start) will be returned.
  std::string_view substr(size_t Start, size_t N = std::string_view::npos) const {
    return str().substr(Start, N);
  }

  /// Return a reference to the substring from [Start, End).
  ///
  /// \param Start The index of the starting character in the substring; if
  /// the index is npos or greater than the length of the string then the
  /// empty substring will be returned.
  ///
  /// \param End The index following the last character to include in the
  /// substring. If this is npos, or less than \p Start, or exceeds the
  /// number of characters remaining in the string, the string suffix
  /// (starting with \p Start) will be returned.
  std::string_view slice(size_t Start, size_t End) const {
    return str().slice(Start, End);
  }

  // Extra methods.

  /// Explicit conversion to std::string_view.
  std::string_view str() const { return std::string_view(this->data(), this->size()); }

  // TODO: Make this const, if it's safe...
  const char* c_str() {
    this->push_back(0);
    this->pop_back();
    return this->data();
  }

  /// Implicit conversion to std::string_view.
  operator std::string_view() const { return str(); }

  explicit operator std::string() const {
    return std::string(this->data(), this->size());
  }

  // Extra operators.
  SmallString &operator=(std::string_view RHS) {
    this->assign(RHS);
    return *this;
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

#endif // WPIUTIL_WPI_SMALLSTRING_H
