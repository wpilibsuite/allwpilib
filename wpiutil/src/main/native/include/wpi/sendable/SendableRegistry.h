// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "wpi/function_ref.h"

namespace wpi {

class Sendable;
class SendableBuilder;

/**
 * The SendableRegistry class is the public interface for registering sensors
 * and actuators for use on dashboards and LiveWindow.
 */
class SendableRegistry final {
 public:
  SendableRegistry() = delete;

  using UID = size_t;

  /**
   * Initializes the SendableRegistry. This is used to ensure initialization and
   * destruction order relative to Sendables.
   */
  static void EnsureInitialized();

  /**
   * Sets the factory for LiveWindow builders.
   *
   * @param factory factory function
   */
  static void SetLiveWindowBuilderFactory(
      std::function<std::unique_ptr<SendableBuilder>()> factory);

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param name component name
   */
  static void Add(Sendable* sendable, std::string_view name);

  /**
   * Adds an object to the registry.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param channel      The channel number the device is plugged into
   */
  static void Add(Sendable* sendable, std::string_view moduleType, int channel);

  /**
   * Adds an object to the registry.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  static void Add(Sendable* sendable, std::string_view moduleType,
                  int moduleNumber, int channel);

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param subsystem subsystem name
   * @param name component name
   */
  static void Add(Sendable* sendable, std::string_view subsystem,
                  std::string_view name);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param name component name
   */
  static void AddLW(Sendable* sendable, std::string_view name);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param channel      The channel number the device is plugged into
   */
  static void AddLW(Sendable* sendable, std::string_view moduleType,
                    int channel);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  static void AddLW(Sendable* sendable, std::string_view moduleType,
                    int moduleNumber, int channel);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param subsystem subsystem name
   * @param name component name
   */
  static void AddLW(Sendable* sendable, std::string_view subsystem,
                    std::string_view name);

  /**
   * Adds a child object to an object.  Adds the child object to the registry
   * if it's not already present.
   *
   * @param parent parent object
   * @param child child object
   */
  static void AddChild(Sendable* parent, Sendable* child);

  /**
   * Adds a child object to an object.  Adds the child object to the registry
   * if it's not already present.
   *
   * @param parent parent object
   * @param child child object
   */
  static void AddChild(Sendable* parent, void* child);

  /**
   * Removes an object from the registry.
   *
   * @param sendable object to remove
   * @return true if the object was removed; false if it was not present
   */
  static bool Remove(Sendable* sendable);

  /**
   * Moves an object in the registry (for use in move constructors/assignments).
   *
   * @param to new object
   * @param from old object
   */
  static void Move(Sendable* to, Sendable* from);

  /**
   * Determines if an object is in the registry.
   *
   * @param sendable object to check
   * @return True if in registry, false if not.
   */
  static bool Contains(const Sendable* sendable);

  /**
   * Gets the name of an object.
   *
   * @param sendable object
   * @return Name (empty if object is not in registry)
   */
  static std::string GetName(const Sendable* sendable);

  /**
   * Sets the name of an object.
   *
   * @param sendable object
   * @param name name
   */
  static void SetName(Sendable* sendable, std::string_view name);

  /**
   * Sets the name of an object with a channel number.
   *
   * @param sendable   object
   * @param moduleType A string that defines the module name in the label for
   *                   the value
   * @param channel    The channel number the device is plugged into
   */
  static void SetName(Sendable* sendable, std::string_view moduleType,
                      int channel);

  /**
   * Sets the name of an object with a module and channel number.
   *
   * @param sendable     object
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  static void SetName(Sendable* sendable, std::string_view moduleType,
                      int moduleNumber, int channel);

  /**
   * Sets both the subsystem name and device name of an object.
   *
   * @param sendable object
   * @param subsystem subsystem name
   * @param name device name
   */
  static void SetName(Sendable* sendable, std::string_view subsystem,
                      std::string_view name);

  /**
   * Gets the subsystem name of an object.
   *
   * @param sendable object
   * @return Subsystem name (empty if object is not in registry)
   */
  static std::string GetSubsystem(const Sendable* sendable);

  /**
   * Sets the subsystem name of an object.
   *
   * @param sendable object
   * @param subsystem subsystem name
   */
  static void SetSubsystem(Sendable* sendable, std::string_view subsystem);

  /**
   * Gets a unique handle for setting/getting data with SetData() and GetData().
   *
   * @return Handle
   */
  static int GetDataHandle();

  /**
   * Associates arbitrary data with an object in the registry.
   *
   * @param sendable object
   * @param handle data handle returned by GetDataHandle()
   * @param data data to set
   * @return Previous data (may be null)
   */
  static std::shared_ptr<void> SetData(Sendable* sendable, int handle,
                                       std::shared_ptr<void> data);

  /**
   * Gets arbitrary data associated with an object in the registry.
   *
   * @param sendable object
   * @param handle data handle returned by GetDataHandle()
   * @return data (may be null if none associated)
   */
  static std::shared_ptr<void> GetData(Sendable* sendable, int handle);

  /**
   * Enables LiveWindow for an object.
   *
   * @param sendable object
   */
  static void EnableLiveWindow(Sendable* sendable);

  /**
   * Disables LiveWindow for an object.
   *
   * @param sendable object
   */
  static void DisableLiveWindow(Sendable* sendable);

  /**
   * Get unique id for an object.  Since objects can move, use this instead
   * of storing Sendable* directly if ownership is in question.
   *
   * @param sendable object
   * @return unique id
   */
  static UID GetUniqueId(Sendable* sendable);

  /**
   * Get sendable object for a given unique id.
   *
   * @param uid unique id
   * @return sendable object (may be null)
   */
  static Sendable* GetSendable(UID uid);

  /**
   * Publishes an object in the registry.
   *
   * @param sendableUid sendable unique id
   * @param builder publisher backend
   */
  static void Publish(UID sendableUid,
                      std::unique_ptr<SendableBuilder> builder);

  /**
   * Updates published information from an object.
   *
   * @param sendableUid sendable unique id
   */
  static void Update(UID sendableUid);

  /**
   * Data passed to ForeachLiveWindow() callback function
   */
  struct CallbackData {
    CallbackData(Sendable* sendable_, std::string_view name_,
                 std::string_view subsystem_, wpi::Sendable* parent_,
                 std::shared_ptr<void>& data_, SendableBuilder& builder_)
        : sendable(sendable_),
          name(name_),
          subsystem(subsystem_),
          parent(parent_),
          data(data_),
          builder(builder_) {}

    Sendable* sendable;
    std::string_view name;
    std::string_view subsystem;
    Sendable* parent;
    std::shared_ptr<void>& data;
    SendableBuilder& builder;
  };

  /**
   * Iterates over LiveWindow-enabled objects in the registry.
   * It is *not* safe to call other SendableRegistry functions from the
   * callback (this will likely deadlock).
   *
   * @param dataHandle data handle to get data pointer passed to callback
   * @param callback function to call for each object
   */
  static void ForeachLiveWindow(
      int dataHandle, wpi::function_ref<void(CallbackData& cbdata)> callback);
};

}  // namespace wpi
