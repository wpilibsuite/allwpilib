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
    StringPrefixTrie<V> node = this;
    while (prefix.hasNext()) {
      int myChar = prefix.nextInt();

      if (node.m_childNodes == null) {
        node.m_childNodes = new TreeMap<>();
      }

      StringPrefixTrie<V> child = node.m_childNodes.get(myChar);
      if (child == null) {
        child = new StringPrefixTrie<>(node.m_charIndex + 1);
        node.m_childNodes.put(myChar, child);
      }
      node = child;
    }

    V previousValue = node.m_theValue;
    node.m_theValue = value;
    return previousValue;
  }

  @Override
  public V remove(PrimitiveIterator.OfInt prefix) {
    StringPrefixTrie<V> node = this;
    // Track the highest suffix that can be detached if the removed node becomes empty.
    StringPrefixTrie<V> pruneParent = null;
    int pruneChar = 0;

    while (prefix.hasNext()) {
      if (node.m_childNodes == null) {
        return null;
      }

      int myChar = prefix.nextInt();

      StringPrefixTrie<V> child = node.m_childNodes.get(myChar);
      if (child == null) {
        return null;
      }
      if (node.m_charIndex == 0 || node.m_theValue != null || node.m_childNodes.size() > 1) {
        pruneParent = node;
        pruneChar = myChar;
      }
      node = child;
    }

    V previousValue = node.m_theValue;
    node.m_theValue = null;
    if (node.m_childNodes == null && pruneParent != null) {
      pruneParent.m_childNodes.remove(pruneChar);
      if (pruneParent.m_childNodes.isEmpty()) {
        pruneParent.m_childNodes = null;
      }
    }
    return previousValue;
  }

  // ==============================================================
  // GET

  @Override
  public V get(PrimitiveIterator.OfInt prefix) {
    StringPrefixTrie<V> node = this;
    while (prefix.hasNext()) {
      if (node.m_childNodes == null) {
        return null;
      }

      int myChar = prefix.nextInt();

      StringPrefixTrie<V> child = node.m_childNodes.get(myChar);
      if (child == null) {
        return null;
      }
      node = child;
    }

    return node.m_theValue;
  }

  // ==============================================================
  // GET SHORTEST

  @Override
  public V getShortestMatch(PrimitiveIterator.OfInt input) {
    StringPrefixTrie<V> node = this;
    while (true) {
      if (node.m_theValue != null || !input.hasNext() || node.m_childNodes == null) {
        return node.m_theValue;
      }

      int myChar = input.nextInt();

      StringPrefixTrie<V> child = node.m_childNodes.get(myChar);
      if (child == null) {
        return null;
      }
      node = child;
    }
  }

  // ==============================================================
  // GET LONGEST

  @Override
  public V getLongestMatch(PrimitiveIterator.OfInt input) {
    StringPrefixTrie<V> node = this;
    V returnValue = node.m_theValue;
    while (input.hasNext() && node.m_childNodes != null) {
      int myChar = input.nextInt();

      StringPrefixTrie<V> child = node.m_childNodes.get(myChar);
      if (child == null) {
        return returnValue;
      }

      node = child;
      if (node.m_theValue != null) {
        returnValue = node.m_theValue;
      }
    }

    return returnValue;
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
      while (m_node != null) {
        V theValue = m_node.m_theValue;

        // Are we at the last possible one for the given input?
        if (!m_input.hasNext() || m_node.m_childNodes == null) {
          m_node = null;
          return theValue;
        }

        int myChar = m_input.nextInt();

        StringPrefixTrie<V> child = m_node.m_childNodes.get(myChar);
        if (child == null) {
          m_node = null; // No more children, so this is where it ends.
          return theValue;
        }

        m_node = child;
        if (theValue != null) {
          return theValue;
        }
      }
      return null;
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

  boolean hasChildNodes() {
    return m_childNodes != null;
  }
}
