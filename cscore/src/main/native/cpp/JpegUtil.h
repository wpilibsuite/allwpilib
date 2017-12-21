/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_JPEGUTIL_H_
#define CSCORE_JPEGUTIL_H_

#include <string>

#include <llvm/StringRef.h>

namespace wpi {
class raw_istream;
}  // namespace wpi

namespace cs {

bool IsJpeg(llvm::StringRef data);

bool GetJpegSize(llvm::StringRef data, int* width, int* height);

bool JpegNeedsDHT(const char* data, size_t* size, size_t* locSOF);

llvm::StringRef JpegGetDHT();

bool ReadJpeg(wpi::raw_istream& is, std::string& buf, int* width, int* height);

}  // namespace cs

#endif  // CSCORE_JPEGUTIL_H_
