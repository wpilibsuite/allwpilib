// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import edu.wpi.first.epilogue.Logged;
import javax.lang.model.element.TypeElement;

public final class StringUtils {
  private StringUtils() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Gets the simple name of a fully qualified class.
   *
   * @param fqn the fully qualified class name
   * @return the simple class name, without a package specifier
   */
  public static String simpleName(String fqn) {
    return fqn.substring(fqn.lastIndexOf('.') + 1);
  }

  /**
   * Capitalizes a string. The first character will be capitalized, and the rest of the string will
   * be left as is.
   *
   * @param str the string to capitalize
   * @return the capitalized string
   */
  public static String capitalize(CharSequence str) {
    return Character.toUpperCase(str.charAt(0)) + str.subSequence(1, str.length()).toString();
  }

  /**
   * Converts a string to a lower camel-cased version. This requires the input string to only
   * consist of alphanumeric characters, without underscores, spaces, or any other special
   * character.
   *
   * @param str the string to convert
   * @return the lower camel-case version of the string
   */
  public static String lowerCamelCase(CharSequence str) {
    StringBuilder builder = new StringBuilder(str.length());

    int i = 0;
    for (;
        i < str.length()
            && (i == 0
                || i == str.length() - 1
                || Character.isUpperCase(str.charAt(i))
                    && Character.isUpperCase(str.charAt(i + 1)));
        i++) {
      builder.append(Character.toLowerCase(str.charAt(i)));
    }

    builder.append(str.subSequence(i, str.length()));
    return builder.toString();
  }

  /**
   * Gets the name of the field used to hold a logger for data of the given type.
   *
   * @param clazz the data type that the logger supports
   * @return the logger field name
   */
  public static String loggerFieldName(TypeElement clazz) {
    return lowerCamelCase(simpleName(loggerClassName(clazz)));
  }

  /**
   * Gets the name of the generated class used to log data of the given type. This will be
   * fully-qualified class name, such as {@code "frc.robot.MyRobotLogger"}.
   *
   * @param clazz the data type that the logger supports
   * @return the logger class name
   */
  public static String loggerClassName(TypeElement clazz) {
    var config = clazz.getAnnotation(Logged.class);
    var className = clazz.getQualifiedName().toString();

    String packageName;
    int lastDot = className.lastIndexOf('.');
    if (lastDot <= 0) {
      packageName = null;
    } else {
      packageName = className.substring(0, lastDot);
    }

    String loggerClassName;

    // Use the name on the class config to set the generated logger names
    // This helps to avoid naming conflicts
    if (config.name().isBlank()) {
      loggerClassName = className + "Logger";
    } else {
      String cleaned = config.name().replaceAll(" ", "");

      var loggerSimpleClassName = StringUtils.capitalize(cleaned) + "Logger";
      if (packageName != null) {
        loggerClassName = packageName + "." + loggerSimpleClassName;
      } else {
        loggerClassName = loggerSimpleClassName;
      }
    }

    return loggerClassName;
  }
}
