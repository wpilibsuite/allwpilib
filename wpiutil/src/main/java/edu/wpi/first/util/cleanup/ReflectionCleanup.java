package edu.wpi.first.util.cleanup;

import java.lang.reflect.Field;

/**
 * Implement this interface to have the default close() method
 * automatically close all fields not marked SkipCleanup().
 */
public interface ReflectionCleanup extends AutoCloseable {

  /**
   * Default close implementation that uses reflection to find all
   * AutoCloseable fields not marked SkipCleanup and call close() on them.
   */
  default void close() {
    Class<?> root = this.getClass();
    Class<?> cls = root;
    while (cls != null) {
      for (Field field : cls.getDeclaredFields()) {
        if (field.isAnnotationPresent(SkipCleanup.class)) {
          continue;
        }
        if (!AutoCloseable.class.isAssignableFrom(field.getType())) {
          continue;
        }
        if (field.trySetAccessible()) {
          try {
            AutoCloseable c = (AutoCloseable)field.get(this);
            c.close();
          } catch (Exception e) {
            // Ignore any exceptions
            e.printStackTrace();
          }
        }
      }
      cls = cls.getSuperclass();
    }
  }
}
