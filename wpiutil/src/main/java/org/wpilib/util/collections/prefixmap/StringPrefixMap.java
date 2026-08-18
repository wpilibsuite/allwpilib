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

import java.util.AbstractCollection;
import java.util.AbstractSet;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;
import java.util.Objects;
import java.util.PrimitiveIterator;
import java.util.Set;
import java.util.TreeMap;
import org.wpilib.util.collections.PrefixMap;

/**
 * The StringPrefixMap is an implementation of PrefixMap where the assumption is that the stored
 * prefixes can be any character in a String.
 *
 * @param <V> The type of the value that is to be stored.
 */
public class StringPrefixMap<V> implements PrefixMap<V> {
  private final PrefixTrie<V> m_prefixTrie;
  private final TreeMap<String, V> m_allPrefixes;
  private final Set<Map.Entry<String, V>> m_entrySet;
  private final Set<String> m_keySet;
  private final Collection<V> m_values;

  /** Constructor. */
  @SuppressWarnings("this-escape")
  public StringPrefixMap() {
    m_prefixTrie = new StringPrefixTrie<>();
    m_allPrefixes = new TreeMap<>();
    m_entrySet = new EntrySet();
    m_keySet = new KeySet();
    m_values = new Values();
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
    m_allPrefixes.put(storedKey(prefix), value);
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
    return m_entrySet;
  }

  @Override
  public boolean containsKey(Object key) {
    if (key instanceof String) {
      return m_allPrefixes.containsKey(storedKey((String) key));
    }
    return false;
  }

  @Override
  public boolean containsValue(Object value) {
    return m_allPrefixes.containsValue(value);
  }

  @Override
  public Set<String> keySet() {
    return m_keySet;
  }

  @Override
  public Collection<V> values() {
    return m_values;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == this) {
      return true;
    }
    return m_allPrefixes.equals(obj);
  }

  @Override
  public int hashCode() {
    return m_allPrefixes.hashCode();
  }

  private abstract class BackingIterator<T> implements Iterator<T> {
    private final Iterator<Map.Entry<String, V>> m_iterator = m_allPrefixes.entrySet().iterator();
    private Map.Entry<String, V> m_current;

    @Override
    public boolean hasNext() {
      return m_iterator.hasNext();
    }

    protected Map.Entry<String, V> nextEntry() {
      m_current = m_iterator.next();
      return m_current;
    }

    @Override
    public void remove() {
      if (m_current == null) {
        throw new IllegalStateException();
      }
      String key = m_current.getKey();
      m_iterator.remove();
      m_prefixTrie.remove(key);
      m_current = null;
    }
  }

  private final class EntryIterator extends BackingIterator<Map.Entry<String, V>> {
    @Override
    public Map.Entry<String, V> next() {
      return new PrefixEntry(nextEntry());
    }
  }

  private final class KeyIterator extends BackingIterator<String> {
    @Override
    public String next() {
      return nextEntry().getKey();
    }
  }

  private final class ValueIterator extends BackingIterator<V> {
    @Override
    public V next() {
      return nextEntry().getValue();
    }
  }

  private final class PrefixEntry implements Map.Entry<String, V> {
    private final Map.Entry<String, V> m_entry;

    PrefixEntry(Map.Entry<String, V> entry) {
      m_entry = entry;
    }

    @Override
    public String getKey() {
      return m_entry.getKey();
    }

    @Override
    public V getValue() {
      return m_entry.getValue();
    }

    @Override
    public V setValue(V value) {
      Objects.requireNonNull(value, "The value may not be null");
      m_prefixTrie.add(getKey(), value);
      return m_entry.setValue(value);
    }

    @Override
    public boolean equals(Object obj) {
      if (!(obj instanceof Map.Entry<?, ?>)) {
        return false;
      }
      Map.Entry<?, ?> other = (Map.Entry<?, ?>) obj;
      return Objects.equals(getKey(), other.getKey())
          && Objects.equals(getValue(), other.getValue());
    }

    @Override
    public int hashCode() {
      return Objects.hashCode(getKey()) ^ Objects.hashCode(getValue());
    }
  }

  private final class EntrySet extends AbstractSet<Map.Entry<String, V>> {
    @Override
    public Iterator<Map.Entry<String, V>> iterator() {
      return new EntryIterator();
    }

    @Override
    public int size() {
      return StringPrefixMap.this.size();
    }

    @Override
    public boolean contains(Object obj) {
      if (!(obj instanceof Map.Entry<?, ?>)) {
        return false;
      }
      Map.Entry<?, ?> entry = (Map.Entry<?, ?>) obj;
      Object key = entry.getKey();
      if (!(key instanceof String)) {
        return false;
      }
      String prefix = storedKey((String) key);
      return m_allPrefixes.containsKey(prefix)
          && Objects.equals(m_allPrefixes.get(prefix), entry.getValue());
    }

    @Override
    public boolean remove(Object obj) {
      if (!(obj instanceof Map.Entry<?, ?>)) {
        return false;
      }
      Map.Entry<?, ?> entry = (Map.Entry<?, ?>) obj;
      Object key = entry.getKey();
      if (!(key instanceof String)) {
        return false;
      }
      String prefix = storedKey((String) key);
      if (!m_allPrefixes.containsKey(prefix)
          || !Objects.equals(m_allPrefixes.get(prefix), entry.getValue())) {
        return false;
      }
      StringPrefixMap.this.remove(prefix);
      return true;
    }

    @Override
    public void clear() {
      StringPrefixMap.this.clear();
    }
  }

  private final class KeySet extends AbstractSet<String> {
    @Override
    public Iterator<String> iterator() {
      return new KeyIterator();
    }

    @Override
    public int size() {
      return StringPrefixMap.this.size();
    }

    @Override
    public boolean contains(Object key) {
      return StringPrefixMap.this.containsKey(key);
    }

    @Override
    public boolean remove(Object key) {
      if (key instanceof String) {
        return StringPrefixMap.this.remove((String) key) != null;
      }
      return false;
    }

    @Override
    public void clear() {
      StringPrefixMap.this.clear();
    }
  }

  private final class Values extends AbstractCollection<V> {
    @Override
    public Iterator<V> iterator() {
      return new ValueIterator();
    }

    @Override
    public int size() {
      return StringPrefixMap.this.size();
    }

    @Override
    public boolean contains(Object value) {
      return StringPrefixMap.this.containsValue(value);
    }

    @Override
    public void clear() {
      StringPrefixMap.this.clear();
    }
  }
}
