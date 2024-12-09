// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.util;

/**
 * A utility interface that allows you to infer epilogue log paths for manual logging.
 * <code>LogPathInference.enable(this);</code> must be called within the constructor of
 * your Robot class before using this interface.
 */
public interface InferLogPath {
  /**
   * Computes the appropriate epilogue log path for a manually logged field.
   *
   * @param logUnder the name to log under. For instance, pivot/logUnder or arm/logUnder.
   * @return An absolute log path that follows epilogue conventions.
   */
  default String logPath(String logUnder) {
    return LogPathInference.logPathMap.getOrDefault(this, LogPathInference.DEFAULT_NAMESPACE)
        + "/"
        + logUnder;
  }
}
