/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.NetworkTableType;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.UnitTestUtility;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import static edu.wpi.first.networktables.NetworkTableType.kBoolean;
import static edu.wpi.first.networktables.NetworkTableType.kDouble;
import static edu.wpi.first.networktables.NetworkTableType.kString;
import static edu.wpi.first.wpilibj.shuffleboard.Shuffleboard.add;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

public class ShuffleboardTest {

  private static NetworkTableInstance instance;

  @BeforeClass
  public static void setUpClass() {
    UnitTestUtility.setupMockBase();
  }

  @Before
  public void setUp() {
    instance = NetworkTableInstance.create();
    Shuffleboard.instance = instance;
  }

  @AfterClass
  public static void tearDown() {
    Shuffleboard.instance = NetworkTableInstance.getDefault();
  }

  @Test
  public void testInvalidInputsToAddEntry() {
    assertThrows(NullPointerException.class, () -> add(null, kDouble));
    assertThrows(NullPointerException.class, () -> add("foo", (NetworkTableType) null));
    assertThrows(IllegalArgumentException.class, () -> add("", kDouble));
    assertThrows(IllegalArgumentException.class, () -> add("a/b", kDouble));
  }

  @Test
  public void testInvalidInputsToAddSendable() {
    assertThrows(NullPointerException.class, () -> add(null, new MockSendable()));
    assertThrows(NullPointerException.class, () -> add("foo", (Sendable) null));
    assertThrows(IllegalArgumentException.class, () -> add("", new MockSendable()));
    assertThrows(IllegalArgumentException.class, () -> add("a/b", new MockSendable()));
    assertThrows(IllegalArgumentException.class, () -> add(new MockSendable()));
    assertThrows(NullPointerException.class, () -> add(new MockSendable(null)));
    assertThrows(IllegalArgumentException.class, () -> add(new MockSendable("a/b")));
  }

  @Test
  public void testEntryInTab() {
    NetworkTableEntry entry = add("Entry Name", kDouble)
        .toTab("Tab Name")
        .getEntry();
    assertEquals("/Shuffleboard/Tab Name/Entry Name", entry.getName());

    Shuffleboard.update();

    NetworkTable tabMetaData = instance.getTable("/Shuffleboard/.tabs/Tab Name");
    assertEquals(
        "Tab Name",
        tabMetaData.getEntry("Name").getString(null));
    assertEquals(
        "/Shuffleboard/Tab Name/",
        tabMetaData.getEntry("AutopopulatePrefix").getString(null));
  }

  @Test
  public void testEntryInLayout() {
    NetworkTableEntry entry = add("Entry Name", kBoolean)
        .toTab("Tab Name")
        .toLayout("List", "Layout Title")
        .getEntry();
    assertEquals("/Shuffleboard/Tab Name/Layout Title/Entry Name", entry.getName());

    Shuffleboard.update();

    NetworkTable layout = instance.getTable("/Shuffleboard/.metadata/Tab Name/Layout Title");
    assertEquals("List", layout.getEntry("Type").getString(null));
    assertEquals("Layout Title", layout.getEntry("Title").getString(null));
  }

  @Test
  public void testEntryLayoutProperties() {
    NetworkTableEntry entry = add("Entry Name", kString)
        .toTab("Tab Name")
        .toLayout("List", "Layout Title")
        .withProperties(mapOf("Property 1", 12.34, "Property 2", "foo"))
        .getEntry();
    assertEquals("/Shuffleboard/Tab Name/Layout Title/Entry Name", entry.getName());

    Shuffleboard.update();

    NetworkTable properties = instance.getTable(
        "/Shuffleboard/.metadata/Tab Name/Layout Title/LayoutProperties");
    assertEquals(setOf("Property 1", "Property 2"), properties.getKeys());
    assertEquals(12.34, properties.getEntry("Property 1").getNumber(0.0));
    assertEquals("foo", properties.getEntry("Property 2").getString(null));
  }

