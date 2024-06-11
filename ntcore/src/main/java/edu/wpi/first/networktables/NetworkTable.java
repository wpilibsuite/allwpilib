// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Consumer;
import us.hebi.quickbuf.ProtoMessage;

/** A network table that knows its subtable path. */
public final class NetworkTable {
  /** The path separator for sub-tables and keys. */
  public static final char PATH_SEPARATOR = '/';

  private final String m_path;
  private final String m_pathWithSep;
  private final NetworkTableInstance m_inst;

  /**
   * Gets the "base name" of a key. For example, "/foo/bar" becomes "bar". If the key has a trailing
   * slash, returns an empty string.
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
   * Normalizes an network table key to contain no consecutive slashes and optionally start with a
   * leading slash. For example:
   *
   * <pre><code>
   * normalizeKey("/foo/bar", true)  == "/foo/bar"
   * normalizeKey("foo/bar", true)   == "/foo/bar"
   * normalizeKey("/foo/bar", false) == "foo/bar"
   * normalizeKey("foo//bar", false) == "foo/bar"
   * </code></pre>
   *
   * @param key the key to normalize
   * @param withLeadingSlash whether or not the normalized key should begin with a leading slash
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
   * Normalizes a network table key to start with exactly one leading slash ("/") and contain no
   * consecutive slashes. For example, {@code "//foo/bar/"} becomes {@code "/foo/bar/"} and {@code
   * "///a/b/c"} becomes {@code "/a/b/c"}.
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
   * Gets a list of the names of all the super tables of a given key. For example, the key
   * "/foo/bar/baz" has a hierarchy of "/", "/foo", "/foo/bar", and "/foo/bar/baz".
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

  /** Constructor. Use NetworkTableInstance.getTable() or getSubTable() instead. */
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

  /**
   * Get (generic) topic.
   *
   * @param name topic name
   * @return Topic
   */
  public Topic getTopic(String name) {
    return m_inst.getTopic(m_pathWithSep + name);
  }

  /**
   * Get boolean topic.
   *
   * @param name topic name
   * @return BooleanTopic
   */
  public BooleanTopic getBooleanTopic(String name) {
    return m_inst.getBooleanTopic(m_pathWithSep + name);
  }

  /**
   * Get long topic.
   *
   * @param name topic name
   * @return IntegerTopic
   */
  public IntegerTopic getIntegerTopic(String name) {
    return m_inst.getIntegerTopic(m_pathWithSep + name);
  }

  /**
   * Get float topic.
   *
   * @param name topic name
   * @return FloatTopic
   */
  public FloatTopic getFloatTopic(String name) {
    return m_inst.getFloatTopic(m_pathWithSep + name);
  }

  /**
   * Get double topic.
   *
   * @param name topic name
   * @return DoubleTopic
   */
  public DoubleTopic getDoubleTopic(String name) {
    return m_inst.getDoubleTopic(m_pathWithSep + name);
  }

  /**
   * Get String topic.
   *
   * @param name topic name
   * @return StringTopic
   */
  public StringTopic getStringTopic(String name) {
    return m_inst.getStringTopic(m_pathWithSep + name);
  }

  /**
   * Get raw topic.
   *
   * @param name topic name
   * @return RawTopic
   */
  public RawTopic getRawTopic(String name) {
    return m_inst.getRawTopic(m_pathWithSep + name);
  }

  /**
   * Get boolean[] topic.
   *
   * @param name topic name
   * @return BooleanArrayTopic
   */
  public BooleanArrayTopic getBooleanArrayTopic(String name) {
    return m_inst.getBooleanArrayTopic(m_pathWithSep + name);
  }

  /**
   * Get long[] topic.
   *
   * @param name topic name
   * @return IntegerArrayTopic
   */
  public IntegerArrayTopic getIntegerArrayTopic(String name) {
    return m_inst.getIntegerArrayTopic(m_pathWithSep + name);
  }

  /**
   * Get float[] topic.
   *
   * @param name topic name
   * @return FloatArrayTopic
   */
  public FloatArrayTopic getFloatArrayTopic(String name) {
    return m_inst.getFloatArrayTopic(m_pathWithSep + name);
  }

