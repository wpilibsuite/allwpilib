// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import java.io.File;
import org.wpilib.opmode.RobotBase;

/**
 * Class for interacting with the Filesystem, particularly, interacting with FRC-related paths on
 * the system, such as the launch and deploy directories.
 *
 * <p>This class is primarily used for obtaining resources in src/main/deploy, and the systemcore
 * path /home/systemcore in a simulation-compatible way.
 */
public final class Filesystem {
  private Filesystem() {}

  /**
   * Obtains the current working path that the program was launched with. This is analogous to the
   * `pwd` command on unix.
   *
   * @return The current working directory (launch directory)
   */
  public static File getLaunchDirectory() {
    // workaround for
    // https://www.chiefdelphi.com/t/filesystem-getdeploydirectory-returning-wrong-location-how-to-fix/427292
    String path =
        System.getProperty("user.dir")
            .replace(
                File.separator + "build" + File.separator + "jni" + File.separator + "release", "");
    return new File(path).getAbsoluteFile();
  }

  /**
   * Obtains the operating directory of the program. On the systemcore, this is /home/systemcore. In
   * simulation, it is where the simulation was launched from (`pwd`).
   *
   * @return The operating directory
   */
  public static File getOperatingDirectory() {
    if (!RobotBase.isSimulation()) {
      return new File("/home/systemcore");
    } else {
      return getLaunchDirectory();
    }
  }

  /**
   * Obtains the 'deploy' directory of the program, located at src/main/deploy, which is deployed by
   * default. On the systemcore, this is /home/systemcore/deploy. In simulation, it is where the
   * simulation was launched from, in the subdirectory "src/main/deploy" (`pwd`/src/main/deploy).
   *
   * @return The 'deploy' directory
   */
  public static File getDeployDirectory() {
    if (!RobotBase.isSimulation()) {
      return new File(getOperatingDirectory(), "deploy");
    } else {
      return new File(
          getOperatingDirectory(), "src" + File.separator + "main" + File.separator + "deploy");
    }
  }
}
