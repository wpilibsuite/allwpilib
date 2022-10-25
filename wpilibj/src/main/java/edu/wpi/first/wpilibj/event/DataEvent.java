package edu.wpi.first.wpilibj.event;

import java.util.Objects;
import java.util.function.BiConsumer;
import java.util.function.BiFunction;
import java.util.function.Consumer;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.function.Supplier;

public class DataEvent<T> {
  private final EventLoop m_loop;
  private final Supplier<T> m_emitter;

  public DataEvent(EventLoop loop, Supplier<T> emitter) {
    m_loop = loop;
    m_emitter = emitter;
  }

  public <R> DataEvent<R> map(Function<T, R> mapper) {
    return new DataEvent<>(m_loop, () -> mapper.apply(m_emitter.get()));
  }

  public <E, R> DataEvent<R> fold(Supplier<E> rhs, BiFunction<T, E, R> mapper) {
    return map(t -> mapper.apply(t, rhs.get()));
  }

  public DataEvent<T> copy() {
    return map(Function.identity());
  }

  public DataEvent<T> observe(Consumer<T> observer) {
    m_loop.bind(() -> true, () -> observer.accept(m_emitter.get()));
    return this;
  }

  public DataEvent<T> observeChange(BiConsumer<T, T> listener) {
    var tracker = new ChangeTracker<>(m_emitter);
    m_loop.bind(tracker::hasChanged, () -> tracker.observe(listener));
    return this;
  }

  public BooleanEvent threshold(Predicate<T> filter) {
    return new BooleanEvent(m_loop, () -> filter.test(m_emitter.get()));
  }

  private static class ChangeTracker<T> {
    private final Supplier<T> m_source;
    private T m_value;
    private T m_prevValue;

    private ChangeTracker(Supplier<T> source) {
      m_source = source;
    }

    boolean hasChanged() {
      m_prevValue = m_value;
      m_value = m_source.get();
      return Objects.equals(m_value, m_prevValue);
    }

    void observe(BiConsumer<T, T> listener) {
      listener.accept(m_prevValue, m_value);
    }
  }
}
