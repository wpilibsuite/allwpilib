// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.framework;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.BiConsumer;
import java.util.function.Consumer;
import java.util.function.Supplier;
import org.wpilib.backend.NetworkTablesTelemetryBackend;
import org.wpilib.backend.NetworkTablesTunableBackend;
import org.wpilib.driverstation.DriverStationErrors;
import org.wpilib.driverstation.RobotState;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.HALUtil;
import org.wpilib.internal.UnitTelemetry;
import org.wpilib.math.util.MathShared;
import org.wpilib.math.util.MathSharedStore;
import org.wpilib.networktables.IntegerPublisher;
import org.wpilib.networktables.MultiSubscriber;
import org.wpilib.networktables.NetworkTableEvent;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.system.RuntimeType;
import org.wpilib.system.Timer;
import org.wpilib.system.WPILibVersion;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.units.Measure;
import org.wpilib.units.Unit;
import org.wpilib.util.Alert;
import org.wpilib.util.WPIUtilJNI;

/**
 * Implement a Robot Program framework. The RobotBase class is intended to be subclassed to create a
 * robot program. The user must implement {@link #startCompetition()}, which will be called once and
 * is not expected to exit. The user must also implement {@link #endCompetition()}, which signals to
 * the code in {@link #startCompetition()} that it should exit.
 *
 * <p>It is not recommended to subclass this class directly - instead subclass IterativeRobotBase or
 * TimedRobot.
 */
public abstract class RobotBase implements AutoCloseable {
  private static final String PROGRAM_START_TIME_TOPIC = "/Robot/ProgramStartTime";
  private static final AtomicInteger s_nextWarningReporterId = new AtomicInteger();

  /** The ID of the main Java thread. */
  // This is usually 1, but it is best to make sure
  private static long m_threadId = -1;

  private final MultiSubscriber m_suball;
  private final IntegerPublisher m_programStartTimePublisher;
  private final Map<String, Alert> m_telemetryWarningAlerts = new HashMap<>();
  private final Map<String, Alert> m_tunableWarningAlerts = new HashMap<>();
  private final BiConsumer<String, String> m_telemetryWarningReporter =
      this::reportTelemetryWarning;
  private final Consumer<String> m_tunableWarningReporter = this::reportTunableWarning;
  private final String m_warningAlertIdPrefix =
      "warning" + s_nextWarningReporterId.getAndIncrement() + "_";
  private final AtomicInteger m_nextWarningAlertId = new AtomicInteger();

  private final int m_connListenerHandle;

  private static void setupMathShared() {
    MathSharedStore.setMathShared(
        new MathShared() {
          @Override
          public void reportError(String error, StackTraceElement[] stackTrace) {
            DriverStationErrors.reportError(error, stackTrace);
          }

          @Override
          public void reportUsage(String resource, String data) {
            HAL.reportUsage(resource, data);
          }

          @Override
          public double getTimestamp() {
            return Timer.getTimestamp();
          }
        });
  }

  private void reportTelemetryWarning(String path, String msg) {
    reportWarningAlert(
        m_telemetryWarningAlerts,
        "Telemetry",
        path + '\n' + msg,
        "Telemetry '" + path + "': warning: " + msg);
  }

  private void reportTunableWarning(String msg) {
    reportWarningAlert(m_tunableWarningAlerts, "Tunables", msg, "Tunable warning: " + msg);
  }

  private void reportWarningAlert(
      Map<String, Alert> alerts, String group, String key, String text) {
    synchronized (alerts) {
      Alert alert = alerts.get(key);
      if (alert == null) {
        alert =
            new Alert(
                group,
                m_warningAlertIdPrefix + m_nextWarningAlertId.getAndIncrement(),
                text,
                Alert.Level.MEDIUM);
        alerts.put(key, alert);
      }

      alert.setText(text);
      alert.set(true);
    }
  }

  private static void closeWarningAlerts(Map<String, Alert> alerts) {
    synchronized (alerts) {
      for (Alert alert : alerts.values()) {
        alert.close();
      }
      alerts.clear();
    }
  }

  @SuppressWarnings("rawtypes")
  private static class TunableMeasure extends Tunable<Measure> implements Tunable.CustomTunable {
    TunableMeasure(Measure initialValue, TunableConfig config) {
      super(config);
      m_unit = initialValue.unit();
      m_baseUnit = m_unit.getBaseUnit();
      m_value = initialValue;
      m_magnitudeTunable =
          new TunableDouble(config, true) {
            @Override
            public void set(double value) {
              m_value = m_unit.ofBaseUnits(value);
              markChanged();
            }

            @Override
            public double get() {
              return m_value.baseUnitMagnitude();
            }
          };
    }

