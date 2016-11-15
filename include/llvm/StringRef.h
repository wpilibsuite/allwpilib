//===--- StringRef.h - Constant String Reference Wrapper --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_STRINGREF_H
#define LLVM_ADT_STRINGREF_H

#include "llvm/iterator_range.h"
#include "llvm/Compiler.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <ostream>
#include <string>
#include <utility>

namespace llvm {
  template <typename T>
  class SmallVectorImpl;
  class hash_code;
  class StringRef;

  /// Helper functions for StringRef::getAsInteger.
  bool getAsUnsignedInteger(StringRef Str, unsigned Radix,
                            unsigned long long &Result);

  bool getAsSignedInteger(StringRef Str, unsigned Radix, long long &Result);

  /// StringRef - Represent a constant reference to a string, i.e. a character
  /// array and a length, which need not be null terminated.
  ///
  /// This class does not own the string data, it is expected to be used in
  /// situations where the character data resides in some other buffer, whose
  /// lifetime extends past that of the StringRef. For this reason, it is not in
  /// general safe to store a StringRef.
  class StringRef {
  public:
    typedef const char *iterator;
    typedef const char *const_iterator;
    static const size_t npos = ~size_t(0);
    typedef size_t size_type;

  private:
    /// The start of the string, in an external buffer.
    const char *Data;

    /// The length of the string.
    /// MSB of length indicates if we are null terminated or not
    /// Flag set is null terminated, flag not set is not
    size_t Length;

    // Workaround memcmp issue with null pointers (undefined behavior)
    // by providing a specialized version
    static int compareMemory(const char *Lhs, const char *Rhs, size_t Length) {
      if (Length == 0) { return 0; }
      return ::memcmp(Lhs,Rhs,Length);
    }

    /// Set the flag to say we are null terminated
    void set_null_terminated(bool set) {
      if (set)
        Length |= ((size_t)1 << (sizeof(size_t) * 8 - 1));
      else {
        Length &= ~((size_t)1 << (sizeof(size_t) * 8 - 1));
      }
    }

  public:
    /// @name Constructors
    /// @{

    /// Construct an empty string ref.
    /*implicit*/ StringRef() : Data(nullptr), Length(0) {}

    /// Construct a string ref from a cstring.
    /*implicit*/ StringRef(const char *Str)
      : Data(Str) {
        assert(Str && "StringRef cannot be built from a NULL argument");
        Length = ::strlen(Str); // invoking strlen(NULL) is undefined behavior
        // Require length to not use MSB of size
        assert(Length < ~((size_t)1 << (sizeof(size_t) * 8 - 1)));
        // If from a const char*, we are null terminated
        set_null_terminated(true);
      }

    /// Construct a string ref from a pointer and length.
    /*implicit*/ StringRef(const char *data, size_t length)
      : Data(data), Length(length) {
        assert((data || length == 0) &&
        "StringRef cannot be built from a NULL argument with non-null length");
        // Require length to not use MSB of size
        assert(Length < ~((size_t)1 << (sizeof(size_t) * 8 - 1)));
        // If passed an explicit length, we are not null terminated
        set_null_terminated(false);
      }

    /// Construct a string ref from an std::string.
    /*implicit*/ StringRef(const std::string &Str)
    : Data(Str.data()), Length(Str.length()) {
        // Require length to not use MSB of size
        assert(Length < ~((size_t)1 << (sizeof(size_t) * 8 - 1)));
        // If from a std::string, we are null terminated
        set_null_terminated(true);
      }

    /// @}
    /// @name Iterators
    /// @{

    iterator begin() const { return Data; }

    iterator end() const { return Data + size(); }

    const unsigned char *bytes_begin() const {
      return reinterpret_cast<const unsigned char *>(begin());
    }
    const unsigned char *bytes_end() const {
      return reinterpret_cast<const unsigned char *>(end());
    }
    iterator_range<const unsigned char *> bytes() const {
      return make_range(bytes_begin(), bytes_end());
    }

    /// @}
    /// @name String Operations
    /// @{

    /// data - Get a pointer to the start of the string (which may not be null
    /// terminated).
    const char *data() const { return Data; }

