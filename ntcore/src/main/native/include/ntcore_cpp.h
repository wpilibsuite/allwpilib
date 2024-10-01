// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <wpi/json_fwd.h>

#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"
#include "ntcore_cpp_types.h"

namespace wpi {
template <typename T>
class SmallVectorImpl;
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

/**
 * Event notification flags.
 *
 * The flags are a bitmask and must be OR'ed together to indicate the
 * combination of events desired to be received.
 *
 */
struct EventFlags {
  EventFlags() = delete;

  static constexpr unsigned int kNone = NT_EVENT_NONE;
  /**
   * Initial listener addition.
   * Set this flag to receive immediate notification of matches to the
   * flag criteria.
   */
  static constexpr unsigned int kImmediate = NT_EVENT_IMMEDIATE;
  /** Client connected (on server, any client connected). */
  static constexpr unsigned int kConnected = NT_EVENT_CONNECTED;
  /** Client disconnected (on server, any client disconnected). */
  static constexpr unsigned int kDisconnected = NT_EVENT_DISCONNECTED;
  /** Any connection event (connect or disconnect). */
  static constexpr unsigned int kConnection = kConnected | kDisconnected;
  /** New topic published. */
  static constexpr unsigned int kPublish = NT_EVENT_PUBLISH;
  /** Topic unpublished. */
  static constexpr unsigned int kUnpublish = NT_EVENT_UNPUBLISH;
  /** Topic properties changed. */
  static constexpr unsigned int kProperties = NT_EVENT_PROPERTIES;
  /** Any topic event (publish, unpublish, or properties changed). */
  static constexpr unsigned int kTopic = kPublish | kUnpublish | kProperties;
  /** Topic value updated (via network). */
  static constexpr unsigned int kValueRemote = NT_EVENT_VALUE_REMOTE;
  /** Topic value updated (local). */
  static constexpr unsigned int kValueLocal = NT_EVENT_VALUE_LOCAL;
  /** Topic value updated (network or local). */
  static constexpr unsigned int kValueAll = kValueRemote | kValueLocal;
  /** Log message. */
  static constexpr unsigned int kLogMessage = NT_EVENT_LOGMESSAGE;
  /** Time synchronized with server. */
  static constexpr unsigned int kTimeSync = NT_EVENT_TIMESYNC;
};

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
   * NetworkTableInstance::StartClient4() or nt::StartClient4()).
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

/** NetworkTables Value Event Data */
class ValueEventData {
 public:
  ValueEventData() = default;
  ValueEventData(NT_Topic topic, NT_Handle subentry, Value value)
      : topic{topic}, subentry{subentry}, value{std::move(value)} {}

  /** Topic handle. */
  NT_Topic topic{0};

  /** Subscriber/entry handle. */
  NT_Handle subentry{0};

  /** The new value. */
  Value value;
};

/** NetworkTables log message. */
class LogMessage {
 public:
  LogMessage() = default;
  LogMessage(unsigned int level, std::string_view filename, unsigned int line,
             std::string_view message)
      : level{level}, filename{filename}, line{line}, message{message} {}

  /** Log level of the message.  See NT_LogLevel. */
  unsigned int level{0};

  /** The filename of the source file that generated the message. */
  std::string filename;

  /** The line number in the source file that generated the message. */
  unsigned int line{0};

  /** The message. */
  std::string message;
};

/** NetworkTables time sync event data. */
class TimeSyncEventData {
 public:
  TimeSyncEventData() = default;
  TimeSyncEventData(int64_t serverTimeOffset, int64_t rtt2, bool valid)
      : serverTimeOffset{serverTimeOffset}, rtt2{rtt2}, valid{valid} {}

  /**
   * Offset between local time and server time, in microseconds. Add this value
   * to local time to get the estimated equivalent server time.
   */
  int64_t serverTimeOffset;

  /** Measured round trip time divided by 2, in microseconds. */
  int64_t rtt2;

  /**
   * If serverTimeOffset and RTT are valid. An event with this set to false is
   * sent when the client disconnects.
   */
  bool valid;
};

