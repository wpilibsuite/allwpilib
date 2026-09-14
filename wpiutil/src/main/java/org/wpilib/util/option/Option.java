// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.option;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.function.Supplier;
import java.util.stream.Stream;

/**
 * Represents a value that may or may not be present. Options are useful for handling data that may
 * or may not be present without risking potential NullPointerExceptions or NoSuchElementExceptions.
 *
 * <p>This interface is intended to be used with pattern-matching {@code if} and {@code switch}
 * statements, a Java language feature introduced in Java 21. See the <a
 * href="https://docs.oracle.com/en/java/javase/21/language/record-patterns.html">official Java
 * documentation</a> for more information on this language feature.
 *
 * <pre>{@code
 * // Usage in if statements:
 * Option<String> potentialText = fetchData(); // Pretend this function exists for sake of example
 * if (potentialText instanceof Some(String text)) {
 *   System.out.println("Got text: " + text);
 * } else {
 *   System.err.println("Didn't get any text");
 * }
 *
 * // Usage in switch statements:
 * switch (potentialText) {
 *   case Some(String text) -> System.out.println("Got text: " + text);
 *   case None() -> System.err.println("Didn't get any text");
 * }
 * }</pre>
 *
 * @param <T> The type of the values that can potentially be stored in the option
 * @see <a
 *     href="https://docs.oracle.com/en/java/javase/21/language/record-patterns.html">https://docs.oracle.com/en/java/javase/21/language/record-patterns.html</a>
 */
public sealed interface Option<T> permits Some, None {
  /**
   * Creates a new option wrapping a value. An exception is thrown if the value is null. If you want
   * to safely handle values that are possibly null, use {@link Option#ofNullable(Object)} instead.
   *
   * @param value The value to wrap. Cannot be null.
   * @param <T> The type of the values that can potentially be stored in the option
   * @return An option for the value.
   * @throws NullPointerException if {@code value} is {@code null}
   */
  static <T> Option<T> some(T value) {
    return Some.of(value);
  }

  /**
   * Creates an empty option containing no data.
   *
   * @param <T> The type of values that could have potentially been stored in the option
   * @return A no-value option
   */
  static <T> None<T> none() {
    return None.none();
  }

  /**
   * Creates an option wrapping a potentially null value. If the value is null, an empty {@link
   * None} value is returned, otherwise a {@link Some} wrapping the non-null value is returned.
   *
   * @param value The value to wrap. May be null.
   * @param <T> The type of values that can be stored in the option
   * @return An option corresponding to the given nullable value
   */
  static <T> Option<T> ofNullable(T value) {
    if (value != null) {
      return new Some<>(value);
    } else {
      return none();
    }
  }

  /**
   * Creates a new option corresponding to a {@code java.util.Optional} object. If the optional is
   * {@link Optional#isPresent() present}, a {@link Some} is option is returned; otherwise, a {@link
   * None} option is returned.
   *
   * @param javaOptional The Java {@code Optional} object to wrap. Cannot be null.
   * @param <T> The type of the values that can potentially be stored in the option.
   * @return An option for the optional value.
   */
  static <T> Option<T> ofOptional(Optional<? extends T> javaOptional) {
    requireNonNullParam(javaOptional, "javaOptional", "Option.ofOptional");

    if (javaOptional.isPresent()) {
      return new Some<>(javaOptional.get());
    } else {
      return none();
    }
  }

  /**
   * Gets the value of this option, falling back to {@code fallbackValue} if the option is empty.
   *
   * @param fallbackValue The default fallback value. Cannot be null.
   * @return The value of this option, if present, or the fallback value.
   * @throws NullPointerException if {@code fallbackValue} is {@code null}
   */
  T orElse(T fallbackValue);

  /**
   * Gets the value of this option, falling back to a value provided by {@code supplier} if the
   * option is empty.
   *
   * @param fallbackSupplier A supplier for fallback values. Cannot be null, and cannot return
   *     nulls.
   * @return The value of this option, if present, or a fallback value provided by {@code
   *     fallbackSupplier}.
   * @throws NullPointerException if {@code supplier} is {@code null}
   * @throws NullPointerException if {@code supplier} returns {@code null}
   */
  T orElseGet(Supplier<? extends T> fallbackSupplier);

  /**
   * Gets the value of this option, throwing an exception if the option is empty.
   *
   * @return The value of this option, if present
   * @throws NoSuchElementException if the option is empty
   */
  T orElseThrow();

  /**
   * Gets the value of this option, throwing an exception if the option is empty.
   *
   * @param exceptionSupplier A provider for an exception to throw if the option is empty. Cannot be
   *     {@code null} and cannot return a {@code null} exception.
   * @param <X> The type of the exception to throw if the option is empty.
   * @return The value of this option, if present.
   * @throws X if the option is empty
   */
  <X extends Throwable> T orElseThrow(Supplier<? extends X> exceptionSupplier) throws X;

  /**
   * If a value is present, returns an option holding the value, otherwise returns an option
   * provided by the supplying function.
   *
   * @param supplier the supplying function that produces an option to return if this option is
   *     empty.
   * @return this option, if a value is present, or an option provided by the supplying function if
   *     no value is present
   * @throws NullPointerException if {@code supplier} is {@code null} or it returns a {@code null}
   *     value
   */
  Option<T> or(Supplier<? extends Option<? extends T>> supplier);

  /**
   * Returns an option describing the value of this option if it matches the given predicate.
   * Otherwise, returns an empty option.
   *
   * @param predicate The predicate to test the value against. Must not be null.
   * @return An option containing the original value if the predicate is satisfied, or an empty
   *     option if the predicate is not satisfied or if this option is empty.
   * @throws NullPointerException if {@code predicate} is {@code null}
   */
  Option<T> filter(Predicate<? super T> predicate);

  /**
   * Applies a mapping function to the value in this option, returning a new option containing the
   * result of the mapping function. The function may return {@code null}; if so, an empty option is
   * returned. Applying a mapping function to an empty option always returns another empty option.
   * This function is equivalent to the following switch statement:
   *
   * <pre>{@code
   * Option<?> option = ...;
   * Option<?> mapped = switch (option) {
   *   case Some(var value) -> Option.of(mappingFunction.apply(value));
   *   case None() -> Option.none();
   * };
   * }</pre>
   *
   * @param mapper The mapping function to apply.
   * @param <R> The result type of the mapping function
   * @return An option containing the result of the mapping operation, or an empty option if the
   *     mapping function returned null
   * @throws NullPointerException if {@code mapper} is {@code null}
   */
  <R> Option<R> map(Function<? super T, ? extends R> mapper);

  /**
   * Like {@link #map(Function)}, but where the function may return another {@code Option} object
   * instead of a raw value. Useful to avoid stackups of nested {@code Option<Option<Option<...>>>}
   * types.
   *
   * @param mapper The mapping function to apply
   * @param <R> The result type of the mapping function
   * @return An option containing the result of the mapping operation
   */
  <R> Option<R> flatMap(Function<? super T, ? extends Option<? extends R>> mapper);

  /**
   * If a value is present, returns a sequential {@link Stream} containing only that value,
   * otherwise returns an empty {@code Stream}.
   *
   * <p>This method can be used to transform a {@code Stream} of option elements to a {@code Stream}
   * of present value elements:
   *
   * <pre>{@code
   * Stream<Option<T>> os = ..
   * Stream<T> s = os.flatMap(Option::stream)
   * }</pre>
   *
   * @return the optional value as a {@code Stream}
   */
  Stream<T> stream();
}
