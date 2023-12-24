// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/protobuf/Protobuf.h>
#include <wpi/struct/Struct.h>

#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableEntry.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

class BooleanArrayTopic;
class BooleanTopic;
class DoubleArrayTopic;
class DoubleTopic;
class FloatArrayTopic;
class FloatTopic;
class IntegerArrayTopic;
class IntegerTopic;
class MultiSubscriber;
template <wpi::ProtobufSerializable T>
class ProtobufTopic;
class RawTopic;
class StringArrayTopic;
class StringTopic;
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructArrayTopic;
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructTopic;
class Subscriber;
class Topic;

/**
 * NetworkTables Instance.
 *
 * Instances are completely independent from each other.  Table operations on
 * one instance will not be visible to other instances unless the instances are
 * connected via the network.  The main limitation on instances is that you
 * cannot have two servers on the same network port.  The main utility of
 * instances is for unit testing, but they can also enable one program to
 * connect to two different NetworkTables networks.
 *
 * The global "default" instance (as returned by GetDefault()) is
 * always available, and is intended for the common case when there is only
 * a single NetworkTables instance being used in the program.  The
 * default instance cannot be destroyed.
 *
 * Additional instances can be created with the Create() function.
 * Instances are not reference counted or RAII.  Instead, they must be
 * explicitly destroyed (with Destroy()).
 *
 * @ingroup ntcore_cpp_api
 */
class NetworkTableInstance final {
 public:
  /**
   * Client/server mode flag values (as returned by GetNetworkMode()).
   * This is a bitmask.
   */
  enum NetworkMode {
    kNetModeNone = NT_NET_MODE_NONE,
    kNetModeServer = NT_NET_MODE_SERVER,
    kNetModeClient3 = NT_NET_MODE_CLIENT3,
    kNetModeClient4 = NT_NET_MODE_CLIENT4,
    kNetModeLocal = NT_NET_MODE_LOCAL
  };

  /**
   * Logging levels (as used by SetLogger()).
   */
  enum LogLevel {
    kLogCritical = NT_LOG_CRITICAL,
    kLogError = NT_LOG_ERROR,
    kLogWarning = NT_LOG_WARNING,
    kLogInfo = NT_LOG_INFO,
    kLogDebug = NT_LOG_DEBUG,
    kLogDebug1 = NT_LOG_DEBUG1,
    kLogDebug2 = NT_LOG_DEBUG2,
    kLogDebug3 = NT_LOG_DEBUG3,
    kLogDebug4 = NT_LOG_DEBUG4
  };

  /**
   * The default port that network tables operates on for NT3.
   */
  static constexpr unsigned int kDefaultPort3 = NT_DEFAULT_PORT3;

  /**
   * The default port that network tables operates on for NT4.
   */
  static constexpr unsigned int kDefaultPort4 = NT_DEFAULT_PORT4;

  /**
   * Construct invalid instance.
   */
  NetworkTableInstance() noexcept;