  @Test
  public void testEntryWithWidgetWithPropertiesInTab() {
    NetworkTableEntry entry = add("Entry Name", kDouble)
        .toTab("Tab Name")
        .withWidget("Widget Type")
        .withProperties(mapOf("Property Title", "foo"))
        .getEntry();
    assertEquals("/Shuffleboard/Tab Name/Entry Name", entry.getName());

    Shuffleboard.update();

    NetworkTable metadata = instance.getTable(
        "/Shuffleboard/.metadata/Tab Name/Entry Name");
    assertEquals("Widget Type", metadata.getEntry("PreferredWidget").getString(null));
    NetworkTable properties = metadata.getSubTable("WidgetProperties");
    assertEquals(setOf("Property Title"), properties.getKeys());
    assertEquals("foo", properties.getEntry("Property Title").getString(null));
  }

  @Test
  public void testEntryWithWidgetInLayout() {
    NetworkTableEntry entry = add("Entry Name", kDouble)
        .toTab("Tab Name")
        .toLayout("List", "Layout Title")
        .withWidget("Widget Type")
        .withProperties(mapOf("Property Title", "foo"))
        .getEntry();
    assertEquals("/Shuffleboard/Tab Name/Layout Title/Entry Name", entry.getName());

    Shuffleboard.update();

    NetworkTable metadata = instance.getTable(
        "/Shuffleboard/.metadata/Tab Name/Layout Title/Entry Name");
    assertEquals("Widget Type", metadata.getEntry("PreferredWidget").getString(null));
    NetworkTable properties = metadata.getSubTable("WidgetProperties");
    assertEquals(setOf("Property Title"), properties.getKeys());
    assertEquals("foo", properties.getEntry("Property Title").getString(null));
  }

  @Test
  public void testSendableInTab() {
    MockSendable sendable = new MockSendable("Sendable Name");
    add(sendable)
        .toTab("Tab Name");

    Shuffleboard.update();

    NetworkTable sendableTable = instance.getTable("/Shuffleboard/Tab Name/Sendable Name");
    assertEquals(setOf(".type", "Value"), sendableTable.getKeys());
    assertEquals("MockSendable", sendableTable.getEntry(".type").getString(null));
    assertEquals(Math.PI, sendableTable.getEntry("Value").getNumber(0.0));
  }

  @Test
  public void testSendableInLayout() {
    MockSendable sendable = new MockSendable("Sendable Name");
    add(sendable)
        .toTab("Tab Name")
        .toLayout("List", "Layout Title");

    Shuffleboard.update();
    NetworkTable sendableTable = instance.getTable(
        "/Shuffleboard/Tab Name/Layout Title/Sendable Name");
    assertEquals(setOf(".type", "Value"), sendableTable.getKeys());
    assertEquals("MockSendable", sendableTable.getEntry(".type").getString(null));
    assertEquals(Math.PI, sendableTable.getEntry("Value").getNumber(0.0));
  }

  @Test
  public void testSendableInLayoutWithProperties() {
    MockSendable sendable = new MockSendable("Sendable Name");
    add(sendable)
        .toTab("Tab Name")
        .toLayout("List", "Layout Title")
        .withProperties(mapOf("Property 1", Math.PI, "Property 2", "bar"));

    Shuffleboard.update();

    NetworkTable metadata = instance.getTable(
        "/Shuffleboard/.metadata/Tab Name/Layout Title");
    assertEquals(setOf("Type", "Title"), metadata.getKeys());
    assertEquals("List", metadata.getEntry("Type").getString(null));
    assertEquals("Layout Title", metadata.getEntry("Title").getString(null));

    NetworkTable properties = metadata.getSubTable("LayoutProperties");
    assertEquals(setOf("Property 1", "Property 2"), properties.getKeys());
    assertEquals(Math.PI, properties.getEntry("Property 1").getDouble(0), 0);
    assertEquals("bar", properties.getEntry("Property 2").getString(null));

    NetworkTable sendableTable = instance.getTable(
        "/Shuffleboard/Tab Name/Layout Title/Sendable Name");
    assertEquals(setOf(".type", "Value"), sendableTable.getKeys());
    assertEquals("MockSendable", sendableTable.getEntry(".type").getString(null));
    assertEquals(Math.PI, sendableTable.getEntry("Value").getNumber(0.0));
  }

