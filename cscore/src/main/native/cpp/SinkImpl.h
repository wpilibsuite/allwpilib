/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_SINKIMPL_H_
#define CSCORE_SINKIMPL_H_

#include <memory>
#include <string>

#include <wpi/Logger.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "SourceImpl.h"

namespace cs {

class Frame;
class Notifier;
class Telemetry;

class SinkImpl : public PropertyContainer {
 public:
  explicit SinkImpl(const wpi::Twine& name, wpi::Logger& logger,
                    Notifier& notifier, Telemetry& telemetry);
  virtual ~SinkImpl();
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
    std::lock_guard<wpi::mutex> lock(m_mutex);
    return m_source;
  }

  std::string GetError() const;
  wpi::StringRef GetError(wpi::SmallVectorImpl<char>& buf) const;

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
