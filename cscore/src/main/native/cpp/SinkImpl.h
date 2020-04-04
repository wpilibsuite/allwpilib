/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_SINKIMPL_H_
#define CSCORE_SINKIMPL_H_

#include <atomic>
#include <memory>
#include <string>

#include <wpi/Logger.h>
#include <wpi/Signal.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "SourceImpl.h"

namespace wpi {
class json;
}  // namespace wpi

namespace cs {

class Frame;

class SinkImpl : public PropertyContainer {
 public:
  explicit SinkImpl(const wpi::Twine& name, wpi::Logger& logger);
  virtual ~SinkImpl();
  SinkImpl(const SinkImpl& queue) = delete;
  SinkImpl& operator=(const SinkImpl& queue) = delete;

  wpi::StringRef GetName() const { return m_name; }

  void SetDescription(const wpi::Twine& description);
  wpi::StringRef GetDescription(wpi::SmallVectorImpl<char>& buf) const;

  void Enable();
  void Disable();
  void SetEnabled(bool enable);

  void SetSource(std::shared_ptr<SourceImpl> source);

  std::shared_ptr<SourceImpl> GetSource() const {
    std::scoped_lock lock(m_mutex);
    return m_source;
  }

  bool HasSource() const { return m_hasSource; }

  std::string GetError() const;
  wpi::StringRef GetError(wpi::SmallVectorImpl<char>& buf) const;

  bool SetConfigJson(wpi::StringRef config, CS_Status* status);
  virtual bool SetConfigJson(const wpi::json& config, CS_Status* status);
  std::string GetConfigJson(CS_Status* status);
  virtual wpi::json GetConfigJsonObject(CS_Status* status);

  wpi::sig::Signal<> enabled;
  wpi::sig::Signal<> disabled;
  wpi::sig::Signal<SourceImpl*> sourceChanged;

 protected:
  // PropertyContainer implementation
  void UpdatePropertyValue(int property, bool setString, int value,
                           const wpi::Twine& valueStr) override;

  virtual void SetSourceImpl(std::shared_ptr<SourceImpl> source);

 protected:
  wpi::Logger& m_logger;

 private:
  std::string m_name;
  std::string m_description;
  std::shared_ptr<SourceImpl> m_source;
  std::atomic_bool m_hasSource{false};
  int m_enabledCount{0};
};

}  // namespace cs

#endif  // CSCORE_SINKIMPL_H_
