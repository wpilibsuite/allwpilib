// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/json_fwd.h>

#include "networktables/NetworkTableType.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

class GenericEntry;
class GenericPublisher;
class GenericSubscriber;
class NetworkTableInstance;

/** NetworkTables Topic. */
class Topic {
 public:
  Topic() = default;
  explicit Topic(NT_Topic handle) : m_handle{handle} {}

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle for the topic.
   *
   * @return Native handle
   */
  NT_Topic GetHandle() const { return m_handle; }

  /**
   * Gets the instance for the topic.
   *
   * @return Instance
   */
  NetworkTableInstance GetInstance() const;

  /**
   * Gets the name of the topic.
   *
   * @return the topic's name
   */
  std::string GetName() const { return ::nt::GetTopicName(m_handle); }

  /**
   * Gets the type of the topic.
   *
   * @return the topic's type
   */
  NetworkTableType GetType() const {
    return static_cast<NetworkTableType>(::nt::GetTopicType(m_handle));
  }

  /**
   * Gets the type string of the topic. This may have more information
   * than the numeric type (especially for raw values).
   *
   * @return the topic's type
   */
  std::string GetTypeString() const {
    return ::nt::GetTopicTypeString(m_handle);
  }

  /**
   * Make value persistent through server restarts.
   *
   * @param persistent True for persistent, false for not persistent.
   */
  void SetPersistent(bool persistent) {
    ::nt::SetTopicPersistent(m_handle, persistent);
  }

  /**
   * Returns whether the value is persistent through server restarts.
   *
   * @return True if the value is persistent.
   */
  bool IsPersistent() const { return ::nt::GetTopicPersistent(m_handle); }

  /**
   * Make the server retain the topic even when there are no publishers.
   *
   * @param retained True for retained, false for not retained.
   */
  void SetRetained(bool retained) {
    ::nt::SetTopicRetained(m_handle, retained);
  }

  /**
   * Returns whether the topic is retained by server when there are no
   * publishers.
   *
   * @return True if the topic is retained.
   */
  bool IsRetained() const { return ::nt::GetTopicRetained(m_handle); }

  /**
   * Allow storage of the topic's last value, allowing the value to be read (and
   * not just accessed through event queues and listeners).
   *
   * @param cached True for cached, false for not cached.
   */
  void SetCached(bool cached) { ::nt::SetTopicCached(m_handle, cached); }

  /**
   * Returns whether the topic's last value is stored.
   *
   * @return True if the topic is cached.
   */
  bool IsCached() const { return ::nt::GetTopicCached(m_handle); }

  /**
   * Determines if the topic is currently being published.
   *
   * @return True if the topic exists, false otherwise.
   */
  bool Exists() const { return nt::GetTopicExists(m_handle); }

  /**
   * Gets the current value of a property (as a JSON object).
   *
   * @param name property name
   * @return JSON object; null object if the property does not exist.
   */
  wpi::json GetProperty(std::string_view name) const;

  /**
   * Sets a property value.
   *
   * @param name property name
   * @param value property value
   */
  void SetProperty(std::string_view name, const wpi::json& value);

  /**
   * Deletes a property.  Has no effect if the property does not exist.
   *
   * @param name property name
   */
  void DeleteProperty(std::string_view name) {
    ::nt::DeleteTopicProperty(m_handle, name);
  }

  /**
   * Gets all topic properties as a JSON object.  Each key in the object
   * is the property name, and the corresponding value is the property value.
   *
   * @return JSON object
   */
  wpi::json GetProperties() const;

  /**
   * Updates multiple topic properties.  Each key in the passed-in object is
   * the name of the property to add/update, and the corresponding value is the
   * property value to set for that property.  Null values result in deletion
   * of the corresponding property.
   *
   * @param properties JSON object with keys to add/update/delete
   * @return False if properties is not an object
   */
  bool SetProperties(const wpi::json& properties) {
    return ::nt::SetTopicProperties(m_handle, properties);
  }

  /**
   * Gets combined information about the topic.
   *
   * @return Topic information
   */
  TopicInfo GetInfo() const { return ::nt::GetTopicInfo(m_handle); }

  /**
   * Create a new subscriber to the topic.
   *
   * <p>The subscriber is only active as long as the returned object
   * is not destroyed.
   *
   * @param options subscribe options
   * @return subscriber
   */
  [[nodiscard]]
  GenericSubscriber GenericSubscribe(
      const PubSubOptions& options = kDefaultPubSubOptions);

  /**
   * Create a new subscriber to the topic.
   *
   * <p>The subscriber is only active as long as the returned object
   * is not destroyed.
   *
   * @note Subscribers that do not match the published data type do not return
   *     any values. To determine if the data type matches, use the appropriate
   *     Topic functions.
   *
   * @param typeString type string
   * @param options subscribe options
   * @return subscriber
   */
  [[nodiscard]]
  GenericSubscriber GenericSubscribe(
      std::string_view typeString,
      const PubSubOptions& options = kDefaultPubSubOptions);

  /**
   * Create a new publisher to the topic.
   *
   * The publisher is only active as long as the returned object
   * is not destroyed.
   *
   * @note It is not possible to publish two different data types to the same
   *     topic. Conflicts between publishers are typically resolved by the
   *     server on a first-come, first-served basis. Any published values that
   *     do not match the topic's data type are dropped (ignored). To determine
   *     if the data type matches, use the appropriate Topic functions.
   *
   * @param typeString type string
   * @param options publish options
   * @return publisher
   */
  [[nodiscard]]
  GenericPublisher GenericPublish(
      std::string_view typeString,
      const PubSubOptions& options = kDefaultPubSubOptions);

