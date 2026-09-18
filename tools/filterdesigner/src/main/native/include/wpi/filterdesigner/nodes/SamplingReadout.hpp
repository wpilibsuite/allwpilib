// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <imgui.h>

#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/nodes/StatusText.hpp"

namespace wpi::filterdesigner {

/**
 * Renders a source node's one-line sampling readout, e.g.
 * "248 Hz, 0.2% filled, longest gap 24.0 ms".
 *
 * The loaders resample rather than reject, so a signal can look plottable
 * while being mostly interpolant across a disabled period. This line is where
 * the user finds out, escalating grey to amber to red as the reconstruction
 * takes over — see @ref ClassifySampling.
 *
 * Wraps at @p wrapWidth: with a gap figure the line outgrows a source node's
 * widgets, and the node would follow it.
 */
inline void DrawSamplingReadout(const Signal& signal,
                                float wrapWidth = kStatusWrapWidth) {
  const std::string text = DescribeSampling(signal);
  switch (ClassifySampling(signal)) {
    case SamplingSeverity::Warn:
      DrawStatusText(kStatusWarnColor, text, wrapWidth);
      break;
    case SamplingSeverity::Bad:
      DrawStatusText(kStatusErrorColor, text, wrapWidth);
      break;
    case SamplingSeverity::Ok:
      DrawStatusTextDisabled(text, wrapWidth);
      break;
  }
}

}  // namespace wpi::filterdesigner
