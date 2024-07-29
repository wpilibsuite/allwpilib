// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_JPEGUTIL_H_
#define CSCORE_JPEGUTIL_H_

#include <string>
#include <string_view>

namespace wpi {
class raw_istream;
}  // namespace wpi

namespace cs {

bool IsJpeg(std::string_view data);

bool GetJpegSize(std::string_view data, int* width, int* height);

bool JpegNeedsDHT(const char* data, size_t* size, size_t* locSOF);

std::string_view JpegGetDHT();

bool ReadJpeg(wpi::raw_istream& is, std::string& buf, int* width, int* height);

}  // namespace cs

#endif  // CSCORE_JPEGUTIL_H_
