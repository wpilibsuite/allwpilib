/*
 * Copyright (C) 2018-2021 Niels Basjes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.wpilib.util.collections.prefixmap;

import org.wpilib.util.collections.PrefixMap;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;
import java.util.Objects;
import java.util.PrimitiveIterator;
import java.util.Set;
import java.util.TreeMap;

/**
 * The StringPrefixMap is an implementation of PrefixMap where the assumption is that the stored
 * prefixes can be any character in a String.
 *
 * @param <V> The type of the value that is to be stored.
 */
public class StringPrefixMap<V> implements PrefixMap<V> {
  private final PrefixTrie<V> m_prefixTrie;
  private final TreeMap<String, V> m_allPrefixes;

  /** Constructor. */
  @SuppressWarnings("this-escape")
  public StringPrefixMap() {
    m_prefixTrie = new StringPrefixTrie<>();
    m_allPrefixes = new TreeMap<>();
  }

  @Override
  public boolean containsPrefix(PrimitiveIterator.OfInt prefix) {
    return m_prefixTrie.containsPrefix(prefix);
  }

  private String storedKey(String key) {
    return key;
  }

  @Override
  public V put(String prefix, V value) {
    Objects.requireNonNull(prefix, "The prefix may not be null");
    Objects.requireNonNull(value, "The value may not be null");

    V previousValue = m_prefixTrie.add(prefix, value);
    if (previousValue == null) {
      m_allPrefixes.put(storedKey(prefix), value);
    }
    return previousValue;
  }

  @Override
  public int size() {
    return m_allPrefixes.size();
  }

  @Override
  public void clear() {
    m_prefixTrie.clear();
    m_allPrefixes.clear();
  }

  @Override
  public V remove(String prefix) {
    Objects.requireNonNull(prefix, "The prefix may not be null");
    V oldValue = m_prefixTrie.remove(prefix);
    if (oldValue != null) {
      m_allPrefixes.remove(storedKey(prefix));
    }
    return oldValue;
  }

  @Override
  public V get(String prefix) {
    return m_prefixTrie.get(prefix);
  }

  @Override
  public V getShortestMatch(PrimitiveIterator.OfInt input) {
    return m_prefixTrie.getShortestMatch(input);
  }

  @Override
  public V getLongestMatch(PrimitiveIterator.OfInt input) {
    return m_prefixTrie.getLongestMatch(input);
  }

  @Override
  public Iterator<V> getAllMatches(PrimitiveIterator.OfInt input) {
    return m_prefixTrie.getAllMatches(input);
  }

  @Override
  public Set<Map.Entry<String, V>> entrySet() {
    return m_allPrefixes.entrySet();
  }

  @Override
  public boolean containsKey(Object key) {
    return m_allPrefixes.containsKey(storedKey((String) key));
  }

  @Override
  public boolean containsValue(Object value) {
    return m_allPrefixes.containsValue(value);
  }

  @Override
  public Set<String> keySet() {
    return m_allPrefixes.keySet();
  }

  @Override
  public Collection<V> values() {
    return m_allPrefixes.values();
  }
}
