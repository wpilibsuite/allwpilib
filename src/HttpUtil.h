/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_HTTPUTIL_H_
#define CS_HTTPUTIL_H_

#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"

namespace wpi {
class raw_istream;
}

namespace cs {

// Read a line from an input stream (up to a maximum length).
// The returned buffer will contain the trailing \n (unless the maximum length
// was reached).  \r's are stripped from the buffer.
// @param buf Buffer for output
// @param error Set to true if an error occurred
// @return Line
llvm::StringRef ReadLine(wpi::raw_istream& is, llvm::SmallVectorImpl<char>& buf,
                         int maxLen, bool* error);

// Unescape a %xx-encoded URI.
// @param buf Buffer for output
// @param error Set to true if an error occurred
// @return Escaped string
llvm::StringRef UnescapeURI(llvm::StringRef str,
                            llvm::SmallVectorImpl<char>& buf, bool* error);

// Escape a string with %xx-encoding.
// @param buf Buffer for output
// @param spacePlus If true, encodes spaces to '+' rather than "%20"
// @return Escaped string
llvm::StringRef EscapeURI(llvm::StringRef str, llvm::SmallVectorImpl<char>& buf,
                          bool spacePlus = true);

}  // namespace cs

#endif  // CS_HTTPUTIL_H_