    /// c_str - Get a null terminated pointer to the start of the string
    /// If string is not null terminated, use buffer to store new string
    const char *c_str(llvm::SmallVectorImpl<char>& buf) const;

    /// empty - Check if the string is empty.
    bool empty() const { return size() == 0; }

    /// size - Get the string size.
    size_t size() const { 
      return Length & ~((size_t)1 << (sizeof(size_t) * 8 - 1)); 
    }

    /// is_null_terminated - Get if the string is guaranteed null terminated
    bool is_null_terminated() const {
      return (Length & ((size_t)1 << (sizeof(size_t) * 8 - 1))) ==
             ((size_t)1 << (sizeof(size_t) * 8 - 1));
    }

    /// front - Get the first character in the string.
    char front() const {
      assert(!empty());
      return Data[0];
    }

    /// back - Get the last character in the string.
    char back() const {
      assert(!empty());
      return Data[size()-1];
    }

    // copy - Allocate copy in Allocator and return StringRef to it.
    template <typename Allocator> StringRef copy(Allocator &A) const {
      // Don't request a length 0 copy from the allocator.
      if (empty())
        return StringRef();
      char *S = A.template Allocate<char>(size());
      std::copy(begin(), end(), S);
      return StringRef(S, size());
    }

    /// equals - Check for string equality, this is more efficient than
    /// compare() when the relative ordering of inequal strings isn't needed.
    bool equals(StringRef RHS) const {
      return (size() == RHS.size() &&
              compareMemory(Data, RHS.Data, RHS.size()) == 0);
    }

    /// equals_lower - Check for string equality, ignoring case.
    bool equals_lower(StringRef RHS) const {
      return size() == RHS.size() && compare_lower(RHS) == 0;
    }

    /// compare - Compare two strings; the result is -1, 0, or 1 if this string
    /// is lexicographically less than, equal to, or greater than the \p RHS.
    int compare(StringRef RHS) const {
      // Check the prefix for a mismatch.
      if (int Res = compareMemory(Data, RHS.Data, std::min(size(), RHS.size())))
        return Res < 0 ? -1 : 1;

      // Otherwise the prefixes match, so we only need to check the lengths.
      if (size() == RHS.size())
        return 0;
      return size() < RHS.size() ? -1 : 1;
    }

    /// compare_lower - Compare two strings, ignoring case.
    int compare_lower(StringRef RHS) const;

    /// compare_numeric - Compare two strings, treating sequences of digits as
    /// numbers.
    int compare_numeric(StringRef RHS) const;

    /// str - Get the contents as an std::string.
    std::string str() const {
      if (!Data) return std::string();
      return std::string(Data, size());
    }

    /// @}
    /// @name Operator Overloads
    /// @{

    char operator[](size_t Index) const {
      assert(Index < size() && "Invalid index!");
      return Data[Index];
    }

    /// @}
    /// @name Type Conversions
    /// @{

    operator std::string() const {
      return str();
    }

    /// @}
    /// @name String Predicates
    /// @{

    /// Check if this string starts with the given \p Prefix.
    bool startswith(StringRef Prefix) const {
      return size() >= Prefix.size() &&
             compareMemory(Data, Prefix.Data, Prefix.size()) == 0;
    }

    /// Check if this string starts with the given \p Prefix, ignoring case.
    bool startswith_lower(StringRef Prefix) const;

    /// Check if this string ends with the given \p Suffix.
    bool endswith(StringRef Suffix) const {
      return size() >= Suffix.size() &&
        compareMemory(end() - Suffix.size(), Suffix.Data, Suffix.size()) == 0;
    }

    /// Check if this string ends with the given \p Suffix, ignoring case.
    bool endswith_lower(StringRef Suffix) const;

    /// @}
    /// @name String Searching
    /// @{

    /// Search for the first character \p C in the string.
    ///
    /// \returns The index of the first occurrence of \p C, or npos if not
    /// found.
    size_t find(char C, size_t From = 0) const {
      size_t FindBegin = std::min(From, size());
      if (FindBegin < size()) { // Avoid calling memchr with nullptr.
        // Just forward to memchr, which is faster than a hand-rolled loop.
        if (const void *P = ::memchr(Data + FindBegin, C, size() - FindBegin))
          return static_cast<const char *>(P) - Data;
      }
      return npos;
    }

