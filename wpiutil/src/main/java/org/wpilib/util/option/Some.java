// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.option;

import static java.util.Objects.requireNonNull;
import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.function.Function;
import java.util.function.Predicate;
import java.util.function.Supplier;
import java.util.stream.Stream;

/**
 * An option type that contains a value.
 *
 * @param value The value of the option. This is never {@code null}
 * @param <T> The type of the value
 */
public record Some<T>(T value) implements Option<T> {
  /**
   * Creates a new option wrapping a value. An exception is thrown if the value is null. If you want
   * to safely handle values that are possibly null, use {@link Option#ofNullable(Object)} instead.
   *
   * @param value The value to wrap. Cannot be null.
   * @param <T> The type of the values that can potentially be stored in the option
   * @return An option for the value.
   * @throws NullPointerException if {@code value} is {@code null}
   */
  public static <T> Some<T> of(T value) {
    requireNonNullParam(value, "value", "Some.of");

    return new Some<>(value);
  }

  /**
   * Canonical constructor. Prefer {@link Some#some(Object)} instead of constructing values
   * directly.
   *
   * @param value The value of the option. Cannot be {@code null}
   * @throws NullPointerException if {@code value} is {@code null}
   */
  public Some {
    requireNonNullParam(value, "value", "Some");
  }

  @Override
  public T orElse(T fallbackValue) {
    requireNonNullParam(fallbackValue, "fallbackValue", "Option.orElse");
    return value;
  }

  @Override
  public T orElseGet(Supplier<? extends T> fallbackSupplier) {
    requireNonNullParam(fallbackSupplier, "fallbackSupplier", "Option.orElseGet");
    return value;
  }

  @Override
  public T orElseThrow() {
    return value;
  }

  @Override
  public <X extends Throwable> T orElseThrow(Supplier<? extends X> exceptionSupplier) {
    requireNonNullParam(exceptionSupplier, "exceptionSupplier", "Option.orElseThrow");

    return value;
  }

  @Override
  public Option<T> or(Supplier<? extends Option<? extends T>> supplier) {
    requireNonNullParam(supplier, "supplier", "Option.or");

    return this;
  }

  @Override
  public Option<T> filter(Predicate<? super T> predicate) {
    requireNonNullParam(predicate, "predicate", "Option.filter");

    if (predicate.test(value)) {
      return this;
    } else {
      return Option.none();
    }
  }

  @Override
  public <R> Option<R> map(Function<? super T, ? extends R> mapper) {
    requireNonNullParam(mapper, "mapper", "Option.map");

    return Option.ofNullable(mapper.apply(value));
  }

  @Override
  @SuppressWarnings("unchecked")
  public <R> Option<R> flatMap(Function<? super T, ? extends Option<? extends R>> mapper) {
    requireNonNullParam(mapper, "mapper", "Option.flatMap");

    var result = mapper.apply(value);
    requireNonNull(result, "The mapping function passed to Option.flatMap returned null");
    return (Option<R>) result;
  }

  @Override
  public Stream<T> stream() {
    return Stream.of(value);
  }
}
