// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import org.wpilib.util.collections.PrefixMap;
import org.wpilib.util.collections.prefixmap.StringPrefixMap;

public final class TunableRegistry {
  private static final PrefixMap<TunableBackend> s_backends = new StringPrefixMap<>();
  private static final ConcurrentMap<String, TunableTable> s_tables = new ConcurrentHashMap<>();

  private TunableRegistry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Registers a backend for creating tunables. When calling getBackend(), the longest prefix
   * match is used.
   *
   * @param prefix prefix for tunables covered by this backend
   * @param backend backend
   */
  public static void registerBackend(String prefix, TunableBackend backend) {
    synchronized (s_backends) {
      s_backends.put(prefix, backend);
    }
  }

  /**
   * Gets the backend for creating a tunable. Should generally only be used internally or by custom
   * backends.
   *
   * @param path full name
   * @return tunable backend, or null if no match
   */
  public static TunableBackend getBackend(String path) {
    synchronized (s_backends) {
      return s_backends.getLongestMatch(path);
    }
  }

  /**
   * Get a tunable table for a particular name.
   *
   * @param path full name
   * @return tunable table
   */
  public static TunableTable getTable(String path) {
    return s_tables.computeIfAbsent(normalizeTableName(path), TunableTable::new);
  }

  private static String normalizeTableName(String path) {
    path = normalizeName(path);
    if (path.charAt(path.length() - 1) != '/') {
      path = path + '/';
    }
    return path;
  }

  public static String normalizeName(String path) {
    if (path.isEmpty() || path.charAt(0) != '/') {
      path = '/' + path;
    }
    return path.replace("//", "/");
  }

  /**
   * Updates all tunable values from backends. Also calls callbacks where appropriate.
   */
  public static void update() {
    synchronized (s_backends) {
      for (var entry : s_backends.entrySet()) {
        entry.getValue().update();
      }
    }
  }
}