    /// Search for the first string \p Str in the string.
    ///
    /// \returns The index of the first occurrence of \p Str, or npos if not
    /// found.
    size_t find(StringRef Str, size_t From = 0) const;

    /// Search for the last character \p C in the string.
    ///
    /// \returns The index of the last occurrence of \p C, or npos if not
    /// found.
    size_t rfind(char C, size_t From = npos) const {
      From = std::min(From, size());
      size_t i = From;
      while (i != 0) {
        --i;
        if (Data[i] == C)
          return i;
      }
      return npos;
    }

    /// Search for the last string \p Str in the string.
    ///
    /// \returns The index of the last occurrence of \p Str, or npos if not
    /// found.
    size_t rfind(StringRef Str) const;

    /// Find the first character in the string that is \p C, or npos if not
    /// found. Same as find.
    size_t find_first_of(char C, size_t From = 0) const {
      return find(C, From);
    }

    /// Find the first character in the string that is in \p Chars, or npos if
    /// not found.
    ///
    /// Complexity: O(size() + Chars.size())
    size_t find_first_of(StringRef Chars, size_t From = 0) const;

    /// Find the first character in the string that is not \p C or npos if not
    /// found.
    size_t find_first_not_of(char C, size_t From = 0) const;

    /// Find the first character in the string that is not in the string
    /// \p Chars, or npos if not found.
    ///
    /// Complexity: O(size() + Chars.size())
    size_t find_first_not_of(StringRef Chars, size_t From = 0) const;

    /// Find the last character in the string that is \p C, or npos if not
    /// found.
    size_t find_last_of(char C, size_t From = npos) const {
      return rfind(C, From);
    }

    /// Find the last character in the string that is in \p C, or npos if not
    /// found.
    ///
    /// Complexity: O(size() + Chars.size())
    size_t find_last_of(StringRef Chars, size_t From = npos) const;

    /// Find the last character in the string that is not \p C, or npos if not
    /// found.
    size_t find_last_not_of(char C, size_t From = npos) const;

    /// Find the last character in the string that is not in \p Chars, or
    /// npos if not found.
    ///
    /// Complexity: O(size() + Chars.size())
    size_t find_last_not_of(StringRef Chars, size_t From = npos) const;

    /// @}
    /// @name Helpful Algorithms
    /// @{

    /// Return the number of occurrences of \p C in the string.
    size_t count(char C) const {
      size_t Count = 0;
      for (size_t i = 0, e = size(); i != e; ++i)
        if (Data[i] == C)
          ++Count;
      return Count;
    }

    /// Return the number of non-overlapped occurrences of \p Str in
    /// the string.
    size_t count(StringRef Str) const;

    /// Parse the current string as an integer of the specified radix.  If
    /// \p Radix is specified as zero, this does radix autosensing using
    /// extended C rules: 0 is octal, 0x is hex, 0b is binary.
    ///
    /// If the string is invalid or if only a subset of the string is valid,
    /// this returns true to signify the error.  The string is considered
    /// erroneous if empty or if it overflows T.
    template <typename T>
    typename std::enable_if<std::numeric_limits<T>::is_signed, bool>::type
    getAsInteger(unsigned Radix, T &Result) const {
      long long LLVal;
      if (getAsSignedInteger(*this, Radix, LLVal) ||
            static_cast<T>(LLVal) != LLVal)
        return true;
      Result = LLVal;
      return false;
    }

    template <typename T>
    typename std::enable_if<!std::numeric_limits<T>::is_signed, bool>::type
    getAsInteger(unsigned Radix, T &Result) const {
      unsigned long long ULLVal;
      // The additional cast to unsigned long long is required to avoid the
      // Visual C++ warning C4805: '!=' : unsafe mix of type 'bool' and type
      // 'unsigned __int64' when instantiating getAsInteger with T = bool.
      if (getAsUnsignedInteger(*this, Radix, ULLVal) ||
          static_cast<unsigned long long>(static_cast<T>(ULLVal)) != ULLVal)
        return true;
      Result = ULLVal;
      return false;
    }

