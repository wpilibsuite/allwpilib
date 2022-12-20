#pragma once

#include "PropertyImpl.h"

namespace cs {

class UsbCameraProperty : public PropertyImpl {
 public:
  UsbCameraProperty() = default;
  explicit UsbCameraProperty(std::string_view name_) : PropertyImpl{name_} {}

  // Software property constructor
  UsbCameraProperty(std::string_view name_, unsigned id_, CS_PropertyKind kind_,
                    int minimum_, int maximum_, int step_, int defaultValue_,
                    int value_)
      : PropertyImpl(name_, kind_, minimum_, maximum_, step_, defaultValue_,
                     value_),
        device{false},
        id{id_} {}

  // Normalized property constructor
  UsbCameraProperty(std::string_view name_, int rawIndex_,
                    const UsbCameraProperty& rawProp, int defaultValue_,
                    int value_)
      : PropertyImpl(name_, rawProp.propKind, 1, defaultValue_, value_),
        percentage{true},
        propPair{rawIndex_},
        id{rawProp.id},
        type{rawProp.type} {
    hasMinimum = true;
    minimum = 0;
    hasMaximum = true;
    maximum = 100;
  }

  bool device{true};
  bool isAutoProp{true};

  bool isControlProperty{false};

  bool percentage{false};

  int propPair{0};

  unsigned id{0};
  int type{0};

 private:
};

}  // namespace cs