  /**
   * Get double[] topic.
   *
   * @param name topic name
   * @return DoubleArrayTopic
   */
  public DoubleArrayTopic getDoubleArrayTopic(String name) {
    return m_inst.getDoubleArrayTopic(m_pathWithSep + name);
  }

  /**
   * Get String[] topic.
   *
   * @param name topic name
   * @return StringArrayTopic
   */
  public StringArrayTopic getStringArrayTopic(String name) {
    return m_inst.getStringArrayTopic(m_pathWithSep + name);
  }

  /**
   * Get protobuf-encoded value topic.
   *
   * @param <T> value class (inferred from proto)
   * @param <MessageType> protobuf message type (inferred from proto)
   * @param name topic name
   * @param proto protobuf serialization implementation
   * @return ProtobufTopic
   */
  public <T, MessageType extends ProtoMessage<?>> ProtobufTopic<T> getProtobufTopic(
      String name, Protobuf<T, MessageType> proto) {
    return m_inst.getProtobufTopic(m_pathWithSep + name, proto);
  }

  /**
   * Get struct-encoded value topic.
   *
   * @param <T> value class (inferred from struct)
   * @param name topic name
   * @param struct struct serialization implementation
   * @return StructTopic
   */
  public <T> StructTopic<T> getStructTopic(String name, Struct<T> struct) {
    return m_inst.getStructTopic(m_pathWithSep + name, struct);
  }

