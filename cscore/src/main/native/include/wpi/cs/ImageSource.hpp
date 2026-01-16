// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/cs/VideoProperty.hpp"
#include "wpi/cs/VideoSource.hpp"

namespace wpi::cs {

/**
 * A base class for single image providing sources.
 */
class ImageSource : public VideoSource {
 protected:
  ImageSource() = default;

 public:
  /**
   * Signal sinks that an error has occurred.  This should be called instead
   * of NotifyFrame when an error occurs.
   *
   * @param msg Notification message.
   */
  void NotifyError(std::string_view msg) {
    m_status = 0;
    NotifySourceError(m_handle, msg, &m_status);
  }

  /**
   * Set source connection status.  Defaults to true.
   *
   * @param connected True for connected, false for disconnected
   */
  void SetConnected(bool connected) {
    m_status = 0;
    SetSourceConnected(m_handle, connected, &m_status);
  }

  /**
   * Set source description.
   *
   * @param description Description
   */
  void SetDescription(std::string_view description) {
    m_status = 0;
    SetSourceDescription(m_handle, description, &m_status);
  }

  /**
   * Create a property.
   *
   * @param name Property name
   * @param kind Property kind
   * @param minimum Minimum value
   * @param maximum Maximum value
   * @param step Step value
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateProperty(std::string_view name, VideoProperty::Kind kind,
                               int minimum, int maximum, int step,
                               int defaultValue, int value) {
    m_status = 0;
    return VideoProperty{CreateSourceProperty(
        m_handle, name, static_cast<CS_PropertyKind>(static_cast<int>(kind)),
        minimum, maximum, step, defaultValue, value, &m_status)};
  }

  /**
   * Create an integer property.
   *
   * @param name Property name
   * @param minimum Minimum value
   * @param maximum Maximum value
   * @param step Step value
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateIntegerProperty(std::string_view name, int minimum,
                                      int maximum, int step, int defaultValue,
                                      int value) {
    m_status = 0;
    return VideoProperty{CreateSourceProperty(
        m_handle, name,
        static_cast<CS_PropertyKind>(
            static_cast<int>(VideoProperty::Kind::kInteger)),
        minimum, maximum, step, defaultValue, value, &m_status)};
  }

  /**
   * Create a boolean property.
   *
   * @param name Property name
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateBooleanProperty(std::string_view name, bool defaultValue,
                                      bool value) {
    m_status = 0;
    return VideoProperty{CreateSourceProperty(
        m_handle, name,
        static_cast<CS_PropertyKind>(
            static_cast<int>(VideoProperty::Kind::kBoolean)),
        0, 1, 1, defaultValue ? 1 : 0, value ? 1 : 0, &m_status)};
  }

  /**
   * Create a string property.
   *
   * @param name Property name
   * @param value Current value
   * @return Property
   */
  VideoProperty CreateStringProperty(std::string_view name,
                                     std::string_view value) {
    m_status = 0;
    auto prop = VideoProperty{CreateSourceProperty(
        m_handle, name,
        static_cast<CS_PropertyKind>(
            static_cast<int>(VideoProperty::Kind::kString)),
        0, 0, 0, 0, 0, &m_status)};
    prop.SetString(value);
    return prop;
  }

  /**
   * Configure enum property choices.
   *
   * @param property Property
   * @param choices Choices
   */
  void SetEnumPropertyChoices(const VideoProperty& property,
                              std::span<const std::string> choices) {
    m_status = 0;
    SetSourceEnumPropertyChoices(m_handle, property.m_handle, choices,
                                 &m_status);
  }

  /**
   * Configure enum property choices.
   *
   * @param property Property
   * @param choices Choices
   */
  template <typename T>
  void SetEnumPropertyChoices(const VideoProperty& property,
                              std::initializer_list<T> choices) {
    std::vector<std::string> vec;
    vec.reserve(choices.size());
    for (const auto& choice : choices) {
      vec.emplace_back(choice);
    }
    m_status = 0;
    SetSourceEnumPropertyChoices(m_handle, property.m_handle, vec, &m_status);
  }
};

}  // namespace wpi::cs
