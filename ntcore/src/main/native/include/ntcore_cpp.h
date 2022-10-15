// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/span.h>

#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"
#include "ntcore_cpp_types.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
class json;
}  // namespace wpi

namespace wpi::log {
class DataLog;
}  // namespace wpi::log

/** NetworkTables (ntcore) namespace */
namespace nt {

/**
 * @defgroup ntcore_cpp_handle_api ntcore C++ API
 *
 * Handle-based interface for C++.
 *
 * @{
 */

/** NetworkTables Topic Information */
struct TopicInfo {
  /** Topic handle */
  NT_Topic topic{0};

  /** Topic name */
  std::string name;

  /** Topic type */
  NT_Type type{NT_UNASSIGNED};

  /** Topic type string */
  std::string type_str;

  /** Topic properties JSON string */
  std::string properties;

  /** Get topic properties as a JSON object. */
  wpi::json GetProperties() const;

  friend void swap(TopicInfo& first, TopicInfo& second) {
    using std::swap;
    swap(first.topic, second.topic);
    swap(first.name, second.name);
    swap(first.type, second.type);
    swap(first.type_str, second.type_str);
    swap(first.properties, second.properties);
  }
};

/** NetworkTables Connection Information */
struct ConnectionInfo {
  /**
   * The remote identifier (as set on the remote node by
   * NetworkTableInstance::SetNetworkIdentity() or nt::SetNetworkIdentity()).
   */
  std::string remote_id;

  /** The IP address of the remote node. */
  std::string remote_ip;

  /** The port number of the remote node. */
  unsigned int remote_port{0};

  /**
   * The last time any update was received from the remote node (same scale as
   * returned by nt::Now()).
   */
  int64_t last_update{0};

  /**
   * The protocol version being used for this connection.  This in protocol
   * layer format, so 0x0200 = 2.0, 0x0300 = 3.0).
   */
  unsigned int protocol_version{0};

  friend void swap(ConnectionInfo& first, ConnectionInfo& second) {
    using std::swap;
    swap(first.remote_id, second.remote_id);
    swap(first.remote_ip, second.remote_ip);
    swap(first.remote_port, second.remote_port);
    swap(first.last_update, second.last_update);
    swap(first.protocol_version, second.protocol_version);
  }
};

/** NetworkTables Topic Notification */
class TopicNotification {
 public:
  TopicNotification() = default;
  TopicNotification(NT_TopicListener listener_, TopicInfo info_,
                    unsigned int flags_)
      : listener(listener_), info(std::move(info_)), flags(flags_) {}

  /** Listener that was triggered. */
  NT_TopicListener listener{0};

  /** Topic info. */
  TopicInfo info;

  /**
   * Notification flags.
   */
  unsigned int flags{0};

  friend void swap(TopicNotification& first, TopicNotification& second) {
    using std::swap;
    swap(first.listener, second.listener);
    swap(first.info, second.info);
    swap(first.flags, second.flags);
  }
};

/** NetworkTables Value Notification */
class ValueNotification {
 public:
  ValueNotification() = default;
  ValueNotification(NT_ValueListener listener_, NT_Topic topic_,
                    NT_Handle subentry_, Value value_, unsigned int flags_)
      : listener(listener_),
        topic(topic_),
        subentry(subentry_),
        value(std::move(value_)),
        flags(flags_) {}

  /** Listener that was triggered. */
  NT_ValueListener listener{0};

  /** Topic handle. */
  NT_Topic topic{0};

  /** Subscriber/entry handle. */
  NT_Handle subentry{0};

  /** The new value. */
  Value value;

  /**
   * Update flags.  For example, NT_NOTIFY_NEW if the key did not previously
   * exist.
   */
  unsigned int flags{0};

  friend void swap(ValueNotification& first, ValueNotification& second) {
    using std::swap;
    swap(first.listener, second.listener);
    swap(first.topic, second.topic);
    swap(first.subentry, second.subentry);
    swap(first.value, second.value);
    swap(first.flags, second.flags);
  }
};

/** NetworkTables Connection Notification */
class ConnectionNotification {
 public:
  ConnectionNotification() = default;
  ConnectionNotification(NT_ConnectionListener listener_, bool connected_,
                         ConnectionInfo conn_)
      : listener(listener_), connected(connected_), conn(std::move(conn_)) {}

  /** Listener that was triggered. */
  NT_ConnectionListener listener{0};

