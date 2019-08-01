/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

import org.junit.jupiter.api.Test;

public interface SendableTest extends Testable<Sendable> {
  @Test
  default void defaultNameTest() {
    assertFalse(createValue().getName().isBlank());
  }

  @Test
  default void callSetNameTest() {
    final String name = "MyName";
    Sendable sendable = createValue();

    sendable.setName(name);

    assertEquals(name, sendable.getName());
  }
}
