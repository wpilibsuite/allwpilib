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
class raw_ostream;

size_t Base64Decode(raw_ostream& os, std::string_view encoded);

std::string Base64Decode(std::string_view encoded, size_t* num_read);

std::vector<uint8_t> Base64DecodeUnsigned(std::string_view encoded,
                                          size_t* num_read);

void Base64Encode(raw_ostream& os, std::string_view plain);

std::string Base64Encode(std::string_view plain);

void Base64Encode(raw_ostream& os, std::span<const uint8_t> plain);

std::string Base64Encode(std::span<const uint8_t> plain);

}  // namespace wpi

#endif  // WPIUTIL_WPI_BASE64_H_
