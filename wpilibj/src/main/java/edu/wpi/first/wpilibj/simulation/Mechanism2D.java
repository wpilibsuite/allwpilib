/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import java.util.HashMap;
import java.util.Map;

public class Mechanism2D {
  private static final SimDevice m_device = SimDevice.create("Mechanism2D");
  private static final Map<String, SimDouble> m_createdItems = new HashMap<String, SimDouble>();

  /**
     * Set/Create the angle of a ligament.
     *
     * @param ligamentPath json path to the ligament
     * @param angle        to set the ligament
     */
  public void setLigamentAngle(String ligamentPath, float angle) {
    ligamentPath = ligamentPath + "/angle";
    if (m_device != null) {
      if (!m_createdItems.containsKey(ligamentPath)) {
        m_createdItems.put(ligamentPath, m_device.createDouble(ligamentPath, false, angle));
      }
      m_createdItems.get(ligamentPath).set(angle);
    }
  }

  /**
   * Set/Create the length of a ligament.
   *
   * @param ligamentPath json path to the ligament
   * @param length       to set the ligament
   */
  public void setLigamentLength(String ligamentPath, float length) {
    ligamentPath = ligamentPath + "/length";
    if (m_device != null) {
      if (!m_createdItems.containsKey(ligamentPath)) {
        m_createdItems.put(ligamentPath, m_device.createDouble(ligamentPath, false, length));
      }
      m_createdItems.get(ligamentPath).set(length);
    }
  }
}
