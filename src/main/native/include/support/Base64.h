/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_BASE64_H_
#define WPIUTIL_SUPPORT_BASE64_H_

#include <cstddef>
#include <string>

#include "llvm/StringRef.h"

namespace llvm {
template <typename T>
class SmallVectorImpl;
class raw_ostream;
}

namespace wpi {

std::size_t Base64Decode(llvm::raw_ostream& os, llvm::StringRef encoded);

std::size_t Base64Decode(llvm::StringRef encoded, std::string* plain);

llvm::StringRef Base64Decode(llvm::StringRef encoded, std::size_t* num_read,
                             llvm::SmallVectorImpl<char>& buf);

void Base64Encode(llvm::raw_ostream& os, llvm::StringRef plain);

void Base64Encode(llvm::StringRef plain, std::string* encoded);

llvm::StringRef Base64Encode(llvm::StringRef plain,
                             llvm::SmallVectorImpl<char>& buf);

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_BASE64_H_
