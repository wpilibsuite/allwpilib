/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_USBUTIL_H_
#define CS_USBUTIL_H_

#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"

namespace cs {

#ifdef __linux__

llvm::StringRef GetUsbNameFromId(int vendor, int product,
                                 llvm::SmallVectorImpl<char>& buf);

int CheckedIoctl(int fd, unsigned long req, void* data, const char* name,
                 const char* file, int line, bool quiet);

#define DoIoctl(fd, req, data) \
  CheckedIoctl(fd, req, data, #req, __FILE__, __LINE__, false)
#define TryIoctl(fd, req, data) \
  CheckedIoctl(fd, req, data, #req, __FILE__, __LINE__, true)

#endif  // __linux__

}  // namespace cs

#endif  // CS_USBUTIL_H_
