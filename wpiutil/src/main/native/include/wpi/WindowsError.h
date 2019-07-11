//===-- WindowsError.h - Support for mapping windows errors to posix-------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_WINDOWSERROR_H
#define WPIUTIL_WPI_WINDOWSERROR_H

#include <system_error>

namespace wpi {
std::error_code mapWindowsError(unsigned EV);
}

#endif
