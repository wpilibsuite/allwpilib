package edu.wpi.first.wpilibj.networktables2.connection;

import static org.junit.Assert.*;

import org.junit.*;

public class BadMessageExceptionTest {
	
	@Test
	public void throwExceptionTest(){
		try {
			throw new BadMessageException("Got some bad message");
		} catch (BadMessageException e) {
			assertEquals("Got some bad message", e.getMessage());
		}
	}

}
