package edu.wpi.first.wpilibj3.command.async;

import java.util.Collection;
import java.util.Iterator;
import java.util.Set;

public final class NopSet<T> implements Set<T> {
  @SuppressWarnings("rawtypes")
  public static final NopSet NOP = new NopSet();

  @SuppressWarnings("unchecked")
  public static <T> Set<T> nop() {
    return NOP;
  }

  private static final Object[] EMPTY_ARRAY = new Object[0];

  @SuppressWarnings("rawtypes")
  private static final Iterator ITERATOR =
      new Iterator() {
        @Override
        public boolean hasNext() {
          return false;
        }

        @Override
        public Object next() {
          return null;
        }
      };

  @Override
  public int size() {
    return 0;
  }

  @Override
  public boolean isEmpty() {
    return true;
  }

  @Override
  public boolean contains(Object o) {
    return false;
  }

  @SuppressWarnings("unchecked")
  @Override
  public Iterator<T> iterator() {
    return ITERATOR;
  }

  @Override
  public Object[] toArray() {
    return EMPTY_ARRAY;
  }

  @Override
  public <T1> T1[] toArray(T1[] a) {
    return a;
  }

  @Override
  public boolean add(T t) {
    return false;
  }

  @Override
  public boolean remove(Object o) {
    return false;
  }

  @Override
  public boolean containsAll(Collection<?> c) {
    return false;
  }

  @Override
  public boolean addAll(Collection<? extends T> c) {
    return false;
  }

  @Override
  public boolean retainAll(Collection<?> c) {
    return false;
  }

  @Override
  public boolean removeAll(Collection<?> c) {
    return false;
  }

  @Override
  public void clear() {}
}
