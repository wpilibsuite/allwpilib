// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.util.Color8Bit;

class Mechanism2dTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    SmartDashboard.setNetworkTableInstance(m_inst);
  }

  @Test
  void testCanvas() {
    try (var mechanism = new Mechanism2d(5, 10);
        var dimsEntry = m_inst.getEntry("/SmartDashboard/mechanism/dims");
        var colorEntry = m_inst.getEntry("/SmartDashboard/mechanism/backgroundColor")) {
      SmartDashboard.putData("mechanism", mechanism);
      SmartDashboard.updateValues();
      assertArrayEquals(new double[] {5, 10}, dimsEntry.getDoubleArray(new double[0]));
      assertEquals("#000020", colorEntry.getString(""));
      mechanism.setBackgroundColor(new Color8Bit(255, 255, 255));
      SmartDashboard.updateValues();
      assertEquals("#FFFFFF", colorEntry.getString(""));
    }
  }

  @Test
  void testRoot() {
    try (var mechanism = new Mechanism2d(5, 10);
        var xEntry = m_inst.getEntry("/SmartDashboard/mechanism/root/x");
        var yEntry = m_inst.getEntry("/SmartDashboard/mechanism/root/y")) {
      final var root = mechanism.getRoot("root", 1, 2);
      SmartDashboard.putData("mechanism", mechanism);
      SmartDashboard.updateValues();
      assertEquals(1, xEntry.getDouble(0));
      assertEquals(2, yEntry.getDouble(0));
      root.setPosition(2, 4);
      SmartDashboard.updateValues();
      assertEquals(2, xEntry.getDouble(0));
      assertEquals(4, yEntry.getDouble(0));
    }
  }

  @Test
  void testLigament() {
    try (var mechanism = new Mechanism2d(5, 10);
        var angleEntry = m_inst.getEntry("/SmartDashboard/mechanism/root/ligament/angle");
        var colorEntry = m_inst.getEntry("/SmartDashboard/mechanism/root/ligament/color");
        var lengthEntry = m_inst.getEntry("/SmartDashboard/mechanism/root/ligament/length");
        var weightEntry = m_inst.getEntry("/SmartDashboard/mechanism/root/ligament/weight")) {
      var root = mechanism.getRoot("root", 1, 2);
      var ligament =
          root.append(new MechanismLigament2d("ligament", 3, 90, 1, new Color8Bit(255, 255, 255)));
      SmartDashboard.putData("mechanism", mechanism);
      SmartDashboard.updateValues();
      assertEquals(ligament.getAngle(), angleEntry.getDouble(0));
      assertEquals(ligament.getColor().toHexString(), colorEntry.getString(""));
      assertEquals(ligament.getLength(), lengthEntry.getDouble(0));
      assertEquals(ligament.getLineWeight(), weightEntry.getDouble(0));
      ligament.setAngle(45);
      ligament.setColor(new Color8Bit(0, 0, 0));
      ligament.setLength(2);
      ligament.setLineWeight(4);
      SmartDashboard.updateValues();
      assertEquals(ligament.getAngle(), angleEntry.getDouble(0));
      assertEquals(ligament.getColor().toHexString(), colorEntry.getString(""));
      assertEquals(ligament.getLength(), lengthEntry.getDouble(0));
      assertEquals(ligament.getLineWeight(), weightEntry.getDouble(0));
      angleEntry.setDouble(22.5);
      colorEntry.setString("#FF00FF");
      lengthEntry.setDouble(4);
      weightEntry.setDouble(6);
      SmartDashboard.updateValues();
      assertEquals(ligament.getAngle(), angleEntry.getDouble(0));
      assertEquals(ligament.getColor().toHexString(), colorEntry.getString(""));
      assertEquals(ligament.getLength(), lengthEntry.getDouble(0));
      assertEquals(ligament.getLineWeight(), weightEntry.getDouble(0));
    }
  }

  @AfterEach
  void tearDown() {
    m_inst.close();
    SmartDashboard.setNetworkTableInstance(NetworkTableInstance.getDefault());
  }
}
