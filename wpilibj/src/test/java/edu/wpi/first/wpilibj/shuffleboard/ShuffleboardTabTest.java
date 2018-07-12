package edu.wpi.first.wpilibj.shuffleboard;

import java.util.HashMap;
import java.util.Map;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.command.InstantCommand;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;

@SuppressWarnings({"MemberName", "PMD.TooManyMethods"})
public class ShuffleboardTabTest {

  private NetworkTableInstance ntInstance;
  private ShuffleboardTab tab;
  private ShuffleboardInstance instance;

  @BeforeEach
  public void setup() {
    ntInstance = NetworkTableInstance.create();
    instance = new ShuffleboardInstance(ntInstance);
    tab = instance.getTab("Tab");
  }

  @Test
  public void testAddDouble() {
    NetworkTableEntry entry = tab.add("Double", 1.0).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Double", entry.getName()),
        () -> assertEquals(1.0, entry.getValue().getDouble()));
  }

  @Test
  public void testAddInteger() {
    NetworkTableEntry entry = tab.add("Int", 1).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Int", entry.getName()),
        () -> assertEquals(1.0, entry.getValue().getDouble()));
  }

  @Test
  public void testAddLong() {
    NetworkTableEntry entry = tab.add("Long", 1L).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Long", entry.getName()),
        () -> assertEquals(1.0, entry.getValue().getDouble()));
  }


  @Test
  public void testAddBoolean() {
    NetworkTableEntry entry = tab.add("Bool", false).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/Bool", entry.getName()),
        () -> assertFalse(entry.getValue().getBoolean()));
  }

  @Test
  public void testAddString() {
    NetworkTableEntry entry = tab.add("String", "foobar").getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/String", entry.getName()),
        () -> assertEquals("foobar", entry.getValue().getString()));
  }

  @Test
  public void testAddNamedSendableWithProperties() {
    Sendable sendable = new InstantCommand("Command");
    String widgetType = "Command Widget";
    tab.add(sendable)
       .withWidget(widgetType)
       .withProperties(mapOf("foo", 1234, "bar", "baz"));

    instance.update();
    String meta = "/Shuffleboard/.metadata/Tab/Command";

    assertAll(
        () -> assertEquals(1234,
                           ntInstance.getEntry(meta + "/Properties/foo").getDouble(-1),
                           "Property 'foo' not set correctly"),
        () -> assertEquals("baz",
                           ntInstance.getEntry(meta + "/Properties/bar").getString(null),
                           "Property 'bar' not set correctly"),
        () -> assertEquals(widgetType,
                           ntInstance.getEntry(meta + "/PreferredComponent").getString(null),
                           "Preferred component not set correctly"));
  }

  @Test
  public void testAddNumberArray() {
    NetworkTableEntry entry = tab.add("DoubleArray", new double[]{1, 2, 3}).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/DoubleArray", entry.getName()),
        () -> assertArrayEquals(new double[]{1, 2, 3}, entry.getValue().getDoubleArray()));
  }

  @Test
  public void testAddBooleanArray() {
    NetworkTableEntry entry = tab.add("BoolArray", new boolean[]{true, false}).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/BoolArray", entry.getName()),
        () -> assertArrayEquals(new boolean[]{true, false}, entry.getValue().getBooleanArray()));
  }

  @Test
  public void testAddStringArray() {
    NetworkTableEntry entry = tab.add("StringArray", new String[]{"foo", "bar"}).getEntry();
    assertAll(
        () -> assertEquals("/Shuffleboard/Tab/StringArray", entry.getName()),
        () -> assertArrayEquals(new String[]{"foo", "bar"}, entry.getValue().getStringArray()));
  }

  @Test
  public void testTitleDuplicates() {
    tab.add("foo", "bar");
    assertThrows(IllegalArgumentException.class, () -> tab.add("foo", "baz"));
  }

  @Test
  public void testAutopopulate() {
    String prefix = "network_table:///SmartDashboard/";
    tab.autopopulate(prefix);
    instance.update();

    assertEquals(prefix,
                 ntInstance.getEntry("/Shuffleboard/.metadata/Tab/Autopopulate").getString(null),
                 "Autopopulate metadata not set");
  }

  /**
   * Stub for Java 9 {@code Map.of()}.
   */
  @SuppressWarnings({"unchecked", "PMD"})
  private static <K, V> Map<K, V> mapOf(Object... entries) {
    Map<K, V> map = new HashMap<>();
    for (int i = 0; i < entries.length; i += 2) {
      map.put((K) entries[i], (V) entries[i + 1]);
    }
    return map;
  }

}
