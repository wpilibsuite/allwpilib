// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.fields;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class LoadConfigTest {
  @ParameterizedTest
  @EnumSource(Fields.class)
  void testLoad(Fields field) {
    FieldConfig config = Assertions.assertDoesNotThrow(() -> FieldConfig.loadField(field));

    Assertions.assertNotNull(config.getImageUrl());
  }
}
