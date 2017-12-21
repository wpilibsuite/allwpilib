/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_PROPERTYIMPL_H_
#define CSCORE_PROPERTYIMPL_H_

#include <string>
#include <vector>

#include <llvm/StringRef.h>

#include "cscore_c.h"

namespace cs {

// Property data
class PropertyImpl {
 public:
  PropertyImpl() = default;
  explicit PropertyImpl(llvm::StringRef name_) : name{name_} {}
  PropertyImpl(llvm::StringRef name_, CS_PropertyKind kind_, int step_,
               int defaultValue_, int value_)
      : name{name_},
        propKind{kind_},
        step{step_},
        defaultValue{defaultValue_},
        value{value_} {}
  virtual ~PropertyImpl() = default;
  PropertyImpl(const PropertyImpl& oth) = delete;
  PropertyImpl& operator=(const PropertyImpl& oth) = delete;

  void SetValue(int v) {
    if (hasMinimum && v < minimum)
      value = minimum;
    else if (hasMaximum && v > maximum)
      value = maximum;
    else
      value = v;
    valueSet = true;
  }

  void SetValue(llvm::StringRef v) {
    valueStr = v;
    valueSet = true;
  }

  void SetDefaultValue(int v) {
    if (hasMinimum && v < minimum)
      defaultValue = minimum;
    else if (hasMaximum && v > maximum)
      defaultValue = maximum;
    else
      defaultValue = v;
  }

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
};

}  // namespace cs

#endif  // CSCORE_PROPERTYIMPL_H_
