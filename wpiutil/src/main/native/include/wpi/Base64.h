// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_BASE64_H_
#define WPIUTIL_WPI_BASE64_H_

#include <cstddef>
#include <string>

#include "wpi/StringRef.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
class raw_ostream;

size_t Base64Decode(raw_ostream& os, StringRef encoded);

size_t Base64Decode(StringRef encoded, std::string* plain);

StringRef Base64Decode(StringRef encoded, size_t* num_read,
                       SmallVectorImpl<char>& buf);

void Base64Encode(raw_ostream& os, StringRef plain);

void Base64Encode(StringRef plain, std::string* encoded);

StringRef Base64Encode(StringRef plain, SmallVectorImpl<char>& buf);

}  // namespace wpi

#endif  // WPIUTIL_WPI_BASE64_H_
