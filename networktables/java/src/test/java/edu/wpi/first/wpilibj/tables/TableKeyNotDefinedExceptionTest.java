package edu.wpi.first.wpilibj.tables;

import static org.junit.Assert.*;

import org.junit.*;

public class TableKeyNotDefinedExceptionTest {
	
	@Test
	public void throwExceptionTest(){
		try {
			throw new TableKeyNotDefinedException("Key 1");
		} catch (TableKeyNotDefinedException e) {
			assertEquals("Unkown Table Key: Key 1", e.getMessage());
		}
	}

}
