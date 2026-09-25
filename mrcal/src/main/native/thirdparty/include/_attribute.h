#pragma once

// To support compilers that don't have __attribute__. In particular MSVC needs
// this

#ifdef _MSC_VER
#define MRCAL_ATTRIBUTE(x)
#else
#define MRCAL_ATTRIBUTE(x) __attribute__(x)
#endif
