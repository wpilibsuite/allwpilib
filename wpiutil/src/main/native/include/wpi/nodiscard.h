// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_NODISCARD_H_
#define WPIUTIL_WPI_NODISCARD_H_

// This should only be used on APIs that can be compiled as C or C++. If the API
// is C++ only, use [[nodiscard]] instead.
#ifndef WPI_NODISCARD
#ifdef __cplusplus
#define WPI_NODISCARD [[nodiscard]]
#elif defined(__GNUC__) || defined(__llvm__)
#define WPI_NODISCARD __attribute__((warn_unused_result))
#elif _MSC_VER
#define WPI_NODISCARD _Check_return_
#endif

#endif

#endif  // WPIUTIL_WPI_NODISCARD_H_
