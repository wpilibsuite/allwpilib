// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.collections;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

/**
 * A variant on {@code java.util.HashMap<K, V>} that uses primitive long ints for map keys instead
 * of autoboxed Long objects like would be used for a {@code Map<Long, V>}.
 *
 * @param <V> the type of the values stored in the map
 */
public class LongToObjectHashMap<V> {
  private static final int kInitialSize = 0;
  private static final int kInitialCapacity = 8; // NOTE: must be a power of two

  /**
   * The default load factor of the hashmap. If the ratio of the number of entries to the map's
   * capacity exceeds this value, the map will be resized (doubled capacity) in order for more
   * values to be easily inserted.
   */
  private static final double kLoadFactor = 75.00 / 100;

  /** The current number of key-value pairs in the map. */
  private int m_size = kInitialSize;

  /**
   * The current maximum capacity of the map. Note that it will be resized before m_size reaches
   * this value.
   */
  private int m_capacity = kInitialCapacity;

  /**
   * The keys in the map. This is a sparse array, and the location of a key may not be equal to the
   * result of calling {@link #bucket(long)} on that key. To handle hash collisions, if a bucket is
   * already in use when trying to insert a value, the bucket number is incremented (wrapping around
   * to 0 if it's equal to m_capacity) and <i>that</i> bucket is checked to see if it's available.
   * This process continues until an empty bucket is found (which is guaranteed because m_size is
   * always less than m_capacity).
   */
  private long[] m_keys = new long[m_capacity];

  /** Tracks which buckets are actually used (have a key-value mapping). */
  private boolean[] m_uses = new boolean[m_capacity];

  /**
   * The values in the map. See the documentation for m_keys for how indexing into this array works.
   */
  @SuppressWarnings("unchecked")
  private V[] m_values = (V[]) new Object[m_capacity];

  /** Default constructor. */
  public LongToObjectHashMap() {}

  /**
   * Puts a value {@code value} corresponding to key {@code key} in the map.
   *
   * @param key the associated key
   * @param value the value to insert
   * @return the previous value that was mapped to the key, or null if no such value existed
   */
  public V put(long key, V value) {
    int bucket = bucket(key);

    // Increment the bucket until we hit an open space (there's always going to be at least one)
    while (m_uses[bucket]) {
      if (m_keys[bucket] == key) {
        // replace the existing value
        var oldValue = m_values[bucket];
        m_values[bucket] = value;
        return oldValue;
      }
      bucket = safeIncrement(bucket);
    }

    m_uses[bucket] = true;
    m_keys[bucket] = key;
    m_values[bucket] = value;
    m_size++;

    if (m_size > maxSize()) {
      grow();
    }
    return null;
  }

  /**
   * Gets the value associated with the given key.
   *
   * @param key the key to retrieve the value for
   * @return the value mapped to the key, or null if the key is not in the map
   */
  public V get(long key) {
    int bucket = bucket(key);
    while (m_uses[bucket]) {
      if (m_keys[bucket] == key) {
        // found it
        return m_values[bucket];
      }
      bucket = safeIncrement(bucket);
    }
    return null;
  }

  /**
   * Removes the value associated with the given key and returns it.
   *
   * @param key the key to remove
   * @return the value corresponding to the key, or null if the key is not in the map
   */
  public V remove(long key) {
    int bucket = bucket(key);
    while (m_uses[bucket]) {
      if (m_keys[bucket] == key) {
        // found it
        // TODO: Shrink the map when below a certain load factor
        //       Current use cases don't remove elements from the map, so there's not much use
        //       for shrinking at the moment.
        m_size--;
        m_keys[bucket] = 0L;
        m_uses[bucket] = false;

        var oldValue = m_values[bucket];
        m_values[bucket] = null;
        return oldValue;
      }
      bucket = safeIncrement(bucket);
    }

    return null;
  }

  /**
   * Checks if a key is contained in the map.
   *
   * @param key the key to check
   * @return true if the key has an associated value, false if not
   */
  public boolean containsKey(long key) {
    int bucket = bucket(key);
    while (m_uses[bucket]) {
      if (m_keys[bucket] == key) {
        return true;
      }
      bucket = safeIncrement(bucket);
    }
    return false;
  }

  /** Clears and removes all entries from the map. */
  public void clear() {
    if (m_size == 0) {
      // Nothing to do
      return;
    }
    m_size = 0;

    Arrays.fill(m_uses, false);
    Arrays.fill(m_keys, 0L);
    Arrays.fill(m_values, null);
  }

  /**
   * Gets the number of key-value pairs currently contained in the map.
   *
   * @return the current size of the map
   */
  public int size() {
    return m_size;
  }

