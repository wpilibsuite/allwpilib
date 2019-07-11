/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_CONFIGURABLESOURCEIMPL_H_
#define CSCORE_CONFIGURABLESOURCEIMPL_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/Twine.h>

#include "SourceImpl.h"

namespace cs {

class ConfigurableSourceImpl : public SourceImpl {
 protected:
  ConfigurableSourceImpl(const wpi::Twine& name, wpi::Logger& logger,
                         Notifier& notifier, Telemetry& telemetry,
                         const VideoMode& mode);

 public:
  ~ConfigurableSourceImpl() override;

  void Start() override;

  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  // OpenCV-specific functions
  void NotifyError(const wpi::Twine& msg);
  int CreateProperty(const wpi::Twine& name, CS_PropertyKind kind, int minimum,
                     int maximum, int step, int defaultValue, int value);
  int CreateProperty(const wpi::Twine& name, CS_PropertyKind kind, int minimum,
                     int maximum, int step, int defaultValue, int value,
                     std::function<void(CS_Property property)> onChange);
  void SetEnumPropertyChoices(int property, wpi::ArrayRef<std::string> choices,
                              CS_Status* status);

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace cs

#endif  // CSCORE_CONFIGURABLESOURCEIMPL_H_
