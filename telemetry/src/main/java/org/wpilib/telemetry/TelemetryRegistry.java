// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.IdentityHashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.BiConsumer;
import org.wpilib.telemetry.util.PathUtil;
import org.wpilib.util.collections.PrefixMap;
import org.wpilib.util.collections.prefixmap.StringPrefixMap;

/** Global registry for telemetry handlers (type handlers and telemetry backends). */
public final class TelemetryRegistry {
  /**
   * Handler for logging objects of specific type. Typically only one handler is specified.
   *
   * @param <T> object type
   */
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

  private record BackendPrefix(String prefix, TelemetryBackend backend) {}

  private record RemovedEntry(TelemetryBackend backend, String path) {}

  private static final class EntryMetadata {
    private final Map<String, String> m_properties = new LinkedHashMap<>();
    private boolean m_keepDuplicates;

    synchronized void keepDuplicates() {
      m_keepDuplicates = true;
    }

    synchronized void setProperty(String key, String value) {
      m_properties.put(key, value);
    }

    void apply(TelemetryEntry entry) {
      boolean keepDuplicates;
      Map<String, String> properties;
      synchronized (this) {
        keepDuplicates = m_keepDuplicates;
        properties = new LinkedHashMap<>(m_properties);
      }

      if (keepDuplicates) {
        entry.keepDuplicates();
      }
      for (var property : properties.entrySet()) {
        entry.setProperty(property.getKey(), property.getValue());
      }
    }
  }

