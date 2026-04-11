// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import static org.wpilib.units.Units.Seconds;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Modifier;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.function.Supplier;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import org.wpilib.driverstation.Alert;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.UserControls;
import org.wpilib.driverstation.UserControlsInstance;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.NotifierJNI;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.internal.PeriodicPriorityQueue;
import org.wpilib.internal.PeriodicPriorityQueue.Callback;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.OpMode;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;
import org.wpilib.opmode.TestOpMode;
import org.wpilib.smartdashboard.SmartDashboard;
import org.wpilib.system.RobotController;
import org.wpilib.system.Watchdog;
import org.wpilib.util.Color;
import org.wpilib.util.ConstructorMatch;

/**
 * OpModeRobot implements the opmode-based robot program framework.
 *
 * <p>The OpModeRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>Classes annotated with {@link Autonomous}, {@link Teleop}, and {@link TestOpMode} in the same
 * package or subpackages as the user's subclass are automatically registered as autonomous, teleop,
 * and test opmodes respectively.
 *
 * <p>Opmodes are constructed when selected on the driver station. While selected and disabled,
 * {@link PeriodicOpMode#disabledPeriodic()} is called. When enabled, {@link PeriodicOpMode#start()}
 * is called once and {@link PeriodicOpMode#periodic()} runs at the rate from {@link #getPeriod()}.
 * On disable or mode switch while enabled, {@link PeriodicOpMode#end()} is called asynchronously
 * and the opmode is then closed and discarded. When no opmode is selected, {@link #nonePeriodic()}
 * is called. {@link #driverStationConnected()} is called once when the DS first connects.
 */
public abstract class OpModeRobot extends RobotBase {
  private final ControlWord m_word = new ControlWord();

  private record OpModeFactory(String name, Supplier<OpMode> supplier) {}

  private final Map<Long, OpModeFactory> m_opModes = new HashMap<>();

  // Callback system fields (match C++ architecture)
  private final PeriodicPriorityQueue m_callbacks = new PeriodicPriorityQueue();
  private int m_notifier;
  private final double m_period;
  private final long m_startTimeUs;

  // OpMode lifecycle state
  private long m_lastModeId = -1;
  private boolean m_calledDriverStationConnected = false;
  private boolean m_lastEnabledState = false;
  private OpMode m_currentOpMode;
  private Callback m_currentOpModePeriodic;
  private final Set<Callback> m_activeOpModeCallbacks = new HashSet<>();
  private final Watchdog m_watchdog;
  private final Alert m_loopOverrunAlert;

  private static void reportAddOpModeError(Class<?> cls, String message) {
    DriverStation.reportError("Error adding OpMode " + cls.getSimpleName() + ": " + message, false);
  }

  private final Optional<Class<? extends UserControls>> m_userControlsBaseClass;
  private UserControls m_userControlsInstance;

  void setUserControlsInstance(UserControls userControlsInstance) {
    if (m_userControlsBaseClass.isEmpty()) {
      throw new IllegalStateException("No UserControls class specified");
    }

    if (!m_userControlsBaseClass.get().isAssignableFrom(userControlsInstance.getClass())) {
      throw new IllegalArgumentException(
          userControlsInstance.getClass().getSimpleName()
              + " is not assignable to "
              + m_userControlsBaseClass.get().getSimpleName());
    }
    m_userControlsInstance = userControlsInstance;
  }

