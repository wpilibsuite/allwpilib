/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_HOSTNAME_H_
#define WPIUTIL_SUPPORT_HOSTNAME_H_

#include <string>

#include "llvm/StringRef.h"

namespace llvm {
template <typename T>
class SmallVectorImpl;
}  // namespace llvm

namespace wpi {
std::string GetHostname();
llvm::StringRef GetHostname(llvm::SmallVectorImpl<char>& name);
}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_HOSTNAME_H_
