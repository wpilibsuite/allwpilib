#pragma once

#ifdef _WIN32
#ifdef WPILIB_BUILDING
#define WPIMATH_DLLEXPORT __declspec(dllexport)
#pragma warning(disable : 4251)
#else
#define WPIMATH_DLLEXPORT
#endif
#else
#define WPIMATH_DLLEXPORT
#endif