  private static final List<TypeHandlerData<?>> s_typeHandlers = new ArrayList<>();
  private static final PrefixMap<TelemetryBackend> s_backends = new StringPrefixMap<>();
  private static final Map<String, TelemetryBackend> s_entryBackends = new LinkedHashMap<>();
  private static final ConcurrentMap<String, TelemetryTable> s_tables = new ConcurrentHashMap<>();
  private static final ConcurrentMap<String, EntryMetadata> s_entryMetadata =
      new ConcurrentHashMap<>();
  private static final TelemetryBackend s_missingBackend = new DiscardTelemetryBackend();
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
   * <p>The reporting function may be called concurrently and must not throw.
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
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public static void registerBackend(String prefix, TelemetryBackend backend) {
    String normalizedPrefix = PathUtil.normalizeBackendPrefix(prefix);
    List<RemovedEntry> removedEntries = new ArrayList<>();
    List<TelemetryBackend> registeredBackends = new ArrayList<>();
    Set<TelemetryBackend> registeredOwnedBackends =
        Collections.newSetFromMap(new IdentityHashMap<>());
    Set<TelemetryBackend> displacedBackends = Collections.newSetFromMap(new IdentityHashMap<>());
    synchronized (s_backends) {
      // Reset table generations before backend routing changes become visible.
      for (var table : s_tables.values()) {
        table.reset();
      }

      TelemetryBackend oldBackend = s_backends.put(normalizedPrefix, backend);
      if (oldBackend != null) {
        displacedBackends.add(oldBackend);
      }

      for (var iterator = s_entryBackends.entrySet().iterator(); iterator.hasNext(); ) {
        var entry = iterator.next();
        TelemetryBackend newBackend = getBackendForNormalizedPath(entry.getKey());
        if (entry.getValue() != newBackend) {
          removedEntries.add(new RemovedEntry(entry.getValue(), entry.getKey()));
          iterator.remove();
        }
      }
      registeredBackends.addAll(s_backends.values());
      for (TelemetryBackend registeredBackend : registeredBackends) {
        collectOwnedBackends(registeredBackend, registeredOwnedBackends);
      }
    }
    removeBackendsSharingOwnershipWith(displacedBackends, registeredBackends);

    try {
      for (var entry : removedEntries) {
        entry.backend().removeEntry(entry.path());
      }
    } finally {
      closeBackends(displacedBackends, registeredOwnedBackends);
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
   * @return telemetry backend, or a discard backend if no match
   */
  public static TelemetryBackend getBackend(String path) {
    String normalized = PathUtil.normalizeName(path);
    TelemetryBackend backend;
    synchronized (s_backends) {
      backend = getBackendForNormalizedPath(normalized);
    }
    if (backend == null) {
      reportWarning(normalized, "no backend for path");
      return s_missingBackend;
    }
    return backend;
  }

  /**
   * Get a telemetry entry for a particular name.
   *
   * @param path full name
   * @return telemetry entry
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  static TelemetryEntry getEntry(String path) {
    String normalized = PathUtil.normalizeName(path);
    for (; ; ) {
      TelemetryBackend backend;
      boolean missingBackend = false;
      synchronized (s_backends) {
        backend = getBackendForNormalizedPath(normalized);
        if (backend == null) {
          backend = s_missingBackend;
          missingBackend = true;
        }
      }
      if (missingBackend) {
        reportWarning(normalized, "no backend for path");
      }

      TelemetryEntry entry = backend.getEntry(normalized);
      synchronized (s_backends) {
        TelemetryBackend currentBackend = getBackendForNormalizedPath(normalized);
        if (currentBackend == null) {
          currentBackend = s_missingBackend;
        }
        if (backend == currentBackend) {
          s_entryBackends.put(normalized, backend);
          return entry;
        }
      }

      backend.removeEntry(normalized);
    }
  }

  static void keepEntryDuplicates(String path) {
    s_entryMetadata
        .computeIfAbsent(PathUtil.normalizeName(path), k -> new EntryMetadata())
        .keepDuplicates();
  }

  static void setEntryProperty(String path, String key, String value) {
    s_entryMetadata
        .computeIfAbsent(PathUtil.normalizeName(path), k -> new EntryMetadata())
        .setProperty(key, value);
  }

  static void applyEntryMetadata(String path, TelemetryEntry entry) {
    EntryMetadata metadata = s_entryMetadata.get(PathUtil.normalizeName(path));
    if (metadata != null) {
      metadata.apply(entry);
    }
  }

  static boolean hasNonDiscardDescendant(String tablePath) {
    String normalized = PathUtil.normalizeTableName(tablePath);
    List<BackendPrefix> descendants = new ArrayList<>();
    synchronized (s_backends) {
      for (Map.Entry<String, TelemetryBackend> entry : s_backends.entrySet()) {
        if (PathUtil.isPathOrDescendant(entry.getKey(), normalized)) {
          descendants.add(new BackendPrefix(entry.getKey(), entry.getValue()));
        }
      }
    }

    for (BackendPrefix descendant : descendants) {
      if (!descendant.backend().getEntry(descendant.prefix()).isDiscard()) {
        return true;
      }
    }
    return false;
  }

  /**
   * Get a telemetry table for a particular name.
   *
   * @param path full name
   * @return telemetry table
   */
  public static TelemetryTable getTable(String path) {
    return s_tables.computeIfAbsent(PathUtil.normalizeTableName(path), TelemetryTable::new);
  }

  /**
   * Clear all registered types and backends and closes all entries. Should typically only be used
   * by unit test code.
   */
  public static void reset() {
    List<RemovedEntry> removedEntries = new ArrayList<>();
    Set<TelemetryBackend> backends = Collections.newSetFromMap(new IdentityHashMap<>());
    synchronized (s_typeHandlers) {
      s_typeHandlers.clear();
    }
    synchronized (s_backends) {
      // Reset table generations before backend routing changes become visible.
      for (var table : s_tables.values()) {
        table.reset();
      }

      for (var entry : s_entryBackends.entrySet()) {
        removedEntries.add(new RemovedEntry(entry.getValue(), entry.getKey()));
      }
      backends.addAll(s_backends.values());
      s_backends.clear();
      s_entryBackends.clear();
    }
    removeBackendsOwnedByOtherBackends(backends);

    for (var entry : removedEntries) {
      entry.backend().removeEntry(entry.path());
    }
    closeBackends(backends, Collections.emptySet());
    s_entryMetadata.clear();
  }

  private static TelemetryBackend getBackendForNormalizedPath(String path) {
    String candidate = path;
    while (true) {
      TelemetryBackend backend = s_backends.get(candidate);
      if (backend != null) {
        return backend;
      }

      int slash = candidate.lastIndexOf('/');
      if (slash <= 0) {
        break;
      }
      candidate = candidate.substring(0, slash);
    }

    TelemetryBackend rootBackend = s_backends.get("/");
    if (rootBackend != null) {
      return rootBackend;
    }
    return s_backends.get("");
  }

  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  private static void closeBackends(
      Collection<TelemetryBackend> backends, Set<TelemetryBackend> skipBackends) {
    Set<TelemetryBackend> closedBackends = Collections.newSetFromMap(new IdentityHashMap<>());
    for (TelemetryBackend backend : backends) {
      try {
        closeBackend(backend, closedBackends, skipBackends);
      } catch (Exception e) {
        System.out.println("Unexpected exception when closing backend: " + e);
      }
    }
  }

  @SuppressWarnings("PMD.SignatureDeclareThrowsException")
  static void closeBackend(
      TelemetryBackend backend,
      Set<TelemetryBackend> closedBackends,
      Set<TelemetryBackend> skipBackends)
      throws Exception {
    if (skipBackends.contains(backend) || closedBackends.contains(backend)) {
      return;
    }
    if (backend instanceof MultiTelemetryBackend multiBackend) {
      multiBackend.close(closedBackends, skipBackends);
      return;
    }
    closedBackends.add(backend);
    backend.close();
  }

  static void collectOwnedBackends(TelemetryBackend backend, Set<TelemetryBackend> ownedBackends) {
    if (backend instanceof MultiTelemetryBackend multiBackend) {
      multiBackend.collectOwnedBackends(ownedBackends);
    } else {
      ownedBackends.add(backend);
    }
  }

  private static void removeBackendsSharingOwnershipWith(
      Set<TelemetryBackend> candidateBackends, Iterable<TelemetryBackend> ownerBackends) {
    for (TelemetryBackend ownerBackend : ownerBackends) {
      for (var iterator = candidateBackends.iterator(); iterator.hasNext(); ) {
        TelemetryBackend candidateBackend = iterator.next();
        if (candidateBackend instanceof MultiTelemetryBackend) {
          continue;
        }
        if (ownerBackend.sharesBackendWith(candidateBackend)) {
          iterator.remove();
        }
      }
    }
  }

  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private static void removeBackendsOwnedByOtherBackends(Set<TelemetryBackend> candidateBackends) {
    List<TelemetryBackend> ownerBackends = new ArrayList<>(candidateBackends);
    for (TelemetryBackend ownerBackend : ownerBackends) {
      for (var iterator = candidateBackends.iterator(); iterator.hasNext(); ) {
        TelemetryBackend candidateBackend = iterator.next();
        if (ownerBackend != candidateBackend && ownerBackend.ownsBackend(candidateBackend)) {
          iterator.remove();
        }
      }
    }
  }
}
