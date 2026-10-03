// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.IdentityHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Consumer;
import org.wpilib.tunable.util.PathUtil;
import org.wpilib.util.collections.PrefixMap;
import org.wpilib.util.collections.prefixmap.StringPrefixMap;

/** Global registry for tunable handlers and tunable backends. */
public final class TunableRegistry {
  /**
   * Handler for tuning objects of specific type. Typically only one handler is specified.
   *
   * @param <T> object type
   */
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

  private record ComplexMigrationRecorder(
      Set<String> publishedPaths, ComplexMigrationRecorder previous) {}

  private static final List<TypeHandlerData<?>> s_typeHandlers = new ArrayList<>();
  private static final PrefixMap<TunableBackend> s_backends = new StringPrefixMap<>();
  private static volatile TunableBackend[] s_backendSnapshot = new TunableBackend[0];
  private static final ThreadLocal<ComplexMigrationRecorder> s_complexMigrationRecorder =
      new ThreadLocal<>();
  private static final IdentityHashMap<TunableBase, Boolean> s_pendingChangedResets =
      new IdentityHashMap<>();
  private static final IdentityHashMap<ComplexTunable, Boolean> s_updatedComplexTunables =
      new IdentityHashMap<>();
  private static final List<Runnable> s_pendingUpdateCallbacks = new ArrayList<>();
  private static int s_updateDepth;
  private static final ConcurrentMap<String, TunableTable> s_tables = new ConcurrentHashMap<>();
  private static final Object s_complexPathsMutex = new Object();
  private static final IdentityHashMap<ComplexTunable, List<String>> s_complexPaths =
      new IdentityHashMap<>();
  private static final Map<String, ComplexTunable> s_complexByPath = new HashMap<>();
  private static final Map<String, TunableBase> s_complexChildrenByPath = new HashMap<>();
  private static final TunableBackend s_missingBackend = new NoopTunableBackend();
  private static Consumer<String> s_reportWarning = TunableRegistry::defaultReportWarning;

  private static final class NoopTunableBackend implements TunableBackend {
    @Override
    public boolean publish(String path, TunableBase tunable) {
      return false;
    }

    @Override
    public boolean publishComplex(String path, ComplexTunable tunable) {
      return false;
    }

    @Override
    public void remove(String path) {}

    @Override
    public List<PublishedTunable> removePrefix(String prefix) {
      return List.of();
    }

    @Override
    public void update() {}

    @Override
    public void close() {}
  }

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
   * <p>The reporting function must not throw.
   *
   * @param func reporting function; pass null to use default
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
  @SuppressWarnings({"PMD.CompareObjectsWithEquals", "PMD.AvoidCatchingGenericException"})
  public static void registerBackend(String prefix, TunableBackend backend) {
    String normalizedPrefix = PathUtil.normalizeBackendPrefix(prefix);
    List<TunableBackend> closeBackends = new ArrayList<>();
    synchronized (s_backends) {
      final List<TunableBackend> oldBackends = new ArrayList<>(s_backends.values());
      s_backends.remove(normalizedPrefix);
      s_backends.put(normalizedPrefix, backend);
      updateBackendSnapshot();

      List<TunableBackend.PublishedTunable> migrations = new ArrayList<>();
      IdentityHashMap<TunableBackend, Boolean> displacedBackends = new IdentityHashMap<>();
      for (TunableBackend oldBackend : oldBackends) {
        if (oldBackend != backend) {
          migrations.addAll(oldBackend.removePrefix(normalizedPrefix));
          displacedBackends.put(oldBackend, Boolean.TRUE);
        }
      }

      // Java complex backends publish the parent and then publish their child entries as a side
      // effect of publishComplex(). Republish complex entries first and record the paths they
      // actually recreate so only those returned entries are suppressed.
      Set<String> publishedComplexChildren = new HashSet<>();
      List<TunableBackend.PublishedTunable> complexMigrations = new ArrayList<>();
      for (var published : migrations) {
        if (published.isComplex()) {
          complexMigrations.add(published);
        }
      }
      complexMigrations.sort((a, b) -> Integer.compare(a.path().length(), b.path().length()));

      for (var published : complexMigrations) {
        if (publishedComplexChildren.contains(published.path())) {
          continue;
        }
        TunableBackend targetBackend = getBackendForNormalizedPath(published.path());
        publishMigratedComplex(targetBackend, published, publishedComplexChildren);
      }

      for (var published : migrations) {
        if (published.isComplex() || publishedComplexChildren.contains(published.path())) {
          continue;
        }
        TunableBackend targetBackend = getBackendForNormalizedPath(published.path());
        targetBackend.publish(published.path(), published.tunable());
      }

      IdentityHashMap<TunableBackend, Boolean> currentBackends = new IdentityHashMap<>();
      for (TunableBackend currentBackend : s_backendSnapshot) {
        currentBackends.put(currentBackend, Boolean.TRUE);
      }
      for (TunableBackend displacedBackend : displacedBackends.keySet()) {
        if (!currentBackends.containsKey(displacedBackend)) {
          closeBackends.add(displacedBackend);
        }
      }
    }

    for (TunableBackend closeBackend : closeBackends) {
      try {
        closeBackend.close();
      } catch (Exception e) {
        System.out.println("Unexpected exception when closing backend: " + e);
      }
    }
  }

