// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.Modifier;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Supplier;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.NotifierJNI;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.LinearOpMode;
import org.wpilib.opmode.OpMode;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;
import org.wpilib.opmode.TestOpMode;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.Watchdog;
import org.wpilib.util.Color;
import org.wpilib.util.WPIUtilJNI;

/**
 * OpModeRobot implements the opmode-based robot program framework.
 *
 * <p>The OpModeRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>Classes annotated with {@link Autonomous}, {@link Teleop}, and {@link TestOpMode} in the same
 * package or subpackages as the user's subclass will be automatically registered as autonomous,
 * teleop, and test opmodes respectively.
 *
 * <p>Opmodes are constructed when selected on the driver station, and closed/no longer used when
 * the robot is disabled after being enabled or a different opmode is selected. When no opmode is
 * selected, nonePeriodic() is called. The driverStationConnected() function is called the first
 * time the driver station connects to the robot.
 */
public abstract class OpModeRobot extends RobotBase {
  private final ControlWord m_word = new ControlWord();

  private record OpModeFactory(String name, Supplier<OpMode> supplier) {}

  private final Map<Long, OpModeFactory> m_opModes = new HashMap<>();
  private final AtomicReference<OpMode> m_activeOpMode = new AtomicReference<>(null);
  private volatile int m_notifier;

  private static void reportAddOpModeError(Class<?> cls, String message) {
    DriverStation.reportError("Error adding OpMode " + cls.getSimpleName() + ": " + message, false);
  }

  /**
   * Find a public constructor to instantiate the opmode. Prefer a single-arg public constructor
   * whose parameter type is assignable from this.getClass() (if multiple, pick the most specific
   * parameter type). Otherwise return the public no-arg constructor. Return null if neither exists.
   */
  private Constructor<?> findOpModeConstructor(Class<?> cls) {
    Constructor<?> bestCtor = null;
    Class<?> bestParam = null;
    for (Constructor<?> ctor : cls.getConstructors()) {
      Class<?>[] params = ctor.getParameterTypes();
      if (params.length != 1) {
        continue;
      }
      Class<?> param = params[0];
      if (!param.isAssignableFrom(getClass())) {
        continue;
      }
      if (bestCtor == null || bestParam.isAssignableFrom(param)) {
        bestCtor = ctor;
        bestParam = param;
      }
    }
    if (bestCtor != null) {
      return bestCtor;
    }
    try {
      return cls.getConstructor();
    } catch (NoSuchMethodException e) {
      return null;
    }
  }

  private OpMode constructOpModeClass(Class<?> cls) {
    Constructor<?> constructor = findOpModeConstructor(cls);
    if (constructor == null) {
      DriverStation.reportError(
          "No suitable constructor to instantiate OpMode " + cls.getSimpleName(), true);
      return null;
    }
    try {
      if (constructor.getParameterCount() == 1) {
        return (OpMode) constructor.newInstance(this);
      } else {
        return (OpMode) constructor.newInstance();
      }
    } catch (ReflectiveOperationException e) {
      DriverStation.reportError(
          "Could not instantiate OpMode " + cls.getSimpleName(), e.getStackTrace());
      return null;
    }
  }

  private void checkOpModeClass(Class<?> cls) {
    // the class must be a subclass of OpMode
    if (!OpMode.class.isAssignableFrom(cls)) {
      throw new IllegalArgumentException("not a subclass of OpMode");
    }
    int modifiers = cls.getModifiers();
    // it cannot be abstract
    if (Modifier.isAbstract(modifiers)) {
      throw new IllegalArgumentException("is abstract");
    }
    // it must be public
    if (!Modifier.isPublic(modifiers)) {
      throw new IllegalArgumentException("not public");
    }
    // it must not be a non-static inner class
    if (cls.getEnclosingClass() != null && !Modifier.isStatic(modifiers)) {
      throw new IllegalArgumentException("is a non-static inner class");
    }
    // it must have a public no-arg constructor or a public constructor that accepts this class
    // (or a superclass/interface) as an argument
    if (findOpModeConstructor(cls) == null) {
      throw new IllegalArgumentException(
          "missing public no-arg constructor or constructor accepting "
              + getClass().getSimpleName());
    }
  }

