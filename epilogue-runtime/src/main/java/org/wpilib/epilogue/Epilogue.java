// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue;

import java.util.IdentityHashMap;
import java.util.Map;
import java.util.ServiceLoader;
import java.util.function.Consumer;
import org.wpilib.driverstation.DriverStationErrors;
import org.wpilib.framework.RobotBase;
import org.wpilib.framework.TimedRobot;
import org.wpilib.util.UsageReporting;
import org.wpilib.util.container.WeakLinkedHashSet;

/** Main entry point for using Epilogue telemetry at runtime. */
public final class Epilogue {
  private Epilogue() {}

  @SuppressWarnings("rawtypes")
  private static final Map<Object, EpilogueService> s_loggers = new IdentityHashMap<>();

  private static final WeakLinkedHashSet<Object> s_knownUnloggable = new WeakLinkedHashSet<>();

  @SuppressWarnings({"rawtypes", "PMD.UseProperClassLoader"})
  private static final ServiceLoader<EpilogueService> s_loader =
      ServiceLoader.load(EpilogueService.class, Epilogue.class.getClassLoader());

  private static final EpilogueConfiguration s_config = new EpilogueConfiguration();

  /**
   * Applies a configuration function to the current configuration. The full set of options can be
   * seen in {@link EpilogueConfiguration}.
   *
   * {@snippet lang="java":
   * Epilogue.configure(config -> {
   *   config.root = "Custom Telemetry Root";
   *   // ... among other options
   * });
   * }
   *
   * @param configurator the configuration function to apply
   * @see EpilogueConfiguration the full set of configurable options
   */
  public static void configure(Consumer<EpilogueConfiguration> configurator) {
    configurator.accept(s_config);
  }

  /**
   * Gets the current set of config options.
   *
   * @return the current configuration object
   */
  public static EpilogueConfiguration getConfig() {
    return s_config;
  }

  /**
   * Checks if data associated with a given importance level should be logged.
   *
   * @param importance the importance level to check
   * @return true if the importance level is at or above the configured minimum
   * @see EpilogueConfiguration#minimumImportance
   * @see #configure(Consumer)
   */
  public static boolean shouldLog(Logged.Importance importance) {
    return importance.compareTo(s_config.minimumImportance) >= 0;
  }

  /**
   * Updates telemetry for the specified robot instance. Call this in a periodic method like {@link
   * TimedRobot#robotPeriodic()}.
   *
   * {@snippet lang="java":
   * import module wpilib;
   *
   * @Logged
   * public class Robot extends TimedRobot {
   *   @Override
   *   public void robotPeriodic() {
   *     Epilogue.update(this);
   *   }
   * }
   * }
   *
   * @param <R> the robot type
   * @param robot the robot instance to update
   * @return true if telemetry was updated, false otherwise
   */
  @SuppressWarnings("unchecked")
  public static <R extends RobotBase> boolean update(R robot) {
    if (s_knownUnloggable.contains(robot)) {
      // We already know this will fail, so we can skip the rest of the method.
      return false;
    }

    if (missingLogger(robot)) {
      s_loader.stream()
          .map(ServiceLoader.Provider::get)
          .filter(s -> s.supportsExactly(robot))
          .findFirst()
          .ifPresent(service -> s_loggers.put(robot, service));
    }
    if (missingLogger(robot)) {
      DriverStationErrors.reportWarning(buildMissingLoggerMessage(robot), false);
      s_knownUnloggable.add(robot);
      return false;
    } else {
      UsageReporting.reportUsage("Epilogue", "update");
    }

    s_loggers.get(robot).update(robot);
    return true;
  }

  /**
   * Binds periodic telemetry updates to the specified timed robot. Updates will execute at the
   * frequency specified in {@link #getConfig()}, with a phase offset of {@link
   * EpilogueConfiguration#loggingPeriodOffset} to avoid collisions with the main robot task.
   *
   * <p>This method is offered as a convenience for setting up telemetry in the robot constructor;
   * it is not required to use Epilogue. Calling {@link #update(RobotBase)} in {@link
   * TimedRobot#robotPeriodic()} would also work and would provide better timing control without
   * phase offsets.
   *
   * {@snippet lang="java":
   * import module wpilib;
   *
   * @Logged
   * public class Robot extends TimedRobot {
   *   public Robot() {
   *      Epilogue.bind(this);
   *   }
   * }
   * }
   *
   * @param <R> the timed robot type
   * @param robot the timed robot instance to bind
   * @return true if a matching bindable logger service was found and bound, false otherwise
   */
  @SuppressWarnings({"unchecked", "rawtypes"})
  public static <R extends TimedRobot> boolean bind(R robot) {
    var maybeLogger =
        s_loader.stream()
            .filter(s -> EpilogueService.Bindable.class.isAssignableFrom(s.type()))
            .map(ServiceLoader.Provider::get)
            .filter(s -> s.supportsExactly(robot))
            .findFirst();

    if (maybeLogger.isPresent()) {
      // The cast should always be safe because we checked the type in the stream above
      var logger = (EpilogueService.Bindable<R>) maybeLogger.get();
      logger.bind(robot);
      s_loggers.put(robot, logger);
      UsageReporting.reportUsage("Epilogue", "bind");
      return true;
    } else {
      DriverStationErrors.reportWarning(buildMissingLoggerMessage(robot), false);
      s_knownUnloggable.add(robot);
      return false;
    }
  }

  private static boolean missingLogger(Object robot) {
    return !s_loggers.containsKey(robot);
  }

  private static String buildMissingLoggerMessage(RobotBase robot) {
    StringBuilder msg =
        new StringBuilder(256)
            .append("[EPILOGUE] No logger was found for ")
            .append(robot.getClass().getName())
            .append('.');
    if (!robot.getClass().isAnnotationPresent(Logged.class)) {
      msg.append(" Does the class have a @Logged annotation?");
    }
    if (robot.getClass().getModule() != null) {
      msg.append(
              " Ensure module-info.java has a `provides org.wpilib.epilogue.EpilogueService with ")
          .append(robot.getClass().getName())
          .append("_EpilogueService` statement.");
    }
    return msg.toString();
  }
}