  // package-private for tests
  int capacity() {
    return m_capacity;
  }

  /**
   * Checks if the map contains any entries.
   *
   * @return true if at least one entry is present, false otherwise
   */
  public boolean isEmpty() {
    return m_size == 0;
  }

  /**
   * Gets the keys contained in the map. Ordering is not guaranteed. The returned set is read-only
   * and immutable. This uses a custom class for primitive long values to avoid unnecessary
   * autoboxing to {@code java.lang.Long}.
   *
   * @return a read-only set of keys
   */
  public ReadOnlyPrimitiveLongSet keySet() {
    // copy the sparse key array into a compact array
    final long[] keys = new long[m_size];
    int i = 0;
    for (int bucket = 0; bucket < m_capacity; bucket++) {
      if (m_uses[bucket]) {
        keys[i] = m_keys[bucket];
        i++;
      }
    }

    return new ReadOnlyPrimitiveLongSet(keys);
  }

  /**
   * Gets the values contained in the map. Ordering is not guaranteed. The returned collection is
   * read-only and immutable.
   *
   * @return a read-only collection of values
   */
  public Collection<V> values() {
    Collection<V> values = new ArrayList<>();
    for (int bucket = 0; bucket < m_capacity; bucket++) {
      if (m_uses[bucket]) {
        values.add(m_values[bucket]);
      }
    }
    return List.copyOf(values); // return a readonly copy
  }

  /**
   * Interface for map iterator function.
   *
   * @param <V> Value type.
   */
  @FunctionalInterface
  public interface IteratorFunction<V> {
    /**
     * Accepts a key-value pair from the map.
     *
     * @param key The key.
     * @param value The value.
     */
    void accept(long key, V value);
  }

  /**
   * Iterates over every key-value pair in the map and passes them to the given function.
   *
   * @param function the function to apply to every key-value pair.
   */
  public void forEach(IteratorFunction<? super V> function) {
    for (int bucket = 0; bucket < m_capacity; bucket++) {
      if (m_uses[bucket]) {
        function.accept(m_keys[bucket], m_values[bucket]);
      }
    }
  }

  private void grow() {
    final int currentSize = m_size;
    final int oldCapacity = m_capacity;
    if (oldCapacity * kLoadFactor >= currentSize) {
      // We're below the maximum allowed size for the current capacity
      // Nothing to do
      return;
    }

    final int newCapacity = oldCapacity * 2;
    final int newMask = newCapacity - 1;

    final boolean[] oldUses = m_uses;
    final long[] oldKeys = m_keys;
    final V[] oldValues = m_values;

    final boolean[] newUses = new boolean[newCapacity];
    final long[] newKeys = new long[newCapacity];
    @SuppressWarnings("unchecked")
    final V[] newValues = (V[]) new Object[newCapacity];

    for (int oldBucket = 0; oldBucket < oldCapacity; oldBucket++) {
      if (!oldUses[oldBucket]) {
        // Bucket is empty, skip
        continue;
      }
      final long key = oldKeys[oldBucket];
      final V value = oldValues[oldBucket];

      int newBucket = (int) (hash(key) & newMask);
      while (newUses[newBucket]) {
        newBucket = (newBucket + 1) & newMask;
      }

      newUses[newBucket] = true;
      newKeys[newBucket] = key;
      newValues[newBucket] = value;
    }

    m_capacity = newCapacity;
    m_uses = newUses;
    m_keys = newKeys;
    m_values = newValues;
  }

  private int maxSize() {
    return (int) (m_capacity * kLoadFactor);
  }

  /**
   * Calculates a hashcode for an input key. Does some bit shuffling to account for poor hash
   * functions.
   *
   * @param key the key to hash
   * @return a hashcode for the input key
   */
  private long hash(long key) {
    return 31 + (key ^ (key >>> 15) ^ (key >>> 31) ^ (key << 31));
  }

  /**
   * The mask to use when translating a hashcode to a bucket index. Relies on m_capacity being a
   * power of two.
   */
  private int mask() {
    return m_capacity - 1;
  }

  /**
   * Calculates the desired bucket index for a particular key. Does nothing to handle the case where
   * the calculated index is already in use by another key.
   *
   * @param key the key to get the bucket for
   * @return the desired bucket index
   */
  private int bucket(long key) {
    var hash = hash(key);
    return (int) (hash & mask());
  }

  /**
   * Increments a bucket index by 1, wrapping around to 0 if the index is already at the maximum.
   *
   * @param bucket the index to increment
   * @return the incremented bucket index
   */
  private int safeIncrement(int bucket) {
    return (bucket + 1) & mask();
  }
}