    @Override
    public void set(Measure value) {
      if (!m_baseUnit.equivalent(value.unit().getBaseUnit())) {
        throw new IllegalArgumentException(
            "Measure unit " + value.unit() + " is not compatible with " + m_unit);
      }
      m_magnitudeTunable.set(value.baseUnitMagnitude());
    }

    @Override
    public Measure get() {
      return m_value;
    }

    @Override
    public Measure mutate() {
      m_magnitudeTunable.set(m_value.baseUnitMagnitude());
      return m_value;
    }

    @Override
    public TunableDouble getInnerTunable() {
      return m_magnitudeTunable;
    }

    @Override
    public boolean hasChanged() {
      return m_magnitudeTunable.hasChanged();
    }

    @Override
    public boolean supportsChangeNotification() {
      return m_magnitudeTunable.supportsChangeNotification();
    }

    @Override
    public void resetChanged() {
      m_magnitudeTunable.resetChanged();
    }

    @Override
    public Class<Measure> getTypeClass() {
      return Measure.class;
    }

    private final Unit m_unit;
    private final Unit m_baseUnit;
    private Measure m_value;
    private final TunableDouble m_magnitudeTunable;
  }

  /**
   * Constructor for a generic robot program. User code can be placed in the constructor that runs
   * before the Autonomous or Operator Control period starts. The constructor will run to completion
   * before Autonomous is entered.
   *
   * <p>This must be used to ensure that the communications code starts. In the future it would be
   * nice to put this code into its own task that loads on boot so ensure that it runs.
   */
  @SuppressWarnings("this-escape")
  protected RobotBase() {
    final NetworkTableInstance inst = NetworkTableInstance.getDefault();
    m_threadId = Thread.currentThread().threadId();
    setupMathShared();
    // subscribe to "" to force persistent values to propagate to local
    m_suball = new MultiSubscriber(inst, new String[] {""}, PubSubOption.DISABLE_SIGNAL);
    if (!isSimulation()) {
      inst.startServer("/home/systemcore/networktables.json", "", "robot");
    } else {
      inst.startServer("networktables.json", "", "robot");
    }

    // set up telemetry
    TelemetryRegistry.setReportWarning(m_telemetryWarningReporter);
    TelemetryRegistry.registerBackend("", new NetworkTablesTelemetryBackend(inst, "/Telemetry"));
    TelemetryRegistry.registerTypeHandler(
        Measure.class,
        (table, name, value) -> {
          UnitTelemetry.log(table, name, value);
        });

    // set up tunables
    TunableRegistry.setReportWarning(m_tunableWarningReporter);
    TunableRegistry.registerBackend("", new NetworkTablesTunableBackend(inst, "/Tunables"));
    TunableRegistry.registerTypeHandler(
        Measure.class,
        (initialValue, config) -> {
          if (config == null) {
            config = new TunableConfig();
          }
          return new TunableMeasure(
              initialValue,
              config.withProperty(
                  "unit", UnitTelemetry.getUnitMetadata(initialValue.unit().getBaseUnit())));
        });

    // wait for the NT server to actually start
    try {
      int count = 0;
      while (inst.getNetworkMode().contains(NetworkTableInstance.NetworkMode.STARTING)) {
        Thread.sleep(10);
        count++;
        if (count > 100) {
          throw new InterruptedException();
        }
      }
    } catch (InterruptedException ex) {
      System.err.println("timed out while waiting for NT server to start");
    }

    m_programStartTimePublisher = inst.getIntegerTopic(PROGRAM_START_TIME_TOPIC).publish();
    m_programStartTimePublisher.set(WPIUtilJNI.getProgramStartTime());

    m_connListenerHandle =
        inst.addConnectionListener(
            false,
            event -> {
              if (event.is(NetworkTableEvent.Kind.CONNECTED)) {
                HAL.reportUsage("NT/" + event.connInfo.remoteId, "");
              }
            });
  }

  /**
   * Returns the main thread ID.
   *
   * @return The main thread ID.
   */
  public static long getMainThreadId() {
    return m_threadId;
  }

  @Override
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public void close() {
    if (TelemetryRegistry.getReportWarning() == m_telemetryWarningReporter) {
      TelemetryRegistry.setReportWarning(null);
    }
    if (TunableRegistry.getReportWarning() == m_tunableWarningReporter) {
      TunableRegistry.setReportWarning(null);
    }
    closeWarningAlerts(m_telemetryWarningAlerts);
    closeWarningAlerts(m_tunableWarningAlerts);
    m_suball.close();
    m_programStartTimePublisher.close();
    NetworkTableInstance.getDefault().removeListener(m_connListenerHandle);
  }