    /// @}
    /// @name String Operations
    /// @{

    // Convert the given ASCII string to lowercase.
    std::string lower() const;

    /// Convert the given ASCII string to uppercase.
    std::string upper() const;

    /// @}
    /// @name Substring Operations
    /// @{

    /// Return a reference to the substring from [Start, Start + N).
    ///
    /// \param Start The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param N The number of characters to included in the substring. If N
    /// exceeds the number of characters remaining in the string, the string
    /// suffix (starting with \p Start) will be returned.
    StringRef substr(size_t Start, size_t N = npos) const {
      Start = std::min(Start, size());
      return StringRef(Data + Start, std::min(N, size() - Start));
    }

    /// Return a StringRef equal to 'this' but with the first \p N elements
    /// dropped.
    StringRef drop_front(size_t N = 1) const {
      assert(size() >= N && "Dropping more elements than exist");
      return substr(N);
    }

    /// Return a StringRef equal to 'this' but with the last \p N elements
    /// dropped.
    StringRef drop_back(size_t N = 1) const {
      assert(size() >= N && "Dropping more elements than exist");
      return substr(0, size()-N);
    }

    /// Return a reference to the substring from [Start, End).
    ///
    /// \param Start The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param End The index following the last character to include in the
    /// substring. If this is npos or exceeds the number of characters
    /// remaining in the string, the string suffix (starting with \p Start)
    /// will be returned. If this is less than \p Start, an empty string will
    /// be returned.
    StringRef slice(size_t Start, size_t End) const {
      Start = std::min(Start, size());
      End = std::min(std::max(Start, End), size());
      return StringRef(Data + Start, End - Start);
    }

    /// Split into two substrings around the first occurrence of a separator
    /// character.
    ///
    /// If \p Separator is in the string, then the result is a pair (LHS, RHS)
    /// such that (*this == LHS + Separator + RHS) is true and RHS is
    /// maximal. If \p Separator is not in the string, then the result is a
    /// pair (LHS, RHS) where (*this == LHS) and (RHS == "").
    ///
    /// \param Separator The character to split on.
    /// \returns The split substrings.
    std::pair<StringRef, StringRef> split(char Separator) const {
      size_t Idx = find(Separator);
      if (Idx == npos)
        return std::make_pair(*this, StringRef());
      return std::make_pair(slice(0, Idx), slice(Idx+1, npos));
    }

    /// Split into two substrings around the first occurrence of a separator
    /// string.
    ///
    /// If \p Separator is in the string, then the result is a pair (LHS, RHS)
    /// such that (*this == LHS + Separator + RHS) is true and RHS is
    /// maximal. If \p Separator is not in the string, then the result is a
    /// pair (LHS, RHS) where (*this == LHS) and (RHS == "").
    ///
    /// \param Separator - The string to split on.
    /// \return - The split substrings.
    std::pair<StringRef, StringRef> split(StringRef Separator) const {
      size_t Idx = find(Separator);
      if (Idx == npos)
        return std::make_pair(*this, StringRef());
      return std::make_pair(slice(0, Idx), slice(Idx + Separator.size(), npos));
    }

    /// Split into substrings around the occurrences of a separator string.
    ///
    /// Each substring is stored in \p A. If \p MaxSplit is >= 0, at most
    /// \p MaxSplit splits are done and consequently <= \p MaxSplit + 1
    /// elements are added to A.
    /// If \p KeepEmpty is false, empty strings are not added to \p A. They
    /// still count when considering \p MaxSplit
    /// An useful invariant is that
    /// Separator.join(A) == *this if MaxSplit == -1 and KeepEmpty == true
    ///
    /// \param A - Where to put the substrings.
    /// \param Separator - The string to split on.
    /// \param MaxSplit - The maximum number of times the string is split.
    /// \param KeepEmpty - True if empty substring should be added.
    void split(SmallVectorImpl<StringRef> &A,
               StringRef Separator, int MaxSplit = -1,
               bool KeepEmpty = true) const;

