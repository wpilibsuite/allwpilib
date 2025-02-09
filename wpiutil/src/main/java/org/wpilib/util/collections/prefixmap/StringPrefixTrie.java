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

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.PrimitiveIterator;
import java.util.TreeMap;

class StringPrefixTrie<V> implements PrefixTrie<V> {
  private final int m_charIndex;
  private TreeMap<Integer, StringPrefixTrie<V>> m_childNodes;
  private V m_theValue;

  StringPrefixTrie() {
    this(0);
  }

  StringPrefixTrie(int charIndex) {
    this.m_charIndex = charIndex;
  }

  @Override
  public V add(PrimitiveIterator.OfInt prefix, V value) {
    V previousValue = m_theValue;
    if (!prefix.hasNext()) {
      m_theValue = value;
      return previousValue;
    }

    int myChar = prefix.nextInt();

    if (m_childNodes == null) {
      m_childNodes = new TreeMap<>();
    }

    PrefixTrie<V> child =
        m_childNodes.computeIfAbsent(myChar, c -> new StringPrefixTrie<>(m_charIndex + 1));
    previousValue = child.add(prefix, value);
    return previousValue;
  }

  @Override
  public V remove(PrimitiveIterator.OfInt prefix) {
    if (!prefix.hasNext()) {
      V previousValue = m_theValue;
      m_theValue = null;
      return previousValue;
    }

    if (m_childNodes == null) {
      return null;
    }

    int myChar = prefix.nextInt(); // This will give us the ASCII value of the char

    PrefixTrie<V> child = m_childNodes.get(myChar);
    if (child == null) {
      return null;
    }
    return child.remove(prefix);
  }

  // ==============================================================
  // GET

  @Override
  public V get(PrimitiveIterator.OfInt prefix) {
    if (!prefix.hasNext()) {
      return m_theValue;
    }

    if (m_childNodes == null) {
      return null;
    }

    int myChar = prefix.nextInt();

    PrefixTrie<V> child = m_childNodes.get(myChar);
    if (child == null) {
      return null;
    }

    return child.get(prefix);
  }

  // ==============================================================
  // GET SHORTEST

  @Override
  public V getShortestMatch(PrimitiveIterator.OfInt input) {
    if (m_theValue != null || !input.hasNext() || m_childNodes == null) {
      return m_theValue;
    }

    int myChar = input.nextInt();

    PrefixTrie<V> child = m_childNodes.get(myChar);
    if (child == null) {
      return null;
    }

    return child.getShortestMatch(input);
  }

  // ==============================================================
  // GET LONGEST

  @Override
  public V getLongestMatch(PrimitiveIterator.OfInt input) {
    if (!input.hasNext() || m_childNodes == null) {
      return m_theValue;
    }

    int myChar = input.nextInt();

    PrefixTrie<V> child = m_childNodes.get(myChar);
    if (child == null) {
      return m_theValue;
    }

    V returnValue = child.getLongestMatch(input);
    return (returnValue == null) ? m_theValue : returnValue;
  }

  // ==============================================================
  // GET ALL VIA ITERATOR

  public static class StringTrieIterator<V> implements Iterator<V> {
    private V m_next;
    private final PrimitiveIterator.OfInt m_input;
    private StringPrefixTrie<V> m_node;

    StringTrieIterator(PrimitiveIterator.OfInt input, StringPrefixTrie<V> node) {
      this.m_input = input;
      this.m_node = node;
      this.m_next = getM_next();
    }

    @Override
    public boolean hasNext() {
      return m_next != null;
    }

    @Override
    public V next() {
      if (m_next == null) {
        throw new NoSuchElementException("Trying next() when hasNext() is false.");
      }
      V result = m_next;
      m_next = getM_next();
      return result;
    }

    private V getM_next() {
      if (m_node == null) {
        return null;
      }

      V theValue = m_node.m_theValue;

      // Are we at the last possible one for the given input?
      if (!m_input.hasNext() || m_node.m_childNodes == null) {
        m_node = null;
        return theValue;
      }

      // Find the next
      int myChar = m_input.nextInt(); // This will give us the ASCII value of the char

      StringPrefixTrie<V> child = m_node.m_childNodes.get(myChar);
      if (child == null) {
        m_node = null; // No more children, so this is where it ends.
        return theValue;
      }

      m_node = child;
      if (theValue == null) {
        return getM_next();
      }
      return theValue;
    }
  }

  @Override
  public Iterator<V> getAllMatches(PrimitiveIterator.OfInt input) {
    return new StringTrieIterator<>(input, this);
  }

  // ==============================================================

  @Override
  public void clear() {
    m_childNodes = null;
    m_theValue = null;
  }
}
