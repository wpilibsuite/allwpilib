// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_PROPERTYCONTAINER_H_
#define CSCORE_PROPERTYCONTAINER_H_

#include <atomic>
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/StringMap.h>
#include <wpi/mutex.h>

#include "PropertyImpl.h"
#include "cscore_cpp.h"

namespace wpi {
class Logger;
template <typename T>
class SmallVectorImpl;
class json;
}  // namespace wpi

namespace cs {

class PropertyContainer {
 public:
  virtual ~PropertyContainer() = default;

  int GetPropertyIndex(std::string_view name) const;
  std::span<int> EnumerateProperties(wpi::SmallVectorImpl<int>& vec,
                                     CS_Status* status) const;
  CS_PropertyKind GetPropertyKind(int property) const;
  std::string_view GetPropertyName(int property,
                                   wpi::SmallVectorImpl<char>& buf,
                                   CS_Status* status) const;
  int GetProperty(int property, CS_Status* status) const;
  virtual void SetProperty(int property, int value, CS_Status* status);
  int GetPropertyMin(int property, CS_Status* status) const;
  int GetPropertyMax(int property, CS_Status* status) const;
  int GetPropertyStep(int property, CS_Status* status) const;
  int GetPropertyDefault(int property, CS_Status* status) const;
  std::string_view GetStringProperty(int property,
                                     wpi::SmallVectorImpl<char>& buf,
                                     CS_Status* status) const;
  virtual void SetStringProperty(int property, std::string_view value,
                                 CS_Status* status);
  std::vector<std::string> GetEnumPropertyChoices(int property,
                                                  CS_Status* status) const;

  bool SetPropertiesJson(const wpi::json& config, wpi::Logger& logger,
                         std::string_view logName, CS_Status* status);
  wpi::json GetPropertiesJsonObject(CS_Status* status);

 protected:
  // Get a property; must be called with m_mutex held.
  PropertyImpl* GetProperty(int property) {
    if (property <= 0 ||
        static_cast<size_t>(property) > m_propertyData.size()) {
      return nullptr;
    }
    return m_propertyData[property - 1].get();
  }
  const PropertyImpl* GetProperty(int property) const {
    if (property <= 0 ||
        static_cast<size_t>(property) > m_propertyData.size()) {
      return nullptr;
    }
    return m_propertyData[property - 1].get();
  }
  // Create or update a property; must be called with m_mutex held.
  // @tparam NewFunc functor that returns a std::unique_ptr<PropertyImpl>
  // @tparam UpdateFunc functor that takes a PropertyImpl&.
  template <typename NewFunc, typename UpdateFunc>
  int CreateOrUpdateProperty(std::string_view name, NewFunc newFunc,
                             UpdateFunc updateFunc) {
    int& ndx = m_properties[name];
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
  int CreateProperty(std::string_view name, NewFunc newFunc) {
    return CreateOrUpdateProperty(name, newFunc, [](PropertyImpl&) {});
  }

  // Create an "empty" property.  This is called by GetPropertyIndex to create
  // properties that don't exist (as GetPropertyIndex can't fail).
  // Note: called with m_mutex held.
  // The default implementation simply creates a PropertyImpl object.
  virtual std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      std::string_view name) const;

  // Cache properties.  Implementations must return false and set status to
  // CS_SOURCE_IS_DISCONNECTED if not possible to cache.
  // The default implementation simply sets m_property_cached to true.
  virtual bool CacheProperties(CS_Status* status) const;

  virtual void NotifyPropertyCreated(int propIndex, PropertyImpl& prop) = 0;

  // Update property value; must be called with m_mutex held.
  virtual void UpdatePropertyValue(int property, bool setString, int value,
                                   std::string_view valueStr) = 0;

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