/** NetworkTables event */
class Event {
 public:
  Event() = default;
  Event(NT_Listener listener, unsigned int flags, ConnectionInfo info)
      : listener{listener}, flags{flags}, data{std::move(info)} {}
  Event(NT_Listener listener, unsigned int flags, TopicInfo info)
      : listener{listener}, flags{flags}, data{std::move(info)} {}
  Event(NT_Listener listener, unsigned int flags, ValueEventData data)
      : listener{listener}, flags{flags}, data{std::move(data)} {}
  Event(NT_Listener listener, unsigned int flags, LogMessage msg)
      : listener{listener}, flags{flags}, data{std::move(msg)} {}
  Event(NT_Listener listener, unsigned int flags, NT_Topic topic,
        NT_Handle subentry, Value value)
      : listener{listener},
        flags{flags},
        data{ValueEventData{topic, subentry, std::move(value)}} {}
  Event(NT_Listener listener, unsigned int flags, unsigned int level,
        std::string_view filename, unsigned int line, std::string_view message)
      : listener{listener},
        flags{flags},
        data{LogMessage{level, filename, line, message}} {}
  Event(NT_Listener listener, unsigned int flags, int64_t serverTimeOffset,
        int64_t rtt2, bool valid)
      : listener{listener},
        flags{flags},
        data{TimeSyncEventData{serverTimeOffset, rtt2, valid}} {}

  /** Listener that triggered this event. */
  NT_Listener listener{0};

  /**
   * Event flags (NT_EventFlags). Also indicates the data included with the
   * event:
   * - NT_EVENT_CONNECTED or NT_EVENT_DISCONNECTED: GetConnectionInfo()
   * - NT_EVENT_PUBLISH, NT_EVENT_UNPUBLISH, or NT_EVENT_PROPERTIES:
   *   GetTopicInfo()
   * - NT_EVENT_VALUE, NT_EVENT_VALUE_LOCAL: GetValueData()
   * - NT_EVENT_LOGMESSAGE: GetLogMessage()
   * - NT_EVENT_TIMESYNC: GetTimeSyncEventData()
   */
  unsigned int flags{0};

  /**
   * Test event flags.
   *
   * @param kind event flag(s) to test
   * @return True if flags matches kind
   */
  bool Is(unsigned int kind) const { return (flags & kind) != 0; }

  /** Event data; content depends on flags. */
  std::variant<ConnectionInfo, TopicInfo, ValueEventData, LogMessage,
               TimeSyncEventData>
      data;

  const ConnectionInfo* GetConnectionInfo() const {
    return std::get_if<ConnectionInfo>(&data);
  }
  ConnectionInfo* GetConnectionInfo() {
    return std::get_if<ConnectionInfo>(&data);
  }

  const TopicInfo* GetTopicInfo() const {
    return std::get_if<TopicInfo>(&data);
  }
  TopicInfo* GetTopicInfo() { return std::get_if<TopicInfo>(&data); }

  const ValueEventData* GetValueEventData() const {
    return std::get_if<ValueEventData>(&data);
  }
  ValueEventData* GetValueEventData() {
    return std::get_if<ValueEventData>(&data);
  }

  const LogMessage* GetLogMessage() const {
    return std::get_if<LogMessage>(&data);
  }
  LogMessage* GetLogMessage() { return std::get_if<LogMessage>(&data); }

  const TimeSyncEventData* GetTimeSyncEventData() const {
    return std::get_if<TimeSyncEventData>(&data);
  }
  TimeSyncEventData* GetTimeSyncEventData() {
    return std::get_if<TimeSyncEventData>(&data);
  }
};

/** NetworkTables publish/subscribe options. */
struct PubSubOptions {
  /**
   * Default value of periodic.
   */
  static constexpr double kDefaultPeriodic = 0.1;

  /**
   * Structure size. Must be set to sizeof(PubSubOptions).
   */
  unsigned int structSize = sizeof(PubSubOptions);

  /**
   * Polling storage size for a subscription. Specifies the maximum number of
   * updates NetworkTables should store between calls to the subscriber's
   * ReadQueue() function. If zero, defaults to 1 if sendAll is false, 20 if
   * sendAll is true.
   */
  unsigned int pollStorage = 0;

  /**
   * How frequently changes will be sent over the network, in seconds.
   * NetworkTables may send more frequently than this (e.g. use a combined
   * minimum period for all values) or apply a restricted range to this value.
   * The default is 100 ms.
   */
  double periodic = kDefaultPeriodic;

  /**
   * For subscriptions, if non-zero, value updates for ReadQueue() are not
   * queued for this publisher.
   */
  NT_Publisher excludePublisher = 0;

  /**
   * Send all value changes over the network.
   */
  bool sendAll = false;

  /**
   * For subscriptions, don't ask for value changes (only topic announcements).
   */
  bool topicsOnly = false;

  /**
   * Preserve duplicate value changes (rather than ignoring them).
   */
  bool keepDuplicates = false;

  /**
   * Perform prefix match on subscriber topic names. Is ignored/overridden by
   * Subscribe() functions; only present in struct for the purposes of getting
   * information about subscriptions.
   */
  bool prefixMatch = false;

