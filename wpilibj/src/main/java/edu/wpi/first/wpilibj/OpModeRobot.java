// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.ControlWord;
import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.RobotMode;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.opmode.Autonomous;
import edu.wpi.first.wpilibj.opmode.OpMode;
import edu.wpi.first.wpilibj.opmode.Teleop;
import edu.wpi.first.wpilibj.opmode.TestOpMode;
import edu.wpi.first.wpilibj.util.Color;
import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

/**
 * OpModeRobot implements the opmode-based robot program framework.
 *
 * <p>The OpModeRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>Classes annotated with {@link edu.wpi.first.wpilibj.opmode.Autonomous}, {@link
 * edu.wpi.first.wpilibj.opmode.Teleop}, and {@link edu.wpi.first.wpilibj.opmode.TestOpMode} in the
 * same package or subpackages as the user's subclass will be automatically registered as
 * autonomous, teleop, and test opmodes respectively.
 *
 * <p>Opmodes are constructed when selected on the driver station, and closed/no longer used when
 * the robot is disabled after being enabled or a different opmode is selected. When no opmode is
 * selected, nonePeriodic() is called. The driverStationConnected() function is called the first
 * time the driver station connects to the robot.
 */
public abstract class OpModeRobot extends RobotBase {
  private final ControlWord m_word = new ControlWord();
  private final Map<Long, Class<?>> m_opModes = new HashMap<>();
  private final AtomicReference<OpMode> m_activeOpMode = new AtomicReference<>(null);
  private final AtomicBoolean m_running = new AtomicBoolean();

  private void reportAddOpModeError(Class<?> cls, String message) {
    DriverStation.reportError("Error adding OpMode " + cls.getSimpleName() + ": " + message, false);
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
    Color tColor = textColor.isBlank() ? null : Color.fromString(textColor);
    Color bColor = backgroundColor.isBlank() ? null : Color.fromString(backgroundColor);
    long id = DriverStation.addOpMode(mode, name, description, group, tColor, bColor);
    m_opModes.put(id, cls);
  }

