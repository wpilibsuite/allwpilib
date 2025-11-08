// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_PROPERTYIMPL_H_
#define CSCORE_PROPERTYIMPL_H_

#include <string>
#include <string_view>
#include <vector>

#include <wpi/Signal.h>

#include "cscore_c.h"

namespace cs {

// Property data
class PropertyImpl {
 public:
  PropertyImpl() = default;
  explicit PropertyImpl(std::string_view name_);
  PropertyImpl(std::string_view name_, CS_PropertyKind kind_, int step_,
               int defaultValue_, int value_);
  PropertyImpl(std::string_view name_, CS_PropertyKind kind_, int minimum_,
               int maximum_, int step_, int defaultValue_, int value_);
  virtual ~PropertyImpl() = default;
  PropertyImpl(const PropertyImpl& oth) = delete;
  PropertyImpl& operator=(const PropertyImpl& oth) = delete;

  void SetValue(int v);
  void SetValue(std::string_view v);
  void SetDefaultValue(int v);

  std::string name;
  CS_PropertyKind propKind{CS_PROP_NONE};
  bool hasMinimum{false};
  bool hasMaximum{false};
  int minimum{0};
  int maximum{100};
  int step{1};
  int defaultValue{0};
  int value{0};
  std::string valueStr;
  std::vector<std::string> enumChoices;
  bool valueSet{false};

  // emitted when value changes
  wpi::sig::Signal<> changed;
};

}  // namespace cs

#endif  // CSCORE_PROPERTYIMPL_H_
