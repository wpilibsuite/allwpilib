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
    kSourceCreated = CS_SOURCE_CREATED,
    kSourceDestroyed = CS_SOURCE_DESTROYED,
    kSourceConnected = CS_SOURCE_CONNECTED,
    kSourceDisconnected = CS_SOURCE_DISCONNECTED,
    kSourceVideoModesUpdated = CS_SOURCE_VIDEOMODES_UPDATED,
    kSourceVideoModeChanged = CS_SOURCE_VIDEOMODE_CHANGED,
    kSourcePropertyCreated = CS_SOURCE_PROPERTY_CREATED,
    kSourcePropertyValueUpdated = CS_SOURCE_PROPERTY_VALUE_UPDATED,
    kSourcePropertyChoicesUpdated = CS_SOURCE_PROPERTY_CHOICES_UPDATED,
    kSinkSourceChanged = CS_SINK_SOURCE_CHANGED,
    kSinkCreated = CS_SINK_CREATED,
    kSinkDestroyed = CS_SINK_DESTROYED,
    kSinkEnabled = CS_SINK_ENABLED,
    kSinkDisabled = CS_SINK_DISABLED,
    kNetworkInterfacesChanged = CS_NETWORK_INTERFACES_CHANGED,
    kTelemetryUpdated = CS_TELEMETRY_UPDATED,
    kSinkPropertyCreated = CS_SINK_PROPERTY_CREATED,
    kSinkPropertyValueUpdated = CS_SINK_PROPERTY_VALUE_UPDATED,
    kSinkPropertyChoicesUpdated = CS_SINK_PROPERTY_CHOICES_UPDATED,
    kUsbCamerasChanged = CS_USB_CAMERAS_CHANGED
  };

  RawEvent() = default;
  explicit RawEvent(RawEvent::Kind kind_) : kind{kind_} {}
  RawEvent(std::string_view name_, CS_Handle handle_, RawEvent::Kind kind_)
      : kind{kind_}, name{name_} {
    if (kind_ == kSinkCreated || kind_ == kSinkDestroyed ||
        kind_ == kSinkEnabled || kind_ == kSinkDisabled) {
      sinkHandle = handle_;
    } else {
      sourceHandle = handle_;
    }
  }
  RawEvent(std::string_view name_, CS_Source source_, const VideoMode& mode_)
      : kind{kSourceVideoModeChanged},
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

  // Valid for kSource* and kSink* respectively
  CS_Source sourceHandle = CS_INVALID_HANDLE;
  CS_Sink sinkHandle = CS_INVALID_HANDLE;

  // Source/sink/property name
  std::string name;

  // Fields for kSourceVideoModeChanged event
  VideoMode mode;

  // Fields for kSourceProperty* events
  CS_Property propertyHandle;
  CS_PropertyKind propertyKind;
  int value;
  std::string valueStr;

  // Listener that was triggered
  CS_Listener listener{0};
};

}  // namespace wpi::cs
