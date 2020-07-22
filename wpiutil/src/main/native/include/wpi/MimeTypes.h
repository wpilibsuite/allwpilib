/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_MIMETYPES_H_
#define WPIUTIL_WPI_MIMETYPES_H_

#include "wpi/StringRef.h"

namespace wpi {

StringRef MimeTypeFromPath(StringRef path);

}  // namespace wpi

#endif  // WPIUTIL_WPI_MIMETYPES_H_
