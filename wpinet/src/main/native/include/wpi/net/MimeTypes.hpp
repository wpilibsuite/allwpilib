// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_MIMETYPES_H_
#define WPINET_MIMETYPES_H_

#include <string_view>

namespace wpi {

std::string_view MimeTypeFromPath(std::string_view path);

}  // namespace wpi

#endif  // WPINET_MIMETYPES_H_