  /**
   * For subscriptions, if remote value updates should not be queued for
   * ReadQueue(). See also disableLocal.
   */
  bool disableRemote = false;

  /**
   * For subscriptions, if local value updates should not be queued for
   * ReadQueue(). See also disableRemote.
   */
  bool disableLocal = false;

  /**
   * For entries, don't queue (for ReadQueue) value updates for the entry's
   * internal publisher.
   */
  bool excludeSelf = false;

  /**
   * For subscriptions, don't share the existence of the subscription with the
   * network. Note this means updates will not be received from the network
   * unless another subscription overlaps with this one, and the subscription
   * will not appear in metatopics.
   */
  bool hidden = false;
};

/**
 * Default publish/subscribe options.
 */
constexpr PubSubOptions kDefaultPubSubOptions;

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
 * Reset the internals of an instance. Every handle previously associated
 * with this instance will no longer be valid, except for the instance
 * handle.
 */
void ResetInstance(NT_Inst inst);

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
 * Returns False if name exists.
 * Otherwise, sets passed in value, and returns True.
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

/**
 * Read Entry Queue.
 *
 * Returns new entry values since last call.
 *
 * @param subentry     subscriber or entry handle
 * @param types        bitmask of NT_Type values; 0 is treated specially
 *                     as a "don't care"
 * @return entry value array
 */
std::vector<Value> ReadQueueValue(NT_Handle subentry, unsigned int types);

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
                                std::span<const std::string_view> types);

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
                                    std::span<const std::string_view> types);

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
 * Sets the cached property of a topic.  If true, the server and clients will
 * store the latest value, allowing the value to be read (and not just accessed
 * through event queues and listeners).
 *
 * @param topic topic handle
 * @param value True for cached, false for not cached
 */
void SetTopicCached(NT_Topic topic, bool value);

/**
 * Gets the cached property of a topic.
 *
 * @param topic topic handle
 * @return cached property value
 */
bool GetTopicCached(NT_Topic topic);

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
                        const PubSubOptions& options = kDefaultPubSubOptions);

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
                     const PubSubOptions& options = kDefaultPubSubOptions);

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
                       const PubSubOptions& options = kDefaultPubSubOptions);

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
                  const PubSubOptions& options = kDefaultPubSubOptions);

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
    NT_Inst inst, std::span<const std::string_view> prefixes,
    const PubSubOptions& options = kDefaultPubSubOptions);

/**
 * Unsubscribes a multi-subscriber.
 *
 * @param sub multi-subscriber handle
 */
void UnsubscribeMultiple(NT_MultiSubscriber sub);

/** @} */

/**
 * @defgroup ntcore_listener_func Listener Functions
 * @{
 */

using ListenerCallback = std::function<void(const Event&)>;

/**
 * Creates a listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using AddPolledListener()) will be stored in the queue and
 * must be collected by calling ReadListenerQueue().
 * The returned handle must be destroyed with DestroyListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_ListenerPoller CreateListenerPoller(NT_Inst inst);

/**
 * Destroys a listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void DestroyListenerPoller(NT_ListenerPoller poller);

/**
 * Read notifications.
 *
 * @param poller    poller handle
 * @return Array of events.  Returns empty array if no events since last call.
 */
std::vector<Event> ReadListenerQueue(NT_ListenerPoller poller);

/**
 * Removes a listener.
 *
 * @param listener Listener handle to remove
 */
void RemoveListener(NT_Listener listener);

/**
 * Wait for the listener queue to be empty. This is primarily useful
 * for deterministic testing. This blocks until either the listener
 * queue is empty (e.g. there are no more events that need to be passed along to
 * callbacks or poll queues) or the timeout expires.
 *
 * @param handle  handle
 * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or a
 *                negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
bool WaitForListenerQueue(NT_Handle handle, double timeout);

/**
 * Create a listener for changes to topics with names that start with any of
 * the given prefixes. This creates a corresponding internal subscriber with the
 * lifetime of the listener.
 *
 * @param inst Instance handle
 * @param prefixes Topic name string prefixes
 * @param mask Bitmask of NT_EventFlags values (only topic and value events will
 *             be generated)
 * @param callback Listener function
 */
NT_Listener AddListener(NT_Inst inst,
                        std::span<const std::string_view> prefixes,
                        unsigned int mask, ListenerCallback callback);

