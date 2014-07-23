/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import edu.wpi.first.wpilibj.util.CheckedAllocationException;

/**
 * Test covers the possible uses of the resource class.
 * @author jonathanleitschuh
 *
 */
public class ResourceTest {
	private Resource testResource;
	private static final int RESOURCE_MAX_SIZE = 5;

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		testResource = new Resource(RESOURCE_MAX_SIZE);
	}
	
	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception{
		Resource.restartProgram();
	}


	/**
	 * Checks that allocating a resource above the range of resources will cause an exception
	 * @throws CheckedAllocationException
	 */
	@Test(expected = CheckedAllocationException.class)
	public void testAllocateAboveRange() throws CheckedAllocationException {
		testResource.allocate(RESOURCE_MAX_SIZE);
	}
	
	/**
	 * Checks that allocating a negative resource value will cause an exception
	 * @throws CheckedAllocationException
	 */
	@Test(expected = CheckedAllocationException.class)
	public void testAllocateBelowRange() throws CheckedAllocationException {
		testResource.allocate(-1);
	}
	
	/**
	 * Checks that allocating the same resource twice causes an exception
	 * @throws CheckedAllocationException
	 */
	@Test(expected = CheckedAllocationException.class)
	public void testAlocateAlreadyAllocatedResource() throws CheckedAllocationException{
		testResource.allocate(0);
		testResource.allocate(0);	
	}
	
	/**
	 * Checks that all valid resources can be allocated without exception.
	 * @throws CheckedAllocationException
	 */
	@Test
	public void testAllocateValidResource() throws CheckedAllocationException{
		for(int i = 0; i < RESOURCE_MAX_SIZE; i++){
			testResource.allocate(i);
		}
		//Should complete without errors
	}
	
	/**
	 * Checks that freeing an resource that has been allocated allows it to be allocated again.
	 * @throws CheckedAllocationException
	 */
	@Test
	public void testAllocateFreedResource() throws CheckedAllocationException{
		for(int i = 0; i < RESOURCE_MAX_SIZE; i++){
			testResource.allocate(i);
		}
		for(int i = 0; i < RESOURCE_MAX_SIZE; i++){
			testResource.free(i);
		}
		for(int i = 0; i < RESOURCE_MAX_SIZE; i++){
			testResource.allocate(i);
		}
		//Should complete without errors
	}
	
	/**
	 * Checks that asking for the next valid allocated resource will cause it to skip an already allocated resource.
	 * @throws CheckedAllocationException
	 */
	@Test
	public void testAllocateNextValidResource() throws CheckedAllocationException{
		testResource.allocate(3);
		int allocatedValue = testResource.allocate();
		assertEquals(0, allocatedValue);
		allocatedValue = testResource.allocate();
		assertEquals(1, allocatedValue);
		allocatedValue = testResource.allocate();
		assertEquals(2, allocatedValue);
		//Three should be skipped because it is already allocated.
		allocatedValue = testResource.allocate();
		assertEquals(4, allocatedValue);
	}
	
	/**
	 * Attempts to allocate a resource with no remaining resources available.
	 * @throws CheckedAllocationException
	 */
	@Test(expected = CheckedAllocationException.class)
	public void testAllocateWithNoRemainingResources() throws CheckedAllocationException{
		for(int i = 0; i < RESOURCE_MAX_SIZE; i++){
			testResource.allocate(i);
		}
		testResource.allocate();
	}

}
