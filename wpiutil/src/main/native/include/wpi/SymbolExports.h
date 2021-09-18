// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef _WIN32
#pragma warning(disable : 4251)

#ifdef WPILIB_EXPORTS
#ifdef __GNUC__
#define WPILIB_DLLEXPORT __attribute__((dllexport))
#else
#define WPILIB_DLLEXPORT __declspec(dllexport)
#endif

#else
#define WPILIB_DLLEXPORT
#endif

#else
#define WPILIB_DLLEXPORT __attribute__((visibility("default")))
#endif
