// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class EpilogueTest {
  @Test
  void testDefaultConfig() {
    assertNotNull(Epilogue.getConfig());
    assertEquals("Robot", Epilogue.getConfig().root);
    assertEquals(Logged.Importance.DEBUG, Epilogue.getConfig().minimumImportance);
  }

  @Test
  void testConfigure() {
    Epilogue.configure(
        config -> {
          config.root = "CustomRoot";
          config.minimumImportance = Logged.Importance.INFO;
        });

    assertEquals("CustomRoot", Epilogue.getConfig().root);
    assertEquals(Logged.Importance.INFO, Epilogue.getConfig().minimumImportance);

    assertTrue(Epilogue.shouldLog(Logged.Importance.CRITICAL));
    assertTrue(Epilogue.shouldLog(Logged.Importance.INFO));
    assertFalse(Epilogue.shouldLog(Logged.Importance.DEBUG));

    // Reset back to defaults for other tests
    Epilogue.configure(
        config -> {
          config.root = "Robot";
          config.minimumImportance = Logged.Importance.DEBUG;
        });
  }
}
