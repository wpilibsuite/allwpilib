// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_SINKIMPL_H_
#define CSCORE_SINKIMPL_H_

#include <memory>
#include <string>

#include <wpi/Logger.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "SourceImpl.h"

namespace wpi {
class json;
}  // namespace wpi

namespace cs {

class Frame;
class Notifier;
class Telemetry;

class SinkImpl : public PropertyContainer {
 public:
  explicit SinkImpl(const wpi::Twine& name, wpi::Logger& logger,
                    Notifier& notifier, Telemetry& telemetry);
  ~SinkImpl() override;
  SinkImpl(const SinkImpl& queue) = delete;
  SinkImpl& operator=(const SinkImpl& queue) = delete;

  wpi::StringRef GetName() const { return m_name; }

  void SetDescription(const wpi::Twine& description);
  wpi::StringRef GetDescription(wpi::SmallVectorImpl<char>& buf) const;

  void Enable();
  void Disable();
  void SetEnabled(bool enabled);

  void SetSource(std::shared_ptr<SourceImpl> source);

  std::shared_ptr<SourceImpl> GetSource() const {
    std::scoped_lock lock(m_mutex);
    return m_source;
  }

  std::string GetError() const;
  wpi::StringRef GetError(wpi::SmallVectorImpl<char>& buf) const;

  bool SetConfigJson(wpi::StringRef config, CS_Status* status);
  virtual bool SetConfigJson(const wpi::json& config, CS_Status* status);
  std::string GetConfigJson(CS_Status* status);
  virtual wpi::json GetConfigJsonObject(CS_Status* status);

 protected:
  // PropertyContainer implementation
  void NotifyPropertyCreated(int propIndex, PropertyImpl& prop) override;
  void UpdatePropertyValue(int property, bool setString, int value,
                           const wpi::Twine& valueStr) override;

  virtual void SetSourceImpl(std::shared_ptr<SourceImpl> source);

 protected:
  wpi::Logger& m_logger;
  Notifier& m_notifier;
  Telemetry& m_telemetry;

 private:
  std::string m_name;
  std::string m_description;
  std::shared_ptr<SourceImpl> m_source;
  int m_enabledCount{0};
};

}  // namespace cs

#endif  // CSCORE_SINKIMPL_H_
