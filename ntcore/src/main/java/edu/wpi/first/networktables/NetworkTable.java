/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Consumer;

/**
 * A network table that knows its subtable path.
 */
public final class NetworkTable {
  /**
   * The path separator for sub-tables and keys.
   */
  public static final char PATH_SEPARATOR = '/';

  private final String m_path;
  private final String m_pathWithSep;
  private final NetworkTableInstance m_inst;

  /**
   * Gets the "base name" of a key. For example, "/foo/bar" becomes "bar".
   * If the key has a trailing slash, returns an empty string.
   *
   * @param key key
   * @return base name
   */
  public static String basenameKey(String key) {
    final int slash = key.lastIndexOf(PATH_SEPARATOR);
    if (slash == -1) {
      return key;
    }
    return key.substring(slash + 1);
  }

  /**
   * Normalizes an network table key to contain no consecutive slashes and
   * optionally start with a leading slash. For example:
   *
   * <pre><code>
   * normalizeKey("/foo/bar", true)  == "/foo/bar"
   * normalizeKey("foo/bar", true)   == "/foo/bar"
   * normalizeKey("/foo/bar", false) == "foo/bar"
   * normalizeKey("foo//bar", false) == "foo/bar"
   * </code></pre>
   *
   * @param key              the key to normalize
   * @param withLeadingSlash whether or not the normalized key should begin
   *                         with a leading slash
   * @return normalized key
   */
  public static String normalizeKey(String key, boolean withLeadingSlash) {
    String normalized;
    if (withLeadingSlash) {
      normalized = PATH_SEPARATOR + key;
    } else {
      normalized = key;
    }
    normalized = normalized.replaceAll(PATH_SEPARATOR + "{2,}", String.valueOf(PATH_SEPARATOR));

    if (!withLeadingSlash && normalized.charAt(0) == PATH_SEPARATOR) {
      // remove leading slash, if present
      normalized = normalized.substring(1);
    }
    return normalized;
  }

  /**
   * Normalizes a network table key to start with exactly one leading slash
   * ("/") and contain no consecutive slashes. For example,
   * {@code "//foo/bar/"} becomes {@code "/foo/bar/"} and
   * {@code "///a/b/c"} becomes {@code "/a/b/c"}.
   *
   * <p>This is equivalent to {@code normalizeKey(key, true)}
   *
   * @param key the key to normalize
   * @return normalized key
   */
  public static String normalizeKey(String key) {
    return normalizeKey(key, true);
  }

  /**
   * Gets a list of the names of all the super tables of a given key. For
   * example, the key "/foo/bar/baz" has a hierarchy of "/", "/foo",
   * "/foo/bar", and "/foo/bar/baz".
   *
   * @param key the key
   * @return List of super tables
   */
  public static List<String> getHierarchy(String key) {
    final String normal = normalizeKey(key, true);
    List<String> hierarchy = new ArrayList<>();
    if (normal.length() == 1) {
      hierarchy.add(normal);
      return hierarchy;
    }
    for (int i = 1; ; i = normal.indexOf(PATH_SEPARATOR, i + 1)) {
      if (i == -1) {
        // add the full key
        hierarchy.add(normal);
        break;
      } else {
        hierarchy.add(normal.substring(0, i));
      }
    }
    return hierarchy;
  }

  /**
   * Constructor.  Use NetworkTableInstance.getTable() or getSubTable() instead.
   */
  NetworkTable(NetworkTableInstance inst, String path) {
    m_path = path;
    m_pathWithSep = path + PATH_SEPARATOR;
    m_inst = inst;
  }

  /**
   * Gets the instance for the table.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }

  @Override
  public String toString() {
    return "NetworkTable: " + m_path;
  }

  private final ConcurrentMap<String, NetworkTableEntry> m_entries = new ConcurrentHashMap<>();

  /**
   * Gets the entry for a sub key.
   *
   * @param key the key name
   * @return Network table entry.
   */
  public NetworkTableEntry getEntry(String key) {
    NetworkTableEntry entry = m_entries.get(key);
    if (entry == null) {
      entry = m_inst.getEntry(m_pathWithSep + key);
      m_entries.putIfAbsent(key, entry);
    }
    return entry;
  }