  /**
   * Adds an opmode for an opmode class annotated with Autonomous, Teleop, or TestOpMode. The class
   * must be a public, non-abstract subclass of OpMode with a public constructor that either takes
   * no arguments or accepts a single argument of this class's type.
   *
   * @param cls class to add
   */
  private void addAnnotatedOpModeImpl(
      Class<?> cls, Autonomous auto, Teleop teleop, TestOpMode test) {
    // the class must be a subclass of OpMode
    if (!OpMode.class.isAssignableFrom(cls)) {
      throw new IllegalArgumentException("not a subclass of OpMode");
    }
    int modifiers = cls.getModifiers();
    // it cannot be abstract
    if (java.lang.reflect.Modifier.isAbstract(modifiers)) {
      throw new IllegalArgumentException("is abstract");
    }
    // it must be public
    if (!java.lang.reflect.Modifier.isPublic(modifiers)) {
      throw new IllegalArgumentException("not public");
    }
    // it must not be a non-static inner class
    if (cls.getEnclosingClass() != null && !java.lang.reflect.Modifier.isStatic(modifiers)) {
      throw new IllegalArgumentException("is a non-static inner class");
    }
    // it must have a public no-arg constructor or a public constructor that accepts this class as
    // an argument
    try {
      cls.getConstructor(getClass());
    } catch (NoSuchMethodException e) {
      try {
        cls.getConstructor();
      } catch (NoSuchMethodException ex) {
        throw new IllegalArgumentException(
            "missing public no-arg constructor or constructor accepting "
                + getClass().getSimpleName());
      }
    }

    // add an opmode for each annotation
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
   * Adds an opmode for an opmode class annotated with Autonomous, Teleoperated, or TestOpMode. The
   * class must be a public, non-abstract subclass of OpMode with a public constructor that either
   * takes no arguments or accepts a single argument of this class's type.
   *
   * @param cls class to add
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addAnnotatedOpMode(Class<? extends OpMode> cls) {
    Autonomous auto = cls.getAnnotation(Autonomous.class);
    Teleop teleop = cls.getAnnotation(Teleop.class);
    TestOpMode test = cls.getAnnotation(TestOpMode.class);
    if (auto == null && teleop == null && test == null) {
      throw new IllegalArgumentException(
          "must be annotated with Autonomous, Teleoperated, or TestOpMode");
    }
    addAnnotatedOpModeImpl(cls, auto, teleop, test);
  }

  private void addAnnotatedOpModeClass(String name) {
    String className = name.replace('/', '.').substring(0, name.length() - 6);
    System.out.println("adding " + className);
    Class<?> cls;
    try {
      cls = Class.forName(className);
    } catch (ClassNotFoundException e) {
      return;
    }
    Autonomous auto = cls.getAnnotation(Autonomous.class);
    Teleop teleop = cls.getAnnotation(Teleop.class);
    TestOpMode test = cls.getAnnotation(TestOpMode.class);
    if (auto == null && teleop == null && test == null) {
      return;
    }
    try {
      addAnnotatedOpModeImpl(cls, auto, teleop, test);
    } catch (IllegalArgumentException e) {
      reportAddOpModeError(cls, e.getMessage());
    }
  }

  private void addAnnotatedOpModeClassesDir(File root, File dir, String packagePath) {
    File[] files = dir.listFiles();
    if (files == null) {
      return;
    }
    for (File file : files) {
      if (file.isDirectory()) {
        addAnnotatedOpModeClassesDir(root, file, packagePath);
      } else if (file.getName().endsWith(".class")) {
        String relPath = root.toPath().relativize(file.toPath()).toString().replace('\\', '/');
        addAnnotatedOpModeClass(packagePath + "." + relPath);
      }
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
    System.out.println("Scanning " + packageName + " " + packagePath);

    try {
      Enumeration<URL> resources = classLoader.getResources(packagePath);
      while (resources.hasMoreElements()) {
        URL resource = resources.nextElement();
        System.out.println("resource " + resource);
        if ("jar".equals(resource.getProtocol())) {
          String jarPath = resource.getPath().substring(5, resource.getPath().indexOf('!'));
          try (JarFile jar = new JarFile(jarPath)) {
            Enumeration<JarEntry> entries = jar.entries();
            while (entries.hasMoreElements()) {
              String name = entries.nextElement().getName();
              System.out.println(name);
              if (!name.startsWith(packagePath) || !name.endsWith(".class")) {
                continue;
              }
              addAnnotatedOpModeClass(name);
            }
          }
        } else if ("file".equals(resource.getProtocol())) {
          // Handle .class files in directories
          File dir = new File(resource.getPath());
          if (dir.exists() && dir.isDirectory()) {
            addAnnotatedOpModeClassesDir(dir, dir, packagePath);
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
    DriverStation.publishOpModes();
  }

  /**
   * Function called exactly once after the DS is connected.
   *
   * <p>Code that needs to know the DS state should go here.
   *
   * <p>Users should override this method for initialization that needs to occur after the DS is
   * connected, such as needing the alliance information.
   */
  public void driverStationConnected() {}

  /**
   * Function called periodically anytime when no opmode is selected, including when the Driver
   * Station is disconnected.
   */
  public void nonePeriodic() {}

  /**
   * Background monitor thread. On mode/opmode change, this checks to see if the change is actually
   * reflected in this class within a reasonable amount of time. If not, that means that the user
   * code is stuck and we need to take action to try to get it to exit (up to and including program
   * termination).
   */
  private void monitorThreadMain(Thread thr, long opmode, int event, int endEvent) {
    ControlWord word = new ControlWord();
    int[] events = {event, endEvent};
    while (true) {
      try {
        int[] signaled = WPIUtilJNI.waitForObjects(events);
        for (int val : signaled) {
          if (val < 0) {
            return; // handle destroyed
          }
        }
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
        return;
      }

      // did the opmode or enable state change?
      DriverStationJNI.getUncachedControlWord(word);
      if (!word.isEnabled() || word.getOpModeId() != opmode) {
        break;
      }
    }

    try {
      WPIUtilJNI.waitForObjectTimeout(endEvent, 0.2);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
      return;
    }

    // if it hasn't transitioned after 200 ms, call opmode stop
    OpMode opMode = m_activeOpMode.get();
    if (opMode != null) {
      DriverStation.reportWarning("OpMode did not exit, calling stop", false);
      opMode.opmodeStop();
    }

    try {
      WPIUtilJNI.waitForObjectTimeout(endEvent, 0.2);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
      return;
    }

    // if it hasn't transitioned after 400 ms, call thread.interrupt()
    DriverStation.reportError("OpMode did not exit, interrupting thread", false);
    thr.interrupt();

    try {
      WPIUtilJNI.waitForObjectTimeout(endEvent, 0.6);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
      return;
    }

    // if it hasn't transitioned after 1 second, terminate the program
    DriverStation.reportError("OpMode did not exit, terminating program", false);
    HAL.terminate();
    HAL.shutdown();
    System.exit(0);
  }

  /** Provide an alternate "main loop" via startCompetition(). */
  @Override
  public final void startCompetition() {
    System.out.println("********** Robot program startup complete **********");

    m_running.set(true);
    int event = WPIUtilJNI.createEvent(false, false);
    DriverStationJNI.provideNewDataEventHandle(event);

    try {
      // Implement the opmode lifecycle
      long lastModeId = -1;
      boolean calledDriverStationConnected = false;
      while (m_running.get()) {
        // Wait for new data from the driver station
        try {
          WPIUtilJNI.waitForObjectTimeout(event, 0.05);
        } catch (InterruptedException ex) {
          Thread.currentThread().interrupt();
          break;
        }

        // Get the latest control word and opmode
        DriverStation.refreshData();
        DriverStation.refreshControlWordFromCache(m_word);
        long modeId = DriverStation.getOpModeId();

        if (!calledDriverStationConnected && m_word.isDSAttached()) {
          calledDriverStationConnected = true;
          driverStationConnected();
        }

        if ((!DriverStation.isDSAttached() || modeId == 0) && m_activeOpMode.get() != null) {
          // no opmode selected but we're currently running one; close it
          OpMode opMode = m_activeOpMode.getAndSet(null);
          if (opMode != null) {
            // Close the previous opmode
            opMode.opmodeClose();
          }
        } else if (DriverStation.isDSAttached()
            && modeId != 0
            && (modeId != lastModeId || m_activeOpMode.get() == null)) {
          // New opmode selected, or closed after running once

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
          nonePeriodic();
          continue;
        }

        DriverStationJNI.observeUserProgram(m_word.getNative());
        if (m_word.isEnabled()) {
          // When enabled, call the opmode run function, then close and clear
          int endMonitor = WPIUtilJNI.createEvent(true, false);
          Thread curThread = Thread.currentThread();
          Thread monitor =
              new Thread(
                  () -> {
                    monitorThreadMain(curThread, modeId, event, endMonitor);
                  });
          monitor.start();
          try {
            opMode.opmodeRun(modeId);
          } catch (InterruptedException e) {
            Thread.interrupted();
          } finally {
            Thread.interrupted();
            WPIUtilJNI.destroyEvent(endMonitor);
            try {
              monitor.join();
            } catch (InterruptedException e) {
              Thread.currentThread().interrupt();
            }
          }
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
  public final void endCompetition() {
    m_running.set(false);
    OpMode opMode = m_activeOpMode.get();
    if (opMode != null) {
      opMode.opmodeStop();
    }
  }
}
