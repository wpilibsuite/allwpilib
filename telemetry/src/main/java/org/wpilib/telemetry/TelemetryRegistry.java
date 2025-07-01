// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.BiConsumer;
import org.wpilib.util.collections.PrefixMap;
import org.wpilib.util.collections.prefixmap.StringPrefixMap;

/** Global registry for telemetry handlers (type handlers and telemetry backends). */
public final class TelemetryRegistry {
  /** Handler for logging objects of specific type. Typically only one handler is specified, the */
  @FunctionalInterface
  public interface TypeHandler<T> {
    /**
     * Log value to table as/under name.
     *
     * @param table telemetry table
     * @param name name
     * @param value value
     */
    void logTo(TelemetryTable table, String name, T value);
  }

  /**
   * Storage for handler for logging objects of specific type.
   *
   * @param cls class this handler applies to
   * @param handler handler for logging
   */
  private record TypeHandlerData<T>(Class<T> cls, TypeHandler<T> handler) {}

  private static final List<TypeHandlerData<?>> s_typeHandlers = new ArrayList<>();
  private static final PrefixMap<TelemetryBackend> s_backends = new StringPrefixMap<>();
  private static final ConcurrentMap<String, TelemetryTable> s_tables = new ConcurrentHashMap<>();
  private static BiConsumer<String, String> s_reportWarning =
      TelemetryRegistry::defaultReportWarning;

  private TelemetryRegistry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  private static void defaultReportWarning(String path, String msg) {
    // TODO: do something smarter here
    StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
    StringBuilder traceString = new StringBuilder();
    for (int i = 1; i < stackTrace.length; i++) {
      String loc = stackTrace[i].toString();
      traceString.append("\tat ").append(loc).append('\n');
    }

    System.err.println("Telemetry '" + path + "': warning: " + msg + "\n" + traceString.toString());
  }

  /**
   * Set function used for reporting warning messages (e.g. type mismatches).
   *
   * @param func reporting function; parameters are path and message; pass null to use default
   */
  public static void setReportWarning(BiConsumer<String, String> func) {
    synchronized (TelemetryRegistry.class) {
      if (func == null) {
        s_reportWarning = TelemetryRegistry::defaultReportWarning;
      } else {
        s_reportWarning = func;
      }
    }
  }

  /**
   * Get function used for reporting warning messages.
   *
   * @return reporting function
   */
  public static BiConsumer<String, String> getReportWarning() {
    synchronized (TelemetryRegistry.class) {
      return s_reportWarning;
    }
  }

  /**
   * Report a warning message (e.g. type mismatch).
   *
   * @param path entry path
   * @param msg warning message
   */
  public static void reportWarning(String path, String msg) {
    synchronized (TelemetryRegistry.class) {
      s_reportWarning.accept(path, msg);
    }
  }

  /**
   * Registers a handler for logging objects of a particular type. The handler should populate the
   * provided TelemetryTable name as appropriate for the object's data.
   *
   * @param <T> class
   * @param cls class object
   * @param handler handler (accepts TelemetryTable, entry name, and object)
   */
  public static <T> void registerTypeHandler(Class<T> cls, TypeHandler<T> handler) {
    synchronized (s_typeHandlers) {
      // we want this ordered such that the more specific types come before the less specific ones
      // this is O(N^2) but N should be small
      boolean replace = false;
      int i = 0;
      for (var entry : s_typeHandlers) {
        if (entry.cls.equals(cls)) {
          // replace existing
          replace = true;
          break;
        }
        if (entry.cls.isAssignableFrom(cls)) {
          // superclass; insert before
          break;
        }
        i++;
      }

      if (replace) {
        s_typeHandlers.set(i, new TypeHandlerData<>(cls, handler));
      } else {
        s_typeHandlers.add(i, new TypeHandlerData<>(cls, handler));
      }
    }
  }

  /**
   * Registers a backend for creating telemetry entries. When calling getEntry(), the longest prefix
   * match is used.
   *
   * @param prefix prefix for entries covered by this backend
   * @param backend backend
   */
  public static void registerBackend(String prefix, TelemetryBackend backend) {
    synchronized (s_backends) {
      s_backends.put(prefix, backend);
    }

    // reset cached entries in tables
    for (var table : s_tables.values()) {
      table.reset();
    }
  }

  /**
   * Gets the handler for logging an object. Should generally only be used by TelemetryTable.
   *
   * @param <T> class
   * @param value value
   * @return handler or null if no match
   */
  public static <T> TypeHandler<T> getTypeHandler(T value) {
    synchronized (s_typeHandlers) {
      for (var entry : s_typeHandlers) {
        if (entry.cls.isInstance(value)) {
          @SuppressWarnings("unchecked")
          TypeHandler<T> handler = (TypeHandler<T>) entry.handler;
          return handler;
        }
      }
      return null;
    }
  }

  /**
   * Gets the backend for logging an object. Should generally only be used internally or by custom
   * backends.
   *
   * @param path full name
   * @return telemetry backend, or null if no match
   */
  public static TelemetryBackend getBackend(String path) {
    String normalized = normalizeName(path);
    synchronized (s_backends) {
      return s_backends.getLongestMatch(normalized);
    }
  }

  /**
   * Get a telemetry entry for a particular name.
   *
   * @param path full name
   * @return telemetry entry
   */
  static TelemetryEntry getEntry(String path) {
    String normalized = normalizeName(path);
    TelemetryBackend backend;
    synchronized (s_backends) {
      backend = s_backends.getLongestMatch(normalized);
    }
    return backend.getEntry(normalized);
  }

  /**
   * Get a telemetry table for a particular name.
   *
   * @param path full name
   * @return telemetry table
   */
  public static TelemetryTable getTable(String path) {
    return s_tables.computeIfAbsent(normalizeTableName(path), TelemetryTable::new);
  }

  /**
   * Clear all registered types and backends and closes all entries. Should typically only be used
   * by unit test code.
   */
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  public static void reset() {
    synchronized (s_typeHandlers) {
      s_typeHandlers.clear();
    }
    synchronized (s_backends) {
      for (TelemetryBackend backend : s_backends.values()) {
        try {
          backend.close();
        } catch (Exception e) {
          System.out.println("Unexpected exception when closing backend: " + e);
        }
      }
      s_backends.clear();
    }

    // reset cached entries in tables
    for (var table : s_tables.values()) {
      table.reset();
    }
  }

  private static String normalizeTableName(String path) {
    path = normalizeName(path);
    if (path.charAt(path.length() - 1) != '/') {
      path = path + '/';
    }
    return path;
  }

  private static String normalizeName(String path) {
    if (path.isEmpty() || path.charAt(0) != '/') {
      path = '/' + path;
    }
    return path.replace("//", "/");
  }
}
