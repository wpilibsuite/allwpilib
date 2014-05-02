/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "../Errors.hpp"
#include "Synchronized.hpp"
#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

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
	virtual ~Resource();
	static void CreateResourceObject(Resource **r, uint32_t elements);
	uint32_t Allocate(const char *resourceDesc);
	uint32_t Allocate(uint32_t index, const char *resourceDesc);
	void Free(uint32_t index);

private:
	explicit Resource(uint32_t size);

	bool *m_isAllocated;
	ReentrantSemaphore m_allocateLock;
	uint32_t m_size;

	static ReentrantSemaphore m_createLock;

	DISALLOW_COPY_AND_ASSIGN(Resource);
};

