// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "SourceImpl.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/json_fwd.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::cs {

class Frame;
class Notifier;
class Telemetry;

class SinkImpl : public PropertyContainer {
 public:
  explicit SinkImpl(std::string_view name, wpi::util::Logger& logger,
                    Notifier& notifier, Telemetry& telemetry);
  ~SinkImpl() override;
  SinkImpl(const SinkImpl& queue) = delete;
  SinkImpl& operator=(const SinkImpl& queue) = delete;

  std::string_view GetName() const { return m_name; }

  void SetDescription(std::string_view description);
  std::string_view GetDescription(wpi::util::SmallVectorImpl<char>& buf) const;

  void Enable();
  void Disable();
  void SetEnabled(bool enabled);

  void SetSource(std::shared_ptr<SourceImpl> source);

  std::shared_ptr<SourceImpl> GetSource() const {
    std::scoped_lock lock(m_mutex);
    return m_source;
  }

  std::string GetError() const;
  std::string_view GetError(wpi::util::SmallVectorImpl<char>& buf) const;

  bool SetConfigJson(std::string_view config, CS_Status* status);
  virtual bool SetConfigJson(const wpi::util::json& config, CS_Status* status);
  std::string GetConfigJson(CS_Status* status);
  virtual wpi::util::json GetConfigJsonObject(CS_Status* status);

 protected:
  // PropertyContainer implementation
  void NotifyPropertyCreated(int propIndex, PropertyImpl& prop) override;
  void UpdatePropertyValue(int property, bool setString, int value,
                           std::string_view valueStr) override;

  virtual void SetSourceImpl(std::shared_ptr<SourceImpl> source);

 protected:
  wpi::util::Logger& m_logger;
  Notifier& m_notifier;
  Telemetry& m_telemetry;

 private:
  std::string m_name;
  std::string m_description;
  std::shared_ptr<SourceImpl> m_source;
  int m_enabledCount{0};
};

}  // namespace wpi::cs
