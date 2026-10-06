// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

import java.util.ArrayDeque;
import java.util.Arrays;
import java.util.Deque;
import java.util.List;
import java.util.stream.Collectors;
import javax.lang.model.element.Element;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;
import org.wpilib.epilogue.Logged;

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
   * Splits a camel-cased string like "fooBar" into individual words like ["foo", "Bar"].
   *
   * @param camelCasedString the camel-cased string to split
   * @return the individual words in the input
   */
  public static List<String> splitToWords(CharSequence camelCasedString) {
    // Implementation from https://stackoverflow.com/a/2560017, refactored for readability

    // Uppercase letter not followed by the first letter of the next word
    // This allows for splitting "IOLayer" into "IO" and "Layer"
    String penultimateUppercaseLetter = "(?<=[A-Z])(?=[A-Z][a-z])";

    // Any character that's NOT an uppercase letter, immediately followed by an uppercase letter
    // This allows for splitting "fooBar" into "foo" and "Bar", or "123Bang" into "123" and "Bang"
    String lastNonUppercaseLetter = "(?<=[^A-Z])(?=[A-Z])";

    // The final letter in a sequence, followed by a non-alpha character like a number or underscore
    // This allows for splitting "foo123" into "foo" and "123"
    String finalLetter = "(?<=[A-Za-z])(?=[^A-Za-z])";

    String regex =
        String.format("%s|%s|%s", penultimateUppercaseLetter, lastNonUppercaseLetter, finalLetter);

    return Arrays.asList(camelCasedString.toString().split(regex));
  }

  /**
   * Gets the nesting of class simple names from outer class to the given class.
   *
   * @param clazz the class element
   * @return a deque of enclosing class simple names down to the class simple name
   */
  public static Deque<String> classNesting(TypeElement clazz) {
    Deque<String> nesting = new ArrayDeque<>();
    Element enclosing = clazz.getEnclosingElement();
    while (!(enclosing instanceof PackageElement)) {
      nesting.addFirst(enclosing.getSimpleName().toString());
      enclosing = enclosing.getEnclosingElement();
    }
    nesting.addLast(clazz.getSimpleName().toString());
    return nesting;
  }

  /**
   * Gets the nested type name used in source code within the class's package (e.g. {@code
   * "Example.Robot"}).
   *
   * @param clazz the class element
   * @return the nested type name
   */
  public static String nestedTypeName(TypeElement clazz) {
    return String.join(".", classNesting(clazz));
  }

  /**
   * Gets the simple class name of the Epilogue service generated for the given robot class (e.g.
   * {@code "Example$Robot_EpilogueService"}).
   *
   * @param clazz the robot class
   * @return the service class simple name
   */
  public static String serviceSimpleClassName(TypeElement clazz) {
    return String.join("$", classNesting(clazz)) + "_EpilogueService";
  }

  /**
   * Gets the fully qualified class name of the Epilogue service generated for the given robot
   * class.
   *
   * @param clazz the robot class
   * @return the service class qualified name
   */
  public static String serviceClassName(TypeElement clazz) {
    Element enclosing = clazz.getEnclosingElement();
    while (!(enclosing instanceof PackageElement p)) {
      enclosing = enclosing.getEnclosingElement();
    }
    String packageName = p.getQualifiedName().toString();
    String simpleName = serviceSimpleClassName(clazz);
    return packageName.isEmpty() ? simpleName : packageName + "." + simpleName;
  }

  /**
   * Gets the name of the field used to hold a logger for data of the given type.
   *
   * @param clazz the data type that the logger supports
   * @return the logger field name
   */
  public static String loggerFieldName(TypeElement clazz) {
    return loggerClassName(clazz).replace('.', '_');
  }

  /**
   * Gets the name of the generated class used to log data of the given type. This will be
   * fully-qualified class name, such as {@code "wpilib.robot.MyRobotLogger"}.
   *
   * @param clazz the data type that the logger supports
   * @return the logger class name
   */
  public static String loggerClassName(TypeElement clazz) {
    var config = clazz.getAnnotation(Logged.class);

    Deque<String> nesting = classNesting(clazz);
    Element enclosing = clazz.getEnclosingElement();
    while (!(enclosing instanceof PackageElement p)) {
      enclosing = enclosing.getEnclosingElement();
    }
    String packageName = p.getQualifiedName().toString();

    String className;
    if (config == null || config.name().isEmpty()) {
      className = String.join("$", nesting);
    } else {
      className = capitalize(config.name()).replaceAll(" ", "");
    }

    return packageName.isEmpty() ? className + "Logger" : packageName + "." + className + "Logger";
  }

  /**
   * Converts a camelCase element name to separate words, removing common field and method name
   * prefixes like "m_" and "get".
   *
   * @param elementName the camelcased element name
   * @return the name split into separate words and sanitized
   */
  public static String toHumanName(String elementName) {
    // Delete common field prefixes (k_name, m_name, s_name)
    var sanitizedName = elementName.replaceFirst("^[msk]_", "");

    // Drop leading "k" prefix from fields
    // (though normally these should be static, and thus not logged)
    if (sanitizedName.matches("^k[A-Z].*$")) {
      sanitizedName = sanitizedName.substring(1);
    }

    // Drop leading "get" from accessor methods
    if (sanitizedName.matches("^get[A-Z].*$")) {
      sanitizedName = sanitizedName.substring(3);
    }

    return splitToWords(sanitizedName).stream()
        .map(StringUtils::capitalize)
        .collect(Collectors.joining(" "));
  }
}