  /** True if event is due to connection being established. */
  bool connected = false;

  /** Connection info. */
  ConnectionInfo conn;

  friend void swap(ConnectionNotification& first,
                   ConnectionNotification& second) {
    using std::swap;
    swap(first.listener, second.listener);
    swap(first.connected, second.connected);
    swap(first.conn, second.conn);
  }
};

/** NetworkTables log message. */
class LogMessage {
 public:
  LogMessage() = default;
  LogMessage(NT_Logger logger_, unsigned int level_, std::string_view filename_,
             unsigned int line_, std::string_view message_)
      : logger(logger_),
        level(level_),
        filename(filename_),
        line(line_),
        message(message_) {}

  /** The logger that generated the message. */
  NT_Logger logger{0};

  /** Log level of the message.  See NT_LogLevel. */
  unsigned int level{0};

  /** The filename of the source file that generated the message. */
  std::string filename;

  /** The line number in the source file that generated the message. */
  unsigned int line{0};

  /** The message. */
  std::string message;

  friend void swap(LogMessage& first, LogMessage& second) {
    using std::swap;
    swap(first.logger, second.logger);
    swap(first.level, second.level);
    swap(first.filename, second.filename);
    swap(first.line, second.line);
    swap(first.message, second.message);
  }
};

/** NetworkTables publish/subscribe option. */
class PubSubOption {
 public:
  constexpr PubSubOption(NT_PubSubOptionType type, double value)
      : type{type}, value{value} {}

  /**
   * How frequently changes will be sent over the network. NetworkTables may
   * send more frequently than this (e.g. use a combined minimum period for all
   * values) or apply a restricted range to this value. The default if
   * unspecified (and the immediate flag is false) is 100 ms. This option and
   * the immediate option override each other.
   *
   * @param time time between updates, in seconds
   * @return option
   */
  static constexpr PubSubOption Periodic(double time) {
    return PubSubOption{NT_PUBSUB_PERIODIC, time};
  }

  /**
   * If enabled, sends all value changes over the network even if only sent
   * periodically. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  static constexpr PubSubOption SendAll(bool enabled) {
    return PubSubOption{NT_PUBSUB_SENDALL, enabled ? 1.0 : 0.0};
  }

  /**
   * If enabled, no value changes are sent over the network. This option
   * defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  static constexpr PubSubOption TopicsOnly(bool enabled) {
    return PubSubOption{NT_PUBSUB_TOPICSONLY, enabled ? 1.0 : 0.0};
  }

  /**
   * If enabled, preserves duplicate value changes (rather than ignoring them).
   * This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  static constexpr PubSubOption KeepDuplicates(bool enabled) {
    return PubSubOption{NT_PUBSUB_KEEPDUPLICATES, enabled ? 1.0 : 0.0};
  }

  /**
   * Polling storage for subscription. Specifies the maximum number of updates
   * NetworkTables should store between calls to the subscriber's poll()
   * function. Defaults to 1 if logging is false, 20 if logging is true.
   *
   * @param depth number of entries to save for polling.
   * @return option
   */
  static constexpr PubSubOption PollStorage(int depth) {
    return PubSubOption{NT_PUBSUB_POLLSTORAGE, static_cast<double>(depth)};
  }

