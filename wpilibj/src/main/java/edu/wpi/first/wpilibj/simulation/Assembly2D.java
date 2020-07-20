/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import java.util.HashMap;

public class Assembly2D {
  SimDevice m_device = SimDevice.create("Assembly2D");
  HashMap<String, SimDouble> createdItems = new HashMap<String, SimDouble>();

  /**
     * Set/Create the angle of a ligament
     *
     * @param ligamentPath json path to the ligament
     * @param angle        to set the ligament
     */
  public void setLigamentAngle(String ligamentPath, float angle) {
      ligamentPath = ligamentPath + "angle/";
    if (m_device != null) {
//        System.out.println("java: " + ligamentPath + " " + angle);
        if(!createdItems.containsKey(ligamentPath)){
            createdItems.put(ligamentPath, m_device.createDouble(ligamentPath, false, angle));
        } else{
            createdItems.get(ligamentPath).set(angle);
        }
    }
  }

    /**
     * Set/Create the length of a ligament
     *
     * @param ligamentPath json path to the ligament
     * @param length       to set the ligament
     */
  public void setLigamentLength(String ligamentPath, float length) {
      ligamentPath = ligamentPath + "length/";
//      System.out.println("java length : " + ligamentPath + " " + length);
    if (m_device != null) {
        if(!createdItems.containsKey(ligamentPath)){
            createdItems.put(ligamentPath, m_device.createDouble(ligamentPath, false, length));
        } else{
            createdItems.get(ligamentPath).set(length);
        }
    }
  }
}
