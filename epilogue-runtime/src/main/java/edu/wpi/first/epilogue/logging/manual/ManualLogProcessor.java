package edu.wpi.first.epilogue.logging.manual;

import static edu.wpi.first.wpilibj.Alert.AlertType.kError;

import edu.wpi.first.epilogue.EpilogueConfiguration;
import edu.wpi.first.epilogue.Logged;
import edu.wpi.first.epilogue.logging.EpilogueBackend;
import edu.wpi.first.epilogue.logging.NullBackend;
import edu.wpi.first.wpilibj.Alert;
import edu.wpi.first.wpilibj.TimedRobot;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;

/**
 * The parser for usage of the {@link ManualLogged} interface. To start, call <code>
 * LogPathInference.start(this);</code> in your robot class.
 */
@SuppressWarnings("PMD.AvoidAccessibilityAlteration")
public final class ManualLogProcessor {
  private static final Map<Object, String> logPathMap = new WeakHashMap<>();
  private static final NullBackend NULL_LOGGER = new NullBackend();

  private static boolean enabled;
  private static EpilogueConfiguration configuration = new EpilogueConfiguration();

  private static final Alert nullValuesWhenParsing =
      new Alert(
          "Manual logging parser has found null values. Make sure you call Epilogue.bind(this)"
              + "(or ManualLogging.enable) after all of your subsystems have been initialized.",
          kError);
  private static final Alert fetchingLoggerBeforeInit =
      new Alert(
          "You have attempted to log values before Epilogue.bind(this)"
              + "(or ManualLogging.enable) has been called. Make sure to not log "
              + "values in the constructor",
          kError);
  private static final Alert failedToFetchLogger = new Alert("", kError);
  private static final List<String> couldNotFetchLoggersOf = new ArrayList<>();

  /**
   * Fetches the global epilogue logger for manual logging.
   *
   * @return The epilogue logger used for global logging
   */
  public static EpilogueBackend globalLogger() {
    return configuration.backend;
  }

  static EpilogueBackend nestedLoggerOf(Object object) {
    if (!enabled) {
      fetchingLoggerBeforeInit.set(true);
      return NULL_LOGGER;
    }
    var logPath = logPathMap.get(object);
    if (logPath == null) {
      couldNotFetchLoggersOf.add(object.toString());
      failedToFetchLogger.setText(
          "Objects " + couldNotFetchLoggersOf + " could not fetch their loggers");
      failedToFetchLogger.set(true);
      return NULL_LOGGER;
    }
    return configuration.backend.getNested(logPath);
  }

  /**
   * Enables log path parsing. This must be called in your robot class to use this interface.
   *
   * @param robotInstance The TimedRobot instance
   * @param config Your current epilogue configuration.
   */
  public static void enable(TimedRobot robotInstance, EpilogueConfiguration config) {
    if (enabled) {
      return;
    }
    enabled = true;
    configuration = config;
    recurseLogPaths(robotInstance, "");
  }

  private ManualLogProcessor() {}

  private static void recurseLogPaths(Object obj, String currentPath) {
    if (obj == null) {
      nullValuesWhenParsing.set(true);
      return;
    }
    logPathMap.put(obj, currentPath);
    var clazz = obj.getClass();
    for (var field : clazz.getDeclaredFields()) {
      try {
        if (ManualLogged.class.isAssignableFrom(field.getType())) {
          field.setAccessible(true);
          recurseLogPaths(field.get(obj), currentPath + "/" + computeLogName(field));
        } else {
          var arraySubtype = field.getType().getComponentType();
          if (arraySubtype == null || !ManualLogged.class.isAssignableFrom(arraySubtype)) {
            continue;
          }
          field.setAccessible(true);
          int index = 0;
          var name = computeLogName(field);
          for (Object value : (Object[]) field.get(obj)) {
            recurseLogPaths(value, currentPath + "/" + name + "/" + index);
            index++;
          }
        }
      } catch (IllegalAccessException e) {
        throw new RuntimeException(e);
      }
    }
  }

  private static String computeLogName(Field field) {
    Logged logAnno = field.getAnnotation(Logged.class);
    return logAnno == null || logAnno.name().isEmpty() ? field.getName() : logAnno.name();
  }
}
