/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/cpp/Resource.hpp"
#include "HAL/Errors.hpp"
#include <cstddef>
#include "HAL/cpp/priority_mutex.h"

namespace hal {

priority_recursive_mutex Resource::m_createLock;

/**
 * Allocate storage for a new instance of Resource.
 * Allocate a bool array of values that will get initialized to indicate that no resources
 * have been allocated yet. The indicies of the resources are [0 .. elements - 1].
 */
Resource::Resource(uint32_t elements) {
  std::lock_guard<priority_recursive_mutex> sync(m_createLock);
  m_isAllocated = std::vector<bool>(elements, false);
}

/**
 * Factory method to create a Resource allocation-tracker *if* needed.
 *
 * @param r -- address of the caller's Resource pointer. If *r == NULL, this
 *    will construct a Resource and make *r point to it. If *r != NULL, i.e.
 *    the caller already has a Resource instance, this won't do anything.
 * @param elements -- the number of elements for this Resource allocator to
 *    track, that is, it will allocate resource numbers in the range
 *    [0 .. elements - 1].
 */
/*static*/ void Resource::CreateResourceObject(Resource **r, uint32_t elements)
{
	std::lock_guard<priority_recursive_mutex> sync(m_createLock);
	if (*r == NULL)
	{
		*r = new Resource(elements);
	}
}

/**
 * Allocate a resource.
 * When a resource is requested, mark it allocated. In this case, a free resource value
 * within the range is located and returned after it is marked allocated.
 */
uint32_t Resource::Allocate(const char *resourceDesc)
{
	std::lock_guard<priority_recursive_mutex> sync(m_allocateLock);
	for (uint32_t i=0; i < m_isAllocated.size(); i++)
	{
		if (!m_isAllocated[i])
		{
			m_isAllocated[i] = true;
			return i;
		}
	}
	// TODO: wpi_setWPIErrorWithContext(NoAvailableResources, resourceDesc);
	return ~0ul;
}

/**
 * Allocate a specific resource value.
 * The user requests a specific resource value, i.e. channel number and it is verified
 * unallocated, then returned.
 */
uint32_t Resource::Allocate(uint32_t index, const char *resourceDesc)
{
	std::lock_guard<priority_recursive_mutex> sync(m_allocateLock);
	if (index >= m_isAllocated.size())
	{
		// TODO: wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, resourceDesc);
		return ~0ul;
	}
	if ( m_isAllocated[index] )
	{
		// TODO: wpi_setWPIErrorWithContext(ResourceAlreadyAllocated, resourceDesc);
		return ~0ul;
	}
	m_isAllocated[index] = true;
	return index;
}


/**
 * Free an allocated resource.
 * After a resource is no longer needed, for example a destructor is called for a channel assignment
 * class, Free will release the resource value so it can be reused somewhere else in the program.
 */
void Resource::Free(uint32_t index)
{
	std::lock_guard<priority_recursive_mutex> sync(m_allocateLock);
	if (index == ~0ul) return;
	if (index >= m_isAllocated.size())
	{
		// TODO: wpi_setWPIError(NotAllocated);
		return;
	}
	if (!m_isAllocated[index])
	{
		// TODO: wpi_setWPIError(NotAllocated);
		return;
	}
	m_isAllocated[index] = false;
}

}  // namespace hal
