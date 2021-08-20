#pragma once

#ifdef _WIN32
#ifdef WPILIB_BUILDING
#define WPIMATH_DLLEXPORT __declspec(dllexport)
#else
#define WPIMATH_DLLEXPORT
#endif
#else
#define WPIMATH_DLLEXPORT
#endif
