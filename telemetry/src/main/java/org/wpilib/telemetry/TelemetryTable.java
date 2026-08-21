// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import java.lang.reflect.Array;
import java.util.Collection;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicLong;
import org.wpilib.telemetry.util.PathUtil;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

/**
 * Telemetry sends information from the robot program to dashboards, debug tools, or log files.
 *
 * <p>For more advanced use cases, use the NetworkTables or DataLog APIs.
 */
public final class TelemetryTable {
  private record StaticFieldLookup(Object value, String warning) {}

  private record CachedEntry(TelemetryEntry entry, long resetGeneration) {}

  private enum EntryMetadataKind {
    KEEP_DUPLICATES,
    PROPERTY
  }

  private static final ClassValue<StaticFieldLookup> s_structLookupCache =
      new ClassValue<>() {
        @Override
        protected StaticFieldLookup computeValue(Class<?> type) {
          return getStaticField(type, "struct");
        }
      };

  private static final ClassValue<StaticFieldLookup> s_protoLookupCache =
      new ClassValue<>() {
        @Override
        protected StaticFieldLookup computeValue(Class<?> type) {
          return getStaticField(type, "proto");
        }
      };

  private final String m_path;
  private final TelemetryBackend m_backend;
  private final ConcurrentMap<String, TelemetryTable> m_tableAliasesMap = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, TelemetryTable> m_tablesMap = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, CachedEntry> m_entriesMap = new ConcurrentHashMap<>();
  private String m_type;
  private Boolean m_hasNonDiscardDescendant;
  private final AtomicLong m_resetGeneration = new AtomicLong();

  private static StaticFieldLookup getStaticField(Class<?> type, String fieldName) {
    try {
      return new StaticFieldLookup(type.getField(fieldName).get(null), null);
    } catch (NoSuchFieldException e) {
      return new StaticFieldLookup(
          null, "could not get " + fieldName + " field for " + type.getName());
    } catch (IllegalAccessException e) {
      return new StaticFieldLookup(
          null, "could not access " + fieldName + " field for " + type.getName());
    }
  }

  /**
   * Constructs a telemetry table.
   *
   * @param path path with trailing "/".
   */
  TelemetryTable(String path) {
    this(path, null);
  }

  /**
   * Constructs a root telemetry table that writes directly to a backend without using the global
   * telemetry registry.
   *
   * @param backend telemetry backend
   */
  public TelemetryTable(TelemetryBackend backend) {
    this("/", Objects.requireNonNull(backend));
  }

  private TelemetryTable(String path, TelemetryBackend backend) {
    m_path = path;
    m_backend = backend;
  }

  /** Clears the table's cached entries. */
  void reset() {
    synchronized (this) {
      m_tableAliasesMap.clear();
      m_tablesMap.clear();
      m_entriesMap.clear();
      m_type = null;
      m_hasNonDiscardDescendant = null;
      m_resetGeneration.incrementAndGet();
    }
  }

  /**
   * Gets the table path.
   *
   * @return path with trailing "/"
   */
  public String getPath() {
    return m_path;
  }

  /**
   * Sets the table type. TelemetryLoggable implementations can use this function to communicate the
   * type of data in the table. Callers should check the return value and not log data if false is
   * returned.
   *
   * @param typeString type string
   * @return False if type mismatch.
   */
  public boolean setType(String typeString) {
    for (; ; ) {
      String expectedType = null;
      long resetGeneration;
      synchronized (this) {
        if (m_type != null) {
          if (m_type.equals(typeString)) {
            return true;
          }
          expectedType = m_type;
          resetGeneration = -1;
        } else {
          m_type = typeString;
          resetGeneration = m_resetGeneration.get();
        }
      }
      if (expectedType != null) {
        typeMismatch(expectedType, typeString);
        return false;
      }

      TelemetryEntry entry = getEntry(".type");
      boolean publishType;
      synchronized (this) {
        publishType =
            m_resetGeneration.get() == resetGeneration && Objects.equals(typeString, m_type);
      }
      if (publishType) {
        if (!entry.isDiscard()) {
          entry.logString(typeString, "string");
        }
        return true;
      }
    }
  }

