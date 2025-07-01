// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

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
}
