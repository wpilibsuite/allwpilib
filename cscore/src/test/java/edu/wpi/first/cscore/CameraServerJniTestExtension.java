// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.WPIUtilJNI;

import java.io.IOException;
import org.junit.jupiter.api.extension.BeforeAllCallback;
import org.junit.jupiter.api.extension.ExtensionContext;
import org.junit.jupiter.api.extension.ExtensionContext.Namespace;

public final class CameraServerJniTestExtension implements BeforeAllCallback {
  private static ExtensionContext getRoot(ExtensionContext context) {
    return context.getParent().map(CameraServerJniTestExtension::getRoot).orElse(context);
  }

  @Override
  public void beforeAll(ExtensionContext context) throws Exception {
    getRoot(context)
        .getStore(Namespace.GLOBAL)
        .getOrComputeIfAbsent(
            "CsCore Initialized",
            key -> {
              initializeNatives();
              return true;
            },
            Boolean.class);
  }

  private void initializeNatives() {
    try {
      WPIUtilJNI.forceLoad();
      CameraServerJNI.forceLoad();
      OpenCvLoader.forceLoad();
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