/**
 * Create a listener.
 *
 * Some combinations of handle and mask have no effect:
 * - connection and log message events are only generated on instances
 * - topic and value events are only generated on non-instances
 *
 * Adding value and topic events on a topic will create a corresponding internal
 * subscriber with the lifetime of the listener.
 *
 * Adding a log message listener through this function will only result in
 * events at NT_LOG_INFO or higher; for more customized settings, use
 * AddLogger().
 *
 * @param handle Instance, topic, subscriber, multi-subscriber, or entry handle
 * @param mask Bitmask of NT_EventFlags values
 * @param callback Listener function
 */
NT_Listener AddListener(NT_Handle handle, unsigned int mask,
                        ListenerCallback callback);

/**
 * Creates a polled listener. This creates a corresponding internal subscriber
 * with the lifetime of the listener.
 * The caller is responsible for calling ReadListenerQueue() to poll.
 *
 * @param poller poller handle
 * @param prefixes array of UTF-8 string prefixes
 * @param mask Bitmask of NT_EventFlags values (only topic and value events will
 *             be generated)
 * @return Listener handle
 */
NT_Listener AddPolledListener(NT_ListenerPoller poller,
                              std::span<const std::string_view> prefixes,
                              unsigned int mask);

/**
 * Creates a polled listener.
 * The caller is responsible for calling ReadListenerQueue() to poll.
 *
 * Some combinations of handle and mask have no effect:
 * - connection and log message events are only generated on instances
 * - topic and value events are only generated on non-instances
 *
 * Adding value and topic events on a topic will create a corresponding internal
 * subscriber with the lifetime of the listener.
 *
 * Adding a log message listener through this function will only result in
 * events at NT_LOG_INFO or higher; for more customized settings, use
 * AddPolledLogger().
 *
 * @param poller poller handle
 * @param handle instance, topic, subscriber, multi-subscriber, or entry handle
 * @param mask NT_EventFlags bitmask
 * @return Listener handle
 */
NT_Listener AddPolledListener(NT_ListenerPoller poller, NT_Handle handle,
                              unsigned int mask);

/** @} */

/**
 * @defgroup ntcore_network_func Client/Server Functions
 * @{
 */

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
 *                          address. (UTF-8 string)
 * @param port3             port to communicate over (NT3)
 * @param port4             port to communicate over (NT4)
 */