  /**
   * Construct from native handle.
   *
   * @param inst Native handle
   */
  explicit NetworkTableInstance(NT_Inst inst) noexcept;

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_handle != 0; }

  /**
   * Get global default instance.
   *
   * @return Global default instance
   */
  static NetworkTableInstance GetDefault();

  /**
   * Create an instance.
   *
   * @return Newly created instance
   */
  static NetworkTableInstance Create();

  /**
   * Destroys an instance (note: this has global effect).
   *
   * @param inst Instance
   */
  static void Destroy(NetworkTableInstance& inst);

  /**
   * Gets the native handle for the entry.
   *
   * @return Native handle
   */
  NT_Inst GetHandle() const;

  /**
   * Gets a "generic" (untyped) topic.
   *
   * @param name topic name
   * @return Topic
   */
  Topic GetTopic(std::string_view name) const;

  /**
   * Gets a boolean topic.
   *
   * @param name topic name
   * @return Topic
   */
  BooleanTopic GetBooleanTopic(std::string_view name) const;

  /**
   * Gets an integer topic.
   *
   * @param name topic name
   * @return Topic
   */
  IntegerTopic GetIntegerTopic(std::string_view name) const;

  /**
   * Gets a float topic.
   *
   * @param name topic name
   * @return Topic
   */
  FloatTopic GetFloatTopic(std::string_view name) const;

  /**
   * Gets a double topic.
   *
   * @param name topic name
   * @return Topic
   */
  DoubleTopic GetDoubleTopic(std::string_view name) const;

  /**
   * Gets a string topic.
   *
   * @param name topic name
   * @return Topic
   */
  StringTopic GetStringTopic(std::string_view name) const;

  /**
   * Gets a raw topic.
   *
   * @param name topic name
   * @return Topic
   */
  RawTopic GetRawTopic(std::string_view name) const;

  /**
   * Gets a boolean array topic.
   *
   * @param name topic name
   * @return Topic
   */
  BooleanArrayTopic GetBooleanArrayTopic(std::string_view name) const;

  /**
   * Gets an integer array topic.
   *
   * @param name topic name
   * @return Topic
   */
  IntegerArrayTopic GetIntegerArrayTopic(std::string_view name) const;

  /**
   * Gets a float array topic.
   *
   * @param name topic name
   * @return Topic
   */
  FloatArrayTopic GetFloatArrayTopic(std::string_view name) const;

  /**
   * Gets a double array topic.
   *
   * @param name topic name
   * @return Topic
   */
  DoubleArrayTopic GetDoubleArrayTopic(std::string_view name) const;

  /**
   * Gets a string array topic.
   *
   * @param name topic name
   * @return Topic
   */
  StringArrayTopic GetStringArrayTopic(std::string_view name) const;

  /**
   * Gets a protobuf serialized value topic.
   *
   * @param name topic name
   * @return Topic
   */
  template <wpi::ProtobufSerializable T>
  ProtobufTopic<T> GetProtobufTopic(std::string_view name) const;

  /**
   * Gets a raw struct serialized value topic.
   *
   * @param name topic name
   * @param info optional struct type info
   * @return Topic
   */
  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  StructTopic<T, I...> GetStructTopic(std::string_view name, I... info) const;

  /**
   * Gets a raw struct serialized array topic.
   *
   * @param name topic name
   * @param info optional struct type info
   * @return Topic
   */
  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  StructArrayTopic<T, I...> GetStructArrayTopic(std::string_view name,
                                                I... info) const;

  /**
   * Get Published Topics.
   *
   * Returns an array of topics.
   *
   * @return Array of topics.
   */
  std::vector<Topic> GetTopics();

  /**
   * Get Published Topics.
   *
   * Returns an array of topics.  The results are filtered by
   * string prefix to only return a subset of all topics.
   *
   * @param prefix  name required prefix; only topics whose name
   *                starts with this string are returned
   * @return Array of topics.
   */
  std::vector<Topic> GetTopics(std::string_view prefix);

  /**
   * Get Published Topics.
   *
   * Returns an array of topics.  The results are filtered by
   * string prefix and type to only return a subset of all topics.
   *
   * @param prefix  name required prefix; only topics whose name
   *                starts with this string are returned
   * @param types   bitmask of NT_Type values; 0 is treated specially
   *                as a "don't care"
   * @return Array of topics.
   */
  std::vector<Topic> GetTopics(std::string_view prefix, unsigned int types);

  /**
   * Get Published Topics.
   *
   * Returns an array of topics.  The results are filtered by
   * string prefix and type to only return a subset of all topics.
   *
   * @param prefix  name required prefix; only topics whose name
   *                starts with this string are returned
   * @param types   array of type strings
   * @return Array of topic handles.
   */
  std::vector<Topic> GetTopics(std::string_view prefix,
                               std::span<std::string_view> types);

  /**
   * Get Topic Information about multiple topics.
   *
   * Returns an array of topic information (handle, name, type, and properties).
   *
   * @return Array of topic information.
   */
  std::vector<TopicInfo> GetTopicInfo();

  /**
   * Get Topic Information about multiple topics.
   *
   * Returns an array of topic information (handle, name, type, and properties).
   * The results are filtered by string prefix to only
   * return a subset of all topics.
   *
   * @param prefix  name required prefix; only topics whose name
   *                starts with this string are returned
   * @return Array of topic information.
   */
  std::vector<TopicInfo> GetTopicInfo(std::string_view prefix);

  /**
   * Get Topic Information about multiple topics.
   *
   * Returns an array of topic information (handle, name, type, and properties).
   * The results are filtered by string prefix and type to only
   * return a subset of all topics.
   *
   * @param prefix  name required prefix; only topics whose name
   *                starts with this string are returned
   * @param types   bitmask of NT_Type values; 0 is treated specially
   *                as a "don't care"
   * @return Array of topic information.
   */
  std::vector<TopicInfo> GetTopicInfo(std::string_view prefix,
                                      unsigned int types);

  /**
   * Get Topic Information about multiple topics.
   *
   * Returns an array of topic information (handle, name, type, and properties).
   * The results are filtered by string prefix and type to only
   * return a subset of all topics.
   *
   * @param prefix  name required prefix; only topics whose name
   *                starts with this string are returned
   * @param types   array of type strings
   * @return Array of topic information.
   */
  std::vector<TopicInfo> GetTopicInfo(std::string_view prefix,
                                      std::span<std::string_view> types);

  /**
   * Gets the entry for a key.
   *
   * @param name Key
   * @return Network table entry.
   */
  NetworkTableEntry GetEntry(std::string_view name);

  /**
   * Gets the table with the specified key.
   *
   * @param key the key name
   * @return The network table
   */
  std::shared_ptr<NetworkTable> GetTable(std::string_view key) const;

  /**
   * @{
   * @name Listener Functions
   */

  /**
   * Remove a listener.
   *
   * @param listener Listener handle to remove
   */
  static void RemoveListener(NT_Listener listener);

  /**
   * Wait for the listener queue to be empty. This is primarily
   * useful for deterministic testing. This blocks until either the
   * listener queue is empty (e.g. there are no more events that need to be
   * passed along to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or
   *                a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForListenerQueue(double timeout);

  /**
   * Add a connection listener. The callback function is called asynchronously
   * on a separate thread, so it's important to use synchronization or atomics
   * when accessing any shared state from the callback function.
   *
   * @param immediate_notify  notify listener of all existing connections
   * @param callback          listener to add
   * @return Listener handle
   */
  NT_Listener AddConnectionListener(bool immediate_notify,
                                    ListenerCallback callback) const;

  /**
   * Add a time synchronization listener. The callback function is called
   * asynchronously on a separate thread, so it's important to use
   * synchronization or atomics when accessing any shared state from the
   * callback function.
   *
   * @param immediate_notify  notify listener of current time synchronization
   *                          value
   * @param callback          listener to add
   * @return Listener handle
   */
  NT_Listener AddTimeSyncListener(bool immediate_notify,
                                  ListenerCallback callback) const;

  /**
   * Add a listener for changes on a particular topic. The callback
   * function is called asynchronously on a separate thread, so it's important
   * to use synchronization or atomics when accessing any shared state from the
   * callback function.
   *
   * This creates a corresponding internal subscriber with the lifetime of the
   * listener.
   *
   * @param topic Topic
   * @param eventMask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener handle
   */
  NT_Listener AddListener(Topic topic, unsigned int eventMask,
                          ListenerCallback listener);

  /**
   * Add a listener for changes on a subscriber. The callback
   * function is called asynchronously on a separate thread, so it's important
   * to use synchronization or atomics when accessing any shared state from the
   * callback function. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventMask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener handle
   */
  NT_Listener AddListener(Subscriber& subscriber, unsigned int eventMask,
                          ListenerCallback listener);

  /**
   * Add a listener for changes on a subscriber. The callback
   * function is called asynchronously on a separate thread, so it's important
   * to use synchronization or atomics when accessing any shared state from the
   * callback function. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventMask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener handle
   */
  NT_Listener AddListener(MultiSubscriber& subscriber, int eventMask,
                          ListenerCallback listener);

  /**
   * Add a listener for changes on an entry. The callback function
   * is called asynchronously on a separate thread, so it's important to use
   * synchronization or atomics when accessing any shared state from the
   * callback function.
   *
   * @param entry Entry
   * @param eventMask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener handle
   */
  NT_Listener AddListener(const NetworkTableEntry& entry, int eventMask,
                          ListenerCallback listener);

  /**
   * Add a listener for changes to topics with names that start with any
   * of the given prefixes. The callback function is called asynchronously on a
   * separate thread, so it's important to use synchronization or atomics when
   * accessing any shared state from the callback function.
   *
   * This creates a corresponding internal subscriber with the lifetime of the
   * listener.
   *
   * @param prefixes Topic name string prefixes
   * @param eventMask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener handle
   */
  NT_Listener AddListener(std::span<const std::string_view> prefixes,
                          int eventMask, ListenerCallback listener);

  /** @} */

  /**
   * @{
   * @name Client/Server Functions
   */

  /**
   * Get the current network mode.
   *
   * @return Bitmask of NetworkMode.
   */
  unsigned int GetNetworkMode() const;

  /**
   * Starts local-only operation.  Prevents calls to StartServer or StartClient
   * from taking effect.  Has no effect if StartServer or StartClient
   * has already been called.
   */
  void StartLocal();

  /**
   * Stops local-only operation.  StartServer or StartClient can be called after
   * this call to start a server or client.
   */
  void StopLocal();

  /**
   * Starts a server using the specified filename, listening address, and port.
   *
   * @param persist_filename  the name of the persist file to use (UTF-8 string,
   *                          null terminated)
   * @param listen_address    the address to listen on, or null to listen on any
   *                          address (UTF-8 string, null terminated)
   * @param port3             port to communicate over (NT3)
   * @param port4             port to communicate over (NT4)
   */
  void StartServer(std::string_view persist_filename = "networktables.json",
                   const char* listen_address = "",
                   unsigned int port3 = kDefaultPort3,
                   unsigned int port4 = kDefaultPort4);

  /**
   * Stops the server if it is running.
   */
  void StopServer();

  /**
   * Starts a NT3 client.  Use SetServer or SetServerTeam to set the server name
   * and port.
   *
   * @param identity  network identity to advertise (cannot be empty string)
   */
  void StartClient3(std::string_view identity);

  /**
   * Starts a NT4 client.  Use SetServer or SetServerTeam to set the server name
   * and port.
   *
   * @param identity  network identity to advertise (cannot be empty string)
   */
  void StartClient4(std::string_view identity);

  /**
   * Stops the client if it is running.
   */
  void StopClient();

  /**
   * Sets server address and port for client (without restarting client).
   *
   * @param server_name server name (UTF-8 string, null terminated)
   * @param port        port to communicate over (0 = default)
   */
  void SetServer(const char* server_name, unsigned int port = 0);

  /**
   * Sets server addresses and ports for client (without restarting client).
   * The client will attempt to connect to each server in round robin fashion.
   *
   * @param servers   array of server address and port pairs
   */
  void SetServer(
      std::span<const std::pair<std::string_view, unsigned int>> servers);

  /**
   * Sets server addresses and port for client (without restarting client).
   * The client will attempt to connect to each server in round robin fashion.
   *
   * @param servers   array of server names
   * @param port      port to communicate over (0 = default)
   */
  void SetServer(std::span<const std::string_view> servers,
                 unsigned int port = 0);

  /**
   * Sets server addresses and port for client (without restarting client).
   * Connects using commonly known robot addresses for the specified team.
   *
   * @param team        team number
   * @param port        port to communicate over (0 = default)
   */
  void SetServerTeam(unsigned int team, unsigned int port = 0);

  /**
   * Disconnects the client if it's running and connected. This will
   * automatically start reconnection attempts to the current server list.
   */
  void Disconnect();

  /**
   * Starts requesting server address from Driver Station.
   * This connects to the Driver Station running on localhost to obtain the
   * server IP address.
   *
   * @param port server port to use in combination with IP from DS (0 = default)
   */
  void StartDSClient(unsigned int port = 0);

  /**
   * Stops requesting server address from Driver Station.
   */
  void StopDSClient();

  /**
   * Flushes all updated values immediately to the local client/server. This
   * does not flush to the network.
   */
  void FlushLocal() const;

  /**
   * Flushes all updated values immediately to the network.
   * @note This is rate-limited to protect the network from flooding.
   * This is primarily useful for synchronizing network updates with
   * user code.
   */
  void Flush() const;

  /**
   * Get information on the currently established network connections.
   * If operating as a client, this will return either zero or one values.
   *
   * @return array of connection information
   */
  std::vector<ConnectionInfo> GetConnections() const;

  /**
   * Return whether or not the instance is connected to another node.
   *
   * @return True if connected.
   */
  bool IsConnected() const;

  /**
   * Get the time offset between server time and local time. Add this value to
   * local time to get the estimated equivalent server time. In server mode,
   * this always returns 0. In client mode, this returns the time offset only if
   * the client and server are connected and have exchanged synchronization
   * messages. Note the time offset may change over time as it is periodically
   * updated; to receive updates as events, add a listener to the "time sync"
   * event.
   *
   * @return Time offset in microseconds (optional)
   */
  std::optional<int64_t> GetServerTimeOffset() const;

  /** @} */

  /**
   * @{
   * @name Data Logger Functions
   */

  /**
   * Starts logging entry changes to a DataLog.
   *
   * @param log data log object; lifetime must extend until StopEntryDataLog is
   *            called or the instance is destroyed
   * @param prefix only store entries with names that start with this prefix;
   *               the prefix is not included in the data log entry name
   * @param logPrefix prefix to add to data log entry names
   * @return Data logger handle
   */
  NT_DataLogger StartEntryDataLog(wpi::log::DataLog& log,
                                  std::string_view prefix,
                                  std::string_view logPrefix);

  /**
   * Stops logging entry changes to a DataLog.
   *
   * @param logger data logger handle
   */
  static void StopEntryDataLog(NT_DataLogger logger);

  /**
   * Starts logging connection changes to a DataLog.
   *
   * @param log data log object; lifetime must extend until
   *            StopConnectionDataLog is called or the instance is destroyed
   * @param name data log entry name
   * @return Data logger handle
   */
  NT_ConnectionDataLogger StartConnectionDataLog(wpi::log::DataLog& log,
                                                 std::string_view name);

  /**
   * Stops logging connection changes to a DataLog.
   *
   * @param logger data logger handle
   */
  static void StopConnectionDataLog(NT_ConnectionDataLogger logger);

  /** @} */

  /**
   * @{
   * @name Logger Functions
   */

  /**
   * Add logger callback function.  By default, log messages are sent to stderr;
   * this function sends log messages with the specified levels to the provided
   * callback function instead.  The callback function will only be called for
   * log messages with level greater than or equal to minLevel and less than or
   * equal to maxLevel; messages outside this range will be silently ignored.
   *
   * @param minLevel    minimum log level
   * @param maxLevel    maximum log level
   * @param func        callback function
   * @return Listener handle
   */
  NT_Listener AddLogger(unsigned int minLevel, unsigned int maxLevel,
                        ListenerCallback func);

  /** @} */

  /**
   * @{
   * @name Schema Functions
   */

  /**
   * Returns whether there is a data schema already registered with the given
   * name. This does NOT perform a check as to whether the schema has already
   * been published by another node on the network.
   *
   * @param name Name (the string passed as the data type for topics using this
   *             schema)
   * @return True if schema already registered
   */
  bool HasSchema(std::string_view name) const;

  /**
   * Registers a data schema.  Data schemas provide information for how a
   * certain data type string can be decoded.  The type string of a data schema
   * indicates the type of the schema itself (e.g. "protobuf" for protobuf
   * schemas, "struct" for struct schemas, etc). In NetworkTables, schemas are
   * published just like normal topics, with the name being generated from the
   * provided name: "/.schema/<name>".  Duplicate calls to this function with
   * the same name are silently ignored.
   *
   * @param name Name (the string passed as the data type for topics using this
   *             schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  void AddSchema(std::string_view name, std::string_view type,
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
   * @param name Name (the string passed as the data type for topics using this
   *             schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  void AddSchema(std::string_view name, std::string_view type,
                 std::string_view schema);

  /**
   * Registers a protobuf schema. Duplicate calls to this function with the same
   * name are silently ignored.
   *
   * @tparam T protobuf serializable type
   * @param msg protobuf message
   */
  template <wpi::ProtobufSerializable T>
  void AddProtobufSchema(wpi::ProtobufMessage<T>& msg);

  /**
   * Registers a struct schema. Duplicate calls to this function with the same
   * name are silently ignored.
   *
   * @tparam T struct serializable type
   * @param info optional struct type info
   */
  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  void AddStructSchema(const I&... info);

  /**
   * Equality operator.  Returns true if both instances refer to the same
   * native handle.
   */
  bool operator==(const NetworkTableInstance&) const = default;

 private:
  /* Native handle */
  NT_Inst m_handle{0};
};

}  // namespace nt

#include "networktables/NetworkTableInstance.inc"
