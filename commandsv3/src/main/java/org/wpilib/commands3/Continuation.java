// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.lang.invoke.WrongMethodTypeException;

/** A wrapper around the JDK internal Continuation class. */
public final class Continuation {
  // The underlying jdk.internal.vm.Continuation object
  final Object m_continuation;

  static final Class<?> jdk_internal_vm_Continuation;
  private static final MethodHandle CONSTRUCTOR;

  @SuppressWarnings("PMD.AvoidFieldNameMatchingMethodName")
  private static final MethodHandle YIELD;

  @SuppressWarnings("PMD.AvoidFieldNameMatchingMethodName")
  private static final MethodHandle RUN;

  private static final MethodHandle IS_DONE;

  private static final MethodHandle java_lang_thread_setContinuation;

  private static final ThreadLocal<Continuation> mountedContinuation = new ThreadLocal<>();

  static {
    try {
      jdk_internal_vm_Continuation = Class.forName("jdk.internal.vm.Continuation");

      var lookup =
          MethodHandles.privateLookupIn(jdk_internal_vm_Continuation, MethodHandles.lookup());

      CONSTRUCTOR =
          lookup.findConstructor(
              jdk_internal_vm_Continuation,
              MethodType.methodType(
                  void.class, ContinuationScope.jdk_internal_vm_ContinuationScope, Runnable.class));

      YIELD =
          lookup.findStatic(
              jdk_internal_vm_Continuation,
              "yield",
              MethodType.methodType(
                  boolean.class, ContinuationScope.jdk_internal_vm_ContinuationScope));

      RUN =
          lookup.findVirtual(
              jdk_internal_vm_Continuation, "run", MethodType.methodType(void.class));

      IS_DONE =
          lookup.findVirtual(
              jdk_internal_vm_Continuation, "isDone", MethodType.methodType(boolean.class));
    } catch (Throwable t) {
      throw new ExceptionInInitializerError(t);
    }
  }

  static {
    try {
      var lookup = MethodHandles.privateLookupIn(Thread.class, MethodHandles.lookup());

      java_lang_thread_setContinuation =
          lookup.findVirtual(
              Thread.class,
              "setContinuation",
              MethodType.methodType(void.class, Continuation.jdk_internal_vm_Continuation));
    } catch (Throwable t) {
      throw new ExceptionInInitializerError(t);
    }
  }

  private final ContinuationScope m_scope;

  /**
   * Constructs a continuation.
   *
   * @param scope the continuation's scope, used in yield
   * @param target the continuation's body
   */
  @SuppressWarnings({"PMD.AvoidRethrowingException", "PMD.AvoidCatchingGenericException"})
  public Continuation(ContinuationScope scope, Runnable target) {
    try {
      m_continuation = CONSTRUCTOR.invoke(scope.m_continuationScope, target);
      m_scope = scope;
    } catch (RuntimeException | Error e) {
      throw e;
    } catch (Throwable t) {
      throw new RuntimeException(t);
    }
  }

  /**
   * Suspends the current continuations up to this continuation's scope.
   *
   * @return {@code true} for success; {@code false} for failure
   * @throws IllegalStateException if not currently in this continuation's scope
   */
  @SuppressWarnings({"PMD.AvoidRethrowingException", "PMD.AvoidCatchingGenericException"})
  public boolean yield() {
    try {
      return (boolean) YIELD.invoke(m_scope.m_continuationScope);
    } catch (RuntimeException e) {
      throw e;
    } catch (Throwable t) {
      throw new Error(t);
    }
  }

  /**
   * Mounts and runs the continuation body. If suspended, continues it from the last suspend point.
   */
  @SuppressWarnings({"PMD.AvoidRethrowingException", "PMD.AvoidCatchingGenericException"})
  public void run() {
    try {
      RUN.invoke(m_continuation);
    } catch (WrongMethodTypeException | ClassCastException e) {
      throw new IllegalStateException("Unable to run the underlying continuation!", e);
    } catch (RuntimeException | Error e) {
      // The bound task threw an exception; re-throw it
      throw e;
    } catch (Throwable t) {
      throw new RuntimeException(t);
    }
  }

  /**
   * Tests whether this continuation is completed.
   *
   * @return whether this continuation is completed
   */
  @SuppressWarnings({"PMD.AvoidRethrowingException", "PMD.AvoidCatchingGenericException"})
  public boolean isDone() {
    try {
      return (boolean) IS_DONE.invoke(m_continuation);
    } catch (RuntimeException | Error e) {
      throw e;
    } catch (Throwable t) {
      throw new RuntimeException(t);
    }
  }

  /**
   * Mounds a continuation to the current thread. Accepts null for clearing the currently mounted
   * continuation.
   *
   * @param continuation the continuation to mount
   */
  @SuppressWarnings({"PMD.AvoidRethrowingException", "PMD.AvoidCatchingGenericException"})
  public static void mountContinuation(Continuation continuation) {
    try {
      if (continuation == null) {
        java_lang_thread_setContinuation.invoke(Thread.currentThread(), null);
        mountedContinuation.remove();
      } else {
        java_lang_thread_setContinuation.invoke(
            Thread.currentThread(), continuation.m_continuation);
        mountedContinuation.set(continuation);
      }
    } catch (RuntimeException | Error e) {
      throw e;
    } catch (Throwable t) {
      // `t` is anything thrown internally by Thread.setContinuation.
      // It only assigns to a field, no way to throw
      // However, if the invocation fails for some reason, we'll end up with an
      // IllegalStateException when attempting to run an unmounted continuation
      throw new RuntimeException(t);
    }
  }

  /**
   * Gets the currently mounted continuation. This is thread-local value; calling this method on two
   * different threads will give two different results.
   *
   * @return The continuation mounted on the current thread.
   */
  public static Continuation getMountedContinuation() {
    return mountedContinuation.get();
  }

  @Override
  public String toString() {
    return m_continuation.toString();
  }

  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  boolean isMounted() {
    return this == getMountedContinuation();
  }
}
