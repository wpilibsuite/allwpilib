// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNull;

import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.simulation.SendableChooserSim;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class SendableChooserTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    SmartDashboard.setNetworkTableInstance(m_inst);
  }

  @ValueSource(ints = {0, 1, 2, 3})
  @ParameterizedTest
  void returnsSelected(int toSelect) {
    try (var chooser = new SendableChooser<Integer>();
        var chooserSim =
            new SendableChooserSim(
                m_inst, "/SmartDashboard/returnsSelectedChooser" + toSelect + "/")) {
      for (int i = 1; i <= 3; i++) {
        chooser.addOption(String.valueOf(i), i);
      }
      chooser.setDefaultOption(String.valueOf(0), 0);

      SmartDashboard.putData("returnsSelectedChooser" + toSelect, chooser);
      SmartDashboard.updateValues();
      chooserSim.setSelected(String.valueOf(toSelect));
      SmartDashboard.updateValues();
      assertEquals(toSelect, chooser.getSelected());
      assertEquals(String.valueOf(toSelect), chooser.getSelectedName());
    }
  }

  @Test
  void defaultIsReturnedOnNoSelect() {
    try (var chooser = new SendableChooser<Integer>()) {
      for (int i = 1; i <= 3; i++) {
        chooser.addOption(String.valueOf(i), i);
      }
      chooser.setDefaultOption(String.valueOf(0), 0);

      assertEquals(0, chooser.getSelected());
      assertEquals(String.valueOf(0), chooser.getSelectedName());
    }
  }

  @Test
  void nullIsReturnedOnNoSelectAndNoDefault() {
    try (var chooser = new SendableChooser<Integer>()) {
      for (int i = 1; i <= 3; i++) {
        chooser.addOption(String.valueOf(i), i);
      }

      assertNull(chooser.getSelected());
      assertEquals("", chooser.getSelectedName());
    }
  }

  @Test
  void test1ArgChangeListener() {
    try (var chooser = new SendableChooser<Integer>();
        var chooserSim =
            new SendableChooserSim(m_inst, "/SmartDashboard/1ArgChangeListenerChooser/")) {
      for (int i = 1; i <= 3; i++) {
        chooser.addOption(String.valueOf(i), i);
      }
      AtomicInteger currentVal = new AtomicInteger();
      chooser.onChange(currentVal::set);

      SmartDashboard.putData("1ArgChangeListenerChooser", chooser);
      SmartDashboard.updateValues();
      chooserSim.setSelected("3");
      SmartDashboard.updateValues();
      assertEquals(3, currentVal.get());
    }
  }

  @Test
  void test2ArgChangeListener() {
    try (var chooser = new SendableChooser<Integer>();
        var chooserSim =
            new SendableChooserSim(m_inst, "/SmartDashboard/2ArgChangeListenerChooser/")) {
      for (int i = 1; i <= 3; i++) {
        chooser.addOption(String.valueOf(i), i);
      }

      AtomicReference<String> currentName = new AtomicReference<>("");
      AtomicInteger currentVal = new AtomicInteger();
      chooser.onChange(
          (String name, Integer value) -> {
            currentName.set(name);
            currentVal.set(value);
          });

      SmartDashboard.putData("2ArgChangeListenerChooser", chooser);
      SmartDashboard.updateValues();
      chooserSim.setSelected("3");
      SmartDashboard.updateValues();
      assertEquals(3, currentVal.get());
      assertEquals("3", currentName.get());
    }
  }

  @AfterEach
  void tearDown() {
    m_inst.close();
    SmartDashboard.setNetworkTableInstance(NetworkTableInstance.getDefault());
  }
}
