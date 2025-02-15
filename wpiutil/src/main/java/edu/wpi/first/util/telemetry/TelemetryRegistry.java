// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.collections.PrefixMap;
import edu.wpi.first.util.collections.prefixmap.StringPrefixMap;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.BiConsumer;

/** Global registry for telemetry handlers (type handlers and telemetry backends). */
public final class TelemetryRegistry {
  record TypeHandler(
      Class<?> cls,
      BiConsumer<Object, TelemetryEntry> entryHandler,
      BiConsumer<Object, TelemetryTable> tableHandler) {}

  private static final List<TypeHandler> s_typeHandlers = new ArrayList<>();
  private static final PrefixMap<TelemetryBackend> s_backends = new StringPrefixMap<>();
  private static final ConcurrentMap<String, TelemetryTable> s_tables = new ConcurrentHashMap<>();

  private TelemetryRegistry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  private static void registerTypeHandler(TypeHandler typeHandler) {
    synchronized (s_typeHandlers) {
      // we want this ordered such that the more specific types come before the less specific ones
      // this is O(N^2) but N should be small
      boolean replace = false;
      int i = 0;
      for (var entry : s_typeHandlers) {
        if (entry.cls.equals(typeHandler.cls)) {
          // replace existing
          replace = true;
          break;
        }
        if (entry.cls.isAssignableFrom(typeHandler.cls)) {
          // superclass; insert before
          break;
        }
        i++;
      }

      if (replace) {
        s_typeHandlers.set(i, typeHandler);
      } else {
        s_typeHandlers.add(i, typeHandler);
      }
    }
  }

  /**
   * Registers a handler for logging objects of a particular type to a single entry. The handler
   * should populate the provided TelemetryEntry as appropriate for the object's data.
   *
   * @param <T> class
   * @param cls class object
   * @param handler handler (accepts object and TelemetryEntry)
   */
  public static <T> void registerTypeEntry(Class<T> cls, BiConsumer<T, TelemetryEntry> handler) {
    registerTypeHandler(
        new TypeHandler(
            cls,
            (v, entry) -> {
              @SuppressWarnings("unchecked")
              T value = (T) v;
              handler.accept(value, entry);
            },
            null));
  }

  /**
   * Registers a handler for logging objects of a particular type to a table. The handler should
   * populate the provided TelemetryTable as appropriate for the object's data.
   *
   * @param <T> class
   * @param cls class object
   * @param handler handler (accepts object and TelemetryTable)
   */
  public static <T> void registerTypeTable(Class<T> cls, BiConsumer<T, TelemetryTable> handler) {
    registerTypeHandler(
        new TypeHandler(
            cls,
            null,
            (v, table) -> {
              @SuppressWarnings("unchecked")
              T value = (T) v;
              handler.accept(value, table);
            }));
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
  }

  /**
   * Gets the handler for logging an object. Should generally only be used by TelemetryTable.
   *
   * @param value object
   * @return handler or null if no match
   */
  public static TypeHandler getTypeHandler(Object value) {
    synchronized (s_typeHandlers) {
      for (var entry : s_typeHandlers) {
        if (entry.cls.isInstance(value)) {
          return entry;
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
    synchronized (s_backends) {
      return s_backends.getLongestMatch(path);
    }
  }

  /**
   * Get a telemetry entry for a particular name.
   *
   * @param path full name
   * @return telemetry entry
   */
  static TelemetryEntry getEntry(String path) {
    return getBackend(path).getEntry(path);
  }

  /**
   * Get a telemetry table for a particular name.
   *
   * @param path full name
   * @return telemetry table
   */
  public static TelemetryTable getTable(String path) {
    return s_tables.computeIfAbsent(path, TelemetryTable::new);
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
}
