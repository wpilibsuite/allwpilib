// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_USBUTIL_H_
#define CSCORE_USBUTIL_H_

#include <stdint.h>

#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>

namespace cs {

wpi::StringRef GetUsbNameFromId(int vendor, int product,
                                wpi::SmallVectorImpl<char>& buf);

int CheckedIoctl(int fd, unsigned long req, void* data,  // NOLINT(runtime/int)
                 const char* name, const char* file, int line, bool quiet);

#define DoIoctl(fd, req, data) \
  CheckedIoctl(fd, req, data, #req, __FILE__, __LINE__, false)
#define TryIoctl(fd, req, data) \
  CheckedIoctl(fd, req, data, #req, __FILE__, __LINE__, true)

}  // namespace cs

#endif  // CSCORE_USBUTIL_H_
