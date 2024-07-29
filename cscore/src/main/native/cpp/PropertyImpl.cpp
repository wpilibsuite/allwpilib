// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PropertyImpl.h"

using namespace cs;

PropertyImpl::PropertyImpl(std::string_view name_) : name{name_} {}
PropertyImpl::PropertyImpl(std::string_view name_, CS_PropertyKind kind_,
                           int step_, int defaultValue_, int value_)
    : name{name_},
      propKind{kind_},
      step{step_},
      defaultValue{defaultValue_},
      value{value_} {}
PropertyImpl::PropertyImpl(std::string_view name_, CS_PropertyKind kind_,
                           int minimum_, int maximum_, int step_,
                           int defaultValue_, int value_)
    : name{name_},
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
  if (hasMinimum && v < minimum) {
    value = minimum;
  } else if (hasMaximum && v > maximum) {
    value = maximum;
  } else {
    value = v;
  }
  bool wasValueSet = valueSet;
  valueSet = true;
  if (!wasValueSet || value != oldValue) {
    changed();
  }
}

void PropertyImpl::SetValue(std::string_view v) {
  bool valueChanged = false;
  if (valueStr != v) {
    valueStr = v;
    valueChanged = true;
  }
  bool wasValueSet = valueSet;
  valueSet = true;
  if (!wasValueSet || valueChanged) {
    changed();
  }
}

void PropertyImpl::SetDefaultValue(int v) {
  if (hasMinimum && v < minimum) {
    defaultValue = minimum;
  } else if (hasMaximum && v > maximum) {
    defaultValue = maximum;
  } else {
    defaultValue = v;
  }
}