  private static void publishMigratedComplex(
      TunableBackend targetBackend,
      TunableBackend.PublishedTunable published,
      Set<String> publishedComplexChildren) {
    ComplexMigrationRecorder previous = s_complexMigrationRecorder.get();
    s_complexMigrationRecorder.set(
        new ComplexMigrationRecorder(publishedComplexChildren, previous));
    try {
      targetBackend.publishComplex(published.path(), published.complex());
    } finally {
      if (previous == null) {
        s_complexMigrationRecorder.remove();
      } else {
        s_complexMigrationRecorder.set(previous);
      }
    }
  }

  private static void recordComplexMigrationPublish(String path) {
    ComplexMigrationRecorder recorder = s_complexMigrationRecorder.get();
    while (recorder != null) {
      recorder.publishedPaths().add(path);
      recorder = recorder.previous();
    }
  }

  private static void updateBackendSnapshot() {
    IdentityHashMap<TunableBackend, Boolean> uniqueBackends = new IdentityHashMap<>();
    List<TunableBackend> snapshot = new ArrayList<>();
    for (TunableBackend backend : s_backends.values()) {
      if (uniqueBackends.put(backend, Boolean.TRUE) == null) {
        snapshot.add(backend);
      }
    }
    s_backendSnapshot = snapshot.toArray(new TunableBackend[0]);
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
   * @return tunable backend, or a no-op backend if no match
   */
  public static TunableBackend getBackend(String path) {
    String normalized = PathUtil.normalizeName(path);
    TunableBackend backend;
    synchronized (s_backends) {
      backend = getBackendForNormalizedPath(normalized);
    }
    if (isMissingBackend(backend)) {
      reportWarning("no backend for path '" + normalized + "'");
    }
    return backend;
  }

  private static TunableBackend getBackendForNormalizedPath(String path) {
    String candidate = path;
    while (true) {
      TunableBackend backend = s_backends.get(candidate);
      if (backend != null) {
        return backend;
      }

      int slash = candidate.lastIndexOf('/');
      if (slash <= 0) {
        break;
      }
      candidate = candidate.substring(0, slash);
    }

    TunableBackend rootBackend = s_backends.get("/");
    if (rootBackend != null) {
      return rootBackend;
    }
    TunableBackend defaultBackend = s_backends.get("");
    if (defaultBackend != null) {
      return defaultBackend;
    }
    return s_missingBackend;
  }

  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private static boolean isMissingBackend(TunableBackend backend) {
    return backend == s_missingBackend;
  }

  static void notifyChanged(TunableBase tunable) {
    for (TunableBackend backend : s_backendSnapshot) {
      backend.markDirty(tunable);
    }
  }

  /**
   * Resets a tunable's changed flag after the current update cycle finishes.
   *
   * <p>Backends should call this after publishing a changed tunable so every alias in every backend
   * can observe the shared changed flag before it is cleared.
   *
   * @param tunable tunable to reset
   */
  public static void resetChangedAfterUpdate(TunableBase tunable) {
    synchronized (s_backends) {
      if (s_updateDepth > 0) {
        s_pendingChangedResets.put(tunable, Boolean.TRUE);
      } else {
        tunable.resetChanged();
      }
    }
  }

  /**
   * Runs a callback after the current update cycle finishes resetting changed flags.
   *
   * <p>Backends should use this for callbacks that react to tuned values so callback mutations are
   * observed as new changes in the next update cycle.
   *
   * @param callback callback to run; must not throw
   */
  public static void runAfterUpdate(Runnable callback) {
    synchronized (s_backends) {
      if (s_updateDepth > 0) {
        s_pendingUpdateCallbacks.add(callback);
      } else {
        callback.run();
      }
    }
  }

  /**
   * Updates a complex tunable if it has not already been updated during the current registry
   * update.
   *
   * <p>This should generally only be used by {@link TunableBackend} implementations.
   *
   * @param tunable tunable to update
   */
  public static void updateComplexIfNeeded(ComplexTunable tunable) {
    synchronized (s_backends) {
      if (s_updateDepth > 0 && s_updatedComplexTunables.put(tunable, Boolean.TRUE) != null) {
        return;
      }
      tunable.updateTunable();
    }
  }

  private static void finishUpdate() {
    s_updateDepth--;
    if (s_updateDepth != 0) {
      return;
    }

    s_updatedComplexTunables.clear();

    if (!s_pendingChangedResets.isEmpty()) {
      for (TunableBase tunable : s_pendingChangedResets.keySet()) {
        tunable.resetChanged();
      }
      s_pendingChangedResets.clear();
    }

    if (s_pendingUpdateCallbacks.isEmpty()) {
      return;
    }

    List<Runnable> callbacks = new ArrayList<>(s_pendingUpdateCallbacks);
    s_pendingUpdateCallbacks.clear();
    for (Runnable callback : callbacks) {
      callback.run();
    }
  }

  /**
   * Get a tunable table for a particular name.
   *
   * @param path full name
   * @return tunable table
   */
  public static TunableTable getTable(String path) {
    return s_tables.computeIfAbsent(PathUtil.normalizeTableName(path), TunableTable::new);
  }

  /**
   * Publishes a complex tunable's children.
   *
   * <p>This should generally only be used by {@link TunableBackend} implementations after they
   * publish the complex parent entry.
   *
   * @param path normalized path
   * @param tunable the tunable
   */
  public static void publishComplexChildren(String path, ComplexTunable tunable) {
    tunable.publishTunable(getTable(path));
  }

  /**
   * Publishes a tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   * @return true if the backend accepted the tunable
   */
  public static boolean publish(String path, TunableBase tunable) {
    if (tunable instanceof Tunable.CustomTunable t) {
      return publish(path, t.getInnerTunable());
    }
    String normalized = PathUtil.normalizeName(path);
    boolean missingBackend = false;
    synchronized (s_backends) {
      TunableBackend backend = getBackendForNormalizedPath(normalized);
      if (isMissingBackend(backend)) {
        missingBackend = true;
      } else if (backend.publish(path, tunable)) {
        addComplexChildPath(path, tunable);
        recordComplexMigrationPublish(path);
        return true;
      }
    }
    if (missingBackend) {
      reportWarning("no backend for path '" + normalized + "'");
    }
    return false;
  }

  /**
   * Publishes a complex tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   * @return true if the backend accepted the tunable
   */
  public static boolean publish(String path, ComplexTunable tunable) {
    String normalized = PathUtil.normalizeName(path);
    boolean missingBackend = false;
    synchronized (s_backends) {
      TunableBackend backend = getBackendForNormalizedPath(normalized);
      if (isMissingBackend(backend)) {
        missingBackend = true;
      } else {
        boolean addedPath = addComplexPath(path, tunable);
        if (backend.publishComplex(path, tunable)) {
          recordComplexMigrationPublish(path);
          return true;
        } else if (addedPath) {
          removeComplexPath(path, tunable);
        }
      }
    }
    if (missingBackend) {
      reportWarning("no backend for path '" + normalized + "'");
    }
    return false;
  }

  /**
   * Publishes a child tunable under every currently published path for a complex tunable.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   * @param tunable the child tunable
   */
  static void publishChild(ComplexTunable parent, String name, TunableBase tunable) {
    for (String path : getComplexPaths(parent)) {
      publish(PathUtil.normalizeName(path + "/" + name), tunable);
    }
  }

  /**
   * Publishes a child complex tunable under every currently published path for a complex tunable.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   * @param tunable the child complex tunable
   */
  static void publishChild(ComplexTunable parent, String name, ComplexTunable tunable) {
    for (String path : getComplexPaths(parent)) {
      publish(PathUtil.normalizeName(path + "/" + name), tunable);
    }
  }

  /**
   * Removes a child tunable from every currently published path for a complex tunable.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   */
  static void removeChild(ComplexTunable parent, String name) {
    for (String path : getComplexPaths(parent)) {
      remove(PathUtil.normalizeName(path + "/" + name));
    }
  }

  /**
   * Marks a child tunable as locally changed.
   *
   * @param parent the parent complex tunable
   * @param name the child name
   */
  static void setChildChanged(ComplexTunable parent, String name) {
    List<TunableBase> children = new ArrayList<>();
    synchronized (s_complexPathsMutex) {
      List<String> paths = s_complexPaths.get(parent);
      if (paths == null) {
        return;
      }
      for (String path : paths) {
        TunableBase child = s_complexChildrenByPath.get(PathUtil.normalizeName(path + "/" + name));
        if (child != null && !children.contains(child)) {
          children.add(child);
        }
      }
    }
    for (TunableBase child : children) {
      child.markChanged();
    }
  }

  /**
   * Removes a tunable.
   *
   * @param path normalized path
   */
  public static void remove(String path) {
    // Backends may have changed since publishing, so remove from all backends
    synchronized (s_backends) {
      String childPrefix = PathUtil.childTablePath(path);
      for (var entry : s_backends.values()) {
        entry.remove(path);
        entry.removePrefix(childPrefix);
      }
    }
    removeComplexChildPaths(path);
    removeComplexPaths(path);
  }

  /**
   * Removes every tracked publication for a complex tunable.
   *
   * @param tunable complex tunable to remove
   */
  public static void remove(ComplexTunable tunable) {
    for (String path : getComplexPaths(tunable)) {
      remove(path);
    }
  }

  /** Updates all tunable values from backends. */
  public static void update() {
    synchronized (s_backends) {
      s_updateDepth++;
      for (TunableBackend entry : s_backendSnapshot) {
        entry.update();
      }
      finishUpdate();
    }
  }

  /**
   * Execute a function while holding the same mutex used for update(). As tunables are not
   * thread-safe, this is designed to allow external synchronization for tunables used in secondary
   * threads outside of the robot main loop.
   *
   * <p>Every competing access, including main-loop access, must use this method or another safe
   * handoff. The update mutex is held while backends and user callbacks run, so the function should
   * be short and must not allow a mutable tunable value to escape and be accessed after the
   * function completes.
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
    List<TunableBackend> backends = new ArrayList<>();
    synchronized (s_typeHandlers) {
      s_typeHandlers.clear();
    }
    synchronized (s_backends) {
      backends.addAll(Arrays.asList(s_backendSnapshot));
      s_backends.clear();
      updateBackendSnapshot();
      s_pendingChangedResets.clear();
      s_updatedComplexTunables.clear();
      s_pendingUpdateCallbacks.clear();
      s_updateDepth = 0;
    }
    for (TunableBackend backend : backends) {
      backend.removePrefix("");
      try {
        backend.close();
      } catch (Exception e) {
        System.out.println("Unexpected exception when closing backend: " + e);
      }
    }
    synchronized (s_complexPathsMutex) {
      s_complexPaths.clear();
      s_complexByPath.clear();
      s_complexChildrenByPath.clear();
    }
  }

  private static void addComplexChildPath(String path, TunableBase tunable) {
    synchronized (s_complexPathsMutex) {
      String bestPrefix = null;
      for (String complexPath : s_complexByPath.keySet()) {
        String childPrefix = PathUtil.childTablePath(complexPath);
        if (path.startsWith(childPrefix)
            && (bestPrefix == null || childPrefix.length() > bestPrefix.length())) {
          bestPrefix = childPrefix;
        }
      }
      if (bestPrefix != null) {
        s_complexChildrenByPath.put(path, tunable);
      }
    }
  }

  private static boolean addComplexPath(String path, ComplexTunable tunable) {
    synchronized (s_complexPathsMutex) {
      if (s_complexByPath.containsKey(path)) {
        return false;
      }
      s_complexByPath.put(path, tunable);
      s_complexPaths.computeIfAbsent(tunable, k -> new ArrayList<>()).add(path);
      return true;
    }
  }

  private static void removeComplexPaths(String path) {
    synchronized (s_complexPathsMutex) {
      List<String> paths = new ArrayList<>();
      for (String complexPath : s_complexByPath.keySet()) {
        if (PathUtil.isPathOrDescendant(complexPath, path)) {
          paths.add(complexPath);
        }
      }
      for (String complexPath : paths) {
        ComplexTunable tunable = s_complexByPath.remove(complexPath);
        removeComplexPath(complexPath, tunable);
      }
    }
  }

  private static void removeComplexChildPaths(String path) {
    synchronized (s_complexPathsMutex) {
      s_complexChildrenByPath
          .keySet()
          .removeIf(childPath -> PathUtil.isPathOrDescendant(childPath, path));
    }
  }

  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private static void removeComplexPath(String path, ComplexTunable tunable) {
    if (tunable == null) {
      return;
    }
    synchronized (s_complexPathsMutex) {
      if (s_complexByPath.get(path) == tunable) {
        s_complexByPath.remove(path);
      }
      List<String> paths = s_complexPaths.get(tunable);
      if (paths == null) {
        return;
      }
      paths.remove(path);
      if (paths.isEmpty()) {
        s_complexPaths.remove(tunable);
      }
    }
  }

  private static List<String> getComplexPaths(ComplexTunable tunable) {
    synchronized (s_complexPathsMutex) {
      List<String> paths = s_complexPaths.get(tunable);
      if (paths == null) {
        return List.of();
      }
      return List.copyOf(paths);
    }
  }
}
