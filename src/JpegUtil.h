/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_JPEGUTIL_H_
#define CS_JPEGUTIL_H_

#include "llvm/StringRef.h"

namespace wpi {
class raw_istream;
}

namespace cs {

bool IsJpeg(llvm::StringRef data);

bool GetJpegSize(llvm::StringRef data, int* width, int* height);

bool JpegNeedsDHT(const char* data, std::size_t* size, std::size_t* locSOF);

llvm::StringRef JpegGetDHT();

bool ReadJpeg(wpi::raw_istream& is, std::string& buf, int* width, int* height);

}  // namespace cs

#endif  // CS_JPEGUTIL_H_
