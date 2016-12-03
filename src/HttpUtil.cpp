/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HttpUtil.h"

#include <cctype>

#include "support/raw_istream.h"
#include "llvm/StringExtras.h"

namespace cs {

llvm::StringRef ReadLine(wpi::raw_istream& is, llvm::SmallVectorImpl<char>& buf,
                         int maxLen, bool* error) {
  buf.clear();
  for (int i = 0; i < maxLen; ++i) {
    char c;
    is.read(c);
    if (is.has_error()) {
      *error = true;
      return llvm::StringRef{buf.data(), buf.size()};
    }
    if (c == '\r') continue;
    buf.push_back(c);
    if (c == '\n') break;
  }
  *error = false;
  return llvm::StringRef{buf.data(), buf.size()};
}

llvm::StringRef UnescapeURI(llvm::StringRef str,
                            llvm::SmallVectorImpl<char>& buf, bool* error) {
  buf.clear();
  for (auto i = str.begin(), end = str.end(); i != end; ++i) {
    // pass non-escaped characters to output
    if (*i != '%') {
      // decode + to space
      if (*i == '+')
        buf.push_back(' ');
      else
        buf.push_back(*i);
      continue;
    }

    // are there enough characters left?
    if (i + 2 >= end) {
      *error = true;
      return llvm::StringRef{};
    }

    // replace %xx with the corresponding character
    unsigned val1 = llvm::hexDigitValue(*++i);
    if (val1 == -1U) {
      *error = true;
      return llvm::StringRef{};
    }
    unsigned val2 = llvm::hexDigitValue(*++i);
    if (val2 == -1U) {
      *error = true;
      return llvm::StringRef{};
    }
    buf.push_back((val1 << 4) | val2);
  }

  *error = false;
  return llvm::StringRef{buf.data(), buf.size()};
}

llvm::StringRef EscapeURI(llvm::StringRef str, llvm::SmallVectorImpl<char>& buf,
                          bool spacePlus) {
  static const char *const hexLut = "0123456789ABCDEF";

  buf.clear();
  for (auto i = str.begin(), end = str.end(); i != end; ++i) {
    // pass unreserved characters to output
    if (std::isalnum(*i) || *i == '-' || *i == '_' || *i == '.' || *i == '~') {
      buf.push_back(*i);
      continue;
    }

    // encode space to +
    if (spacePlus && *i == ' ') {
      buf.push_back('+');
      continue;
    }

    // convert others to %xx
    buf.push_back('%');
    buf.push_back(hexLut[((*i) >> 4) & 0x0f]);
    buf.push_back(hexLut[(*i) & 0x0f]);
  }

  return llvm::StringRef{buf.data(), buf.size()};
}

}  // namespace cs