  /**
   * Listen to keys only within this table.
   *
   * @param listener    listener to add
   * @param flags       {@link EntryListenerFlags} bitmask
   * @return Listener handle
   */
  public int addEntryListener(TableEntryListener listener, int flags) {
    final int prefixLen = m_path.length() + 1;
    return m_inst.addEntryListener(m_pathWithSep, event -> {
      String relativeKey = event.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1) {
        // part of a sub table
        return;
      }
      listener.valueChanged(this, relativeKey, event.getEntry(), event.value, event.flags);
    }, flags);
  }

  /**
   * Listen to a single key.
   *
   * @param key         the key name
   * @param listener    listener to add
   * @param flags       {@link EntryListenerFlags} bitmask
   * @return Listener handle
   */
  public int addEntryListener(String key, TableEntryListener listener, int flags) {
    final NetworkTableEntry entry = getEntry(key);
    return m_inst.addEntryListener(entry,
        event -> listener.valueChanged(this, key, entry, event.value, event.flags), flags);
  }

  /**
   * Remove an entry listener.
   *
   * @param listener    listener handle
   */
  public void removeEntryListener(int listener) {
    m_inst.removeEntryListener(listener);
  }

  /**
   * Listen for sub-table creation.
   * This calls the listener once for each newly created sub-table.
   * It immediately calls the listener for any existing sub-tables.
   *
   * @param listener        listener to add
   * @param localNotify     notify local changes as well as remote
   * @return Listener handle
   */
  public int addSubTableListener(TableListener listener, boolean localNotify) {
    int flags = EntryListenerFlags.kNew | EntryListenerFlags.kImmediate;
    if (localNotify) {
      flags |= EntryListenerFlags.kLocal;
    }

    final int prefixLen = m_path.length() + 1;
    final NetworkTable parent = this;

    return m_inst.addEntryListener(m_pathWithSep, new Consumer<EntryNotification>() {
      final Set<String> m_notifiedTables = new HashSet<>();

      @Override
      public void accept(EntryNotification event) {
        String relativeKey = event.name.substring(prefixLen);
        int endSubTable = relativeKey.indexOf(PATH_SEPARATOR);
        if (endSubTable == -1) {
          return;
        }
        String subTableKey = relativeKey.substring(0, endSubTable);
        if (m_notifiedTables.contains(subTableKey)) {
          return;
        }
        m_notifiedTables.add(subTableKey);
        listener.tableCreated(parent, subTableKey, parent.getSubTable(subTableKey));
      }
    }, flags);
  }

  /**
   * Remove a sub-table listener.
   *
   * @param listener    listener handle
   */
  public void removeTableListener(int listener) {
    m_inst.removeEntryListener(listener);
  }

  /**
   * Returns the table at the specified key. If there is no table at the
   * specified key, it will create a new table
   *
   * @param key the name of the table relative to this one
   * @return a sub table relative to this one
   */
  public NetworkTable getSubTable(String key) {
    return new NetworkTable(m_inst, m_pathWithSep + key);
  }

  /**
   * Checks the table and tells if it contains the specified key.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  public boolean containsKey(String key) {
    return !("".equals(key)) && getEntry(key).exists();
  }

  /**
   * Checks the table and tells if it contains the specified sub table.
   *
   * @param key the key to search for
   * @return true if there is a subtable with the key which contains at least one key/subtable of
   *     its own
   */
  public boolean containsSubTable(String key) {
    int[] handles = NetworkTablesJNI.getEntries(m_inst.getHandle(),
        m_pathWithSep + key + PATH_SEPARATOR, 0);
    return handles.length != 0;
  }

  /**
   * Gets all keys in the table (not including sub-tables).
   *
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  public Set<String> getKeys(int types) {
    Set<String> keys = new HashSet<>();
    int prefixLen = m_path.length() + 1;
    for (EntryInfo info : m_inst.getEntryInfo(m_pathWithSep, types)) {
      String relativeKey = info.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1) {
        continue;
      }
      keys.add(relativeKey);
      // populate entries as we go
      if (m_entries.get(relativeKey) == null) {
        m_entries.putIfAbsent(relativeKey, new NetworkTableEntry(m_inst, info.entry));
      }
    }
    return keys;
  }

  /**
   * Gets all keys in the table (not including sub-tables).
   *
   * @return keys currently in the table
   */
  public Set<String> getKeys() {
    return getKeys(0);
  }

  /**
   * Gets the names of all subtables in the table.
   *
   * @return subtables currently in the table
   */
  public Set<String> getSubTables() {
    Set<String> keys = new HashSet<>();
    int prefixLen = m_path.length() + 1;
    for (EntryInfo info : m_inst.getEntryInfo(m_pathWithSep, 0)) {
      String relativeKey = info.name.substring(prefixLen);
      int endSubTable = relativeKey.indexOf(PATH_SEPARATOR);
      if (endSubTable == -1) {
        continue;
      }
      keys.add(relativeKey.substring(0, endSubTable));
    }
    return keys;
  }

  /**
   * Deletes the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   */
  public void delete(String key) {
    getEntry(key).delete();
  }

  /**
   * Put a value in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  boolean putValue(String key, NetworkTableValue value) {
    return getEntry(key).setValue(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  boolean setDefaultValue(String key, NetworkTableValue defaultValue) {
    return getEntry(key).setDefaultValue(defaultValue);
  }

  /**
   * Gets the value associated with a key as an object.
   *
   * @param key the key of the value to look up
   * @return the value associated with the given key, or nullptr if the key does not exist
   */
  NetworkTableValue getValue(String key) {
    return getEntry(key).getValue();
  }

  /**
   * Get the path of the NetworkTable.
   */
  public String getPath() {
    return m_path;
  }

  /**
   * Save table values to a file.  The file format used is identical to
   * that used for SavePersistent.
   *
   * @param filename  filename
   * @throws PersistentException if error saving file
   */
  public void saveEntries(String filename) throws PersistentException {
    m_inst.saveEntries(filename, m_pathWithSep);
  }

  /**
   * Load table values from a file.  The file format used is identical to
   * that used for SavePersistent / LoadPersistent.
   *
   * @param filename  filename
   * @return List of warnings (errors result in an exception instead)
   * @throws PersistentException if error saving file
   */
  public String[] loadEntries(String filename) throws PersistentException {
    return m_inst.loadEntries(filename, m_pathWithSep);
  }

  @Override
  public boolean equals(Object other) {
    if (other == this) {
      return true;
    }
    if (!(other instanceof NetworkTable)) {
      return false;
    }
    NetworkTable ntOther = (NetworkTable) other;
    return m_inst.equals(ntOther.m_inst) && m_path.equals(ntOther.m_path);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_inst, m_path);
  }
}