  /**
   * Get struct-encoded value array topic.
   *
   * @param <T> value class (inferred from struct)
   * @param name topic name
   * @param struct struct serialization implementation
   * @return StructTopic
   */
  public <T> StructArrayTopic<T> getStructArrayTopic(String name, Struct<T> struct) {
    return m_inst.getStructArrayTopic(m_pathWithSep + name, struct);
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
      NetworkTableEntry oldEntry = m_entries.putIfAbsent(key, entry);
      if (oldEntry != null) {
        entry = oldEntry;
      }
    }
    return entry;
  }

  /**
   * Returns the table at the specified key. If there is no table at the specified key, it will
   * create a new table
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
    return !"".equals(key) && getTopic(key).exists();
  }

  /**
   * Checks the table and tells if it contains the specified sub table.
   *
   * @param key the key to search for
   * @return true if there is a subtable with the key which contains at least one key/subtable of
   *     its own
   */
  public boolean containsSubTable(String key) {
    Topic[] topics = m_inst.getTopics(m_pathWithSep + key + PATH_SEPARATOR, 0);
    return topics.length != 0;
  }

  /**
   * Gets topic information for all keys in the table (not including sub-tables).
   *
   * @param types bitmask of types (NetworkTableType values); 0 is treated as a "don't care".
   * @return topic information for keys currently in the table
   */
  public List<TopicInfo> getTopicInfo(int types) {
    List<TopicInfo> infos = new ArrayList<>();
    int prefixLen = m_path.length() + 1;
    for (TopicInfo info : m_inst.getTopicInfo(m_pathWithSep, types)) {
      String relativeKey = info.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1) {
        continue;
      }
      infos.add(info);
    }
    return infos;
  }

  /**
   * Gets topic information for all keys in the table (not including sub-tables).
   *
   * @return topic information for keys currently in the table
   */
  public List<TopicInfo> getTopicInfo() {
    return getTopicInfo(0);
  }

  /**
   * Gets all topics in the table (not including sub-tables).
   *
   * @param types bitmask of types (NetworkTableType values); 0 is treated as a "don't care".
   * @return topic for keys currently in the table
   */
  public List<Topic> getTopics(int types) {
    List<Topic> topics = new ArrayList<>();
    int prefixLen = m_path.length() + 1;
    for (TopicInfo info : m_inst.getTopicInfo(m_pathWithSep, types)) {
      String relativeKey = info.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1) {
        continue;
      }
      topics.add(info.getTopic());
    }
    return topics;
  }

  /**
   * Gets all topics in the table (not including sub-tables).
   *
   * @return topic for keys currently in the table
   */
  public List<Topic> getTopics() {
    return getTopics(0);
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
    for (TopicInfo info : m_inst.getTopicInfo(m_pathWithSep, types)) {
      String relativeKey = info.name.substring(prefixLen);
      if (relativeKey.indexOf(PATH_SEPARATOR) != -1) {
        continue;
      }
      keys.add(relativeKey);
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
    for (TopicInfo info : m_inst.getTopicInfo(m_pathWithSep, 0)) {
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
   * Put a value in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  public boolean putValue(String key, NetworkTableValue value) {
    return getEntry(key).setValue(value);
  }

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return False if the table key exists with a different type
   */
  public boolean setDefaultValue(String key, NetworkTableValue defaultValue) {
    return getEntry(key).setDefaultValue(defaultValue);
  }

  /**
   * Gets the value associated with a key as an object.
   *
   * @param key the key of the value to look up
   * @return the value associated with the given key, or nullptr if the key does not exist
   */
  public NetworkTableValue getValue(String key) {
    return getEntry(key).getValue();
  }

  /**
   * Get the path of the NetworkTable.
   *
   * @return The path of the NetworkTable.
   */
  public String getPath() {
    return m_path;
  }

  /** A listener that listens to events on topics in a {@link NetworkTable}. */
  @FunctionalInterface
  public interface TableEventListener {
    /**
     * Called when an event occurs on a topic in a {@link NetworkTable}.
     *
     * @param table the table the topic exists in
     * @param key the key associated with the topic that changed
     * @param event the event
     */
    void accept(NetworkTable table, String key, NetworkTableEvent event);
  }

  /**
   * Listen to topics only within this table.
   *
   * @param eventKinds set of event kinds to listen to
   * @param listener listener to add
   * @return Listener handle
   */
  public int addListener(EnumSet<NetworkTableEvent.Kind> eventKinds, TableEventListener listener) {
    final int prefixLen = m_path.length() + 1;
    return m_inst.addListener(
        new String[] {m_pathWithSep},
        eventKinds,
        event -> {
          String topicName = null;
          if (event.topicInfo != null) {
            topicName = event.topicInfo.name;
          } else if (event.valueData != null) {
            topicName = event.valueData.getTopic().getName();
          }
          if (topicName == null) {
            return;
          }
          String relativeKey = topicName.substring(prefixLen);
          if (relativeKey.indexOf(PATH_SEPARATOR) != -1) {
            // part of a sub table
            return;
          }
          listener.accept(this, relativeKey, event);
        });
  }

  /**
   * Listen to a single key.
   *
   * @param key the key name
   * @param eventKinds set of event kinds to listen to
   * @param listener listener to add
   * @return Listener handle
   */
  public int addListener(
      String key, EnumSet<NetworkTableEvent.Kind> eventKinds, TableEventListener listener) {
    NetworkTableEntry entry = getEntry(key);
    return m_inst.addListener(entry, eventKinds, event -> listener.accept(this, key, event));
  }

  /** A listener that listens to new tables in a {@link NetworkTable}. */
  @FunctionalInterface
  public interface SubTableListener {
    /**
     * Called when a new table is created within a {@link NetworkTable}.
     *
     * @param parent the parent of the table
     * @param name the name of the new table
     * @param table the new table
     */
    void tableCreated(NetworkTable parent, String name, NetworkTable table);
  }

  /**
   * Listen for sub-table creation. This calls the listener once for each newly created sub-table.
   * It immediately calls the listener for any existing sub-tables.
   *
   * @param listener listener to add
   * @return Listener handle
   */
  public int addSubTableListener(SubTableListener listener) {
    final int prefixLen = m_path.length() + 1;
    final NetworkTable parent = this;

    return m_inst.addListener(
        new String[] {m_pathWithSep},
        EnumSet.of(NetworkTableEvent.Kind.kPublish, NetworkTableEvent.Kind.kImmediate),
        new Consumer<>() {
          final Set<String> m_notifiedTables = new HashSet<>();

          @Override
          public void accept(NetworkTableEvent event) {
            if (event.topicInfo == null) {
              return; // should not happen
            }
            String relativeKey = event.topicInfo.name.substring(prefixLen);
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
        });
  }

  /**
   * Remove a listener.
   *
   * @param listener listener handle
   */
  public void removeListener(int listener) {
    m_inst.removeListener(listener);
  }

  @Override
  public boolean equals(Object other) {
    return other instanceof NetworkTable ntOther
        && m_inst.equals(ntOther.m_inst)
        && m_path.equals(ntOther.m_path);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_inst, m_path);
  }
}
