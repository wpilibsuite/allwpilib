// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable.util;

/** Path helper functions for tunable backends and registry internals. */
public final class PathUtil {
  private PathUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Normalizes a tunable table path by adding a leading slash, collapsing duplicate slashes, and
   * adding a trailing slash.
   *
   * @param path input path
   * @return normalized table path
   */
  public static String normalizeTableName(String path) {
    path = normalizeName(path);
    if (path.charAt(path.length() - 1) != '/') {
      path = path + '/';
    }
    return path;
  }

  /**
   * Normalizes a registry backend prefix.
   *
   * <p>Trailing slashes are removed, except for the root prefix.
   *
   * @param prefix input prefix
   * @return normalized backend prefix
   */
  public static String normalizeBackendPrefix(String prefix) {
    if (prefix.isEmpty()) {
      return "";
    }
    String normalized = normalizeName(prefix);
    while (normalized.length() > 1 && normalized.endsWith("/")) {
      normalized = normalized.substring(0, normalized.length() - 1);
    }
    return normalized;
  }

  /**
   * Normalizes a backend-owned prefix.
   *
   * <p>Unlike registry backend registration prefixes, this keeps a trailing slash so prefix-based
   * removals can match only descendants when requested.
   *
   * @param prefix input prefix
   * @return normalized prefix
   */
  public static String normalizePrefix(String prefix) {
    return prefix.isEmpty() ? "" : normalizeName(prefix);
  }

  /**
   * Returns a child table path by adding a trailing slash when needed.
   *
   * @param path input path
   * @return child table path
   */
  public static String childTablePath(String path) {
    if (path.isEmpty() || path.charAt(path.length() - 1) != '/') {
      return path + '/';
    }
    return path;
  }

  /**
   * Normalizes a child name by collapsing path separators and removing a leading slash.
   *
   * @param name input name
   * @return normalized child name
   */
  public static String normalizeChildName(String name) {
    String normalized = normalizeName(name);
    if (!normalized.isEmpty() && normalized.charAt(0) == '/') {
      return normalized.substring(1);
    }
    return normalized;
  }

  /**
   * Normalizes a tunable path by adding a leading slash and collapsing duplicate slashes.
   *
   * @param path input path
   * @return normalized path
   */
  public static String normalizeName(String path) {
    if (!path.isEmpty() && path.charAt(0) == '/' && !path.contains("//")) {
      return path;
    }

    StringBuilder normalized = new StringBuilder(path.length() + 1);
    if (path.isEmpty() || path.charAt(0) != '/') {
      normalized.append('/');
    }
    char previousChar = '\0';
    for (int i = 0; i < path.length(); i++) {
      char ch = path.charAt(i);
      if (ch != '/' || previousChar != '/') {
        normalized.append(ch);
      }
      previousChar = ch;
    }
    return normalized.toString();
  }

  /**
   * Returns whether a path is equal to a root path or is one of its descendants.
   *
   * <p>An empty root and "/" both match every path.
   *
   * @param path path to test
   * @param root root path
   * @return true if path is root or a descendant of root
   */
  public static boolean isPathOrDescendant(String path, String root) {
    if (root.isEmpty() || "/".equals(root) || path.equals(root)) {
      return true;
    }
    if (root.endsWith("/")) {
      return path.startsWith(root);
    }
    return path.length() > root.length()
        && path.startsWith(root)
        && path.charAt(root.length()) == '/';
  }
}
