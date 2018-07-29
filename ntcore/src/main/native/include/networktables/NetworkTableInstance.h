/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLEINSTANCE_H_
#define NTCORE_NETWORKTABLES_NETWORKTABLEINSTANCE_H_

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableEntry.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

using wpi::ArrayRef;
using wpi::StringRef;
using wpi::Twine;

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
    kNetModeClient = NT_NET_MODE_CLIENT,
    kNetModeStarting = NT_NET_MODE_STARTING,
    kNetModeFailure = NT_NET_MODE_FAILURE
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
   * The default port that network tables operates on.
   */
  enum { kDefaultPort = NT_DEFAULT_PORT };

  /**
   * Construct invalid instance.
   */
  NetworkTableInstance() noexcept;

  /**
   * Construct from native handle.
   *
   * @param handle Native handle
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
  static void Destroy(NetworkTableInstance inst);

  /**
   * Gets the native handle for the entry.
   *
   * @return Native handle
   */
  NT_Inst GetHandle() const;

  /**
   * Gets the entry for a key.
   *
   * @param name Key
   * @return Network table entry.
   */
  NetworkTableEntry GetEntry(const Twine& name);

  /**
   * Get entries starting with the given prefix.
   *
   * The results are optionally filtered by string prefix and entry type to
   * only return a subset of all entries.
   *
   * @param prefix entry name required prefix; only entries whose name
   * starts with this string are returned
   * @param types bitmask of types; 0 is treated as a "don't care"
   * @return Array of entries.
   */
  std::vector<NetworkTableEntry> GetEntries(const Twine& prefix,
                                            unsigned int types);

  /**
   * Get information about entries starting with the given prefix.
   *
   * The results are optionally filtered by string prefix and entry type to
   * only return a subset of all entries.
   *
   * @param prefix entry name required prefix; only entries whose name
   * starts with this string are returned
   * @param types bitmask of types; 0 is treated as a "don't care"
   * @return Array of entry information.
   */
  std::vector<EntryInfo> GetEntryInfo(const Twine& prefix,
                                      unsigned int types) const;

  /**
   * Gets the table with the specified key.
   *
   * @param key the key name
   * @return The network table
   */
  std::shared_ptr<NetworkTable> GetTable(const Twine& key) const;

  /**
   * Deletes ALL keys in ALL subtables (except persistent values).
   * Use with caution!
   */
  void DeleteAllEntries();

  /**
   * @{
   * @name Entry Listener Functions
   */

  /**
   * Add a listener for all entries starting with a certain prefix.
   *
   * @param prefix            UTF-8 string prefix
   * @param callback          listener to add
   * @param flags             EntryListenerFlags bitmask
   * @return Listener handle
   */
  NT_EntryListener AddEntryListener(
      const Twine& prefix,
      std::function<void(const EntryNotification& event)> callback,
      unsigned int flags) const;

  /**
   * Remove an entry listener.
   *
   * @param entry_listener Listener handle to remove
   */
  static void RemoveEntryListener(NT_EntryListener entry_listener);

  /**
   * Wait for the entry listener queue to be empty.  This is primarily useful
   * for deterministic testing.  This blocks until either the entry listener
   * queue is empty (e.g. there are no more events that need to be passed along
   * to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForEntryListenerQueue(double timeout);

  /** @} */

  /**
   * @{
   * @name Connection Listener Functions
   */

  /**
   * Add a connection listener.
   *
   * @param callback          listener to add
   * @param immediate_notify  notify listener of all existing connections
   * @return Listener handle
   */
  NT_ConnectionListener AddConnectionListener(
      std::function<void(const ConnectionNotification& event)> callback,
      bool immediate_notify) const;

  /**
   * Remove a connection listener.
   *
   * @param conn_listener Listener handle to remove
   */
  static void RemoveConnectionListener(NT_ConnectionListener conn_listener);

  /**
   * Wait for the connection listener queue to be empty.  This is primarily
   * useful for deterministic testing.  This blocks until either the connection
   * listener queue is empty (e.g. there are no more events that need to be
   * passed along to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForConnectionListenerQueue(double timeout);

  /** @} */

  /**
   * @{
   * @name Remote Procedure Call Functions
   */

  /**
   * Wait for the incoming RPC call queue to be empty.  This is primarily useful
   * for deterministic testing.  This blocks until either the RPC call
   * queue is empty (e.g. there are no more events that need to be passed along
   * to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForRpcCallQueue(double timeout);

  /** @} */

  /**
   * @{
   * @name Client/Server Functions
   */

  /**
   * Set the network identity of this node.
   *
   * This is the name used during the initial connection handshake, and is
   * visible through ConnectionInfo on the remote node.
   *
   * @param name      identity to advertise
   */
  void SetNetworkIdentity(const Twine& name);

  /**
   * Get the current network mode.
   *
   * @return Bitmask of NetworkMode.
   */
  unsigned int GetNetworkMode() const;

  /**
   * Starts a server using the specified filename, listening address, and port.
   *
   * @param persist_filename  the name of the persist file to use (UTF-8 string,
   *                          null terminated)
   * @param listen_address    the address to listen on, or null to listen on any
   *                          address (UTF-8 string, null terminated)
   * @param port              port to communicate over
   */
  void StartServer(const Twine& persist_filename = "networktables.ini",
                   const char* listen_address = "",
                   unsigned int port = kDefaultPort);

  /**
   * Stops the server if it is running.
   */
  void StopServer();

  /**
   * Starts a client.  Use SetServer to set the server name and port.
   */
  void StartClient();

  /**
   * Starts a client using the specified server and port
   *
   * @param server_name server name (UTF-8 string, null terminated)
   * @param port        port to communicate over
   */
  void StartClient(const char* server_name, unsigned int port = kDefaultPort);

  /**
   * Starts a client using the specified (server, port) combinations.  The
   * client will attempt to connect to each server in round robin fashion.
   *
   * @param servers   array of server name and port pairs
   */
  void StartClient(ArrayRef<std::pair<StringRef, unsigned int>> servers);

  /**
   * Starts a client using the specified servers and port.  The
   * client will attempt to connect to each server in round robin fashion.
   *
   * @param servers   array of server names
   * @param port      port to communicate over
   */
  void StartClient(ArrayRef<StringRef> servers,
                   unsigned int port = kDefaultPort);

  /**
   * Starts a client using commonly known robot addresses for the specified
   * team.
   *
   * @param team        team number
   * @param port        port to communicate over
   */
  void StartClientTeam(unsigned int team, unsigned int port = kDefaultPort);

  /**
   * Stops the client if it is running.
   */
  void StopClient();

  /**
   * Sets server address and port for client (without restarting client).
   *
   * @param server_name server name (UTF-8 string, null terminated)
   * @param port        port to communicate over
   */
  void SetServer(const char* server_name, unsigned int port = kDefaultPort);

  /**
   * Sets server addresses and ports for client (without restarting client).
   * The client will attempt to connect to each server in round robin fashion.
   *
   * @param servers   array of server name and port pairs
   */
  void SetServer(ArrayRef<std::pair<StringRef, unsigned int>> servers);

  /**
   * Sets server addresses and port for client (without restarting client).
   * The client will attempt to connect to each server in round robin fashion.
   *
   * @param servers   array of server names
   * @param port      port to communicate over
   */
  void SetServer(ArrayRef<StringRef> servers, unsigned int port = kDefaultPort);

  /**
   * Sets server addresses and port for client (without restarting client).
   * Connects using commonly known robot addresses for the specified team.
   *
   * @param team        team number
   * @param port        port to communicate over
   */
  void SetServerTeam(unsigned int team, unsigned int port = kDefaultPort);

  /**
   * Starts requesting server address from Driver Station.
   * This connects to the Driver Station running on localhost to obtain the
   * server IP address.
   *
   * @param port server port to use in combination with IP from DS
   */
  void StartDSClient(unsigned int port = kDefaultPort);

  /**
   * Stops requesting server address from Driver Station.
   */
  void StopDSClient();

  /**
   * Set the periodic update rate.
   * Sets how frequently updates are sent to other nodes over the network.
   *
   * @param interval update interval in seconds (range 0.01 to 1.0)
   */
  void SetUpdateRate(double interval);

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

  /** @} */

  /**
   * @{
   * @name File Save/Load Functions
   */

  /**
   * Save persistent values to a file.  The server automatically does this,
   * but this function provides a way to save persistent values in the same
   * format to a file on either a client or a server.
   *
   * @param filename  filename
   * @return error string, or nullptr if successful
   */
  const char* SavePersistent(const Twine& filename) const;

  /**
   * Load persistent values from a file.  The server automatically does this
   * at startup, but this function provides a way to restore persistent values
   * in the same format from a file at any time on either a client or a server.
   *
   * @param filename  filename
   * @param warn      callback function for warnings
   * @return error string, or nullptr if successful
   */
  const char* LoadPersistent(
      const Twine& filename,
      std::function<void(size_t line, const char* msg)> warn);

  /**
   * Save table values to a file.  The file format used is identical to
   * that used for SavePersistent.
   *
   * @param filename  filename
   * @param prefix    save only keys starting with this prefix
   * @return error string, or nullptr if successful
   */
  const char* SaveEntries(const Twine& filename, const Twine& prefix) const;

  /**
   * Load table values from a file.  The file format used is identical to
   * that used for SavePersistent / LoadPersistent.
   *
   * @param filename  filename
   * @param prefix    load only keys starting with this prefix
   * @param warn      callback function for warnings
   * @return error string, or nullptr if successful
   */
  const char* LoadEntries(
      const Twine& filename, const Twine& prefix,
      std::function<void(size_t line, const char* msg)> warn);

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
   * @param func        log callback function
   * @param minLevel    minimum log level
   * @param maxLevel    maximum log level
   * @return Logger handle
   */
  NT_Logger AddLogger(std::function<void(const LogMessage& msg)> func,
                      unsigned int min_level, unsigned int max_level);

  /**
   * Remove a logger.
   *
   * @param logger Logger handle to remove
   */
  static void RemoveLogger(NT_Logger logger);

  /**
   * Wait for the incoming log event queue to be empty.  This is primarily
   * useful for deterministic testing.  This blocks until either the log event
   * queue is empty (e.g. there are no more events that need to be passed along
   * to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForLoggerQueue(double timeout);

  /** @} */

  /**
   * Equality operator.  Returns true if both instances refer to the same
   * native handle.
   */
  bool operator==(const NetworkTableInstance& other) const {
    return m_handle == other.m_handle;
  }

  /** Inequality operator. */
  bool operator!=(const NetworkTableInstance& other) const {
    return !(*this == other);
  }

 private:
  /* Native handle */
  NT_Inst m_handle;
};

}  // namespace nt

#include "networktables/NetworkTableInstance.inl"

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLEINSTANCE_H_
