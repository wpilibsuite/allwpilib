// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.collections.PrefixMap;
import edu.wpi.first.util.collections.prefixmap.StringPrefixMap;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.BiConsumer;

/** Global registry for telemetry handlers (type handlers and telemetry backends). */
public final class TelemetryRegistry {
  // FIXME: this is unordered, and really should be ordered in class hierarchy order
  private static final ConcurrentMap<Class<?>, BiConsumer<Object, TelemetryEntry>> s_typeHandlers =
      new ConcurrentHashMap<>();
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
    s_typeHandlers.put(
        cls,
        (v, entry) -> {
          @SuppressWarnings("unchecked")
          T value = (T) v;
          handler.accept(value, entry);
        });
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
    for (var entry : s_typeHandlers.entrySet()) {
      if (entry.getKey().isInstance(value)) {
        return entry.getValue();
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