  @Test
  public void testSendableWithWidget() {
    MockSendable sendable = new MockSendable("Sendable Name");
    add(sendable)
        .toTab("Tab Name")
        .withWidget("Widget Type")
        .withProperties(mapOf("foo", "bar", "baz", "buq"));

    Shuffleboard.update();

    NetworkTable metadata = instance.getTable("/Shuffleboard/.metadata/Tab Name/Sendable Name");
    assertEquals(setOf("PreferredWidget"), metadata.getKeys());
    assertEquals("Widget Type", metadata.getEntry("PreferredWidget").getString(null));
    assertEquals(setOf("foo", "baz"), metadata.getSubTable("WidgetProperties").getKeys());
    assertEquals("bar", metadata.getEntry("WidgetProperties/foo").getString(null));
    assertEquals("buq", metadata.getEntry("WidgetProperties/baz").getString(null));

    NetworkTable sendableTable = instance.getTable("/Shuffleboard/Tab Name/Sendable Name");
    assertEquals(setOf(".type", "Value"), sendableTable.getKeys());
    assertEquals("MockSendable", sendableTable.getEntry(".type").getString(null));
    assertEquals(Math.PI, sendableTable.getEntry("Value").getNumber(0.0));
  }

  @Test
  public void testSendableWithWidgetInLayout() {
    MockSendable sendable = new MockSendable("Sendable Name");
    add(sendable)
        .toTab("Tab Name")
        .toLayout("List", "Layout Title")
        .withWidget("Widget Type")
        .withProperties(mapOf("foo", "bar", "baz", "buq"));

    Shuffleboard.update();

    NetworkTable metadata = instance.getTable(
        "/Shuffleboard/.metadata/Tab Name/Layout Title/Sendable Name");
    assertEquals(setOf("PreferredWidget"), metadata.getKeys());
    assertEquals("Widget Type", metadata.getEntry("PreferredWidget").getString(null));
    assertEquals(setOf("foo", "baz"), metadata.getSubTable("WidgetProperties").getKeys());
    assertEquals("bar", metadata.getEntry("WidgetProperties/foo").getString(null));
    assertEquals("buq", metadata.getEntry("WidgetProperties/baz").getString(null));

    NetworkTable sendableTable = instance.getTable(
        "/Shuffleboard/Tab Name/Layout Title/Sendable Name");
    assertEquals(setOf(".type", "Value"), sendableTable.getKeys());
    assertEquals("MockSendable", sendableTable.getEntry(".type").getString(null));
    assertEquals(Math.PI, sendableTable.getEntry("Value").getNumber(0.0));
  }

  private static final class MockSendable extends SendableBase {

    MockSendable() {
    }

    MockSendable(String name) {
      setName(name);
    }

    @Override
    public void initSendable(SendableBuilder builder) {
      builder.setSmartDashboardType("MockSendable");
      builder.addDoubleProperty("Value", () -> Math.PI, null);
    }
  }

  /**
   * Stub for Java 9 {@code Set.of(E...)}.
   */
  private static <T> Set<T> setOf(T... values) {
    Set<T> set = new HashSet<>();
    Collections.addAll(set, values);
    return set;
  }

  /**
   * Stub for Java 9 {@code Map.of(K, V)}.
   */
  private static <K, V> Map<K, V> mapOf(K key, V value) {
    return Collections.singletonMap(key, value);
  }

  /**
   * Stub for Java 9 {@code Map.of(K, V, K, V)}.
   */
  private static <K, V> Map<K, V> mapOf(K key1, V value1, K key2, V value2) {
    Map<K, V> map = new HashMap<>();
    map.put(key1, value1);
    map.put(key2, value2);
    return map;
  }

  /**
   * Stub for JUnit 5 {@code assertThrows}.
   */
  private static void assertThrows(Class<? extends Throwable> exceptionClass, Runnable action) {
    try {
      action.run();
      fail("Expected a " + exceptionClass.getName() + ", but nothing was thrown");
    } catch (Throwable throwable) {
      assertEquals(exceptionClass, throwable.getClass());
    }
  }

}
