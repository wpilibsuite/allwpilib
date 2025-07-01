// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class SelectableTest {
  MockTunableBackend m_mock;

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
    assertEquals(toSelect, chooser.getSelected());
  }

  @Test
  void publishesEntriesMetadataAndIgnoresImmutableRemoteWrites() {
    var chooser = new Selectable<Integer>();
    chooser.add("one", 1);
    chooser.addDefault("two", 2);

    assertEquals("Selectable", chooser.getTunableType());

    Tunables.publish("metadataChooser", chooser);

    assertEquals("two", m_mock.getValue("/metadataChooser/default"));
    assertArrayEquals(new String[] {"one", "two"}, m_mock.getValue("/metadataChooser/options"));
    assertEquals("", m_mock.getValue("/metadataChooser/selected"));

    m_mock.setValue("/metadataChooser/default", "one");
    m_mock.setArray("/metadataChooser/options", new String[] {"remote"});
    TunableRegistry.update();

    assertEquals("two", m_mock.getValue("/metadataChooser/default"));
    assertArrayEquals(new String[] {"one", "two"}, m_mock.getValue("/metadataChooser/options"));
    assertEquals(2, chooser.getSelected());
  }

  @Test
  void defaultIsReturnedOnNoSelect() {
    var chooser = new Selectable<Integer>();
    for (int i = 1; i <= 3; i++) {
      chooser.add(String.valueOf(i), i);
    }
    chooser.addDefault(String.valueOf(0), 0);

    assertEquals(0, chooser.getSelected());
  }

  @Test
  void nullIsReturnedOnNoSelectAndNoDefault() {
    var chooser = new Selectable<Integer>();
    for (int i = 1; i <= 3; i++) {
      chooser.add(String.valueOf(i), i);
    }

    assertNull(chooser.getSelected());
  }

  @Test
  void testChangeListener() {
    var chooser = new Selectable<Integer>();
    for (int i = 1; i <= 3; i++) {
      chooser.add(String.valueOf(i), i);
    }
    AtomicInteger currentVal = new AtomicInteger();
    chooser.onChange(currentVal::set);

    Tunables.publish("changeListenerChooser", chooser);
    TunableRegistry.update();
    m_mock.setValue("/changeListenerChooser/selected", "3");
    TunableRegistry.update();
    assertEquals(3, currentVal.get());
  }

  @Test
  void listenerIsNotCalledForUnknownSelection() {
    var chooser = new Selectable<Integer>();
    chooser.add("one", 1);
    AtomicInteger currentVal = new AtomicInteger();
    chooser.onChange(currentVal::set);

    Tunables.publish("unknownSelectionChooser", chooser);
    m_mock.setValue("/unknownSelectionChooser/selected", "missing");
    TunableRegistry.update();

    assertEquals(0, currentVal.get());
    assertNull(chooser.getSelected());
  }

  @Test
  void listenerReplacementUsesLatestListener() {
    var chooser = new Selectable<Integer>();
    chooser.add("one", 1);
    AtomicInteger first = new AtomicInteger();
    AtomicInteger second = new AtomicInteger();
    chooser.onChange(first::set);
    chooser.onChange(second::set);

    Tunables.publish("listenerReplacementChooser", chooser);
    m_mock.setValue("/listenerReplacementChooser/selected", "one");
    TunableRegistry.update();

    assertEquals(0, first.get());
    assertEquals(1, second.get());
  }

  @Test
  void duplicateOptionReplacesValueWithoutDuplicatingOptions() {
    var chooser = new Selectable<Integer>();
    chooser.add("mode", 1);
    chooser.add("mode", 2);

    Tunables.publish("duplicateChooser", chooser);
    m_mock.setValue("/duplicateChooser/selected", "mode");
    TunableRegistry.update();

    assertEquals(2, chooser.getSelected());
    assertArrayEquals(new String[] {"mode"}, m_mock.getValue("/duplicateChooser/options"));
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
    assertNull(chooser.getSelected());
    assertEquals("", m_mock.getValue("/clearChooser/default"));
    assertArrayEquals(new String[0], m_mock.getValue("/clearChooser/options"));

    chooser.add("two", 22);
    assertEquals(22, chooser.getSelected());
    assertArrayEquals(new String[] {"two"}, m_mock.getValue("/clearChooser/options"));
  }
}
