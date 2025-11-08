// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.util.Objects;

/** A continuation scope. */
final class ContinuationScope {
  // The underlying jdk.internal.vm.ContinuationScope object
  // https://github.com/openjdk/jdk/blob/jdk-21%2B35/src/java.base/share/classes/jdk/internal/vm/ContinuationScope.java
  final Object m_continuationScope;

  static final Class<?> jdk_internal_vm_ContinuationScope;
  private static final MethodHandle CONSTRUCTOR;

  static {
    try {
      jdk_internal_vm_ContinuationScope = Class.forName("jdk.internal.vm.ContinuationScope");

      var lookup =
          MethodHandles.privateLookupIn(jdk_internal_vm_ContinuationScope, MethodHandles.lookup());

      CONSTRUCTOR =
          lookup.findConstructor(
              jdk_internal_vm_ContinuationScope, MethodType.methodType(void.class, String.class));
    } catch (Throwable t) {
      throw new ExceptionInInitializerError(t);
    }
  }

  /**
   * Constructs a new scope.
   *
   * @param name The scope's name
   */
  ContinuationScope(String name) {
    Objects.requireNonNull(name);
    try {
      m_continuationScope = CONSTRUCTOR.invoke(name);
    } catch (Throwable e) {
      throw new RuntimeException(e);
    }
  }

  @Override
  public String toString() {
    return m_continuationScope.toString();
  }
}
