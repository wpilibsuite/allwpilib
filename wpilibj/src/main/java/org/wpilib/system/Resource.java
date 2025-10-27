// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.util.AllocationException;
import edu.wpi.first.hal.util.CheckedAllocationException;

/**
 * Track resources in the program. The Resource class is a convenient way of keeping track of
 * allocated arbitrary resources in the program. Resources are just indices that have a lower and
 * upper bound that are tracked by this class. In the library they are used for tracking allocation
 * of hardware channels but this is purely arbitrary. The resource class does not do any actual
 * allocation, but simply tracks if a given index is currently in use.
 *
 * <p><b>WARNING:</b> this should only be statically allocated. When the program loads into memory
 * all the static constructors are called. At that time a linked list of all the "Resources" is
 * created. Then, when the program actually starts - in the Robot constructor, all resources are
 * initialized. This ensures that the program is restartable in memory without having to
 * unload/reload.
 *
 * @deprecated Will be removed with no replacement.
 */
@Deprecated(forRemoval = true, since = "2025")
public final class Resource {
  private static Resource resourceList;
  private final boolean[] m_numAllocated;
  private final int m_size;
  private final Resource m_nextResource;

  /** Clears all allocated resources. */
  public static void restartProgram() {
    for (Resource r = Resource.resourceList; r != null; r = r.m_nextResource) {
      for (int i = 0; i < r.m_size; i++) {
        r.m_numAllocated[i] = false;
      }
    }
  }

  /**
   * Allocate storage for a new instance of Resource. Allocate a bool array of values that will get
   * initialized to indicate that no resources have been allocated yet. The indices of the resources
   * are 0..size-1.
   *
   * @param size The number of blocks to allocate
   */
  public Resource(final int size) {
    m_size = size;
    m_numAllocated = new boolean[size];
    for (int i = 0; i < size; i++) {
      m_numAllocated[i] = false;
    }
    m_nextResource = Resource.resourceList;
    Resource.resourceList = this;
  }

  /**
   * Allocate a resource. When a resource is requested, mark it allocated. In this case, a free
   * resource value within the range is located and returned after it is marked allocated.
   *
   * @return The index of the allocated block.
   * @throws CheckedAllocationException If there are no resources available to be allocated.
   */
  public int allocate() throws CheckedAllocationException {
    for (int i = 0; i < m_size; i++) {
      if (!m_numAllocated[i]) {
        m_numAllocated[i] = true;
        return i;
      }
    }
    throw new CheckedAllocationException("No available resources");
  }

  /**
   * Allocate a specific resource value. The user requests a specific resource value, i.e. channel
   * number, and it is verified unallocated, then returned.
   *
   * @param index The resource to allocate
   * @return The index of the allocated block
   * @throws CheckedAllocationException If there are no resources available to be allocated.
   */
  public int allocate(final int index) throws CheckedAllocationException {
    if (index >= m_size || index < 0) {
      throw new CheckedAllocationException("Index " + index + " out of range");
    }
    if (m_numAllocated[index]) {
      throw new CheckedAllocationException("Resource at index " + index + " already allocated");
    }
    m_numAllocated[index] = true;
    return index;
  }

  /**
   * Free an allocated resource. After a resource is no longer needed, for example a destructor is
   * called for a channel assignment class, this method will release the resource value, so it can
   * be reused somewhere else in the program.
   *
   * @param index The index of the resource to free.
   */
  public void free(final int index) {
    if (!m_numAllocated[index]) {
      throw new AllocationException("No resource available to be freed");
    }
    m_numAllocated[index] = false;
  }
}
