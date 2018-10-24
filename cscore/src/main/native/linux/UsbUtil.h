/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
