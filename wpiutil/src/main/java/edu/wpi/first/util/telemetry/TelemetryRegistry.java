// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.collections.PrefixMap;
import edu.wpi.first.util.collections.prefixmap.StringPrefixMap;
import java.util.ArrayList;
import java.util.List;
import java.util.function.BiConsumer;

/** Global registry for telemetry handlers (type handlers and telemetry backends). */
public final class TelemetryRegistry {
  private static record TypeHandler(Class<?> cls, BiConsumer<Object, TelemetryEntry> handler) {}
  private static final List<TypeHandler> s_typeHandlers = new ArrayList<>();
  private static final PrefixMap<TelemetryBackend> s_backends = new StringPrefixMap<>();

  private TelemetryRegistry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Registers a handler for logging objects of a particular type. The handler should populate the
   * provided TelemetryEntry as appropriate for the object's data.
   *
   * @param <T> class
   * @param cls class object
   * @param handler handler (accepts object and TelemetryEntry)
   */
  public static <T> void registerType(Class<T> cls, BiConsumer<T, TelemetryEntry> handler) {
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

    TypeHandler typeHandler = new TypeHandler(cls, (v, entry) -> {
      @SuppressWarnings("unchecked")
      T value = (T) v;
      handler.accept(value, entry);
    });
    if (replace) {
      s_typeHandlers.set(i, typeHandler);
    } else {
      s_typeHandlers.add(i, typeHandler);
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
  }

  /**
   * Get the handler for logging an object.
   *
   * @param value object
   * @return handler or null if no match
   */
  public static BiConsumer<Object, TelemetryEntry> getHandler(Object value) {
    for (var entry : s_typeHandlers) {
      if (entry.cls.isInstance(value)) {
        return entry.handler;
      }
    }
    return null;
  }

  /**
   * Get a telemetry entry for a particular name. This is not cached internally; a new entry is
   * created on each call. Callers are responsible for saving/caching the return value.
   *
   * @param name name
   * @return telemetry entry
   */
  public static TelemetryEntry getEntry(String name) {
    synchronized (s_backends) {
      return s_backends.getLongestMatch(name).getEntry(name);
    }
  }

  /** Clear all registered types and backends. */
  public static void clear() {
    s_typeHandlers.clear();
    synchronized (s_backends) {
      s_backends.clear();
    }
  }
}
