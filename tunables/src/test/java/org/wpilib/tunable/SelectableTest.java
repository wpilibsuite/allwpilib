// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class SelectableTest {
  MockTunableBackend m_mock;

  class DataHolder {
    String currentKey;
    int currentVal;

    public void set(String key, int val) {
      currentKey = key;
      currentVal = val;
    }
  }

  @BeforeEach
  void setUp() {
    m_mock = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", m_mock);
  }

  @AfterEach
  public void shutdown() {
    TunableRegistry.reset();
  }

  @ValueSource(ints = {0, 1, 2, 3})
  @ParameterizedTest
  void returnsSelected(int toSelect) {
    var chooser = new Selectable<Integer>();
    for (int i = 1; i <= 3; i++) {
      chooser.add(String.valueOf(i), i);
    }
    chooser.addDefault(String.valueOf(0), 0);

    Tunables.publish("returnsSelectedChooser" + toSelect, chooser);
    TunableRegistry.update();
    m_mock.setValue("/returnsSelectedChooser" + toSelect + "/selected", String.valueOf(toSelect));
    TunableRegistry.update();
    assertEquals(String.valueOf(toSelect), chooser.getSelectedName());
    assertEquals(toSelect, chooser.getSelected());
  }

  @Test
  void publishesEntriesMetadataAndIgnoresImmutableRemoteWrites() {
    var chooser = new Selectable<Integer>();
    chooser.add("one", 1);
    chooser.addDefault("two", 2);

    assertEquals("Selectable", chooser.getTunableType());

    Tunables.publish("metadataChooser", chooser);

    assertEquals("two", m_mock.getValue("/metadataChooser/default", String.class));
    assertArrayEquals(
        new String[] {"one", "two"}, m_mock.getValue("/metadataChooser/options", String[].class));
    assertEquals("", m_mock.getValue("/metadataChooser/selected", String.class));

    m_mock.setValue("/metadataChooser/default", "one");
    m_mock.setArray("/metadataChooser/options", new String[] {"remote"});
    TunableRegistry.update();

    assertEquals("two", m_mock.getValue("/metadataChooser/default", String.class));
    assertArrayEquals(
        new String[] {"one", "two"}, m_mock.getValue("/metadataChooser/options", String[].class));
    assertEquals("two", chooser.getSelectedName());
    assertEquals(2, chooser.getSelected());
  }

  @Test
  void defaultIsReturnedOnNoSelect() {
    var chooser = new Selectable<Integer>();
    for (int i = 1; i <= 3; i++) {
      chooser.add(String.valueOf(i), i);
    }
    chooser.addDefault(String.valueOf(0), 0);

    assertEquals("0", chooser.getSelectedName());
    assertEquals(0, chooser.getSelected());
  }

  @Test
  void defaultIsReturnedOnUnknownSelect() {
    var chooser = new Selectable<Integer>();
    chooser.addDefault("one", 1);
    chooser.add("two", 2);

    Tunables.publish("unknownDefaultChooser", chooser);
    m_mock.setValue("/unknownDefaultChooser/selected", "missing");
    TunableRegistry.update();

    assertEquals("one", chooser.getSelectedName());
    assertEquals(1, chooser.getSelected());
  }

  @Test
  void nullIsReturnedOnNoSelectAndNoDefault() {
    var chooser = new Selectable<Integer>();
    for (int i = 1; i <= 3; i++) {
      chooser.add(String.valueOf(i), i);
    }

    assertNull(chooser.getSelectedName());
    assertNull(chooser.getSelected());
  }

  @Test
  void testChangeListener() {
    var chooser = new Selectable<Integer>();
    for (int i = 1; i <= 3; i++) {
      chooser.add(String.valueOf(i), i);
    }
    var data = new DataHolder();
    chooser.onChange(data::set);

    Tunables.publish("changeListenerChooser", chooser);
    TunableRegistry.update();
    m_mock.setValue("/changeListenerChooser/selected", "3");
    TunableRegistry.update();
    assertEquals("3", data.currentKey);
    assertEquals(3, data.currentVal);
  }

  @Test
  void changeListenerUsesDefaultWhenSelectionIsUnknown() {
    var chooser = new Selectable<Integer>();
    chooser.addDefault("one", 1);
    chooser.add("two", 2);
    var data = new DataHolder();
    chooser.onChange(data::set);

    Tunables.publish("changeListenerUnknownDefaultChooser", chooser);
    m_mock.setValue("/changeListenerUnknownDefaultChooser/selected", "missing");
    TunableRegistry.update();

    assertEquals("one", data.currentKey);
    assertEquals(1, data.currentVal);
  }

  @Test
  void listenerIsNotCalledForUnknownSelection() {
    var chooser = new Selectable<Integer>();
    chooser.add("one", 1);
    var data = new DataHolder();
    chooser.onChange(data::set);

    Tunables.publish("unknownSelectionChooser", chooser);
    m_mock.setValue("/unknownSelectionChooser/selected", "missing");
    TunableRegistry.update();

    assertNull(data.currentKey);
    assertEquals(0, data.currentVal);
    assertNull(chooser.getSelectedName());
    assertNull(chooser.getSelected());
  }

  @Test
  void listenerReplacementUsesLatestListener() {
    var chooser = new Selectable<Integer>();
    chooser.add("one", 1);
    var first = new DataHolder();
    var second = new DataHolder();
    chooser.onChange(first::set);
    chooser.onChange(second::set);

    Tunables.publish("listenerReplacementChooser", chooser);
    m_mock.setValue("/listenerReplacementChooser/selected", "one");
    TunableRegistry.update();

    assertNull(first.currentKey);
    assertEquals(0, first.currentVal);
    assertEquals("one", second.currentKey);
    assertEquals(1, second.currentVal);
  }

  @Test
  void duplicateOptionReplacesValueWithoutDuplicatingOptions() {
    var chooser = new Selectable<Integer>();
    chooser.add("mode", 1);
    chooser.add("mode", 2);

    Tunables.publish("duplicateChooser", chooser);
    m_mock.setValue("/duplicateChooser/selected", "mode");
    TunableRegistry.update();

    assertEquals("mode", chooser.getSelectedName());
    assertEquals(2, chooser.getSelected());
    assertArrayEquals(
        new String[] {"mode"}, m_mock.getValue("/duplicateChooser/options", String[].class));
  }

  @Test
  void removeDeletesSingleOptionAndClearsDefaultButPreservesSelectionName() {
    var chooser = new Selectable<Integer>();
    chooser.addDefault("one", 1);
    chooser.add("two", 2);
    chooser.add("three", 3);
    Tunables.publish("removeChooser", chooser);
    m_mock.setValue("/removeChooser/selected", "two");
    TunableRegistry.update();

    chooser.remove("one");
    assertEquals("two", chooser.getSelectedName());
    assertEquals(2, chooser.getSelected());
    assertEquals("", m_mock.getValue("/removeChooser/default", String.class));
    assertArrayEquals(
        new String[] {"two", "three"}, m_mock.getValue("/removeChooser/options", String[].class));

    chooser.remove("two");
    assertNull(chooser.getSelectedName());
    assertNull(chooser.getSelected());
    assertArrayEquals(
        new String[] {"three"}, m_mock.getValue("/removeChooser/options", String[].class));

    chooser.add("two", 22);
    assertEquals("two", chooser.getSelectedName());
    assertEquals(22, chooser.getSelected());
    assertArrayEquals(
        new String[] {"three", "two"}, m_mock.getValue("/removeChooser/options", String[].class));

    chooser.remove("missing");
    assertEquals("two", chooser.getSelectedName());
    assertEquals(22, chooser.getSelected());
    assertArrayEquals(
        new String[] {"three", "two"}, m_mock.getValue("/removeChooser/options", String[].class));
  }

  @Test
  void clearResetsOptionsAndDefaultButPreservesSelectionName() {
    var chooser = new Selectable<Integer>();
    chooser.addDefault("one", 1);
    chooser.add("two", 2);
    Tunables.publish("clearChooser", chooser);
    m_mock.setValue("/clearChooser/selected", "two");
    TunableRegistry.update();

    chooser.clear();
    assertNull(chooser.getSelectedName());
    assertNull(chooser.getSelected());
    assertEquals("", m_mock.getValue("/clearChooser/default", String.class));
    assertArrayEquals(new String[0], m_mock.getValue("/clearChooser/options", String[].class));

    chooser.add("two", 22);
    assertEquals("two", chooser.getSelectedName());
    assertEquals(22, chooser.getSelected());
    assertArrayEquals(
        new String[] {"two"}, m_mock.getValue("/clearChooser/options", String[].class));
  }
}