    /// Split into substrings around the occurrences of a separator character.
    ///
    /// Each substring is stored in \p A. If \p MaxSplit is >= 0, at most
    /// \p MaxSplit splits are done and consequently <= \p MaxSplit + 1
    /// elements are added to A.
    /// If \p KeepEmpty is false, empty strings are not added to \p A. They
    /// still count when considering \p MaxSplit
    /// An useful invariant is that
    /// Separator.join(A) == *this if MaxSplit == -1 and KeepEmpty == true
    ///
    /// \param A - Where to put the substrings.
    /// \param Separator - The string to split on.
    /// \param MaxSplit - The maximum number of times the string is split.
    /// \param KeepEmpty - True if empty substring should be added.
    void split(SmallVectorImpl<StringRef> &A, char Separator, int MaxSplit = -1,
               bool KeepEmpty = true) const;

    /// Split into two substrings around the last occurrence of a separator
    /// character.
    ///
    /// If \p Separator is in the string, then the result is a pair (LHS, RHS)
    /// such that (*this == LHS + Separator + RHS) is true and RHS is
    /// minimal. If \p Separator is not in the string, then the result is a
    /// pair (LHS, RHS) where (*this == LHS) and (RHS == "").
    ///
    /// \param Separator - The character to split on.
    /// \return - The split substrings.
    std::pair<StringRef, StringRef> rsplit(char Separator) const {
      size_t Idx = rfind(Separator);
      if (Idx == npos)
        return std::make_pair(*this, StringRef());
      return std::make_pair(slice(0, Idx), slice(Idx+1, npos));
    }

    /// Return string with consecutive \p Char characters starting from the
    /// the left removed.
    StringRef ltrim(char Char) const {
      return drop_front(std::min(size(), find_first_not_of(Char)));
    }

    /// Return string with consecutive characters in \p Chars starting from
    /// the left removed.
    StringRef ltrim(StringRef Chars = " \t\n\v\f\r") const {
      return drop_front(std::min(size(), find_first_not_of(Chars)));
    }

    /// Return string with consecutive \p Char characters starting from the
    /// right removed.
    StringRef rtrim(char Char) const {
      return drop_back(size() - std::min(size(), find_last_not_of(Char) + 1));
    }

    /// Return string with consecutive characters in \p Chars starting from
    /// the right removed.
    StringRef rtrim(StringRef Chars = " \t\n\v\f\r") const {
      return drop_back(size() - std::min(size(), find_last_not_of(Chars) + 1));
    }

    /// Return string with consecutive \p Char characters starting from the
    /// left and right removed.
    StringRef trim(char Char) const {
      return ltrim(Char).rtrim(Char);
    }

    /// Return string with consecutive characters in \p Chars starting from
    /// the left and right removed.
    StringRef trim(StringRef Chars = " \t\n\v\f\r") const {
      return ltrim(Chars).rtrim(Chars);
    }

    /// @}
  };

  /// @name StringRef Comparison Operators
  /// @{

  inline bool operator==(StringRef LHS, StringRef RHS) {
    return LHS.equals(RHS);
  }

  inline bool operator!=(StringRef LHS, StringRef RHS) {
    return !(LHS == RHS);
  }

  inline bool operator<(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) == -1;
  }

  inline bool operator<=(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) != 1;
  }

  inline bool operator>(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) == 1;
  }

  inline bool operator>=(StringRef LHS, StringRef RHS) {
    return LHS.compare(RHS) != -1;
  }

  inline std::string &operator+=(std::string &buffer, StringRef string) {
    return buffer.append(string.data(), string.size());
  }

  inline std::ostream &operator<<(std::ostream &os, StringRef string) {
    os.write(string.data(), string.size());
    return os;
  }

  /// @}

  /// \brief Compute a hash_code for a StringRef.
  hash_code hash_value(StringRef S);

  // StringRefs can be treated like a POD type.
  template <typename T> struct isPodLike;
  template <> struct isPodLike<StringRef> { static const bool value = true; };
} // namespace llvm

#endif