  /**
   * Find a public constructor to instantiate the opmode. This constructor can have up to 2
   * parameters. The first parameter (if present) must be assignable from this.getClass(). The
   * second parameter (if present) must be assignable from DriverStationBase. If multiple, first
   * sort by most parameters, then by most specific first, then by most specific second.
   */
  private <T> Optional<ConstructorMatch<T>> findOpModeConstructor(Class<T> cls) {
    Optional<ConstructorMatch<T>> ctor;

    // try 2-parameter constructor
    if (m_userControlsInstance != null) {
      ctor =
          ConstructorMatch.findBestConstructor(cls, getClass(), m_userControlsInstance.getClass());
      if (ctor.isPresent()) {
        return ctor;
      }
    }

    // try 1-parameter constructor with RobotBase parameter
    ctor = ConstructorMatch.findBestConstructor(cls, getClass());
    if (ctor.isPresent()) {
      return ctor;
    }

    // try 1-parameter constructor with UserControls parameter
    if (m_userControlsInstance != null) {
      ctor = ConstructorMatch.findBestConstructor(cls, m_userControlsInstance.getClass());
      if (ctor.isPresent()) {
        return ctor;
      }
    }

    // try no-parameter constructor
    ctor = ConstructorMatch.findBestConstructor(cls);
    return ctor;
  }

