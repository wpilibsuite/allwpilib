// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/cs/VideoMode.hpp"
#include "wpi/cs/cscore_c.h"

namespace wpi::cs {

/**
 * Listener event
 */
struct RawEvent {
  enum Kind {
    SOURCE_CREATED = CS_SOURCE_CREATED,
    SOURCE_DESTROYED = CS_SOURCE_DESTROYED,
    SOURCE_CONNECTED = CS_SOURCE_CONNECTED,
    SOURCE_DISCONNECTED = CS_SOURCE_DISCONNECTED,
    SOURCE_VIDEO_MODES_UPDATED = CS_SOURCE_VIDEOMODES_UPDATED,
    SOURCE_VIDEO_MODE_CHANGED = CS_SOURCE_VIDEOMODE_CHANGED,
    SOURCE_PROPERTY_CREATED = CS_SOURCE_PROPERTY_CREATED,
    SOURCE_PROPERTY_VALUE_UPDATED = CS_SOURCE_PROPERTY_VALUE_UPDATED,
    SOURCE_PROPERTY_CHOICES_UPDATED = CS_SOURCE_PROPERTY_CHOICES_UPDATED,
    SINK_SOURCE_CHANGED = CS_SINK_SOURCE_CHANGED,
    SINK_CREATED = CS_SINK_CREATED,
    SINK_DESTROYED = CS_SINK_DESTROYED,
    SINK_ENABLED = CS_SINK_ENABLED,
    SINK_DISABLED = CS_SINK_DISABLED,
    NETWORK_INTERFACES_CHANGED = CS_NETWORK_INTERFACES_CHANGED,
    TELEMETRY_UPDATED = CS_TELEMETRY_UPDATED,
    SINK_PROPERTY_CREATED = CS_SINK_PROPERTY_CREATED,
    SINK_PROPERTY_VALUE_UPDATED = CS_SINK_PROPERTY_VALUE_UPDATED,
    SINK_PROPERTY_CHOICES_UPDATED = CS_SINK_PROPERTY_CHOICES_UPDATED,
    USB_CAMERAS_CHANGED = CS_USB_CAMERAS_CHANGED
  };

  RawEvent() = default;
  explicit RawEvent(RawEvent::Kind kind_) : kind{kind_} {}
  RawEvent(std::string_view name_, CS_Handle handle_, RawEvent::Kind kind_)
      : kind{kind_}, name{name_} {
    if (kind_ == SINK_CREATED || kind_ == SINK_DESTROYED ||
        kind_ == SINK_ENABLED || kind_ == SINK_DISABLED) {
      sinkHandle = handle_;
    } else {
      sourceHandle = handle_;
    }
  }
  RawEvent(std::string_view name_, CS_Source source_, const VideoMode& mode_)
      : kind{SOURCE_VIDEO_MODE_CHANGED},
        sourceHandle{source_},
        name{name_},
        mode{mode_} {}
  RawEvent(std::string_view name_, CS_Source source_, RawEvent::Kind kind_,
           CS_Property property_, CS_PropertyKind propertyKind_, int value_,
           std::string_view valueStr_)
      : kind{kind_},
        sourceHandle{source_},
        name{name_},
        propertyHandle{property_},
        propertyKind{propertyKind_},
        value{value_},
        valueStr{valueStr_} {}

  Kind kind;

  // Valid for SOURCE* and SINK* respectively
  CS_Source sourceHandle = CS_INVALID_HANDLE;
  CS_Sink sinkHandle = CS_INVALID_HANDLE;

  // Source/sink/property name
  std::string name;

  // Fields for SOURCE_VIDEO_MODE_CHANGED event
  VideoMode mode;

  // Fields for SOURCE_PROPERTY* events
  CS_Property propertyHandle;
  CS_PropertyKind propertyKind;
  int value;
  std::string valueStr;

  // Listener that was triggered
  CS_Listener listener{0};
};

}  // namespace wpi::cs