  /**
   * Create a new publisher to the topic, with type string and initial
   * properties.
   *
   * The publisher is only active as long as the returned object
   * is not destroyed.
   *
   * @note It is not possible to publish two different data types to the same
   *     topic. Conflicts between publishers are typically resolved by the
   *     server on a first-come, first-served basis. Any published values that
   *     do not match the topic's data type are dropped (ignored). To determine
   *     if the data type matches, use the appropriate Topic functions.
   *
   * @param typeString type string
   * @param properties JSON properties
   * @param options publish options
   * @return publisher
   */
  [[nodiscard]]
  GenericPublisher GenericPublishEx(
      std::string_view typeString, const wpi::json& properties,
      const PubSubOptions& options = kDefaultPubSubOptions);

  /**
   * Create a new generic entry for the topic.
   *
   * Entries act as a combination of a subscriber and a weak publisher. The
   * subscriber is active as long as the entry is not destroyed. The publisher
   * is created when the entry is first written to, and remains active until
   * either Unpublish() is called or the entry is destroyed.
   *
   * @note It is not possible to use two different data types with the same
   *     topic. Conflicts between publishers are typically resolved by the
   *     server on a first-come, first-served basis. Any published values that
   *     do not match the topic's data type are dropped (ignored), and the entry
   *     will show no new values if the data type does not match. To determine
   *     if the data type matches, use the appropriate Topic functions.
   *
   * @param options publish and/or subscribe options
   * @return entry
   */
  [[nodiscard]]
  GenericEntry GetGenericEntry(
      const PubSubOptions& options = kDefaultPubSubOptions);

  /**
   * Create a new generic entry for the topic.
   *
   * Entries act as a combination of a subscriber and a weak publisher. The
   * subscriber is active as long as the entry is not destroyed. The publisher
   * is created when the entry is first written to, and remains active until
   * either Unpublish() is called or the entry is destroyed.
   *
   * @note It is not possible to use two different data types with the same
   *     topic. Conflicts between publishers are typically resolved by the
   *     server on a first-come, first-served basis. Any published values that
   *     do not match the topic's data type are dropped (ignored), and the entry
   *     will show no new values if the data type does not match. To determine
   *     if the data type matches, use the appropriate Topic functions.
   *
   * @param typeString type string
   * @param options publish and/or subscribe options
   * @return entry
   */
  [[nodiscard]]
  GenericEntry GetGenericEntry(
      std::string_view typeString,
      const PubSubOptions& options = kDefaultPubSubOptions);

  /**
   * Equality operator.  Returns true if both instances refer to the same
   * native handle.
   */
  bool operator==(const Topic&) const = default;

 protected:
  NT_Topic m_handle{0};
};

/** NetworkTables subscriber. */
class Subscriber {
 public:
  virtual ~Subscriber() { ::nt::Release(m_subHandle); }

  Subscriber(const Subscriber&) = delete;
  Subscriber& operator=(const Subscriber&) = delete;

  Subscriber(Subscriber&& rhs) : m_subHandle{rhs.m_subHandle} {
    rhs.m_subHandle = 0;
  }

  Subscriber& operator=(Subscriber&& rhs) {
    if (m_subHandle != 0) {
      ::nt::Release(m_subHandle);
    }
    m_subHandle = rhs.m_subHandle;
    rhs.m_subHandle = 0;
    return *this;
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_subHandle != 0; }

  /**
   * Gets the native handle for the subscriber.
   *
   * @return Native handle
   */
  NT_Subscriber GetHandle() const { return m_subHandle; }

  /**
   * Determines if the topic is currently being published.
   *
   * @return True if the topic exists, false otherwise.
   */
  bool Exists() const { return nt::GetTopicExists(m_subHandle); }

  /**
   * Gets the last time the value was changed.
   * Note: this is not atomic with Get(); use GetAtomic() to get
   * both the value and last change as an atomic operation.
   *
   * @return Topic last change time
   */
  int64_t GetLastChange() const {
    return ::nt::GetEntryLastChange(m_subHandle);
  }

  /**
   * Gets the subscribed-to topic.
   *
   * @return Topic
   */
  Topic GetTopic() const {
    return Topic{::nt::GetTopicFromHandle(m_subHandle)};
  }

 protected:
  Subscriber() = default;
  explicit Subscriber(NT_Subscriber handle) : m_subHandle{handle} {}

  NT_Subscriber m_subHandle{0};

 private:
  void anchor();
};

/** NetworkTables publisher. */
class Publisher {
 public:
  virtual ~Publisher() { ::nt::Release(m_pubHandle); }

  Publisher(const Publisher&) = delete;
  Publisher& operator=(const Publisher&) = delete;

  Publisher(Publisher&& rhs) : m_pubHandle{rhs.m_pubHandle} {
    rhs.m_pubHandle = 0;
  }

  Publisher& operator=(Publisher&& rhs) {
    if (m_pubHandle != 0) {
      ::nt::Release(m_pubHandle);
    }
    m_pubHandle = rhs.m_pubHandle;
    rhs.m_pubHandle = 0;
    return *this;
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_pubHandle != 0; }

  /**
   * Gets the native handle for the publisher.
   *
   * @return Native handle
   */
  NT_Publisher GetHandle() const { return m_pubHandle; }

  /**
   * Gets the published-to topic.
   *
   * @return Topic
   */
  Topic GetTopic() const {
    return Topic{::nt::GetTopicFromHandle(m_pubHandle)};
  }

 protected:
  Publisher() = default;
  explicit Publisher(NT_Publisher handle) : m_pubHandle{handle} {}

  /// NetworkTables handle.
  NT_Publisher m_pubHandle{0};

 private:
  void anchor();
};

}  // namespace nt