  /**
   * Get the current runtime type.
   *
   * @return Current runtime type.
   */
  public static RuntimeType getRuntimeType() {
    return RuntimeType.getValue(HALUtil.getHALRuntimeType());
  }

  /**
   * Get if the robot is a simulation.
   *
   * @return If the robot is running in simulation.
   */
  public static boolean isSimulation() {
    return getRuntimeType() == RuntimeType.SIMULATION;
  }

  /**
   * Get if the robot is real.
   *
   * @return If the robot is running in the real world.
   */
  public static boolean isReal() {
    RuntimeType runtimeType = getRuntimeType();
    return runtimeType == RuntimeType.SYSTEMCORE;
  }

  /**
   * Determine if the Robot is currently disabled.
   *
   * @return True if the Robot is currently disabled by the Driver Station.
   */
  public static boolean isDisabled() {
    return RobotState.isDisabled();
  }

  /**
   * Determine if the Robot is currently enabled.
   *
   * @return True if the Robot is currently enabled by the Driver Station.
   */
  public static boolean isEnabled() {
    return RobotState.isEnabled();
  }

  /**
   * Determine if the robot is currently in Autonomous mode as determined by the Driver Station.
   *
   * @return True if the robot is currently operating Autonomously.
   */
  public static boolean isAutonomous() {
    return RobotState.isAutonomous();
  }

  /**
   * Determine if the robot is currently in Autonomous mode and enabled as determined by the Driver
   * Station.
   *
   * @return True if the robot is currently operating autonomously while enabled.
   */
  public static boolean isAutonomousEnabled() {
    return RobotState.isAutonomousEnabled();
  }

  /**
   * Determine if the robot is currently in Utility mode as determined by the Driver Station.
   *
   * @return True if the robot is currently operating in Utility mode.
   */
  public static boolean isUtility() {
    return RobotState.isUtility();
  }

  /**
   * Determine if the robot is current in Utility mode and enabled as determined by the Driver
   * Station.
   *
   * @return True if the robot is currently operating in Utility mode while enabled.
   */
  public static boolean isUtilityEnabled() {
    return RobotState.isUtilityEnabled();
  }

  /**
   * Determine if the robot is currently in Operator Control mode as determined by the Driver
   * Station.
   *
   * @return True if the robot is currently operating in Tele-Op mode.
   */
  public static boolean isTeleop() {
    return RobotState.isTeleop();
  }

  /**
   * Determine if the robot is currently in Operator Control mode and enabled as determined by the
   * Driver Station.
   *
   * @return True if the robot is currently operating in Tele-Op mode while enabled.
   */
  public static boolean isTeleopEnabled() {
    return RobotState.isTeleopEnabled();
  }

  /**
   * Gets the currently selected operating mode of the driver station. Note this does not mean the
   * robot is enabled; use isEnabled() for that.
   *
   * @return the unique ID provided by the DriverStation.addOpMode() function; may return 0 or a
   *     unique ID not added, so callers should be prepared to handle that case
   */
  public static long getOpModeId() {
    return RobotState.getOpModeId();
  }

  /**
   * Gets the currently selected operating mode of the driver station. Note this does not mean the
   * robot is enabled; use isEnabled() for that.
   *
   * @return Operating mode string; may return a string not in the list of options, so callers
   *     should be prepared to handle that case
   */
  public static String getOpMode() {
    return RobotState.getOpMode();
  }

  /**
   * Start the main robot code. This function will be called once and should not exit until
   * signalled by {@link #endCompetition()}
   */
  public abstract void startCompetition();

  /** Ends the main loop in {@link #startCompetition()}. */
  public abstract void endCompetition();

  private static final ReentrantLock m_runMutex = new ReentrantLock();
  private static RobotBase m_robotCopy;
  private static boolean m_suppressExitWarning;

  /**
   * Gets the Robot subclass name from a stack trace.
   *
   * @param elements The stack trace elements to walk.
   * @return The Robot subclass name.
   */
  protected static String getRobotName(StackTraceElement[] elements) {
    // Walk bottom to top to account for multiple layers of subclassing
    for (int i = elements.length - 1; i >= 0; i--) {
      StackTraceElement element = elements[i];
      try {
        // Skip our own class when walking
        if (RobotBase.class.equals(Class.forName(element.getClassName()))) {
          continue;
        }
        if (RobotBase.class.isAssignableFrom(Class.forName(element.getClassName()))) {
          return element.getClassName();
        }
      } catch (ClassNotFoundException e) {
        // Unreachable
      }
    }
    return "Unknown";
  }