  NT_PubSubOptionType type;
  double value;
};

/**
 * @defgroup ntcore_instance_func Instance Functions
 * @{
 */

/**
 * Get default instance.
 * This is the instance used by non-handle-taking functions.
 *
 * @return Instance handle
 */
NT_Inst GetDefaultInstance();

/**
 * Create an instance.
 *
 * @return Instance handle
 */
NT_Inst CreateInstance();

/**
 * Destroy an instance.
 * The default instance cannot be destroyed.
 *
 * @param inst Instance handle
 */
void DestroyInstance(NT_Inst inst);

/**
 * Get instance handle from another handle.
 *
 * @param handle    entry/instance/etc. handle
 * @return Instance handle
 */
NT_Inst GetInstanceFromHandle(NT_Handle handle);

/** @} */

/**
 * @defgroup ntcore_table_func Table Functions
 * @{
 */

/**
 * Get Entry Handle.
 *
 * @param inst      instance handle
 * @param name      entry name (UTF-8 string)
 * @return entry handle
 */
NT_Entry GetEntry(NT_Inst inst, std::string_view name);

/**
 * Gets the name of the specified entry.
 * Returns an empty string if the handle is invalid.
 *
 * @param entry   entry handle
 * @return Entry name
 */
std::string GetEntryName(NT_Entry entry);

/**
 * Gets the type for the specified entry, or unassigned if non existent.
 *
 * @param entry   entry handle
 * @return Entry type
 */
NT_Type GetEntryType(NT_Entry entry);

/**
 * Gets the last time the entry was changed.
 * Returns 0 if the handle is invalid.
 *
 * @param subentry   subscriber or entry handle
 * @return Entry last change time
 */
int64_t GetEntryLastChange(NT_Handle subentry);

/**
 * Get Entry Value.
 *
 * Returns copy of current entry value.
 * Note that one of the type options is "unassigned".
 *
 * @param subentry     subscriber or entry handle
 * @return entry value
 */
Value GetEntryValue(NT_Handle subentry);

/**
 * Set Default Entry Value
 *
 * Returns copy of current entry value if it exists.
 * Otherwise, sets passed in value, and returns set value.
 * Note that one of the type options is "unassigned".
 *
 * @param entry     entry handle
 * @param value     value to be set if name does not exist
 * @return False on error (value not set), True on success
 */
bool SetDefaultEntryValue(NT_Entry entry, const Value& value);

/**
 * Set Entry Value.
 *
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, returns error and does not update value.
 *
 * @param entry     entry handle
 * @param value     new entry value
 * @return False on error (type mismatch), True on success
 */
bool SetEntryValue(NT_Entry entry, const Value& value);

/**
 * Set Entry Flags.
 *
 * @param entry     entry handle
 * @param flags     flags value (bitmask of NT_EntryFlags)
 */
void SetEntryFlags(NT_Entry entry, unsigned int flags);

/**
 * Get Entry Flags.
 *
 * @param entry     entry handle
 * @return Flags value (bitmask of NT_EntryFlags)
 */
unsigned int GetEntryFlags(NT_Entry entry);

/**
 * Read Entry Queue.
 *
 * Returns new entry values since last call.
 *
 * @param subentry     subscriber or entry handle
 * @return entry value array
 */
std::vector<Value> ReadQueueValue(NT_Handle subentry);

/** @} */

/**
 * @defgroup ntcore_topic_func Topic Functions
 * @{
 */

/**
 * Get Published Topics.
 *
 * Returns an array of topic handles.  The results are optionally filtered by
 * string prefix and type to only return a subset of all topics.
 *
 * @param inst    instance handle
 * @param prefix  name required prefix; only topics whose name
 *                starts with this string are returned
 * @param types   bitmask of NT_Type values; 0 is treated specially
 *                as a "don't care"
 * @return Array of topic handles.
 */
std::vector<NT_Topic> GetTopics(NT_Inst inst, std::string_view prefix,
                                unsigned int types);

/**
 * Get Published Topics.
 *
 * Returns an array of topic handles.  The results are optionally filtered by
 * string prefix and type to only return a subset of all topics.
 *
 * @param inst    instance handle
 * @param prefix  name required prefix; only topics whose name
 *                starts with this string are returned
 * @param types   array of type strings
 * @return Array of topic handles.
 */
std::vector<NT_Topic> GetTopics(NT_Inst inst, std::string_view prefix,
                                wpi::span<const std::string_view> types);

/**
 * Get Topic Information about multiple topics.
 *
 * Returns an array of topic information (handle, name, type, and properties).
 * The results are optionally filtered by string prefix and type to only
 * return a subset of all topics.
 *
 * @param inst    instance handle
 * @param prefix  name required prefix; only topics whose name
 *                starts with this string are returned
 * @param types   bitmask of NT_Type values; 0 is treated specially
 *                as a "don't care"
 * @return Array of topic information.
 */
std::vector<TopicInfo> GetTopicInfo(NT_Inst inst, std::string_view prefix,
                                    unsigned int types);

/**
 * Get Topic Information about multiple topics.
 *
 * Returns an array of topic information (handle, name, type, and properties).
 * The results are optionally filtered by string prefix and type to only
 * return a subset of all topics.
 *
 * @param inst    instance handle
 * @param prefix  name required prefix; only topics whose name
 *                starts with this string are returned
 * @param types   array of type strings
 * @return Array of topic information.
 */
std::vector<TopicInfo> GetTopicInfo(NT_Inst inst, std::string_view prefix,
                                    wpi::span<const std::string_view> types);

/**
 * Gets Topic Information.
 *
 * Returns information about a topic (name, type, and properties).
 *
 * @param topic   handle
 * @return Topic information.
 */
TopicInfo GetTopicInfo(NT_Topic topic);

/**
 * Gets Topic Handle.
 *
 * Returns topic handle.
 *
 * @param inst   instance handle
 * @param name   topic name
 * @return Topic handle.
 */
NT_Topic GetTopic(NT_Inst inst, std::string_view name);

/**
 * Gets the name of the specified topic.
 * Returns an empty string if the handle is invalid.
 *
 * @param topic   topic handle
 * @return Topic name
 */
std::string GetTopicName(NT_Topic topic);

/**
 * Gets the type for the specified topic, or unassigned if non existent.
 *
 * @param topic   topic handle
 * @return Topic type
 */
NT_Type GetTopicType(NT_Topic topic);

/**
 * Gets the type string for the specified topic, or empty string if non
 * existent.  This may have more information than the numeric type (especially
 * for raw values).
 *
 * @param topic   topic handle
 * @return Topic type string
 */
std::string GetTopicTypeString(NT_Topic topic);

/**
 * Sets the persistent property of a topic.  If true, the stored value is
 * persistent through server restarts.
 *
 * @param topic topic handle
 * @param value True for persistent, false for not persistent.
 */
void SetTopicPersistent(NT_Topic topic, bool value);

/**
 * Gets the persistent property of a topic.  If true, the server retains the
 * topic even when there are no publishers.
 *
 * @param topic topic handle
 * @return persistent property value
 */
bool GetTopicPersistent(NT_Topic topic);

/**
 * Sets the retained property of a topic.
 *
 * @param topic topic handle
 * @param value new retained property value
 */
void SetTopicRetained(NT_Topic topic, bool value);

/**
 * Gets the retained property of a topic.
 *
 * @param topic topic handle
 * @return retained property value
 */
bool GetTopicRetained(NT_Topic topic);

/**
 * Determine if topic exists (e.g. has at least one publisher).
 *
 * @param handle Topic, entry, or subscriber handle.
 * @return True if topic exists.
 */
bool GetTopicExists(NT_Handle handle);

/**
 * Gets the current value of a property (as a JSON object).
 *
 * @param topic topic handle
 * @param name property name
 * @return JSON object; null object if the property does not exist.
 */
wpi::json GetTopicProperty(NT_Topic topic, std::string_view name);

/**
 * Sets a property value.
 *
 * @param topic topic handle
 * @param name property name
 * @param value property value
 */
void SetTopicProperty(NT_Topic topic, std::string_view name,
                      const wpi::json& value);

/**
 * Deletes a property.  Has no effect if the property does not exist.
 *
 * @param topic topic handle
 * @param name property name
 */
void DeleteTopicProperty(NT_Topic topic, std::string_view name);

/**
 * Gets all topic properties as a JSON object.  Each key in the object
 * is the property name, and the corresponding value is the property value.
 *
 * @param topic topic handle
 * @return JSON object
 */
wpi::json GetTopicProperties(NT_Topic topic);

/**
 * Updates multiple topic properties.  Each key in the passed-in object is
 * the name of the property to add/update, and the corresponding value is the
 * property value to set for that property.  Null values result in deletion
 * of the corresponding property.
 *
 * @param topic topic handle
 * @param update JSON object with keys to add/update/delete
 * @return False if update is not a JSON object
 */
bool SetTopicProperties(NT_Topic topic, const wpi::json& update);

/**
 * Creates a new subscriber to value changes on a topic.
 *
 * @param topic topic handle
 * @param type expected type
 * @param typeStr expected type string
 * @param options subscription options
 * @return Subscriber handle
 */
NT_Subscriber Subscribe(NT_Topic topic, NT_Type type, std::string_view typeStr,
                        wpi::span<const PubSubOption> options = {});

/**
 * Stops subscriber.
 *
 * @param sub subscriber handle
 */
void Unsubscribe(NT_Subscriber sub);

/**
 * Creates a new publisher to a topic.
 *
 * @param topic topic handle
 * @param type type
 * @param typeStr type string
 * @param options publish options
 * @return Publisher handle
 */
NT_Publisher Publish(NT_Topic topic, NT_Type type, std::string_view typeStr,
                     wpi::span<const PubSubOption> options = {});

/**
 * Creates a new publisher to a topic.
 *
 * @param topic topic handle
 * @param type type
 * @param typeStr type string
 * @param properties initial properties
 * @param options publish options
 * @return Publisher handle
 */
NT_Publisher PublishEx(NT_Topic topic, NT_Type type, std::string_view typeStr,
                       const wpi::json& properties,
                       wpi::span<const PubSubOption> options = {});

/**
 * Stops publisher.
 *
 * @param pubentry publisher/entry handle
 */
void Unpublish(NT_Handle pubentry);

/**
 * @brief Creates a new entry (subscriber and weak publisher) to a topic.
 *
 * @param topic topic handle
 * @param type type
 * @param typeStr type string
 * @param options publish options
 * @return Entry handle
 */
NT_Entry GetEntry(NT_Topic topic, NT_Type type, std::string_view typeStr,
                  wpi::span<const PubSubOption> options = {});

/**
 * Stops entry subscriber/publisher.
 *
 * @param entry entry handle
 */
void ReleaseEntry(NT_Entry entry);

/**
 * Stops entry/subscriber/publisher.
 *
 * @param pubsubentry entry/subscriber/publisher handle
 */
void Release(NT_Handle pubsubentry);

/**
 * Gets the topic handle from an entry/subscriber/publisher handle.
 *
 * @param pubsubentry entry/subscriber/publisher handle
 * @return Topic handle
 */
NT_Topic GetTopicFromHandle(NT_Handle pubsubentry);

/** @} */

/**
 * @defgroup ntcore_advancedsub_func Advanced Subscriber Functions
 * @{
 */

/**
 * Subscribes to multiple topics based on one or more topic name prefixes. Can
 * be used in combination with a Value Listener or ReadQueueValue() to get value
 * changes across all matching topics.
 *
 * @param inst instance handle
 * @param prefixes topic name prefixes
 * @param options subscriber options
 * @return subscriber handle
 */
NT_MultiSubscriber SubscribeMultiple(
    NT_Inst inst, wpi::span<const std::string_view> prefixes,
    wpi::span<const PubSubOption> options = {});

/**
 * Unsubscribes a multi-subscriber.
 *
 * @param sub multi-subscriber handle
 */
void UnsubscribeMultiple(NT_MultiSubscriber sub);

/** @} */

/**
 * @defgroup ntcore_topiclistener_func Topic Listener Functions
 * @{
 */

/**
 * Create a listener for changes to topics with names that start with any of
 * the given prefixes.
 *
 * @param inst Instance handle
 * @param prefixes Topic name string prefixes
 * @param mask Bitmask of NT_TopicListenerFlags values
 * @param callback Listener function
 */
NT_TopicListener AddTopicListener(
    NT_Inst inst, wpi::span<const std::string_view> prefixes, unsigned int mask,
    std::function<void(const TopicNotification&)> callback);

/**
 * Create a listener for changes on a particular topic.
 *
 * @param handle Topic, subscriber, multi-subscriber, or entry handle
 * @param mask Bitmask of NT_TopicListenerFlags values
 * @param callback Listener function
 */
NT_TopicListener AddTopicListener(
    NT_Handle handle, unsigned int mask,
    std::function<void(const TopicNotification&)> callback);

/**
 * Creates a topic listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using AddPolledTopicListener()) will be stored in the queue and
 * must be collected by calling ReadTopicListenerQueue().
 * The returned handle must be destroyed with DestroyTopicListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_TopicListenerPoller CreateTopicListenerPoller(NT_Inst inst);

/**
 * Destroys a topic listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void DestroyTopicListenerPoller(NT_TopicListenerPoller poller);

/**
 * Read topic notifications.
 *
 * @param poller    poller handle
 * @return Array of topic notifications.  Returns empty array if no
 *         notifications since last call.
 */
std::vector<TopicNotification> ReadTopicListenerQueue(
    NT_TopicListenerPoller poller);

/**
 * Creates a polled topic listener.
 * The caller is responsible for calling ReadTopicListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param prefixes          array of UTF-8 string prefixes
 * @param mask              NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_TopicListener AddPolledTopicListener(
    NT_TopicListenerPoller poller, wpi::span<const std::string_view> prefixes,
    unsigned int mask);

/**
 * Creates a polled topic listener.
 * The caller is responsible for calling ReadTopicListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param handle            topic, subscriber, multi-subscriber, or entry handle
 * @param mask              NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_TopicListener AddPolledTopicListener(NT_TopicListenerPoller poller,
                                        NT_Handle handle, unsigned int mask);

/**
 * Removes a topic listener.
 *
 * @param listener Listener handle to remove
 */
void RemoveTopicListener(NT_TopicListener listener);

/** @} */

/**
 * @defgroup ntcore_valuelistener_func Value Listener Functions
 * @{
 */

/**
 * Create a listener for value changes on a subscriber.
 *
 * @param subentry Subscriber/entry
 * @param mask Bitmask of NT_ValueListenerFlags values
 * @param callback Listener function
 */
NT_ValueListener AddValueListener(
    NT_Handle subentry, unsigned int mask,
    std::function<void(const ValueNotification&)> callback);

/**
 * Create a value listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using AddPolledValueListener()) will be stored in the queue and
 * must be collected by calling ReadValueListenerQueue().
 * The returned handle must be destroyed with DestroyValueListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_ValueListenerPoller CreateValueListenerPoller(NT_Inst inst);

/**
 * Destroy a value listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void DestroyValueListenerPoller(NT_ValueListenerPoller poller);

/**
 * Reads value listener queue (all value changes since last call).
 *
 * @param poller    poller handle
 * @return Array of value notifications.
 */
std::vector<ValueNotification> ReadValueListenerQueue(
    NT_ValueListenerPoller poller);

/**
 * Create a polled value listener.
 * The caller is responsible for calling ReadValueListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param subentry          subscriber or entry handle
 * @param mask              NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_ValueListener AddPolledValueListener(NT_ValueListenerPoller poller,
                                        NT_Handle subentry, unsigned int mask);

/**
 * Remove a value listener.
 *
 * @param listener Listener handle to remove
 */
void RemoveValueListener(NT_ValueListener listener);

/** @} */

/**
 * @defgroup ntcore_connectionlistener_func Connection Listener Functions
 * @{
 */

/**
 * Add a connection listener.
 *
 * @param inst              instance handle
 * @param callback          listener to add
 * @param immediate_notify  notify listener of all existing connections
 * @return Listener handle
 */
NT_ConnectionListener AddConnectionListener(
    NT_Inst inst,
    std::function<void(const ConnectionNotification& event)> callback,
    bool immediate_notify);

/**
 * Create a connection listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using AddPolledConnectionListener()) will be stored in the queue and
 * must be collected by calling PollConnectionListener().
 * The returned handle must be destroyed with DestroyConnectionListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_ConnectionListenerPoller CreateConnectionListenerPoller(NT_Inst inst);

/**
 * Destroy a connection listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void DestroyConnectionListenerPoller(NT_ConnectionListenerPoller poller);

/**
 * Create a polled connection listener.
 * The caller is responsible for calling PollConnectionListener() to poll.
 *
 * @param poller            poller handle
 * @param immediate_notify  notify listener of all existing connections
 */
NT_ConnectionListener AddPolledConnectionListener(
    NT_ConnectionListenerPoller poller, bool immediate_notify);

/**
 * Get the next connection event.  This blocks until the next connect or
 * disconnect occurs.  This is intended to be used with
 * AddPolledConnectionListener(); connection listeners created using
 * AddConnectionListener() will not be serviced through this function.
 *
 * @param poller    poller handle
 * @return Information on the connection events.  Only returns empty if an
 *         error occurred (e.g. the instance was invalid or is shutting down).
 */
std::vector<ConnectionNotification> ReadConnectionListenerQueue(
    NT_ConnectionListenerPoller poller);

/**
 * Remove a connection listener.
 *
 * @param conn_listener Listener handle to remove
 */
void RemoveConnectionListener(NT_ConnectionListener conn_listener);

/** @} */

/**
 * @defgroup ntcore_network_func Client/Server Functions
 * @{
 */

/**
 * Set the network identity of this node.
 * This is the name used during the initial connection handshake, and is
 * visible through ConnectionInfo on the remote node.
 *
 * @param inst      instance handle
 * @param name      identity to advertise
 */
void SetNetworkIdentity(NT_Inst inst, std::string_view name);

/**
 * Get the current network mode.
 *
 * @param inst  instance handle
 * @return Bitmask of NT_NetworkMode.
 */
unsigned int GetNetworkMode(NT_Inst inst);

/**
 * Starts local-only operation.  Prevents calls to StartServer or StartClient
 * from taking effect.  Has no effect if StartServer or StartClient
 * has already been called.
 */
void StartLocal(NT_Inst inst);

/**
 * Stops local-only operation.  StartServer or StartClient can be called after
 * this call to start a server or client.
 */
void StopLocal(NT_Inst inst);

/**
 * Starts a server using the specified filename, listening address, and port.
 *
 * @param inst              instance handle
 * @param persist_filename  the name of the persist file to use (UTF-8 string,
 *                          null terminated)
 * @param listen_address    the address to listen on, or null to listen on any
 *                          address. (UTF-8 string, null terminated)
 * @param port3             port to communicate over (NT3)
 * @param port4             port to communicate over (NT4)
 */
void StartServer(NT_Inst inst, std::string_view persist_filename,
                 const char* listen_address, unsigned int port3,
                 unsigned int port4);

/**
 * Stops the server if it is running.
 *
 * @param inst  instance handle
 */
void StopServer(NT_Inst inst);

/**
 * Starts a NT3 client.  Use SetServer or SetServerTeam to set the server name
 * and port.
 *
 * @param inst  instance handle
 */
void StartClient3(NT_Inst inst);

/**
 * Starts a NT4 client.  Use SetServer or SetServerTeam to set the server name
 * and port.
 *
 * @param inst  instance handle
 */
void StartClient4(NT_Inst inst);

/**
 * Stops the client if it is running.
 *
 * @param inst  instance handle
 */
void StopClient(NT_Inst inst);

/**
 * Sets server address and port for client (without restarting client).
 *
 * @param inst        instance handle
 * @param server_name server name (UTF-8 string, null terminated)
 * @param port        port to communicate over
 */
void SetServer(NT_Inst inst, const char* server_name, unsigned int port);

/**
 * Sets server addresses for client (without restarting client).
 * The client will attempt to connect to each server in round robin fashion.
 *
 * @param inst      instance handle
 * @param servers   array of server name and port pairs
 */
void SetServer(
    NT_Inst inst,
    wpi::span<const std::pair<std::string_view, unsigned int>> servers);

/**
 * Sets server addresses and port for client (without restarting client).
 * Connects using commonly known robot addresses for the specified team.
 *
 * @param inst        instance handle
 * @param team        team number
 * @param port        port to communicate over
 */
void SetServerTeam(NT_Inst inst, unsigned int team, unsigned int port);

/**
 * Starts requesting server address from Driver Station.
 * This connects to the Driver Station running on localhost to obtain the
 * server IP address.
 *
 * @param inst  instance handle
 * @param port  server port to use in combination with IP from DS
 */
void StartDSClient(NT_Inst inst, unsigned int port);

/**
 * Stops requesting server address from Driver Station.
 *
 * @param inst  instance handle
 */
void StopDSClient(NT_Inst inst);

/**
 * Flush local updates.
 *
 * Forces an immediate flush of all local changes to the client/server.
 * This does not flush to the network.
 *
 * Normally this is done on a regularly scheduled interval.
 *
 * @param inst      instance handle
 */
void FlushLocal(NT_Inst inst);

/**
 * Flush to network.
 *
 * Forces an immediate flush of all local entry changes to network.
 * Normally this is done on a regularly scheduled interval (set
 * by update rates on individual publishers).
 *
 * Note: flushes are rate limited to avoid excessive network traffic.  If
 * the time between calls is too short, the flush will occur after the minimum
 * time elapses (rather than immediately).
 *
 * @param inst      instance handle
 */
void Flush(NT_Inst inst);

/**
 * Get information on the currently established network connections.
 * If operating as a client, this will return either zero or one values.
 *
 * @param inst  instance handle
 * @return      array of connection information
 */
std::vector<ConnectionInfo> GetConnections(NT_Inst inst);

/**
 * Return whether or not the instance is connected to another node.
 *
 * @param inst  instance handle
 * @return True if connected.
 */
bool IsConnected(NT_Inst inst);

/** @} */

/**
 * @defgroup ntcore_utility_func Utility Functions
 * @{
 */

/**
 * Returns monotonic current time in 1 us increments.
 * This is the same time base used for value and connection timestamps.
 * This function by default simply wraps wpi::Now(), but if SetNow() is
 * called, this function instead returns the value passed to SetNow();
 * this can be used to reduce overhead.
 *
 * @return Timestamp
 */
int64_t Now();

/**
 * Sets the current timestamp used for timestamping values that do not
 * provide a timestamp (e.g. a value of 0 is passed).  For consistency,
 * it also results in Now() returning the set value.  This should generally
 * be used only if the overhead of calling wpi::Now() is a concern.
 * If used, it should be called periodically with the value of wpi::Now().
 *
 * @param timestamp timestamp (1 us increments)
 */
void SetNow(int64_t timestamp);

/**
 * Turns a type string into a type enum value.
 *
 * @param typeString type string
 * @return Type value
 */
NT_Type GetTypeFromString(std::string_view typeString);

/**
 * Turns a type enum value into a type string.
 *
 * @param type type enum
 * @return Type string
 */
std::string_view GetStringFromType(NT_Type type);

/** @} */

/**
 * @defgroup ntcore_data_logger_func Data Logger Functions
 * @{
 */

/**
 * Starts logging entry changes to a DataLog.
 *
 * @param inst instance handle
 * @param log data log object; lifetime must extend until StopEntryDataLog is
 *            called or the instance is destroyed
 * @param prefix only store entries with names that start with this prefix;
 *               the prefix is not included in the data log entry name
 * @param logPrefix prefix to add to data log entry names
 * @return Data logger handle
 */
NT_DataLogger StartEntryDataLog(NT_Inst inst, wpi::log::DataLog& log,
                                std::string_view prefix,
                                std::string_view logPrefix);

/**
 * Stops logging entry changes to a DataLog.
 *
 * @param logger data logger handle
 */
void StopEntryDataLog(NT_DataLogger logger);

/**
 * Starts logging connection changes to a DataLog.
 *
 * @param inst instance handle
 * @param log data log object; lifetime must extend until StopConnectionDataLog
 *            is called or the instance is destroyed
 * @param name data log entry name
 * @return Data logger handle
 */
NT_ConnectionDataLogger StartConnectionDataLog(NT_Inst inst,
                                               wpi::log::DataLog& log,
                                               std::string_view name);

/**
 * Stops logging connection changes to a DataLog.
 *
 * @param logger data logger handle
 */
void StopConnectionDataLog(NT_ConnectionDataLogger logger);

/** @} */

/**
 * @defgroup ntcore_logger_func Logger Functions
 * @{
 */

/**
 * Add logger callback function.  By default, log messages are sent to stderr;
 * this function sends log messages to the provided callback function instead.
 * The callback function will only be called for log messages with level
 * greater than or equal to min_level and less than or equal to max_level;
 * messages outside this range will be silently ignored.
 *
 * @param inst        instance handle
 * @param func        log callback function
 * @param min_level   minimum log level
 * @param max_level   maximum log level
 * @return Logger handle
 */
NT_Logger AddLogger(NT_Inst inst,
                    std::function<void(const LogMessage& msg)> func,
                    unsigned int min_level, unsigned int max_level);

/**
 * Create a log poller.  A poller provides a single queue of poll events.
 * The returned handle must be destroyed with DestroyLoggerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_LoggerPoller CreateLoggerPoller(NT_Inst inst);

/**
 * Destroy a log poller.  This will abort any blocked polling call and prevent
 * additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void DestroyLoggerPoller(NT_LoggerPoller poller);

/**
 * Set the log level for a log poller.  Events will only be generated for
 * log messages with level greater than or equal to min_level and less than or
 * equal to max_level; messages outside this range will be silently ignored.
 *
 * @param poller        poller handle
 * @param min_level     minimum log level
 * @param max_level     maximum log level
 * @return Logger handle
 */
NT_Logger AddPolledLogger(NT_LoggerPoller poller, unsigned int min_level,
                          unsigned int max_level);

/**
 * Get the next log event.  This blocks until the next log occurs.
 *
 * @param poller    poller handle
 * @return Information on the log events.  Only returns empty if an error
 *         occurred (e.g. the instance was invalid or is shutting down).
 */
std::vector<LogMessage> ReadLoggerQueue(NT_LoggerPoller poller);

/**
 * Remove a logger.
 *
 * @param logger Logger handle to remove
 */
void RemoveLogger(NT_Logger logger);

/** @} */
/** @} */

}  // namespace nt
