/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef DEPRECATED_H_
#define DEPRECATED_H_

// [[deprecated(msg)]] is a C++14 feature not supported by MSVC or GCC < 4.9.
// We provide an equivalent warning implementation for those compilers here.
#ifndef WPI_DEPRECATED
  #if defined(_MSC_VER)
    #define WPI_DEPRECATED(msg) __declspec(deprecated(msg))
  #elif defined(__GNUC__)
    #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 8)
      #if __cplusplus > 201103L
        #define WPI_DEPRECATED(msg) [[deprecated(msg)]]
      #else
        #define WPI_DEPRECATED(msg) [[gnu::deprecated(msg)]]
      #endif
    #else
      #define WPI_DEPRECATED(msg) __attribute__((deprecated(msg)))
    #endif
  #elif __cplusplus > 201103L
    #define WPI_DEPRECATED(msg) [[deprecated(msg)]]
  #else
    #define WPI_DEPRECATED(msg) /*nothing*/
  #endif
#endif

#endif  // DEPRECATED_H_
