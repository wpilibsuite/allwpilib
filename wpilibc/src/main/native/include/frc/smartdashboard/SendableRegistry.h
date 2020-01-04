/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <networktables/NetworkTable.h>
#include <wpi/STLExtras.h>
#include <wpi/Twine.h>

namespace frc {

class Sendable;
class SendableBuilderImpl;

/**
 * The SendableRegistry class is the public interface for registering sensors
 * and actuators for use on dashboards and LiveWindow.
 */
class SendableRegistry {
 public:
  SendableRegistry(const SendableRegistry&) = delete;
  SendableRegistry& operator=(const SendableRegistry&) = delete;

  using UID = size_t;

  /**
   * Gets an instance of the SendableRegistry class.
   *
   * This is a singleton to guarantee that there is only a single instance
   * regardless of how many times GetInstance is called.
   */
  static SendableRegistry& GetInstance();

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param name component name
   */
  void Add(Sendable* sendable, const wpi::Twine& name);

  /**
   * Adds an object to the registry.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param channel      The channel number the device is plugged into
   */
  void Add(Sendable* sendable, const wpi::Twine& moduleType, int channel);

  /**
   * Adds an object to the registry.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  void Add(Sendable* sendable, const wpi::Twine& moduleType, int moduleNumber,
           int channel);

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param subsystem subsystem name
   * @param name component name
   */
  void Add(Sendable* sendable, const wpi::Twine& subsystem,
           const wpi::Twine& name);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param name component name
   */
  void AddLW(Sendable* sendable, const wpi::Twine& name);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param channel      The channel number the device is plugged into
   */
  void AddLW(Sendable* sendable, const wpi::Twine& moduleType, int channel);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable     object to add
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  void AddLW(Sendable* sendable, const wpi::Twine& moduleType, int moduleNumber,
             int channel);

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param subsystem subsystem name
   * @param name component name
   */
  void AddLW(Sendable* sendable, const wpi::Twine& subsystem,
             const wpi::Twine& name);

  /**
   * Adds a child object to an object.  Adds the child object to the registry
   * if it's not already present.
   *
   * @param parent parent object
   * @param child child object
   */
  void AddChild(Sendable* parent, Sendable* child);

  /**
   * Adds a child object to an object.  Adds the child object to the registry
   * if it's not already present.
   *
   * @param parent parent object
   * @param child child object
   */
  void AddChild(Sendable* parent, void* child);

  /**
   * Removes an object from the registry.
   *
   * @param sendable object to remove
   * @return true if the object was removed; false if it was not present
   */
  bool Remove(Sendable* sendable);

  /**
   * Moves an object in the registry (for use in move constructors/assignments).
   *
   * @param to new object
   * @param from old object
   */
  void Move(Sendable* to, Sendable* from);

  /**
   * Determines if an object is in the registry.
   *
   * @param sendable object to check
   * @return True if in registry, false if not.
   */
  bool Contains(const Sendable* sendable) const;

  /**
   * Gets the name of an object.
   *
   * @param sendable object
   * @return Name (empty if object is not in registry)
   */
  std::string GetName(const Sendable* sendable) const;

  /**
   * Sets the name of an object.
   *
   * @param sendable object
   * @param name name
   */
  void SetName(Sendable* sendable, const wpi::Twine& name);

  /**
   * Sets the name of an object with a channel number.
   *
   * @param sendable   object
   * @param moduleType A string that defines the module name in the label for
   *                   the value
   * @param channel    The channel number the device is plugged into
   */
  void SetName(Sendable* sendable, const wpi::Twine& moduleType, int channel);

  /**
   * Sets the name of an object with a module and channel number.
   *
   * @param sendable     object
   * @param moduleType   A string that defines the module name in the label for
   *                     the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into
   */
  void SetName(Sendable* sendable, const wpi::Twine& moduleType,
               int moduleNumber, int channel);

  /**
   * Sets both the subsystem name and device name of an object.
   *
   * @param sendable object
   * @param subsystem subsystem name
   * @param name device name
   */
  void SetName(Sendable* sendable, const wpi::Twine& subsystem,
               const wpi::Twine& name);

  /**
   * Gets the subsystem name of an object.
   *
   * @param sendable object
   * @return Subsystem name (empty if object is not in registry)
   */
  std::string GetSubsystem(const Sendable* sendable) const;

  /**
   * Sets the subsystem name of an object.
   *
   * @param sendable object
   * @param subsystem subsystem name
   */
  void SetSubsystem(Sendable* sendable, const wpi::Twine& subsystem);

  /**
   * Gets a unique handle for setting/getting data with SetData() and GetData().
   *
   * @return Handle
   */
  int GetDataHandle();

  /**
   * Associates arbitrary data with an object in the registry.
   *
   * @param sendable object
   * @param handle data handle returned by GetDataHandle()
   * @param data data to set
   * @return Previous data (may be null)
   */
  std::shared_ptr<void> SetData(Sendable* sendable, int handle,
                                std::shared_ptr<void> data);

  /**
   * Gets arbitrary data associated with an object in the registry.
   *
   * @param sendable object
   * @param handle data handle returned by GetDataHandle()
   * @return data (may be null if none associated)
   */
  std::shared_ptr<void> GetData(Sendable* sendable, int handle);

  /**
   * Enables LiveWindow for an object.
   *
   * @param sendable object
   */
  void EnableLiveWindow(Sendable* sendable);

  /**
   * Disables LiveWindow for an object.
   *
   * @param sendable object
   */
  void DisableLiveWindow(Sendable* sendable);

  /**
   * Get unique id for an object.  Since objects can move, use this instead
   * of storing Sendable* directly if ownership is in question.
   *
   * @param sendable object
   * @return unique id
   */
  UID GetUniqueId(Sendable* sendable);

  /**
   * Get sendable object for a given unique id.
   *
   * @param uid unique id
   * @return sendable object (may be null)
   */
  Sendable* GetSendable(UID uid);

  /**
   * Publishes an object in the registry to a network table.
   *
   * @param sendableUid sendable unique id
   * @param table network table
   */
  void Publish(UID sendableUid, std::shared_ptr<NetworkTable> table);

  /**
   * Updates network table information from an object.
   *
   * @param sendableUid sendable unique id
   */
  void Update(UID sendableUid);

  /**
   * Data passed to ForeachLiveWindow() callback function
   */
  struct CallbackData {
    CallbackData(Sendable* sendable_, wpi::StringRef name_,
                 wpi::StringRef subsystem_, Sendable* parent_,
                 std::shared_ptr<void>& data_, SendableBuilderImpl& builder_)
        : sendable(sendable_),
          name(name_),
          subsystem(subsystem_),
          parent(parent_),
          data(data_),
          builder(builder_) {}

    Sendable* sendable;
    wpi::StringRef name;
    wpi::StringRef subsystem;
    Sendable* parent;
    std::shared_ptr<void>& data;
    SendableBuilderImpl& builder;
  };

  /**
   * Iterates over LiveWindow-enabled objects in the registry.
   * It is *not* safe to call other SendableRegistry functions from the
   * callback (this will likely deadlock).
   *
   * @param dataHandle data handle to get data pointer passed to callback
   * @param callback function to call for each object
   */
  void ForeachLiveWindow(
      int dataHandle,
      wpi::function_ref<void(CallbackData& cbdata)> callback) const;

 private:
  SendableRegistry();

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace frc
