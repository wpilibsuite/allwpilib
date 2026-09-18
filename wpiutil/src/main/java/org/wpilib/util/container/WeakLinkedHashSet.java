// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.container;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.lang.ref.WeakReference;
import java.util.AbstractSet;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.NoSuchElementException;
import java.util.SequencedSet;
import java.util.Spliterator;
import java.util.Spliterators;

/**
 * A variant of {@link java.util.LinkedHashSet} with weak references to entries. This container will
 * not prevent its entries from being garbage collected, unlike a typical {@code Set}. Null elements
 * are not permitted; calling {@link #add(Object) add(null)} will always throw a {@code
 * NullPointerException}.
 *
 * @param <E> The type of elements in the set.
 * @see java.util.LinkedHashSet
 * @see WeakReference
 */
public final class WeakLinkedHashSet<E> extends AbstractSet<E> implements SequencedSet<E> {
  private final LinkedHashMap<WeakKey<E>, Object> map = new LinkedHashMap<>();
  private static final Object PRESENT = new Object();

  /**
   * Constructs a new, empty linked hash set with the default initial capacity (16) and load factor
   * (0.75).
   */
  public WeakLinkedHashSet() {
    super();
  }

  /**
   * Constructs a new linked hash set with the same elements as the specified collection. The linked
   * hash set is created with an initial capacity sufficient to hold the elements in the specified
   * collection and the default load factor (0.75).
   *
   * @param collection the collection whose elements are to be placed into this set
   * @throws NullPointerException if the specified collection is null
   */
  public WeakLinkedHashSet(Collection<? extends E> collection) {
    requireNonNullParam(collection, "collection", "WeakLinkedHashSet");
    addAll(collection);
  }

  @Override
  public boolean add(E e) {
    requireNonNullParam(e, "e", "WeakLinkedHashSet.add");

    purgeCollected();
    return map.putLast(new WeakKey<>(e), PRESENT) == null;
  }

  @Override
  public void addFirst(E e) {
    requireNonNullParam(e, "e", "WeakLinkedHashSet.addFirst");

    purgeCollected();
    map.putFirst(new WeakKey<>(e), PRESENT);
  }

  @Override
  public void addLast(E e) {
    requireNonNullParam(e, "e", "WeakLinkedHashSet.addLast");

    purgeCollected();
    map.putLast(new WeakKey<>(e), PRESENT);
  }

  @Override
  public boolean remove(Object o) {
    requireNonNullParam(o, "o", "WeakLinkedHashSet.remove");

    purgeCollected();
    return map.remove(new WeakKey<>(o)) != null;
  }

  @Override
  public boolean contains(Object o) {
    requireNonNullParam(o, "o", "WeakLinkedHashSet.contains");

    purgeCollected();
    return map.containsKey(new WeakKey<>(o));
  }

  @Override
  public int size() {
    purgeCollected();
    return map.size();
  }

  // Removes entries where the element has been garbage collected
  private void purgeCollected() {
    map.keySet().removeIf(key -> key.get() == null);
  }

  @Override
  public Iterator<E> iterator() {
    purgeCollected();
    return new WeakKeyIterator(map.keySet().iterator());
  }

  @Override
  public SequencedSet<E> reversed() {
    purgeCollected();

    return new ReversedView<>(this, map.sequencedKeySet().reversed());
  }

  @Override
  public Spliterator<E> spliterator() {
    return Spliterators.spliteratorUnknownSize(
        iterator(), Spliterator.DISTINCT | Spliterator.ORDERED | Spliterator.NONNULL);
  }

  private static final class WeakKey<T> extends WeakReference<T> {
    private final int m_hashCode;

    WeakKey(T referent) {
      super(referent);
      m_hashCode = referent.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
      if (!(obj instanceof WeakKey<?> other)) {
        return false;
      }

      T ref = get();
      return ref != null && ref.equals(other.get());
    }

    @Override
    public int hashCode() {
      return m_hashCode;
    }
  }

  private class WeakKeyIterator implements Iterator<E> {
    private final ArrayList<WeakKey<E>> keys = new ArrayList<>();
    private int nextIndex;
    private E nextElement = null;
    private E lastReturnedElement = null;

    WeakKeyIterator(Iterator<WeakKey<E>> mapIterator) {
      mapIterator.forEachRemaining(keys::add);
    }

    @Override
    public boolean hasNext() {
      while (nextElement == null && nextIndex < keys.size()) {
        WeakKey<E> key = keys.get(nextIndex++);
        nextElement = key.get();
        if (nextElement == null) {
          map.remove(key);
        }
      }
      return nextElement != null;
    }

    @Override
    public E next() {
      if (!hasNext()) {
        throw new NoSuchElementException();
      }
      E element = nextElement;
      lastReturnedElement = element;
      nextElement = null;
      return element;
    }

    @Override
    public void remove() {
      if (lastReturnedElement == null) {
        throw new IllegalStateException();
      }

      map.remove(new WeakKey<>(lastReturnedElement));
      lastReturnedElement = null;
    }
  }

  private static class ReversedView<E> extends AbstractSet<E> implements SequencedSet<E> {
    private final WeakLinkedHashSet<E> originalSet;
    private final SequencedSet<WeakKey<E>> reversedKeys;

    ReversedView(WeakLinkedHashSet<E> originalSet, SequencedSet<WeakKey<E>> reversedKeys) {
      this.originalSet = originalSet;
      this.reversedKeys = reversedKeys;
    }

    @Override
    public int size() {
      return originalSet.size();
    }

    @Override
    public Iterator<E> iterator() {
      return originalSet.new WeakKeyIterator(reversedKeys.iterator());
    }

    @Override
    public SequencedSet<E> reversed() {
      return originalSet; // Reversing a reversed view returns the original set
    }

    @Override
    public boolean add(E e) {
      return originalSet.add(e);
    }

    @Override
    public boolean remove(Object o) {
      return originalSet.remove(o);
    }

    @Override
    public boolean contains(Object o) {
      return originalSet.contains(o);
    }

    @Override
    public void addFirst(E e) {
      originalSet.addLast(e);
    }

    @Override
    public void addLast(E e) {
      originalSet.addFirst(e);
    }

    @Override
    public E getFirst() {
      return originalSet.getLast();
    }

    @Override
    public E getLast() {
      return originalSet.getFirst();
    }

    @Override
    public E removeFirst() {
      return originalSet.removeLast();
    }

    @Override
    public E removeLast() {
      return originalSet.removeFirst();
    }

    @Override
    public Spliterator<E> spliterator() {
      return Spliterators.spliteratorUnknownSize(
          iterator(), Spliterator.DISTINCT | Spliterator.ORDERED | Spliterator.NONNULL);
    }
  }
}
