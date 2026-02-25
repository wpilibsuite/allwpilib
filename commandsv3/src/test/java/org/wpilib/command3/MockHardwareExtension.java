// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import org.junit.jupiter.api.extension.BeforeEachCallback;
import org.junit.jupiter.api.extension.ExtensionContext;

public class MockHardwareExtension implements BeforeEachCallback {
  @Override
  public void beforeEach(ExtensionContext context) {
    OpModeFetcher.setFetcher(new MockOpModeFetcher());
  }

  private static final class MockOpModeFetcher extends OpModeFetcher {
    @Override
    long getOpModeId() {
      return 0;
    }

    @Override
    String getOpModeName() {
      return "";
    }
  }
}
