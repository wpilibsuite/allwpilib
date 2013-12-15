package edu.wpi.first.wpilibj.networktables2.type;

import static org.junit.Assert.*;

import java.io.*;

import org.junit.*;

import edu.wpi.first.wpilibj.networktables2.type.*;

public class EntryTypeTest {

	@Test
	public void testIdToType() {
		assertSame(DefaultEntryTypes.BOOLEAN, new NetworkTableEntryTypeManager().getType((byte)0));
		assertSame(DefaultEntryTypes.DOUBLE, new NetworkTableEntryTypeManager().getType((byte)1));
		assertSame(DefaultEntryTypes.STRING, new NetworkTableEntryTypeManager().getType((byte)2));
		assertNull(new NetworkTableEntryTypeManager().getType((byte)3));
		assertNull(new NetworkTableEntryTypeManager().getType((byte)4));
		assertNull(new NetworkTableEntryTypeManager().getType((byte)5));
		assertNull(new NetworkTableEntryTypeManager().getType((byte)127));
		assertNull(new NetworkTableEntryTypeManager().getType((byte)-1));
	}
	@Test
	public void testTypeIds() {
		assertEquals(0, DefaultEntryTypes.BOOLEAN.id);
		assertEquals(1, DefaultEntryTypes.DOUBLE.id);
		assertEquals(2, DefaultEntryTypes.STRING.id);
	}
	@Test
	public void testTypeNames() {
		assertEquals("Boolean", DefaultEntryTypes.BOOLEAN.name);
		assertEquals("Double", DefaultEntryTypes.DOUBLE.name);
		assertEquals("String", DefaultEntryTypes.STRING.name);
	}
	@Test
	public void testTypeToString() {
		assertEquals("NetworkTable type: Boolean", DefaultEntryTypes.BOOLEAN.toString());
		assertEquals("NetworkTable type: Double", DefaultEntryTypes.DOUBLE.toString());
		assertEquals("NetworkTable type: String", DefaultEntryTypes.STRING.toString());
	}
	
	
	@Test
	public void testBooleanRead() {
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{0});
			assertEquals(false, DefaultEntryTypes.BOOLEAN.readValue(new DataInputStream(input)));
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{1});
			assertEquals(true, DefaultEntryTypes.BOOLEAN.readValue(new DataInputStream(input)));
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[0]);
			assertEquals(false, DefaultEntryTypes.BOOLEAN.readValue(new DataInputStream(input)));
			fail();
		} catch (IOException e) {}
	}
	
	@Test
	public void testBooleanWrite() {
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.BOOLEAN.sendValue(false, new DataOutputStream(output));
			assertArrayEquals(new byte[]{0}, output.toByteArray());
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.BOOLEAN.sendValue(true, new DataOutputStream(output));
			assertArrayEquals(new byte[]{1}, output.toByteArray());
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.BOOLEAN.sendValue(new Object(), new DataOutputStream(output));
			fail();
		} catch (IOException e) {}
	}
	
	
	
	
	

	
	
	@Test
	public void testDoubleRead() {
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{64, 41, 0, 0, 0, 0, 0, 0});
			assertEquals(12.5, DefaultEntryTypes.DOUBLE.readValue(new DataInputStream(input)));
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{64, 100, 0, 0, 0, 0, 0, 0});
			assertEquals(160.0, DefaultEntryTypes.DOUBLE.readValue(new DataInputStream(input)));
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[0]);
			assertEquals(false, DefaultEntryTypes.DOUBLE.readValue(new DataInputStream(input)));
			fail();
		} catch (IOException e) {}
	}
	
	@Test
	public void testDoubleWrite() {
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.DOUBLE.sendValue(12.5, new DataOutputStream(output));
			assertArrayEquals(new byte[]{64, 41, 0, 0, 0, 0, 0, 0}, output.toByteArray());
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.DOUBLE.sendValue(160.0, new DataOutputStream(output));
			assertArrayEquals(new byte[]{64, 100, 0, 0, 0, 0, 0, 0}, output.toByteArray());
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.DOUBLE.sendValue(new Object(), new DataOutputStream(output));
			fail();
		} catch (IOException e) {}
	}

	
	
	
	
	

	
	
	@Test
	public void testStringRead() {
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{0, 8, 77, 121, 32, 69, 110, 116, 114, 121});
			assertEquals("My Entry", DefaultEntryTypes.STRING.readValue(new DataInputStream(input)));
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[]{0, 5, 86, 97, 76, 117, 69});
			assertEquals("VaLuE", DefaultEntryTypes.STRING.readValue(new DataInputStream(input)));
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayInputStream input = new ByteArrayInputStream(new byte[0]);
			assertEquals(false, DefaultEntryTypes.STRING.readValue(new DataInputStream(input)));
			fail();
		} catch (IOException e) {}
	}
	
	@Test
	public void testStringWrite() {
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.STRING.sendValue("My Entry", new DataOutputStream(output));
			assertArrayEquals(new byte[]{0, 8, 77, 121, 32, 69, 110, 116, 114, 121}, output.toByteArray());
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.STRING.sendValue("VaLuE", new DataOutputStream(output));
			assertArrayEquals(new byte[]{0, 5, 86, 97, 76, 117, 69}, output.toByteArray());
		} catch (IOException e) {
			fail();
		}
		try {
			ByteArrayOutputStream output = new ByteArrayOutputStream();
			DefaultEntryTypes.STRING.sendValue(new Object(), new DataOutputStream(output));
			fail();
		} catch (IOException e) {}
	}

}