  /**
   * Adds an opmode using a factory function that creates the opmode. It's necessary to call
   * publishOpModes() to make the added mode visible to the driver station.
   *
   * @param factory factory function to create the opmode
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color, or null for default
   * @param backgroundColor background color, or null for default
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpModeFactory(
      Supplier<OpMode> factory,
      RobotMode mode,
      String name,
      String group,
      String description,
      Color textColor,
      Color backgroundColor) {
    long id = DriverStation.addOpMode(mode, name, group, description, textColor, backgroundColor);
    m_opModes.put(id, new OpModeFactory(name, factory));
  }

  /**
   * Adds an opmode using a factory function that creates the opmode. It's necessary to call
   * publishOpModes() to make the added mode visible to the driver station.
   *
   * @param factory factory function to create the opmode
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpModeFactory(
      Supplier<OpMode> factory, RobotMode mode, String name, String group, String description) {
    addOpModeFactory(factory, mode, name, group, description, null, null);
  }

  /**
   * Adds an opmode using a factory function that creates the opmode. It's necessary to call
   * publishOpModes() to make the added mode visible to the driver station.
   *
   * @param factory factory function to create the opmode
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpModeFactory(
      Supplier<OpMode> factory, RobotMode mode, String name, String group) {
    addOpModeFactory(factory, mode, name, group, "");
  }

  /**
   * Adds an opmode using a factory function that creates the opmode. It's necessary to call
   * publishOpModes() to make the added mode visible to the driver station.
   *
   * @param factory factory function to create the opmode
   * @param mode robot mode
   * @param name name of the operating mode
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpModeFactory(Supplier<OpMode> factory, RobotMode mode, String name) {
    addOpModeFactory(factory, mode, name, "");
  }

  /**
   * Adds an opmode for an opmode class. The class must be a public, non-abstract subclass of OpMode
   * with a public constructor that either takes no arguments or accepts a single argument of this
   * class's type (the latter is preferred). It's necessary to call publishOpModes() to make the
   * added mode visible to the driver station.
   *
   * @param cls class to add
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @param textColor text color, or null for default
   * @param backgroundColor background color, or null for default
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpMode(
      Class<? extends OpMode> cls,
      RobotMode mode,
      String name,
      String group,
      String description,
      Color textColor,
      Color backgroundColor) {
    checkOpModeClass(cls);
    addOpModeFactory(
        () -> constructOpModeClass(cls),
        mode,
        name,
        group,
        description,
        textColor,
        backgroundColor);
  }

  /**
   * Adds an opmode for an opmode class. The class must be a public, non-abstract subclass of OpMode
   * with a public constructor that either takes no arguments or accepts a single argument of this
   * class's type (the latter is preferred). It's necessary to call publishOpModes() to make the
   * added mode visible to the driver station.
   *
   * @param cls class to add
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @param description description of the operating mode
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpMode(
      Class<? extends OpMode> cls, RobotMode mode, String name, String group, String description) {
    addOpMode(cls, mode, name, group, description, null, null);
  }

  /**
   * Adds an opmode for an opmode class. The class must be a public, non-abstract subclass of OpMode
   * with a public constructor that either takes no arguments or accepts a single argument of this
   * class's type (the latter is preferred). It's necessary to call publishOpModes() to make the
   * added mode visible to the driver station.
   *
   * @param cls class to add
   * @param mode robot mode
   * @param name name of the operating mode
   * @param group group of the operating mode
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpMode(Class<? extends OpMode> cls, RobotMode mode, String name, String group) {
    addOpMode(cls, mode, name, group, "");
  }

  /**
   * Adds an opmode for an opmode class. The class must be a public, non-abstract subclass of OpMode
   * with a public constructor that either takes no arguments or accepts a single argument of this
   * class's type (the latter is preferred). It's necessary to call publishOpModes() to make the
   * added mode visible to the driver station.
   *
   * @param cls class to add
   * @param mode robot mode
   * @param name name of the operating mode
   * @throws IllegalArgumentException if class does not meet criteria
   */
  public void addOpMode(Class<? extends OpMode> cls, RobotMode mode, String name) {
    addOpMode(cls, mode, name, "");
  }

  private void addOpModeClassImpl(
      Class<?> cls,
      RobotMode mode,
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
    long id = DriverStation.addOpMode(mode, name, group, description, tColor, bColor);
    m_opModes.put(id, new OpModeFactory(name, () -> constructOpModeClass(cls)));
  }

