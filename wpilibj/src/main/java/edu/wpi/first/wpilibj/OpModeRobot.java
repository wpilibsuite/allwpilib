// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.RobotMode;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.opmode.Autonomous;
import edu.wpi.first.wpilibj.opmode.OpMode;
import edu.wpi.first.wpilibj.opmode.Teleoperated;
import edu.wpi.first.wpilibj.opmode.TestOpMode;
import edu.wpi.first.wpilibj.util.Color;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public abstract class OpModeRobot extends RobotBase {
  private final DSControlWord m_word = new DSControlWord();
  private final Map<Long, Class<?>> m_opModes = new HashMap<>();
  private final AtomicReference<OpMode> m_activeOpMode = new AtomicReference<>(null);

  private void reportAddOpModeError(Class<?> cls, String message) {
    DriverStation.reportError("Error adding OpMode " + cls.getSimpleName() + ": " + message, false);
  }

  private Color colorFromString(String str) {
    if (str == null || str.isEmpty()) {
      return null;
    }

    // #RRGGBB style
    if (str.charAt(0) == '#') {
      return new Color(str);
    }

    // RGB style
    if (str.startsWith("rgb(") && str.endsWith(")")) {
      String[] components = str.substring(4, str.length() - 1).split(",");
      if (components.length != 3) {
        return null;
      }
      try {
        int r = Integer.parseInt(components[0].trim());
        int g = Integer.parseInt(components[1].trim());
        int b = Integer.parseInt(components[2].trim());
        return new Color(r / 255.0, g / 255.0, b / 255.0);
      } catch (NumberFormatException e) {
        return null;
      }
    }

    // try to parse as a named color by matching against k-prefixed static constants in the Color
    // class
    String search = str.startsWith("k") ? str : "k" + str;
    for (var field : Color.class.getFields()) {
      if (java.lang.reflect.Modifier.isStatic(field.getModifiers())
          && Color.class.isAssignableFrom(field.getType())
          && field.getName().equalsIgnoreCase(search)) {
        try {
          return (Color) field.get(null);
        } catch (IllegalAccessException e) {
          // Ignore and continue
        }
      }
    }

    return null;
  }

  private void addOpMode(
      RobotMode mode,
      Class<?> cls,
      String name,
      String group,
      String description,
      String textColor,
      String backgroundColor) {
    if (name == null || name.isBlank()) {
      name = cls.getSimpleName();
    }
    long id;
    try {
      id =
          DriverStation.addOpMode(
              mode,
              name,
              description,
              group,
              colorFromString(textColor),
              colorFromString(backgroundColor));
    } catch (IllegalArgumentException e) {
      reportAddOpModeError(cls, "duplicate name");
      return;
    }
    m_opModes.put(id, cls);
  }

  /**
   * Adds an opmode for an opmode class annotated with Autonomous, Teleoperated, or TestOpMode. The
   * class must be a public, non-abstract subclass of OpMode with a public constructor that either
   * takes no arguments or accepts a single argument of this class's type.
   *
   * @param cls class to add
   */
  public void addAnnotatedOpMode(Class<?> cls) {
    // the class must be a subclass of OpMode
    if (!OpMode.class.isAssignableFrom(cls)) {
      reportAddOpModeError(cls, "not a subclass of OpMode");
      return;
    }
    int modifiers = cls.getModifiers();
    // it cannot be abstract
    if (java.lang.reflect.Modifier.isAbstract(modifiers)) {
      reportAddOpModeError(cls, "is abstract");
      return;
    }
    // it must be public
    if (!java.lang.reflect.Modifier.isPublic(modifiers)) {
      reportAddOpModeError(cls, "not public");
      return;
    }
    // it must not be a non-static inner class
    if (cls.getEnclosingClass() != null && !java.lang.reflect.Modifier.isStatic(modifiers)) {
      reportAddOpModeError(cls, "is a non-static inner class");
      return;
    }
    // it must have a public no-arg constructor or a public constructor that accepts this class as
    // an argument
    try {
      cls.getConstructor(getClass());
    } catch (NoSuchMethodException e) {
      try {
        cls.getConstructor();
      } catch (NoSuchMethodException ex) {
        reportAddOpModeError(
            cls,
            "missing public no-arg constructor or constructor accepting "
                + getClass().getSimpleName());
        return;
      }
    }

    // add an opmode for each annotation
    Autonomous auto = cls.getAnnotation(Autonomous.class);
    Teleoperated teleop = cls.getAnnotation(Teleoperated.class);
    TestOpMode test = cls.getAnnotation(TestOpMode.class);
    if (auto != null) {
      addOpMode(
          RobotMode.AUTONOMOUS,
          cls,
          auto.name(),
          auto.group(),
          auto.description(),
          auto.textColor(),
          auto.backgroundColor());
    }
    if (teleop != null) {
      addOpMode(
          RobotMode.TELEOPERATED,
          cls,
          teleop.name(),
          teleop.group(),
          teleop.description(),
          teleop.textColor(),
          teleop.backgroundColor());
    }
    if (test != null) {
      addOpMode(
          RobotMode.TEST,
          cls,
          test.name(),
          test.group(),
          test.description(),
          test.textColor(),
          test.backgroundColor());
    }
  }

  /**
   * Scans for classes in the specified package and all nested packages that are annotated with
   * Autonomous, Teleoperated, or TestOpMode and registers them.
   *
   * @param pkg package to scan
   */
  private void addAnnotatedOpModeClasses(Package pkg) {
    String packageName = pkg.getName();
    String packagePath = packageName.replace('.', '/');
    ClassLoader classLoader = Thread.currentThread().getContextClassLoader();

    try {
      Enumeration<URL> resources = classLoader.getResources(packagePath);
      while (resources.hasMoreElements()) {
        URL resource = resources.nextElement();
        if ("jar".equals(resource.getProtocol())) {
          String jarPath = resource.getPath().substring(5, resource.getPath().indexOf('!'));
          try (JarFile jar = new JarFile(jarPath)) {
            Enumeration<JarEntry> entries = jar.entries();
            while (entries.hasMoreElements()) {
              String name = entries.nextElement().getName();
              if (name.startsWith(packagePath) && name.endsWith(".class")) {
                String className = name.replace('/', '.').substring(0, name.length() - 6);
                try {
                  addAnnotatedOpMode(Class.forName(className));
                } catch (ClassNotFoundException e) {
                  // Ignore
                }
              }
            }
          }
        }
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  /** Constructor. */
  @SuppressWarnings("this-escape")
  public OpModeRobot() {
    // Scan for annotated opmode classes within the derived class's package and subpackages
    addAnnotatedOpModeClasses(getClass().getPackage());
  }

  /** Provide an alternate "main loop" via startCompetition(). */
  @Override
  public void startCompetition() {
    System.out.println("********** Robot program startup complete **********");

    int event = WPIUtilJNI.createEvent(false, false);
    DriverStationJNI.provideNewDataEventHandle(event);

    try {
      // Implement the opmode lifecycle
      long lastModeId = -1;
      while (true) {
        // Wait for new data from the driver station
        try {
          WPIUtilJNI.waitForObject(event);
        } catch (InterruptedException ex) {
          Thread.currentThread().interrupt();
          break;
        }

        // Get the latest control word and opmode
        boolean prevEnabled = m_word.isEnabled();
        m_word.refresh();
        long modeId = DriverStation.getOpModeId();

        if (modeId != 0
            && (modeId != lastModeId
                || (!prevEnabled && m_word.isEnabled() && m_activeOpMode.get() == null))) {
          // New opmode selected, or re-enabled after running once

          OpMode opMode = m_activeOpMode.getAndSet(null);
          if (opMode != null) {
            // Close the previous opmode
            opMode.opmodeClose();
          }

          Class<?> cls = m_opModes.get(modeId);
          if (cls == null) {
            DriverStation.reportError("No OpMode found for mode " + modeId, false);
            continue;
          }

          // Instantiate the opmode
          System.out.println("********** Starting OpMode " + cls.getSimpleName() + " **********");
          Constructor<?> constructor;
          boolean hasThis;
          try {
            constructor = cls.getConstructor(getClass());
            hasThis = true;
          } catch (NoSuchMethodException e) {
            try {
              constructor = cls.getConstructor();
            } catch (NoSuchMethodException e2) {
              DriverStation.reportError(
                  "Could not instantiate OpMode " + cls.getSimpleName(), e2.getStackTrace());
              continue;
            }
            hasThis = false;
          }
          try {
            if (hasThis) {
              opMode = (OpMode) constructor.newInstance(this);
            } else {
              opMode = (OpMode) constructor.newInstance();
            }
          } catch (ReflectiveOperationException e) {
            DriverStation.reportError(
                "Could not instantiate OpMode " + cls.getSimpleName(), e.getStackTrace());
            continue;
          }
          m_activeOpMode.set(opMode);
          if (lastModeId == -1) {
            // Tell the DS that the robot is ready
            DriverStationJNI.observeUserProgramStarting();
          }
          lastModeId = modeId;
        }

        OpMode opMode = m_activeOpMode.get();
        if (opMode == null) {
          continue;
        }

        DriverStationJNI.observeUserProgramOpMode(modeId, m_word.isEnabled());
        if (m_word.isEnabled()) {
          // When enabled, call the opmode run function, then close and clear
          opMode.opmodeRun(lastModeId);
          opMode = m_activeOpMode.getAndSet(null);
          if (opMode != null) {
            opMode.opmodeClose();
          }
        } else {
          // When disabled, call the disabledPeriodic function
          opMode.disabledPeriodic();
        }
      }
    } finally {
      DriverStationJNI.removeNewDataEventHandle(event);
      WPIUtilJNI.destroyEvent(event);
    }
  }

  /** Ends the main loop in startCompetition(). */
  @Override
  public void endCompetition() {
    OpMode opMode = m_activeOpMode.getAndSet(null);
    if (opMode != null) {
      opMode.opmodeStop();
    }
  }
}
