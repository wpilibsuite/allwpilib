/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_PROPERTYCONTAINER_H_
#define CSCORE_PROPERTYCONTAINER_H_

#include <atomic>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/SmallVector.h>
#include <wpi/StringMap.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "PropertyImpl.h"
#include "cscore_cpp.h"

namespace cs {

class PropertyContainer {
 public:
  virtual ~PropertyContainer() = default;

  int GetPropertyIndex(const wpi::Twine& name) const;
  wpi::ArrayRef<int> EnumerateProperties(wpi::SmallVectorImpl<int>& vec,
                                         CS_Status* status) const;
  CS_PropertyKind GetPropertyKind(int property) const;
  wpi::StringRef GetPropertyName(int property, wpi::SmallVectorImpl<char>& buf,
                                 CS_Status* status) const;
  int GetProperty(int property, CS_Status* status) const;
  virtual void SetProperty(int property, int value, CS_Status* status);
  int GetPropertyMin(int property, CS_Status* status) const;
  int GetPropertyMax(int property, CS_Status* status) const;
  int GetPropertyStep(int property, CS_Status* status) const;
  int GetPropertyDefault(int property, CS_Status* status) const;
  wpi::StringRef GetStringProperty(int property,
                                   wpi::SmallVectorImpl<char>& buf,
                                   CS_Status* status) const;
  virtual void SetStringProperty(int property, const wpi::Twine& value,
                                 CS_Status* status);
  std::vector<std::string> GetEnumPropertyChoices(int property,
                                                  CS_Status* status) const;

 protected:
  // Get a property; must be called with m_mutex held.
  PropertyImpl* GetProperty(int property) {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return m_propertyData[property - 1].get();
  }
  const PropertyImpl* GetProperty(int property) const {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return m_propertyData[property - 1].get();
  }
  // Create or update a property; must be called with m_mutex held.
  // @tparam NewFunc functor that returns a std::unique_ptr<PropertyImpl>
  // @tparam UpdateFunc functor that takes a PropertyImpl&.
  template <typename NewFunc, typename UpdateFunc>
  int CreateOrUpdateProperty(const wpi::Twine& name, NewFunc newFunc,
                             UpdateFunc updateFunc) {
    wpi::SmallVector<char, 64> nameBuf;
    int& ndx = m_properties[name.toStringRef(nameBuf)];
    if (ndx == 0) {
      // create a new index
      ndx = m_propertyData.size() + 1;
      m_propertyData.emplace_back(newFunc());
    } else {
      // update existing
      updateFunc(*GetProperty(ndx));
    }
    return ndx;
  }
  template <typename NewFunc>
  int CreateProperty(const wpi::Twine& name, NewFunc newFunc) {
    return CreateOrUpdateProperty(name, newFunc, [](PropertyImpl&) {});
  }

  // Create an "empty" property.  This is called by GetPropertyIndex to create
  // properties that don't exist (as GetPropertyIndex can't fail).
  // Note: called with m_mutex held.
  // The default implementation simply creates a PropertyImpl object.
  virtual std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      const wpi::Twine& name) const;

  // Cache properties.  Implementations must return false and set status to
  // CS_SOURCE_IS_DISCONNECTED if not possible to cache.
  // The default implementation simply sets m_property_cached to true.
  virtual bool CacheProperties(CS_Status* status) const;

  virtual void NotifyPropertyCreated(int propIndex, PropertyImpl& prop) = 0;

  // Update property value; must be called with m_mutex held.
  virtual void UpdatePropertyValue(int property, bool setString, int value,
                                   const wpi::Twine& valueStr) = 0;

  // Whether CacheProperties() has been successful at least once (and thus
  // should not be called again)
  mutable std::atomic_bool m_properties_cached{false};

  mutable wpi::mutex m_mutex;

  // Cached properties (protected with m_mutex)
  mutable std::vector<std::unique_ptr<PropertyImpl>> m_propertyData;
  mutable wpi::StringMap<int> m_properties;
};

}  // namespace cs

#endif  // CSCORE_PROPERTYCONTAINER_H_
