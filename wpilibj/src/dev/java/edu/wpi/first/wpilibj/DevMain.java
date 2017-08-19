package edu.wpi.first.wpilibj;

import edu.wpi.first.wpiutil.RuntimeDetector;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.networktables.NetworkTablesJNI;

public class DevMain {
  /**
   * Main entry point.
   */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    System.out.println(RuntimeDetector.getPlatformPath());
    System.out.println(NetworkTablesJNI.now());
    System.out.println(HALUtil.getHALRuntimeType());
  }
}
