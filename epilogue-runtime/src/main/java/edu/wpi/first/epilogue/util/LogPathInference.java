package edu.wpi.first.epilogue.util;

import edu.wpi.first.epilogue.Logged;
import java.lang.reflect.Field;
import java.util.Map;
import java.util.WeakHashMap;

/**
 * The parser for usage of the {@link InferLogPath} interface. To start, call <code>
 * LogPathInference.start(this);</code> in your robot class.
 */
@SuppressWarnings("PMD.AvoidAccessibilityAlteration")
public final class LogPathInference {
  static final Map<Object, String> logPathMap = new WeakHashMap<>();
  static final String DEFAULT_NAMESPACE = "UNKNOWN";
  private static boolean enabled;

  /**
   * Enables log path parsing. This must be called in your robot class to use this interface.
   *
   * @param robotInstance The TimedRobot instance
   */
  public static void enable(Object robotInstance) {
    if (enabled) {
      return;
    }
    enabled = true;
    recurseLogPaths(robotInstance, "");
  }

  private LogPathInference() {}

  private static void recurseLogPaths(Object obj, String currentPath) {
    logPathMap.put(obj, currentPath);
    var clazz = obj.getClass();
    for (var field : clazz.getDeclaredFields()) {
      try {
        if (InferLogPath.class.isAssignableFrom(field.getType())) {
          field.setAccessible(true);
          recurseLogPaths(field.get(obj), currentPath + "/" + computeLogName(field));
        } else {
          var arraySubtype = field.getType().getComponentType();
          if (arraySubtype == null || !InferLogPath.class.isAssignableFrom(arraySubtype)) {
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