  private void typeMismatch(String expectedType, String typeString) {
    TelemetryRegistry.reportWarning(
        m_path, "table type mismatch, expected '" + expectedType + "', got '" + typeString + "'");
  }

  /**
   * Gets the table type.
   *
   * @return Table type as set by setType(), or null if none set.
   */
  public String getType() {
    synchronized (this) {
      return m_type;
    }
  }

  /**
   * Gets a child telemetry table.
   *
   * @param name table name
   * @return table
   */
  public TelemetryTable getTable(String name) {
    TelemetryTable table = m_tableAliasesMap.get(name);
    if (table != null) {
      return table;
    }

    String path = getTablePath(name);
    table = m_tablesMap.get(path);
    if (table != null) {
      TelemetryTable aliasTable = m_tableAliasesMap.putIfAbsent(name, table);
      return aliasTable != null ? aliasTable : table;
    }

    TelemetryTable newTable =
        m_backend != null ? new TelemetryTable(path, m_backend) : TelemetryRegistry.getTable(path);
    table = m_tablesMap.putIfAbsent(path, newTable);
    if (table == null) {
      table = newTable;
    }

    TelemetryTable aliasTable = m_tableAliasesMap.putIfAbsent(name, table);
    return aliasTable != null ? aliasTable : table;
  }

  private String getEntryPath(String name) {
    return PathUtil.normalizeName(m_path + name);
  }

  private String getTablePath(String name) {
    return PathUtil.normalizeTableName(m_path + name + "/");
  }

  /**
   * Gets a telemetry entry.
   *
   * @param name name
   * @return entry
   */
  private TelemetryEntry getEntry(String name) {
    for (; ; ) {
      CachedEntry cachedEntry = m_entriesMap.get(name);
      if (cachedEntry != null) {
        if (cachedEntry.resetGeneration() == m_resetGeneration.get()) {
          return cachedEntry.entry();
        }
        m_entriesMap.remove(name, cachedEntry);
        continue;
      }

      long resetGeneration = m_resetGeneration.get();
      String path = getEntryPath(name);
      TelemetryEntry newEntry =
          m_backend != null ? m_backend.getEntry(path) : TelemetryRegistry.getEntry(path);
      if (m_backend == null && !newEntry.isDiscard()) {
        TelemetryRegistry.applyEntryMetadata(path, newEntry);
      }

      CachedEntry newCachedEntry = new CachedEntry(newEntry, resetGeneration);
      cachedEntry = m_entriesMap.putIfAbsent(name, newCachedEntry);
      if (cachedEntry == null) {
        cachedEntry = newCachedEntry;
      }
      if (cachedEntry.resetGeneration() == m_resetGeneration.get()) {
        return cachedEntry.entry();
      }
      m_entriesMap.remove(name, cachedEntry);
    }
  }

  private void applyEntryMetadata(
      String name, String path, EntryMetadataKind kind, String key, String value) {
    for (; ; ) {
      CachedEntry cachedEntry = m_entriesMap.get(name);
      if (cachedEntry != null) {
        if (cachedEntry.resetGeneration() == m_resetGeneration.get()) {
          applyEntryMetadata(cachedEntry.entry(), kind, key, value);
          return;
        }
        m_entriesMap.remove(name, cachedEntry);
        continue;
      }

      long resetGeneration = m_resetGeneration.get();
      TelemetryEntry newEntry =
          m_backend != null ? m_backend.getEntry(path) : TelemetryRegistry.getEntry(path);
      boolean metadataApplied = false;
      if (m_backend == null && !newEntry.isDiscard()) {
        TelemetryRegistry.applyEntryMetadata(path, newEntry);
        metadataApplied = true;
      }

      CachedEntry newCachedEntry = new CachedEntry(newEntry, resetGeneration);
      cachedEntry = m_entriesMap.putIfAbsent(name, newCachedEntry);
      boolean inserted = cachedEntry == null;
      if (cachedEntry == null) {
        cachedEntry = newCachedEntry;
      }
      if (cachedEntry.resetGeneration() == m_resetGeneration.get()) {
        if (!(inserted && metadataApplied)) {
          applyEntryMetadata(cachedEntry.entry(), kind, key, value);
        }
        return;
      }
      m_entriesMap.remove(name, cachedEntry);
    }
  }

