package edu.wpi.first.epilogue;

import edu.wpi.first.epilogue.logging.EpilogueBackend;

/** A base interface that represents a manually loggable class. */
public interface ManualLogged {
  default EpilogueBackend nested() {
    return ManualLogProcessor.nestedLoggerOf(this);
  }
}
