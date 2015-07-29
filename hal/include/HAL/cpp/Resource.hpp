/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "../Errors.hpp"
#include "HAL/cpp/priority_mutex.h"
#include <stdint.h>

#include <vector>

// TODO: Replace this with something appropriate to avoid conflicts with
// wpilibC++ Resource class (which performs an essentially identical function).
namespace hal {

/**
 * The Resource class is a convenient way to track allocated resources.
 * It tracks them as indicies in the range [0 .. elements - 1].
 * E.g. the library uses this to track hardware channel allocation.
 *
 * The Resource class does not allocate the hardware channels or other
 * resources; it just tracks which indices were marked in use by
 * Allocate and not yet freed by Free.
 */
class Resource
{
public:
	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;
	explicit Resource(uint32_t size);
	virtual ~Resource() = default;
	static void CreateResourceObject(Resource **r, uint32_t elements);
	uint32_t Allocate(const char *resourceDesc);
	uint32_t Allocate(uint32_t index, const char *resourceDesc);
	void Free(uint32_t index);

private:
	std::vector<bool> m_isAllocated;
	priority_recursive_mutex m_allocateLock;

	static priority_recursive_mutex m_createLock;
};

}  // namespace hal