  private void applyEntryMetadata(
      TelemetryEntry entry, EntryMetadataKind kind, String key, String value) {
    if (!entry.isDiscard()) {
      if (kind == EntryMetadataKind.KEEP_DUPLICATES) {
        entry.keepDuplicates();
      } else {
        entry.setProperty(key, value);
      }
    }
  }

  private boolean shouldLogTableValue(String name, TelemetryTable table) {
    TelemetryEntry entry = getEntry(name);
    return !entry.isDiscard() || m_backend == null && table.hasNonDiscardDescendant();
  }

  private boolean hasNonDiscardDescendant() {
    for (; ; ) {
      long resetGeneration;
      synchronized (this) {
        if (m_hasNonDiscardDescendant != null) {
          return m_hasNonDiscardDescendant;
        }
        resetGeneration = m_resetGeneration.get();
      }

      boolean hasNonDiscardDescendant = TelemetryRegistry.hasNonDiscardDescendant(m_path);
      synchronized (this) {
        if (m_hasNonDiscardDescendant != null) {
          return m_hasNonDiscardDescendant;
        }
        if (resetGeneration != m_resetGeneration.get()) {
          continue;
        }
        m_hasNonDiscardDescendant = hasNonDiscardDescendant;
        return m_hasNonDiscardDescendant;
      }
    }
  }

  /**
   * Indicates duplicate values should be preserved. Normally duplicate values are ignored.
   *
   * @param name the name
   */
  public void keepDuplicates(String name) {
    if (m_backend != null) {
      TelemetryEntry entry = getEntry(name);
      if (!entry.isDiscard()) {
        entry.keepDuplicates();
      }
      return;
    }

    String path = getEntryPath(name);
    boolean done = false;
    while (!done) {
      long resetGeneration = m_resetGeneration.get();
      TelemetryRegistry.keepEntryDuplicates(path);
      applyEntryMetadata(name, path, EntryMetadataKind.KEEP_DUPLICATES, null, null);
      done = m_resetGeneration.get() == resetGeneration;
    }
  }

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param name the name
   * @param key property key
   * @param value property value
   */
  public void setProperty(String name, String key, String value) {
    if (m_backend != null) {
      TelemetryEntry entry = getEntry(name);
      if (!entry.isDiscard()) {
        entry.setProperty(key, value);
      }
      return;
    }

    String path = getEntryPath(name);
    boolean done = false;
    while (!done) {
      long resetGeneration = m_resetGeneration.get();
      TelemetryRegistry.setEntryProperty(path, key, value);
      applyEntryMetadata(name, path, EntryMetadataKind.PROPERTY, key, value);
      done = m_resetGeneration.get() == resetGeneration;
    }
  }

