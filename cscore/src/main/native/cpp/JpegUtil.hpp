// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

namespace wpi::util {
class raw_istream;
}  // namespace wpi::util

namespace wpi::cs {

bool IsJpeg(std::string_view data);

bool GetJpegSize(std::string_view data, int* width, int* height);

bool JpegNeedsDHT(const char* data, size_t* size, size_t* locSOF);

std::string_view JpegGetDHT();

bool ReadJpeg(wpi::util::raw_istream& is, std::string& buf, int* width,
              int* height);

}  // namespace wpi::cs
