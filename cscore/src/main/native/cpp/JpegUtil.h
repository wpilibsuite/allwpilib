/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef ALLWPILIB_JPEGUTIL_H_
#define ALLWPILIB_JPEGUTIL_H_

#include <wpi/StringRef.h>

#include <string>

namespace wpi {
class raw_istream;
}  // namespace wpi

namespace cs {

bool IsJpeg(wpi::StringRef data);

bool GetJpegSize(wpi::StringRef data, int* width, int* height);

bool JpegNeedsDHT(const char* data, size_t* size, size_t* locSOF);

wpi::StringRef JpegGetDHT();

bool ReadJpeg(wpi::raw_istream& is, std::string& buf, int* width, int* height);

}  // namespace cs

#endif  // ALLWPILIB_JPEGUTIL_H_
