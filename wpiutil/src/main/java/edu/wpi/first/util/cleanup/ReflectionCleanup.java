// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.cleanup;

import java.lang.reflect.Field;

/**
 * Implement this interface to have access to a `reflectionCleanup` method that can be called from
 * your `close` method, that will use reflection to find all `AutoCloseable` instance members and
 * close them.
 */
@SuppressWarnings("PMD.ImplicitFunctionalInterface")
public interface ReflectionCleanup extends AutoCloseable {
  /**
   * Default implementation that uses reflection to find all AutoCloseable fields not marked
   * SkipCleanup and call close() on them. Call this from your `close()` method with the class level
   * you want to close.
   *
   * @param cls the class level to clean up
   */
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  default void reflectionCleanup(Class<? extends ReflectionCleanup> cls) {
    if (!cls.isAssignableFrom(getClass())) {
      System.out.println("Passed in class is not assignable from \"this\"");
      System.out.println("Expected something in the hierarchy of" + cls.getName());
      System.out.println("This is " + getClass().getName());
      return;
    }
    for (Field field : cls.getDeclaredFields()) {
      if (field.isAnnotationPresent(SkipCleanup.class)) {
        continue;
      }
      if (!AutoCloseable.class.isAssignableFrom(field.getType())) {
        continue;
      }
      if (field.trySetAccessible()) {
        try {
          AutoCloseable c = (AutoCloseable) field.get(this);
          if (c != null) {
            c.close();
          }
        } catch (Exception e) {
          // Ignore any exceptions
          e.printStackTrace();
        }
      }
    }
  }
}