  private <T extends OpMode> T constructOpModeClass(Class<T> cls) {
    Optional<ConstructorMatch<T>> constructor = findOpModeConstructor(cls);
    if (constructor.isEmpty()) {
      DriverStation.reportError(
          "No suitable constructor to instantiate OpMode " + cls.getSimpleName(), true);
      return null;
    }
    try {
      if (m_userControlsInstance != null) {
        return constructor.get().newInstance(this, m_userControlsInstance);
      } else {
        return constructor.get().newInstance(this);
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
    if (findOpModeConstructor(cls).isEmpty()) {
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
   * with a public constructor that either takes no arguments or accepts a single argument
   * assignable from this robot class type (the latter is preferred; if multiple match, the most
   * specific parameter type is used). It's necessary to call publishOpModes() to make the added
   * mode visible to the driver station.
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
   * with a public constructor that either takes no arguments or accepts a single argument
   * assignable from this robot class type (the latter is preferred; if multiple match, the most
   * specific parameter type is used). It's necessary to call publishOpModes() to make the added
   * mode visible to the driver station.
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
   * with a public constructor that either takes no arguments or accepts a single argument
   * assignable from this robot class type (the latter is preferred; if multiple match, the most
   * specific parameter type is used). It's necessary to call publishOpModes() to make the added
   * mode visible to the driver station.
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
   * with a public constructor that either takes no arguments or accepts a single argument
   * assignable from this robot class type (the latter is preferred; if multiple match, the most
   * specific parameter type is used). It's necessary to call publishOpModes() to make the added
   * mode visible to the driver station.
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
      Class<? extends OpMode> cls,
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
      Class<? extends OpMode> cls, Autonomous auto, Teleop teleop, TestOpMode test) {
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
   * constructor that either takes no arguments or accepts a single argument assignable from this
   * robot class type (if multiple match, the most specific parameter type is used). It's necessary
   * to call publishOpModes() to make the added mode visible to the driver station.
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
    Class<? extends OpMode> cls;
    try {
      cls = Class.forName(className).asSubclass(OpMode.class);
    } catch (ClassNotFoundException | ClassCastException e) {
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

  /** Default loop period. */
  public static final double DEFAULT_PERIOD = 0.02;

  /** Constructor with default period. */
  @SuppressWarnings("this-escape")
  public OpModeRobot() {
    this(DEFAULT_PERIOD);
  }

  /**
   * Constructor with specified period.
   *
   * @param period the period at which to run the robot and opmode periodic callbacks.
   */
  @SuppressWarnings("this-escape")
  public OpModeRobot(double period) {
    m_period = period;

    // Create our own notifier and callback queue (match C++)
    m_notifier = NotifierJNI.createNotifier();
    NotifierJNI.setNotifierName(m_notifier, "OpModeRobot");

    m_startTimeUs = RobotController.getMonotonicTime();

    m_loopOverrunAlert =
        new Alert("Loop time of \"" + m_period + "\"s overrun", Alert.Level.MEDIUM);
    m_watchdog = new Watchdog(Seconds.of(m_period), () -> m_loopOverrunAlert.set(true));

    // Add LoopFunc as periodic callback (match C++)
    addPeriodic(this::loopFunc, period);

    // Check to see if we have a DS annotation
    UserControlsInstance userControlsAnnotation =
        getClass().getAnnotation(UserControlsInstance.class);
    if (userControlsAnnotation != null) {
      m_userControlsBaseClass = Optional.of(userControlsAnnotation.value());
    } else {
      m_userControlsBaseClass = Optional.empty();
    }
    // Scan for annotated opmode classes within the derived class's package and subpackages
    addAnnotatedOpModeClasses(getClass().getPackage());
    DriverStation.publishOpModes();

    HAL.reportUsage("Framework", "OpModeRobot");
  }

  /**
   * Add a callback to run at a specific period.
   *
   * @param callback The callback to run.
   * @param period The period at which to run the callback.
   */
  public void addPeriodic(Runnable callback, double period) {
    m_callbacks.add(callback, m_startTimeUs, period);
  }

  /**
   * Get the period at which robot and opmode periodic callbacks are run.
   *
   * @return The period at which robot and opmode periodic callbacks are run.
   */
  public double getPeriod() {
    return m_period;
  }

  /**
   * Code that needs to know the DS state should go here.
   *
   * <p>Users should override this method for initialization that needs to occur after the DS is
   * connected, such as needing the alliance information.
   */
  public void driverStationConnected() {}

  /** Function called periodically every loop, regardless of enabled state or OpMode selection. */
  public void robotPeriodic() {}

  /** Function called once during robot initialization in simulation. */
  public void simulationInit() {}

  /** Function called periodically in simulation. */
  public void simulationPeriodic() {}

  /** Function called once when the robot becomes disabled. */
  public void disabledInit() {}

  /** Function called periodically while the robot is disabled. */
  public void disabledPeriodic() {}

  /** Function called once when the robot exits disabled state. */
  public void disabledExit() {}

  /**
   * Function called periodically anytime when no opmode is selected, including when the Driver
   * Station is disconnected.
   */
  public void nonePeriodic() {}

  /**
   * Return the system clock time in microseconds for the start of the current periodic loop. This
   * is in the same time base as Timer.getMonotonicTimestamp(), but is stable through a loop. It is
   * updated at the beginning of every periodic callback (including the normal periodic loop).
   *
   * @return Robot running time in microseconds, as of the start of the current periodic function.
   */
  public long getLoopStartTime() {
    return m_callbacks.getLoopStartTime();
  }

  /** Main robot loop function. Handles disabled state logic and opmode management. */
  private void loopFunc() {
    DriverStation.refreshData();

    // Get current enabled state and opmode
    DriverStation.refreshControlWordFromCache(m_word);
    m_watchdog.reset();
    boolean enabled = m_word.isEnabled();
    long modeId = m_word.isDSAttached() ? m_word.getOpModeId() : 0;

    if (!m_calledDriverStationConnected && m_word.isDSAttached()) {
      m_calledDriverStationConnected = true;
      driverStationConnected();
      m_watchdog.addEpoch("driverStationConnected()");
    }

    // Handle opmode changes
    if (modeId != m_lastModeId) {
      // Clean up current opmode
      if (m_currentOpMode != null) {
        // Remove opmode callbacks
        m_callbacks.remove(m_currentOpModePeriodic);
        m_callbacks.removeAll(m_activeOpModeCallbacks);
        m_activeOpModeCallbacks.clear();
        m_currentOpMode.end();
        m_currentOpMode.close();
        m_currentOpMode = null;
      }

      // Set up new opmode
      if (modeId != 0) {
        OpModeFactory factory = m_opModes.get(modeId);
        if (factory != null) {
          // Instantiate the new opmode
          System.out.println("********** Starting OpMode " + factory.name() + " **********");
          m_currentOpMode = factory.supplier().get();
          if (m_currentOpMode != null) {
            // Ensure disabledPeriodic is called at least once
            m_currentOpMode.disabledPeriodic();
            m_watchdog.addEpoch("opMode.disabledPeriodic()");
            // Register the opmode's periodic callbacks
            m_currentOpModePeriodic =
                m_callbacks.add(m_currentOpMode::periodic, m_startTimeUs, m_period);
            m_activeOpModeCallbacks.addAll(m_currentOpMode.getCallbacks());
            m_callbacks.addAll(m_activeOpModeCallbacks);
          }
        } else {
          DriverStation.reportError("No OpMode found for mode " + modeId, false);
        }
      }
      m_lastModeId = modeId;
    }

    // Handle enabled state changes
    boolean justCalledDisabledInit = false;
    if (m_lastEnabledState != enabled) {
      if (enabled) {
        // Transitioning to enabled
        disabledExit();
        m_watchdog.addEpoch("disabledExit()");
        if (m_currentOpMode != null) {
          m_currentOpMode.start();
          m_watchdog.addEpoch("opMode.start()");
        }
      } else {
        // Transitioning to disabled
        if (m_currentOpMode != null && m_lastEnabledState) {
          // Was enabled, now disabled
          m_currentOpMode.end();
          m_watchdog.addEpoch("opMode.end()");
        }
        disabledInit();
        m_watchdog.addEpoch("disabledInit()");
        justCalledDisabledInit = true;
      }
      m_lastEnabledState = enabled;
    }

    // Call periodic functions based on current state
    if (!enabled) {
      // Only call disabledPeriodic if we didn't just call disabledInit
      if (!justCalledDisabledInit) {
        disabledPeriodic();
        m_watchdog.addEpoch("disabledPeriodic()");
      }

      // Call opmode disabledPeriodic if we have one
      if (m_currentOpMode != null) {
        m_currentOpMode.disabledPeriodic();
        m_watchdog.addEpoch("opMode.disabledPeriodic()");
      }
    }

    // Call nonePeriodic when no opmode is selected
    if (DriverStation.getOpModeId() == 0) {
      nonePeriodic();
      m_watchdog.addEpoch("nonePeriodic()");
    }

    // Always call robotPeriodic
    robotPeriodic();
    m_watchdog.addEpoch("robotPeriodic()");

    // Always observe user program state
    DriverStationJNI.observeUserProgram(m_word.getNative());

    SmartDashboard.updateValues();
    m_watchdog.addEpoch("SmartDashboard.updateValues()");

    // Call simulationPeriodic if in simulation
    if (isSimulation()) {
      HAL.simPeriodicBefore();
      simulationPeriodic();
      HAL.simPeriodicAfter();
      m_watchdog.addEpoch("simulationPeriodic()");
    }

    m_watchdog.disable();

    // Flush NetworkTables
    NetworkTableInstance.getDefault().flushLocal();

    // Warn on loop time overruns
    if (m_watchdog.isExpired()) {
      m_watchdog.printEpochs();
    }
  }

  /** Provide an alternate "main loop" via startCompetition(). */
  @Override
  public final void startCompetition() {
    System.out.println("********** Robot program startup complete **********");

    if (isSimulation()) {
      simulationInit();
    }

    // Tell the DS that the robot is ready to be enabled
    DriverStation.observeUserProgramStarting();

    // Loop forever, calling the callback system which handles periodic functions
    while (true) {
      if (!m_callbacks.runCallbacks(m_notifier)) {
        break;
      }
    }
  }

  @Override
  public void close() {
    NotifierJNI.destroyNotifier(m_notifier);
  }

  /** Ends the main loop in startCompetition(). */
  @Override
  public final void endCompetition() {
    NotifierJNI.destroyNotifier(m_notifier);
  }

  /** Prints list of epochs added so far and their times. */
  public void printWatchdogEpochs() {
    m_watchdog.printEpochs();
  }
}
