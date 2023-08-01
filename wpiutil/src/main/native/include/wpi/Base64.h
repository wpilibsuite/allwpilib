// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_BASE64_H_
#define WPIUTIL_WPI_BASE64_H_

#include <stdint.h>

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace wpi {
template <typename T>
class SmallVectorImpl;
class raw_ostream;

size_t Base64Decode(raw_ostream& os, std::string_view encoded);

size_t Base64Decode(std::string_view encoded, std::string* plain);

std::string_view Base64Decode(std::string_view encoded, size_t* num_read,
                              SmallVectorImpl<char>& buf);

size_t Base64Decode(std::string_view encoded, std::vector<uint8_t>* plain);

std::span<uint8_t> Base64Decode(std::string_view encoded, size_t* num_read,
                                SmallVectorImpl<uint8_t>& buf);

void Base64Encode(raw_ostream& os, std::string_view plain);

void Base64Encode(std::string_view plain, std::string* encoded);

std::string_view Base64Encode(std::string_view plain,
                              SmallVectorImpl<char>& buf);

void Base64Encode(raw_ostream& os, std::span<const uint8_t> plain);

void Base64Encode(std::span<const uint8_t> plain, std::string* encoded);

std::string_view Base64Encode(std::span<const uint8_t> plain,
                              SmallVectorImpl<char>& buf);

}  // namespace wpi

#endif  // WPIUTIL_WPI_BASE64_H_
