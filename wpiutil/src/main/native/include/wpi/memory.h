/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_MEMORY_H_
#define WPIUTIL_WPI_MEMORY_H_

#include <cstdlib>

namespace wpi {

/**
 * Wrapper around std::calloc that calls std::terminate on out of memory.
 * @param num number of objects to allocate
 * @param size number of bytes per object to allocate
 * @return Pointer to beginning of newly allocated memory.
 */
void* CheckedCalloc(size_t num, size_t size);

/**
 * Wrapper around std::malloc that calls std::terminate on out of memory.
 * @param size number of bytes to allocate
 * @return Pointer to beginning of newly allocated memory.
 */
void* CheckedMalloc(size_t size);

/**
 * Wrapper around std::realloc that calls std::terminate on out of memory.
 * @param ptr memory previously allocated
 * @param size number of bytes to allocate
 * @return Pointer to beginning of newly allocated memory.
 */
void* CheckedRealloc(void* ptr, size_t size);

}  // namespace wpi

#endif  // WPIUTIL_WPI_MEMORY_H_
