/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PropertyImpl.h"

using namespace cs;

PropertyImpl::PropertyImpl(const wpi::Twine& name_) : name{name_.str()} {}
PropertyImpl::PropertyImpl(const wpi::Twine& name_, CS_PropertyKind kind_,
                           int step_, int defaultValue_, int value_)
    : name{name_.str()},
      propKind{kind_},
      step{step_},
      defaultValue{defaultValue_},
      value{value_} {}
PropertyImpl::PropertyImpl(const wpi::Twine& name_, CS_PropertyKind kind_,
                           int minimum_, int maximum_, int step_,
                           int defaultValue_, int value_)
    : name{name_.str()},
      propKind{kind_},
      hasMinimum{true},
      hasMaximum{true},
      minimum{minimum_},
      maximum{maximum_},
      step{step_},
      defaultValue{defaultValue_},
      value{value_} {}

void PropertyImpl::SetValue(int v) {
  int oldValue = value;
  if (hasMinimum && v < minimum)
    value = minimum;
  else if (hasMaximum && v > maximum)
    value = maximum;
  else
    value = v;
  bool wasValueSet = valueSet;
  valueSet = true;
  if (!wasValueSet || value != oldValue) changed();
}

void PropertyImpl::SetValue(const wpi::Twine& v) {
  bool valueChanged = false;
  std::string vStr = v.str();
  if (valueStr != vStr) {
    valueStr = vStr;
    valueChanged = true;
  }
  bool wasValueSet = valueSet;
  valueSet = true;
  if (!wasValueSet || valueChanged) changed();
}

void PropertyImpl::SetDefaultValue(int v) {
  if (hasMinimum && v < minimum)
    defaultValue = minimum;
  else if (hasMaximum && v > maximum)
    defaultValue = maximum;
  else
    defaultValue = v;
}
