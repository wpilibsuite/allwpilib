// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Consumer;
import org.wpilib.util.collections.PrefixMap;
import org.wpilib.util.collections.prefixmap.StringPrefixMap;

public final class TunableRegistry {
  /** Handler for tuning objects of specific type. Typically only one handler is specified. */
  @FunctionalInterface
  public interface TypeHandler<T> {
    /**
     * Create tunable.
     *
     * @param initialValue initial value
     * @param config tunable configuration
     * @return tunable
     */
    Tunable<T> createTunable(T initialValue, TunableConfig config);
  }

  /**
   * Storage for handler for tuning objects of specific type.
   *
   * @param cls class this handler applies to
   * @param handler handler for tuning
   */
  private record TypeHandlerData<T>(Class<T> cls, TypeHandler<T> handler) {}

  private static final List<TypeHandlerData<?>> s_typeHandlers = new ArrayList<>();
  private static final PrefixMap<TunableBackend> s_backends = new StringPrefixMap<>();
  private static final ConcurrentMap<String, TunableTable> s_tables = new ConcurrentHashMap<>();
  private static Consumer<String> s_reportWarning = TunableRegistry::defaultReportWarning;

  private TunableRegistry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  private static void defaultReportWarning(String msg) {
    // TODO: do something smarter here
    StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
    StringBuilder traceString = new StringBuilder();
    for (int i = 1; i < stackTrace.length; i++) {
      String loc = stackTrace[i].toString();
      traceString.append("\tat ").append(loc).append('\n');
    }

    System.err.println("Tunable warning: " + msg + "\n" + traceString.toString());
  }

  /**
   * Set function used for reporting warning messages (e.g. type mismatches).
   *
   * @param func reporting function; parameters are path and message; pass null to use default
   */
  public static void setReportWarning(Consumer<String> func) {
    synchronized (TunableRegistry.class) {
      if (func == null) {
        s_reportWarning = TunableRegistry::defaultReportWarning;
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
  public static Consumer<String> getReportWarning() {
    synchronized (TunableRegistry.class) {
      return s_reportWarning;
    }
  }

  /**
   * Report a warning message (e.g. type mismatch).
   *
   * @param msg warning message
   */
  public static void reportWarning(String msg) {
    synchronized (TunableRegistry.class) {
      s_reportWarning.accept(msg);
    }
  }

  /**
   * Registers a handler for tuning objects of a particular type. The handler should populate the
   * provided TunableTable name as appropriate for the object's data.
   *
   * @param <T> class
   * @param cls class object
   * @param handler handler (accepts TunableTable, entry name, and object)
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
   * Registers a backend for creating tunables. When calling getBackend(), the longest prefix match
   * is used.
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
   * Gets the handler for logging an object. Should generally only be used by Tunable.
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

  /**
   * Normalizes a tunable name.
   *
   * @param path input path
   * @return normalized path
   */
  public static String normalizeName(String path) {
    if (path.isEmpty() || path.charAt(0) != '/') {
      path = '/' + path;
    }
    return path.replace("//", "/");
  }

  /**
   * Publishes a tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   */
  public static void publish(String path, TunableBase tunable) {
    if (tunable instanceof Tunable.CustomTunable t) {
      publish(path, t.getInnerTunable());
      return;
    }
    getBackend(path).publish(path, tunable);
  }

  /**
   * Publishes a complex tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   */
  public static void publish(String path, ComplexTunable tunable) {
    getBackend(path).publishComplex(path, tunable);
  }

  /**
   * Removes a tunable.
   *
   * @param path normalized path
   */
  public static void remove(String path) {
    // Backends may have changed since publishing, so remove from all backends
    synchronized (s_backends) {
      for (var entry : s_backends.entrySet()) {
        entry.getValue().remove(path);
      }
    }
  }

  /** Updates all tunable values from backends. */
  public static void update() {
    synchronized (s_backends) {
      for (var entry : s_backends.entrySet()) {
        entry.getValue().update();
      }
    }
  }

  /**
   * Execute a function while holding the same mutex used for update(). As tunables are not
   * thread-safe, this is designed to allow external synchronization for tunables used in secondary
   * threads outside of the robot main loop.
   *
   * @param func function to execute
   */
  public static void withUpdateMutex(Runnable func) {
    synchronized (s_backends) {
      func.run();
    }
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
      for (TunableBackend backend : s_backends.values()) {
        try {
          backend.close();
        } catch (Exception e) {
          System.out.println("Unexpected exception when closing backend: " + e);
        }
      }
      s_backends.clear();
    }
  }
}