void StartServer(NT_Inst inst, std::string_view persist_filename,
                 std::string_view listen_address, unsigned int port3,
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
 * @param inst      instance handle
 * @param identity  network identity to advertise (cannot be empty string)
 */
void StartClient3(NT_Inst inst, std::string_view identity);

/**
 * Starts a NT4 client.  Use SetServer or SetServerTeam to set the server name
 * and port.
 *
 * @param inst      instance handle
 * @param identity  network identity to advertise (cannot be empty string)
 */
void StartClient4(NT_Inst inst, std::string_view identity);

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
void SetServer(NT_Inst inst, std::string_view server_name, unsigned int port);

/**
 * Sets server addresses for client (without restarting client).
 * The client will attempt to connect to each server in round robin fashion.
 *
 * @param inst      instance handle
 * @param servers   array of server name and port pairs
 */
void SetServer(
    NT_Inst inst,
    std::span<const std::pair<std::string_view, unsigned int>> servers);

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
 * Disconnects the client if it's running and connected. This will automatically
 * start reconnection attempts to the current server list.
 *
 * @param inst instance handle
 */
void Disconnect(NT_Inst inst);

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

/**
 * Get the time offset between server time and local time. Add this value to
 * local time to get the estimated equivalent server time. In server mode, this
 * always returns 0. In client mode, this returns the time offset only if the
 * client and server are connected and have exchanged synchronization messages.
 * Note the time offset may change over time as it is periodically updated; to
 * receive updates as events, add a listener to the "time sync" event.
 *
 * @param inst instance handle
 * @return Time offset in microseconds (optional)
 */
std::optional<int64_t> GetServerTimeOffset(NT_Inst inst);

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
 * @param min_level   minimum log level
 * @param max_level   maximum log level
 * @param func        listener callback function
 * @return Listener handle
 */
NT_Listener AddLogger(NT_Inst inst, unsigned int min_level,
                      unsigned int max_level, ListenerCallback func);

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
NT_Listener AddPolledLogger(NT_ListenerPoller poller, unsigned int min_level,
                            unsigned int max_level);

/** @} */

/**
 * @defgroup ntcore_schema_func Schema Functions
 * @{
 */

/**
 * Returns whether there is a data schema already registered with the given
 * name. This does NOT perform a check as to whether the schema has already
 * been published by another node on the network.
 *
 * @param inst instance
 * @param name Name (the string passed as the data type for topics using this
 *             schema)
 * @return True if schema already registered
 */
bool HasSchema(NT_Inst inst, std::string_view name);

/**
 * Registers a data schema.  Data schemas provide information for how a
 * certain data type string can be decoded.  The type string of a data schema
 * indicates the type of the schema itself (e.g. "protobuf" for protobuf
 * schemas, "struct" for struct schemas, etc). In NetworkTables, schemas are
 * published just like normal topics, with the name being generated from the
 * provided name: "/.schema/<name>".  Duplicate calls to this function with
 * the same name are silently ignored.
 *
 * @param inst instance
 * @param name Name (the string passed as the data type for topics using this
 *             schema)
 * @param type Type of schema (e.g. "protobuf", "struct", etc)
 * @param schema Schema data
 */
void AddSchema(NT_Inst inst, std::string_view name, std::string_view type,
               std::span<const uint8_t> schema);

/**
 * Registers a data schema.  Data schemas provide information for how a
 * certain data type string can be decoded.  The type string of a data schema
 * indicates the type of the schema itself (e.g. "protobuf" for protobuf
 * schemas, "struct" for struct schemas, etc). In NetworkTables, schemas are
 * published just like normal topics, with the name being generated from the
 * provided name: "/.schema/<name>".  Duplicate calls to this function with
 * the same name are silently ignored.
 *
 * @param inst instance
 * @param name Name (the string passed as the data type for topics using this
 *             schema)
 * @param type Type of schema (e.g. "protobuf", "struct", etc)
 * @param schema Schema data
 */
inline void AddSchema(NT_Inst inst, std::string_view name,
                      std::string_view type, std::string_view schema) {
  AddSchema(
      inst, name, type,
      std::span<const uint8_t>{reinterpret_cast<const uint8_t*>(schema.data()),
                               schema.size()});
}

/** @} */
/** @} */

/**
 * NetworkTables meta-topic decoding functions.
 */
namespace meta {

/**
 * @defgroup ntcore_cpp_meta_api ntcore C++ meta-topic API
 *
 * Meta-topic decoders for C++.
 *
 * @{
 */

/**
 * Subscriber options. Different from PubSubOptions in this reflects only
 * options that are sent over the network.
 */
struct SubscriberOptions {
  double periodic = 0.1;
  bool topicsOnly = false;
  bool sendAll = false;
  bool prefixMatch = false;
  // std::string otherStr;
};

/**
 * Topic publisher (as published via `$pub$<topic>`).
 */
struct TopicPublisher {
  std::string client;
  uint64_t pubuid = 0;
};

/**
 * Topic subscriber (as published via `$sub$<topic>`).
 */
struct TopicSubscriber {
  std::string client;
  uint64_t subuid = 0;
  SubscriberOptions options;
};

/**
 * Client publisher (as published via `$clientpub$<client>` or `$serverpub`).
 */
struct ClientPublisher {
  int64_t uid = -1;
  std::string topic;
};

/**
 * Client subscriber (as published via `$clientsub$<client>` or `$serversub`).
 */
struct ClientSubscriber {
  int64_t uid = -1;
  std::vector<std::string> topics;
  SubscriberOptions options;
};

/**
 * Client (as published via `$clients`).
 */
struct Client {
  std::string id;
  std::string conn;
  uint16_t version = 0;
};

/**
 * Decodes `$pub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @return Vector of TopicPublishers, or empty optional on decoding error.
 */
std::optional<std::vector<TopicPublisher>> DecodeTopicPublishers(
    std::span<const uint8_t> data);

/**
 * Decodes `$sub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @return Vector of TopicSubscribers, or empty optional on decoding error.
 */
std::optional<std::vector<TopicSubscriber>> DecodeTopicSubscribers(
    std::span<const uint8_t> data);

/**
 * Decodes `$clientpub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @return Vector of ClientPublishers, or empty optional on decoding error.
 */
std::optional<std::vector<ClientPublisher>> DecodeClientPublishers(
    std::span<const uint8_t> data);

/**
 * Decodes `$clientsub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @return Vector of ClientSubscribers, or empty optional on decoding error.
 */
std::optional<std::vector<ClientSubscriber>> DecodeClientSubscribers(
    std::span<const uint8_t> data);

/**
 * Decodes `$clients` meta-topic data.
 *
 * @param data data contents
 * @return Vector of Clients, or empty optional on decoding error.
 */
std::optional<std::vector<Client>> DecodeClients(std::span<const uint8_t> data);

/** @} */

}  // namespace meta
}  // namespace nt
