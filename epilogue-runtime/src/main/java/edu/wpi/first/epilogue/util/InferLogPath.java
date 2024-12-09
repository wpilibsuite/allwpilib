package edu.wpi.first.epilogue.util;

import edu.wpi.first.epilogue.Logged;
import java.lang.reflect.Field;
import java.util.Map;
import java.util.WeakHashMap;

/**
 * A utility interface that allows you to infer epilogue log paths for manual logging.
 *
 * <h5>InferLogPath.Parser.enable(this) must be called in your Robot class first before using this
 * interface.</h5>
 */
public interface InferLogPath {
  /**
   * Computes the appropriate epilogue log path for a manually logged field.
   *
   * @param logUnder the name to log under. For instance, pivot/logUnder or arm/logUnder.
   * @return An absolute log path that follows epilogue conventions.
   */
  default String logPath(String logUnder) {
    return Parser.logPathMap.getOrDefault(this, Parser.DEFAULT_NAMESPACE) + "/" + logUnder;
  }

  class Parser {
    private static final Map<Object, String> logPathMap = new WeakHashMap<>();
    private static final String DEFAULT_NAMESPACE = "UNKNOWN";
    private static boolean enabled = false;
    
    /**
     * Enables log path parsing. This must be called in your robot class to use this interface.
     *
     * @param robotInstance The TimedRobot instance
     */
    public static void enable(Object robotInstance) {
      if (enabled) return;
      enabled = true;
      recurseLogPaths(robotInstance, "");
    }

    private Parser() {}

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
            if (arraySubtype == null || !InferLogPath.class.isAssignableFrom(arraySubtype))
              continue;
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
}
