// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_DEPRECATED_H_
#define WPIUTIL_WPI_DEPRECATED_H_

#ifndef WPI_IGNORE_DEPRECATED
#ifdef __GNUC__
#define WPI_IGNORE_DEPRECATED    \
  _Pragma("GCC diagnostic push") \
      _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif defined(_WIN32)
#define WPI_IGNORE_DEPRECATED _Pragma("warning(disable : 4996)")
#else
#define WPI_IGNORE_DEPRECATED
#endif

#endif

#ifndef WPI_UNIGNORE_DEPRECATED
#ifdef __GNUC__
#define WPI_UNIGNORE_DEPRECATED _Pragma("GCC diagnostic pop")
#elif defined(_WIN32)
#define WPI_UNIGNORE_DEPRECATED _Pragma("warning(default : 4996)")
#else
#define WPI_UNIGNORE_DEPRECATED
#endif
#endif

#endif  // WPIUTIL_WPI_DEPRECATED_H_