  /** Run the robot main loop. */
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  private static <T extends RobotBase> void runRobot(Supplier<T> robotConstructor) {
    System.out.println("********** Robot program starting **********");

    T robot;
    try {
      robot = robotConstructor.get();
    } catch (Throwable throwable) {
      Throwable cause = throwable.getCause();
      if (cause != null) {
        throwable = cause;
      }
      StackTraceElement[] elements = throwable.getStackTrace();
      String robotName = getRobotName(elements);
      DriverStationErrors.reportError(
          "Unhandled exception instantiating robot " + robotName + " " + throwable, elements);
      DriverStationErrors.reportError(
          "The robot program quit unexpectedly."
              + " This is usually due to a code error.\n"
              + "  The above stacktrace can help determine where the error occurred.\n"
              + "  See https://wpilib.org/stacktrace for more information.\n",
          false);
      DriverStationErrors.reportError("Could not instantiate robot " + robotName + "!", false);
      DriverStationErrors.reportCrash("Could not instantiate robot " + robotName + "!", elements);
      return;
    }

    m_runMutex.lock();
    m_robotCopy = robot;
    m_runMutex.unlock();

    boolean errorOnExit = false;
    try {
      robot.startCompetition();
    } catch (Throwable throwable) {
      Throwable cause = throwable.getCause();
      if (cause != null) {
        throwable = cause;
      }
      DriverStationErrors.reportError(
          "Unhandled exception: " + throwable, throwable.getStackTrace());
      DriverStationErrors.reportCrash(
          "Unhandled exception: " + throwable, throwable.getStackTrace());
      errorOnExit = true;
    } finally {
      m_runMutex.lock();
      boolean suppressExitWarning = m_suppressExitWarning;
      m_runMutex.unlock();
      if (!suppressExitWarning) {
        // startCompetition never returns unless exception occurs....
        DriverStationErrors.reportWarning(
            "The robot program quit unexpectedly."
                + " This is usually due to a code error.\n"
                + "  The above stacktrace can help determine where the error occurred.\n"
                + "  See https://wpilib.org/stacktrace for more information.",
            false);
        if (errorOnExit) {
          DriverStationErrors.reportError(
              "The startCompetition() method (or methods called by it) should have "
                  + "handled the exception above.",
              false);
        } else {
          DriverStationErrors.reportError(
              "Unexpected return from startCompetition() method.", false);
        }
      }
    }
  }

  /**
   * Suppress the "The robot program quit unexpectedly." message.
   *
   * @param value True if exit warning should be suppressed.
   */
  public static void suppressExitWarning(boolean value) {
    m_runMutex.lock();
    m_suppressExitWarning = value;
    m_runMutex.unlock();
  }

  /**
   * Starting point for the applications.
   *
   * @param <T> Robot subclass.
   * @param robotConstructor Robot constructor.
   */
  public static <T extends RobotBase> void startRobot(Supplier<T> robotConstructor) {
    // Check that the MSVC runtime is valid.
    WPIUtilJNI.checkMsvcRuntime();

    if (!HAL.initialize()) {
      throw new IllegalStateException("Failed to initialize. Terminating");
    }

    // Force refresh DS data
    DriverStationBackend.refreshData();

    HAL.reportUsage("Language", "Java");
    HAL.reportUsage("WPILibVersion", WPILibVersion.Version);
    HAL.publishWpilibVersion(WPILibVersion.Version + " (Java)");

    if (HAL.hasMain()) {
      Thread thread =
          new Thread(
              () -> {
                runRobot(robotConstructor);
                HAL.exitMain();
              },
              "robot main");
      thread.setDaemon(true);
      thread.start();
      HAL.runMain();
      suppressExitWarning(true);
      m_runMutex.lock();
      RobotBase robot = m_robotCopy;
      m_runMutex.unlock();
      if (robot != null) {
        robot.endCompetition();
      }
      try {
        thread.join(1000);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    } else {
      runRobot(robotConstructor);
    }

    // On RIO, this will just terminate rather than shutting down cleanly (it's a no-op in sim).
    // It's not worth the risk of hanging on shutdown when we want the code to restart as quickly
    // as possible.
    HAL.terminate();

    HAL.shutdown();

    System.exit(0);
  }
}