  private void addAnnotatedOpModeImpl(
      Class<?> cls, Autonomous auto, Teleop teleop, TestOpMode test) {
    checkOpModeClass(cls);

    // add an opmode for each annotation
    if (auto != null) {
      addOpModeClassImpl(
          cls,
          RobotMode.AUTONOMOUS,
          auto.name(),
          auto.group(),
          auto.description(),
          auto.textColor(),
          auto.backgroundColor());
    }
    if (teleop != null) {
      addOpModeClassImpl(
          cls,
          RobotMode.TELEOPERATED,
          teleop.name(),
          teleop.group(),
          teleop.description(),
          teleop.textColor(),
          teleop.backgroundColor());
    }
    if (test != null) {
      addOpModeClassImpl(
          cls,
          RobotMode.TEST,
          test.name(),
          test.group(),
          test.description(),
          test.textColor(),
          test.backgroundColor());
    }
  }

  /**
   * Adds an opmode for an opmode class annotated with {@link Autonomous}, {@link Teleop}, or {@link
   * TestOpMode}. The class must be a public, non-abstract subclass of OpMode with a public
   * constructor that either takes no arguments or accepts a single argument of this class's type.
   * It's necessary to call publishOpModes() to make the added mode visible to the driver station.
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
          "must be annotated with Autonomous, Teleop, or TestOpMode");
    }
    addAnnotatedOpModeImpl(cls, auto, teleop, test);
  }

  private void addAnnotatedOpModeClass(String name) {
    // trim ".class" from end
    String className = name.replace('/', '.').substring(0, name.length() - 6);
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
   * {@link Autonomous}, {@link Teleop}, or {@link TestOpMode} and registers them. It's necessary to
   * call publishOpModes() to make the added modes visible to the driver station.
   *
   * @param pkg package to scan
   */
  public void addAnnotatedOpModeClasses(Package pkg) {
    String packageName = pkg.getName();
    String packagePath = packageName.replace('.', '/');
    ClassLoader classLoader = Thread.currentThread().getContextClassLoader();

    try {
      Enumeration<URL> resources = classLoader.getResources(packagePath);
      while (resources.hasMoreElements()) {
        URL resource = resources.nextElement();
        if ("jar".equals(resource.getProtocol())) {
          // Get path of JAR file from URL path (format "file:<path_to_jar_file>!/path_to_entry")
          String jarPath = resource.getPath().substring(5, resource.getPath().indexOf('!'));
          try (JarFile jar = new JarFile(jarPath)) {
            Enumeration<JarEntry> entries = jar.entries();
            while (entries.hasMoreElements()) {
              String name = entries.nextElement().getName();
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

  /**
   * Removes an operating mode option. It's necessary to call publishOpModes() to make the removed
   * mode no longer visible to the driver station.
   *
   * @param mode robot mode
   * @param name name of the operating mode
   */
  public void removeOpMode(RobotMode mode, String name) {
    long id = DriverStation.removeOpMode(mode, name);
    if (id != 0) {
      m_opModes.remove(id);
    }
  }

  /** Publishes the operating mode options to the driver station. */
  public void publishOpModes() {
    DriverStation.publishOpModes();
  }

  /** Clears all operating mode options and publishes an empty list to the driver station. */
  public void clearOpModes() {
    DriverStation.clearOpModes();
    m_opModes.clear();
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
   * Station is disconnected. By default, this function calls {@link OpModeRobot#disabledPeriodic}.
   */
  public void nonePeriodic() {
    disabledPeriodic();
  }

  /**
   * Function called periodically when the robot is disabled. When an OpMode is selected, this is
   * called after the OpMode-specific {@link OpMode#disabledPeriodic} function.
   */
  public void disabledPeriodic() {}

  /**
   * Function called periodically by {@link PeriodicOpMode} following the OpMode-specific {@link
   * PeriodicOpMode#periodic} function. This is not called by {@link LinearOpMode}.
   */
  public void robotPeriodic() {}

  /**
   * Function called periodically during simulation by {@link PeriodicOpMode} following {@link
   * OpModeRobot#robotPeriodic}. This is not called by {@link LinearOpMode}.
   */
  public void simulationPeriodic() {}

  /**
   * Internal periodic function. This is responsible for calling {@link OpModeRobot#robotPeriodic}
   * and {@link OpModeRobot#simulationPeriodic} (during simulation). {@link PeriodicOpMode}
   * automatically calls this every loop, but {@link LinearOpMode} does not.
   *
   * @param watchdog watchdog instance, typically passed in from the calling {@link PeriodicOpMode}.
   */
  public final void internalRobotPeriodic(Watchdog watchdog) {
    robotPeriodic();
    watchdog.addEpoch("robotPeriodic()");

    SmartDashboard.updateValues();
    watchdog.addEpoch("SmartDashboard.updateValues()");

    if (isSimulation()) {
      HAL.simPeriodicBefore();
      simulationPeriodic();
      HAL.simPeriodicAfter();
      watchdog.addEpoch("simulationPeriodic()");
    }
  }

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

    // call opmode stop
    OpMode opMode = m_activeOpMode.get();
    if (opMode != null) {
      opMode.opModeStop();
    }

    events[0] = m_notifier;
    NotifierJNI.setNotifierAlarm(m_notifier, 200000, 0, false, true); // 200 ms
    try {
      int[] signaled = WPIUtilJNI.waitForObjects(events);
      for (int val : signaled) {
        if (val < 0 || val == endEvent) {
          return; // transitioned, or handle destroyed
        }
      }
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
      return;
    }

    // if it hasn't transitioned after 200 ms, call thread.interrupt()
    DriverStation.reportError("OpMode did not exit, interrupting thread", false);
    thr.interrupt();

    NotifierJNI.setNotifierAlarm(m_notifier, 800000, 0, false, true); // 800 ms
    try {
      int[] signaled = WPIUtilJNI.waitForObjects(events);
      for (int val : signaled) {
        if (val < 0 || val == endEvent) {
          return; // transitioned, or handle destroyed
        }
      }
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
    DriverStationJNI.observeUserProgramStarting();

    int event = WPIUtilJNI.createEvent(false, false);
    DriverStationJNI.provideNewDataEventHandle(event);

    m_notifier = NotifierJNI.createNotifier();
    NotifierJNI.setNotifierName(m_notifier, "OpModeRobot");

    try {
      // Implement the opmode lifecycle
      long lastModeId = -1;
      boolean calledDriverStationConnected = false;
      int[] events = {event, m_notifier};
      while (true) {
        // Wait for new data from the driver station, with 50 ms timeout
        NotifierJNI.setNotifierAlarm(m_notifier, 50000, 0, false, true);
        try {
          int[] signaled = WPIUtilJNI.waitForObjects(events);
          for (int val : signaled) {
            if (val < 0) {
              return; // handle destroyed
            }
          }
        } catch (InterruptedException ex) {
          Thread.currentThread().interrupt();
          break;
        }

        // Get the latest control word and opmode
        DriverStation.refreshData();
        DriverStation.refreshControlWordFromCache(m_word);

        if (!calledDriverStationConnected && m_word.isDSAttached()) {
          calledDriverStationConnected = true;
          driverStationConnected();
        }

        long modeId;
        if (!m_word.isDSAttached()) {
          modeId = 0;
        } else {
          modeId = m_word.getOpModeId();
        }

        OpMode opMode = m_activeOpMode.get();
        if (opMode == null || modeId != lastModeId) {
          if (opMode != null) {
            // no or different opmode selected
            m_activeOpMode.set(null);
            opMode.opModeClose();
          }

          if (modeId == 0) {
            // no opmode selected
            nonePeriodic();
            DriverStationJNI.observeUserProgram(m_word.getNative());
            continue;
          }

          OpModeFactory factory = m_opModes.get(modeId);
          if (factory == null) {
            DriverStation.reportError("No OpMode found for mode " + modeId, false);
            m_word.setOpModeId(0);
            DriverStationJNI.observeUserProgram(m_word.getNative());
            continue;
          }

          // Instantiate the opmode
          System.out.println("********** Starting OpMode " + factory.name() + " **********");
          opMode = factory.supplier().get();
          if (opMode == null) {
            // could not construct
            m_word.setOpModeId(0);
            DriverStationJNI.observeUserProgram(m_word.getNative());
            continue;
          }
          m_activeOpMode.set(opMode);
          lastModeId = modeId;
          // Ensure disabledPeriodic is always called at least once
          opMode.disabledPeriodic();
          this.disabledPeriodic();
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
            opMode.opModeRun(modeId);
          } catch (InterruptedException e) {
            // ignored
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
            opMode.opModeClose();
          }
        } else {
          // When disabled, call the disabledPeriodic function
          opMode.disabledPeriodic();
        }
      }
    } finally {
      DriverStationJNI.removeNewDataEventHandle(event);
      WPIUtilJNI.destroyEvent(event);
      NotifierJNI.destroyNotifier(m_notifier);
    }
  }

  /** Ends the main loop in startCompetition(). */
  @Override
  public final void endCompetition() {
    NotifierJNI.destroyNotifier(m_notifier);
    OpMode opMode = m_activeOpMode.get();
    if (opMode != null) {
      opMode.opModeStop();
    }
  }
}