  /**
   * Logs a generic object.
   *
   * <p>For non-null values, the value is dispatched in this order: {@link TelemetryLoggable},
   * {@link org.wpilib.util.struct.StructSerializable}, {@link
   * org.wpilib.util.protobuf.ProtobufSerializable}, boxed primitive and string types, and finally
   * handlers registered with {@link TelemetryRegistry#registerTypeHandler(Class,
   * TelemetryRegistry.TypeHandler)}. {@link TelemetryLoggable} values are logged to a child table
   * named {@code name}. Struct and protobuf serializable values are logged with their public static
   * {@code struct} or {@code proto} serializers. {@link Boolean}, {@link Float}, {@link Double},
   * {@link Byte}, {@link Short}, {@link Integer}, {@link Long}, other {@link Number} values, and
   * {@link String} are logged as primitive or string entries. Generic {@link Number} values other
   * than boxed integral types are logged as double entries. If no registered handler matches, the
   * value falls back to being logged as a string using {@link Object#toString()}. Collection values
   * must be logged with an explicit element type.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value; must be non-null
   */
  public <T> void log(String name, T value) {
    switch (value) {
      case TelemetryLoggable v -> {
        TelemetryTable table = getTable(name);
        if (!shouldLogTableValue(name, table)) {
          return;
        }
        String typeString = v.getTelemetryType();
        if (typeString != null && !table.setType(typeString)) {
          return;
        }
        v.logTo(table);
      }
      case StructSerializable v -> {
        TelemetryEntry entry = getEntry(name);
        if (entry.isDiscard()) {
          return;
        }
        var lookup = s_structLookupCache.get(v.getClass());
        if (lookup.warning() != null) {
          TelemetryRegistry.reportWarning(getEntryPath(name), lookup.warning());
          return;
        }
        switch (lookup.value()) {
          case Struct<?> s when s.getTypeClass().isAssignableFrom(value.getClass()) -> {
            @SuppressWarnings("unchecked")
            Struct<? super T> s2 = (Struct<? super T>) s;
            entry.logStruct(value, s2);
          }
          case Struct<?> s ->
              TelemetryRegistry.reportWarning(
                  getEntryPath(name),
                  "type mismatch, expected '"
                      + s.getTypeClass().getName()
                      + "', got '"
                      + value.getClass().getName()
                      + "'");
          default ->
              TelemetryRegistry.reportWarning(
                  getEntryPath(name),
                  "struct field for " + v.getClass().getName() + " is not of Struct<?> type");
        }
      }
      case ProtobufSerializable v -> {
        TelemetryEntry entry = getEntry(name);
        if (entry.isDiscard()) {
          return;
        }
        var lookup = s_protoLookupCache.get(v.getClass());
        if (lookup.warning() != null) {
          TelemetryRegistry.reportWarning(getEntryPath(name), lookup.warning());
          return;
        }
        switch (lookup.value()) {
          case Protobuf<?, ?> s when s.getTypeClass().isAssignableFrom(value.getClass()) -> {
            @SuppressWarnings("unchecked")
            Protobuf<? super T, ?> s2 = (Protobuf<? super T, ?>) s;
            entry.logProtobuf(value, s2);
          }
          case Protobuf<?, ?> s ->
              TelemetryRegistry.reportWarning(
                  getEntryPath(name),
                  "type mismatch, expected '"
                      + s.getTypeClass().getName()
                      + "', got '"
                      + value.getClass().getName()
                      + "'");
          default ->
              TelemetryRegistry.reportWarning(
                  getEntryPath(name),
                  "proto field for " + v.getClass().getName() + " is not of Protobuf<?, ?> type");
        }
      }
      case Boolean v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logBoolean(v.booleanValue());
        }
      }
      case Float v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logFloat(v.floatValue());
        }
      }
      case Double v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logDouble(v.doubleValue());
        }
      }
      case Byte v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logLong(v.longValue());
        }
      }
      case Short v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logLong(v.longValue());
        }
      }
      case Integer v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logLong(v.longValue());
        }
      }
      case Long v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logLong(v.longValue());
        }
      }
      case Number v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logDouble(v.doubleValue());
        }
      }
      case String v -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          entry.logString(v, "string");
        }
      }
      case Collection<?> _ -> {
        TelemetryEntry entry = getEntry(name);
        if (!entry.isDiscard()) {
          TelemetryRegistry.reportWarning(
              getEntryPath(name), "collection element type must be specified");
        }
      }
      default -> {
        // try other handlers
        var handler = TelemetryRegistry.getTypeHandler(value);
        if (handler != null) {
          TelemetryTable table = getTable(name);
          if (!shouldLogTableValue(name, table)) {
            return;
          }
          handler.logTo(this, name, value);
        } else {
          // fall back to string
          TelemetryEntry entry = getEntry(name);
          if (!entry.isDiscard()) {
            entry.logString(value.toString(), "string");
          }
        }
      }
    }
  }

  /**
   * Logs an object with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   */
  public <T> void log(String name, T value, Struct<? super T> struct) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logStruct(value, struct);
  }

  /**
   * Logs an object with a Protobuf serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param proto protobuf serializer for the value type or one of its supertypes
   */
  public <T> void log(String name, T value, Protobuf<? super T, ?> proto) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logProtobuf(value, proto);
  }

  /**
   * Logs a generic array.
   *
   * <p>{@link Byte} arrays are logged as raw byte arrays.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   */
  public <T> void log(String name, T[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    switch (value) {
      case StructSerializable[] _ -> {
        Class<?> componentType = value.getClass().getComponentType();
        var lookup = s_structLookupCache.get(componentType);
        if (lookup.warning() != null) {
          TelemetryRegistry.reportWarning(getEntryPath(name), lookup.warning());
          return;
        }
        switch (lookup.value()) {
          case Struct<?> s when s.getTypeClass().isAssignableFrom(componentType) -> {
            @SuppressWarnings("unchecked")
            Struct<? super T> s2 = (Struct<? super T>) s;
            entry.logStructArray(value, s2);
          }
          case Struct<?> s ->
              TelemetryRegistry.reportWarning(
                  getEntryPath(name),
                  "type mismatch, expected '"
                      + s.getTypeClass().getName()
                      + "', got '"
                      + value.getClass().getName()
                      + "'");
          default ->
              TelemetryRegistry.reportWarning(
                  getEntryPath(name),
                  "struct field for " + componentType.getName() + " is not of Struct<?> type");
        }
      }
      case Boolean[] v -> {
        boolean[] arr = new boolean[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].booleanValue();
        }
        entry.logBooleanArray(arr);
      }
      case Byte[] v -> {
        byte[] arr = new byte[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].byteValue();
        }
        entry.logRaw(arr, "raw");
      }
      case Float[] v -> {
        float[] arr = new float[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].floatValue();
        }
        entry.logFloatArray(arr);
      }
      case Double[] v -> {
        double[] arr = new double[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].doubleValue();
        }
        entry.logDoubleArray(arr);
      }
      case Number[] v -> {
        logNumberArray(entry, v);
      }
      default -> {
        // TODO: use other Object handler?
        // fall back to string array
        String[] strs = new String[value.length];
        for (int i = 0; i < value.length; i++) {
          strs[i] = String.valueOf(value[i]);
        }
        entry.logStringArray(strs);
      }
    }
  }

  /**
   * Logs an array of objects with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   */
  public <T> void log(String name, T[] value, Struct<? super T> struct) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logStructArray(value, struct);
  }

  /**
   * Rejects logging a collection without an explicit element type.
   *
   * <p>Use {@link #log(String, Collection, Class)} or {@link #log(String, Collection, Struct)}
   * instead.
   *
   * @param name the name
   * @param value the value
   * @throws CollectionElementTypeRequiredException always
   */
  public void log(String name, Collection<?> value) throws CollectionElementTypeRequiredException {
    throw new CollectionElementTypeRequiredException();
  }

  /**
   * Logs a collection with an explicit element type.
   *
   * <p>The element type selects the backend representation. {@link Byte} element types are logged
   * as raw byte arrays. {@link Boolean}, {@link Float}, {@link Double}, and {@link String} element
   * types are logged like arrays of the same element type. {@link Number} and other non-integral
   * number element types are logged as double arrays. {@link Short}, {@link Integer}, and {@link
   * Long} element types are logged as integer arrays. {@link StructSerializable} element types are
   * logged with their public static {@code struct} serializer. Other element types fall back to
   * string arrays using {@link Object#toString()}.
   *
   * @param name the name
   * @param value the value
   * @param elementType the collection element type
   */
  public void log(String name, Collection<?> value, Class<?> elementType) {
    Objects.requireNonNull(value, "value");
    Objects.requireNonNull(elementType, "elementType");
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }

    if (isBooleanType(elementType)) {
      logBooleanCollection(name, entry, value);
    } else if (isByteType(elementType)) {
      logByteCollection(name, entry, value);
    } else if (isFloatType(elementType)) {
      logFloatCollection(name, entry, value);
    } else if (isDoubleType(elementType)) {
      logDoubleCollection(name, entry, value);
    } else if (isIntegralNumberType(elementType)) {
      logLongCollection(name, entry, value);
    } else if (isNumberType(elementType)) {
      logDoubleCollection(name, entry, value);
    } else if (elementType == String.class) {
      logStringCollection(name, entry, value);
    } else if (StructSerializable.class.isAssignableFrom(elementType)) {
      var lookup = s_structLookupCache.get(elementType);
      if (lookup.warning() != null) {
        TelemetryRegistry.reportWarning(getEntryPath(name), lookup.warning());
        return;
      }
      switch (lookup.value()) {
        case Struct<?> s when s.getTypeClass().isAssignableFrom(elementType) ->
            logStructCollection(name, entry, value, s);
        case Struct<?> s ->
            TelemetryRegistry.reportWarning(
                getEntryPath(name),
                "type mismatch, expected '"
                    + s.getTypeClass().getName()
                    + "', got '"
                    + elementType.getName()
                    + "'");
        default ->
            TelemetryRegistry.reportWarning(
                getEntryPath(name),
                "struct field for " + elementType.getName() + " is not of Struct<?> type");
      }
    } else {
      logFallbackStringCollection(entry, value);
    }
  }

  /**
   * Logs a collection of objects with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   */
  public <T> void log(String name, Collection<T> value, Struct<? super T> struct) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logStructArray(toArray(value, struct.getTypeClass()), struct);
  }

  /**
   * Logs a boolean.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, boolean value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logBoolean(value);
  }

  /**
   * Logs a byte.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, byte value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logByte(value);
  }

  /**
   * Logs a short.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, short value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logShort(value);
  }

  /**
   * Logs an int.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, int value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logInt(value);
  }

  /**
   * Logs a long.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, long value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logLong(value);
  }

  /**
   * Logs a float.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, float value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logFloat(value);
  }

  /**
   * Logs a double.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, double value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logDouble(value);
  }

  /**
   * Logs a String.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, String value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logString(value, "string");
  }

  /**
   * Logs a String with a custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  public void log(String name, String value, String typeString) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logString(value, typeString);
  }

  /**
   * Logs a boolean array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, boolean[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logBooleanArray(value);
  }

  /**
   * Logs a short array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, short[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logShortArray(value);
  }

  /**
   * Logs an int array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, int[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logIntArray(value);
  }

  /**
   * Logs a long array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, long[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logLongArray(value);
  }

  /**
   * Logs a float array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, float[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logFloatArray(value);
  }

  /**
   * Logs a double array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, double[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logDoubleArray(value);
  }

  /**
   * Logs a String array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, String[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logStringArray(value);
  }

  /**
   * Logs a raw value (byte array).
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, byte[] value) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logRaw(value, "raw");
  }

  /**
   * Logs a raw value (byte array) with a custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  public void log(String name, byte[] value, String typeString) {
    TelemetryEntry entry = getEntry(name);
    if (entry.isDiscard()) {
      return;
    }
    entry.logRaw(value, typeString);
  }

  @SuppressWarnings("unchecked")
  private static <T> T[] toArray(Collection<?> value, Class<?> componentType) {
    return value.toArray((T[]) Array.newInstance(componentType, value.size()));
  }

  private static boolean isBooleanType(Class<?> type) {
    return type == Boolean.class || type == Boolean.TYPE;
  }

  private static boolean isByteType(Class<?> type) {
    return type == Byte.class || type == Byte.TYPE;
  }

  private static boolean isFloatType(Class<?> type) {
    return type == Float.class || type == Float.TYPE;
  }

  private static boolean isDoubleType(Class<?> type) {
    return type == Double.class || type == Double.TYPE;
  }

  private static boolean isIntegralNumberType(Class<?> type) {
    return type == Short.class
        || type == Short.TYPE
        || type == Integer.class
        || type == Integer.TYPE
        || type == Long.class
        || type == Long.TYPE;
  }

  private static boolean isNumberType(Class<?> type) {
    return Number.class.isAssignableFrom(type);
  }

  private void logBooleanCollection(String name, TelemetryEntry entry, Collection<?> value) {
    boolean[] arr = new boolean[value.size()];
    int i = 0;
    for (Object element : value) {
      if (!(element instanceof Boolean v)) {
        collectionElementTypeMismatch(name, Boolean.class, element);
        return;
      }
      arr[i++] = v;
    }
    entry.logBooleanArray(arr);
  }

  private void logByteCollection(String name, TelemetryEntry entry, Collection<?> value) {
    byte[] arr = new byte[value.size()];
    int i = 0;
    for (Object element : value) {
      if (!(element instanceof Number v)) {
        collectionElementTypeMismatch(name, Number.class, element);
        return;
      }
      arr[i++] = v.byteValue();
    }
    entry.logRaw(arr, "raw");
  }

  private void logFloatCollection(String name, TelemetryEntry entry, Collection<?> value) {
    float[] arr = new float[value.size()];
    int i = 0;
    for (Object element : value) {
      if (!(element instanceof Number v)) {
        collectionElementTypeMismatch(name, Number.class, element);
        return;
      }
      arr[i++] = v.floatValue();
    }
    entry.logFloatArray(arr);
  }

  private void logDoubleCollection(String name, TelemetryEntry entry, Collection<?> value) {
    double[] arr = new double[value.size()];
    int i = 0;
    for (Object element : value) {
      if (!(element instanceof Number v)) {
        collectionElementTypeMismatch(name, Number.class, element);
        return;
      }
      arr[i++] = v.doubleValue();
    }
    entry.logDoubleArray(arr);
  }

  private void logLongCollection(String name, TelemetryEntry entry, Collection<?> value) {
    long[] arr = new long[value.size()];
    int i = 0;
    for (Object element : value) {
      if (!(element instanceof Number v)) {
        collectionElementTypeMismatch(name, Number.class, element);
        return;
      }
      arr[i++] = v.longValue();
    }
    entry.logLongArray(arr);
  }

  private void logStringCollection(String name, TelemetryEntry entry, Collection<?> value) {
    String[] arr = new String[value.size()];
    int i = 0;
    for (Object element : value) {
      if (!(element instanceof String v)) {
        collectionElementTypeMismatch(name, String.class, element);
        return;
      }
      arr[i++] = v;
    }
    entry.logStringArray(arr);
  }

  private void logStructCollection(
      String name, TelemetryEntry entry, Collection<?> value, Struct<?> struct) {
    logStructCollectionImpl(name, entry, value, struct);
  }

  private <T> void logStructCollectionImpl(
      String name, TelemetryEntry entry, Collection<?> value, Struct<T> struct) {
    try {
      entry.logStructArray(toArray(value, struct.getTypeClass()), struct);
    } catch (ArrayStoreException e) {
      collectionElementTypeMismatch(name, struct.getTypeClass(), null);
    }
  }

  private static void logFallbackStringCollection(TelemetryEntry entry, Collection<?> value) {
    String[] strs = new String[value.size()];
    int i = 0;
    for (Object element : value) {
      strs[i++] = String.valueOf(element);
    }
    entry.logStringArray(strs);
  }

  private void collectionElementTypeMismatch(String name, Class<?> expectedType, Object element) {
    TelemetryRegistry.reportWarning(
        getEntryPath(name),
        "collection element type mismatch, expected '"
            + expectedType.getName()
            + "', got '"
            + (element == null ? "null" : element.getClass().getName())
            + "'");
  }

  private static void logNumberArray(TelemetryEntry entry, Number[] value) {
    if (isIntegralNumberType(value.getClass().getComponentType())) {
      long[] arr = new long[value.length];
      for (int i = 0; i < value.length; i++) {
        arr[i] = value[i].longValue();
      }
      entry.logLongArray(arr);
    } else {
      double[] arr = new double[value.length];
      for (int i = 0; i < value.length; i++) {
        arr[i] = value[i].doubleValue();
      }
      entry.logDoubleArray(arr);
    }
  }
}
