// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

/** The base interface for complex (internally mutable) tunable objects. */
@SuppressWarnings("PMD.ImplicitFunctionalInterface")
public interface ComplexTunable {
  /**
   * Gets the tunable table type. Default is no specified table type (null).
   *
   * @return Table type
   */
  default String getTunableType() {
    return null;
  }

  /**
   * Publishes the tunable to the given table.
   *
   * <p>This function may be called when the tunable is initially published or republished during
   * backend migration, and it must not throw. If an exception escapes, registry and backend state
   * is not guaranteed to be restored.
   *
   * @param table table
   */
  void publishTunable(TunableTable table);

  /**
   * Updates this complex tunable.
   *
   * <p>This function must not throw. If an exception escapes, registry and backend state is not
   * guaranteed to be restored.
   */
  default void updateTunable() {}

  /**
   * Publishes a child tunable under all currently published paths for this complex tunable.
   *
   * <p>If this complex tunable is not currently published, this function does nothing. Complex
   * tunables should still publish their current children from {@link #publishTunable(TunableTable)}
   * so backend migration and full republish operations can recreate the complete child set.
   *
   * @param name the child name
   * @param tunable the child tunable
   */
  default void publishChildTunable(String name, TunableBase tunable) {
    TunableRegistry.publishChild(this, name, tunable);
  }

  /**
   * Publishes a child complex tunable under all currently published paths for this complex tunable.
   *
   * <p>If this complex tunable is not currently published, this function does nothing. Complex
   * tunables should still publish their current children from {@link #publishTunable(TunableTable)}
   * so backend migration and full republish operations can recreate the complete child set.
   *
   * @param name the child name
   * @param tunable the child complex tunable
   */
  default void publishChildTunable(String name, ComplexTunable tunable) {
    TunableRegistry.publishChild(this, name, tunable);
  }

  /**
   * Removes a child tunable from all currently published paths for this complex tunable.
   *
   * <p>If this complex tunable is not currently published, this function does nothing.
   *
   * @param name the child name
   */
  default void removeChildTunable(String name) {
    TunableRegistry.removeChild(this, name);
  }

  /**
   * Marks a child tunable as locally changed.
   *
   * <p>Use this for child tunables published from plain member variables when the member is mutated
   * outside of the child tunable object.
   *
   * <p>If this complex tunable or the child tunable is not currently published, this function does
   * nothing.
   *
   * @param name the child name
   */
  default void setChildTunableChanged(String name) {
    TunableRegistry.setChildChanged(this, name);
  }
}
