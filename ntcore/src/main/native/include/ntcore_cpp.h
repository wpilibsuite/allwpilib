/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_NTCORE_CPP_H_
#define NTCORE_NTCORE_CPP_H_

#include <stdint.h>

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpi/deprecated.h>

#include "networktables/NetworkTableValue.h"

/** NetworkTables (ntcore) namespace */
namespace nt {

/**
 * @defgroup ntcore_cpp_handle_api ntcore C++ API
 *
 * Handle-based interface for C++.
 *
 * @{
 */

using wpi::ArrayRef;
using wpi::StringRef;
using wpi::Twine;

/** NetworkTables Entry Information */
struct EntryInfo {
  /** Entry handle */
  NT_Entry entry;

  /** Entry name */
  std::string name;

  /** Entry type */
  NT_Type type;

  /** Entry flags */
  unsigned int flags;

  /** Timestamp of last change to entry (type or value). */
  uint64_t last_change;

  friend void swap(EntryInfo& first, EntryInfo& second) {
    using std::swap;
    swap(first.entry, second.entry);
    swap(first.name, second.name);
    swap(first.type, second.type);
    swap(first.flags, second.flags);
    swap(first.last_change, second.last_change);
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
  uint64_t last_update{0};

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

/** NetworkTables RPC Version 1 Definition Parameter */
struct RpcParamDef {
  RpcParamDef() = default;
  RpcParamDef(StringRef name_, std::shared_ptr<Value> def_value_)
      : name(name_), def_value(def_value_) {}

  std::string name;
  std::shared_ptr<Value> def_value;
};

/** NetworkTables RPC Version 1 Definition Result */
struct RpcResultDef {
  RpcResultDef() = default;
  RpcResultDef(StringRef name_, NT_Type type_) : name(name_), type(type_) {}

  std::string name;
  NT_Type type;
};

/** NetworkTables RPC Version 1 Definition */
struct RpcDefinition {
  unsigned int version;
  std::string name;
  std::vector<RpcParamDef> params;
  std::vector<RpcResultDef> results;
};

/** NetworkTables Remote Procedure Call (Server Side) */
class RpcAnswer {
 public:
  RpcAnswer() : entry(0), call(0) {}
  RpcAnswer(NT_Entry entry_, NT_RpcCall call_, StringRef name_,
            StringRef params_, const ConnectionInfo& conn_)
      : entry(entry_), call(call_), name(name_), params(params_), conn(conn_) {}

  /** Entry handle. */
  NT_Entry entry;

  /** Call handle. */
  mutable NT_RpcCall call;

  /** Entry name. */
  std::string name;

  /** Call raw parameters. */
  std::string params;

  /** Connection that called the RPC. */
  ConnectionInfo conn;

  /**
   * Determines if the native handle is valid.
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return call != 0; }

  /**
   * Post RPC response (return value) for a polled RPC.
   * @param result  result raw data that will be provided to remote caller
   * @return True if posting the response is valid, otherwise false
   */
  bool PostResponse(StringRef result) const;

  friend void swap(RpcAnswer& first, RpcAnswer& second) {
    using std::swap;
    swap(first.entry, second.entry);
    swap(first.call, second.call);
    swap(first.name, second.name);
    swap(first.params, second.params);
    swap(first.conn, second.conn);
  }
};

/** NetworkTables Entry Notification */
class EntryNotification {
 public:
  EntryNotification() : listener(0), entry(0), flags(0) {}
  EntryNotification(NT_EntryListener listener_, NT_Entry entry_,
                    StringRef name_, std::shared_ptr<Value> value_,
                    unsigned int flags_)
      : listener(listener_),
        entry(entry_),
        name(name_),
        value(value_),
        flags(flags_) {}

  /** Listener that was triggered. */
  NT_EntryListener listener;

  /** Entry handle. */
  NT_Entry entry;

  /** Entry name. */
  std::string name;

  /** The new value. */
  std::shared_ptr<Value> value;

  /**
   * Update flags.  For example, NT_NOTIFY_NEW if the key did not previously
   * exist.
   */
  unsigned int flags;

  friend void swap(EntryNotification& first, EntryNotification& second) {
    using std::swap;
    swap(first.listener, second.listener);
    swap(first.entry, second.entry);
    swap(first.name, second.name);
    swap(first.value, second.value);
    swap(first.flags, second.flags);
  }
};

/** NetworkTables Connection Notification */
class ConnectionNotification {
 public:
  ConnectionNotification() : listener(0), connected(false) {}
  ConnectionNotification(NT_ConnectionListener listener_, bool connected_,
                         const ConnectionInfo& conn_)
      : listener(listener_), connected(connected_), conn(conn_) {}

  /** Listener that was triggered. */
  NT_ConnectionListener listener;

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
  LogMessage() : logger(0), level(0), filename(""), line(0) {}
  LogMessage(NT_Logger logger_, unsigned int level_, const char* filename_,
             unsigned int line_, StringRef message_)
      : logger(logger_),
        level(level_),
        filename(filename_),
        line(line_),
        message(message_) {}

  /** The logger that generated the message. */
  NT_Logger logger;

  /** Log level of the message.  See NT_LogLevel. */
  unsigned int level;

  /** The filename of the source file that generated the message. */
  const char* filename;

  /** The line number in the source file that generated the message. */
  unsigned int line;

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
NT_Entry GetEntry(NT_Inst inst, const Twine& name);

/**
 * Get Entry Handles.
 *
 * Returns an array of entry handles.  The results are optionally
 * filtered by string prefix and entry type to only return a subset of all
 * entries.
 *
 * @param inst          instance handle
 * @param prefix        entry name required prefix; only entries whose name
 *                      starts with this string are returned
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @return Array of entry handles.
 */
std::vector<NT_Entry> GetEntries(NT_Inst inst, const Twine& prefix,
                                 unsigned int types);

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
 * @param entry   entry handle
 * @return Entry last change time
 */
uint64_t GetEntryLastChange(NT_Entry entry);

/**
 * Get Entry Value.
 *
 * Returns copy of current entry value.
 * Note that one of the type options is "unassigned".
 *
 * @param name      entry name (UTF-8 string)
 * @return entry value
 */
WPI_DEPRECATED("use NT_Entry function instead")
std::shared_ptr<Value> GetEntryValue(StringRef name);

/**
 * Get Entry Value.
 *
 * Returns copy of current entry value.
 * Note that one of the type options is "unassigned".
 *
 * @param entry     entry handle
 * @return entry value
 */
std::shared_ptr<Value> GetEntryValue(NT_Entry entry);

/**
 * Set Default Entry Value
 *
 * Returns copy of current entry value if it exists.
 * Otherwise, sets passed in value, and returns set value.
 * Note that one of the type options is "unassigned".
 *
 * @param name      entry name (UTF-8 string)
 * @param value     value to be set if name does not exist
 * @return False on error (value not set), True on success
 */
WPI_DEPRECATED("use NT_Entry function instead")
bool SetDefaultEntryValue(StringRef name, std::shared_ptr<Value> value);

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
bool SetDefaultEntryValue(NT_Entry entry, std::shared_ptr<Value> value);

/**
 * Set Entry Value.
 *
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, returns error and does not update value.
 *
 * @param name      entry name (UTF-8 string)
 * @param value     new entry value
 * @return False on error (type mismatch), True on success
 */
WPI_DEPRECATED("use NT_Entry function instead")
bool SetEntryValue(StringRef name, std::shared_ptr<Value> value);

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
bool SetEntryValue(NT_Entry entry, std::shared_ptr<Value> value);

/**
 * Set Entry Type and Value.
 *
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, the currently stored entry type is overridden
 * (generally this will generate an Entry Assignment message).
 *
 * This is NOT the preferred method to update a value; generally
 * SetEntryValue() should be used instead, with appropriate error handling.
 *
 * @param name      entry name (UTF-8 string)
 * @param value     new entry value
 */
WPI_DEPRECATED("use NT_Entry function instead")
void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value);

/**
 * Set Entry Type and Value.
 *
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, the currently stored entry type is overridden
 * (generally this will generate an Entry Assignment message).
 *
 * This is NOT the preferred method to update a value; generally
 * SetEntryValue() should be used instead, with appropriate error handling.
 *
 * @param entry     entry handle
 * @param value     new entry value
 */
void SetEntryTypeValue(NT_Entry entry, std::shared_ptr<Value> value);

/**
 * Set Entry Flags.
 *
 * @param name      entry name (UTF-8 string)
 * @param flags     flags value (bitmask of NT_EntryFlags)
 */
WPI_DEPRECATED("use NT_Entry function instead")
void SetEntryFlags(StringRef name, unsigned int flags);

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
 * @param name      entry name (UTF-8 string)
 * @return Flags value (bitmask of NT_EntryFlags)
 */
WPI_DEPRECATED("use NT_Entry function instead")
unsigned int GetEntryFlags(StringRef name);

/**
 * Get Entry Flags.
 *
 * @param entry     entry handle
 * @return Flags value (bitmask of NT_EntryFlags)
 */
unsigned int GetEntryFlags(NT_Entry entry);

/**
 * Delete Entry.
 *
 * Deletes an entry.  This is a new feature in version 3.0 of the protocol,
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 *
 * @param name      entry name (UTF-8 string)
 */
WPI_DEPRECATED("use NT_Entry function instead")
void DeleteEntry(StringRef name);

/**
 * Delete Entry.
 *
 * Deletes an entry.  This is a new feature in version 3.0 of the protocol,
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 *
 * @param entry     entry handle
 */
void DeleteEntry(NT_Entry entry);

/**
 * Delete All Entries.
 *
 * Deletes ALL table entries.  This is a new feature in version 3.0 of the
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 */
WPI_DEPRECATED("use NT_Inst function instead")
void DeleteAllEntries();

/**
 * @copydoc DeleteAllEntries()
 *
 * @param inst      instance handle
 */
void DeleteAllEntries(NT_Inst inst);

/**
 * Get Entry Information.
 *
 * Returns an array of entry information (name, entry type,
 * and timestamp of last change to type/value).  The results are optionally
 * filtered by string prefix and entry type to only return a subset of all
 * entries.
 *
 * @param prefix        entry name required prefix; only entries whose name
 *                      starts with this string are returned
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @return Array of entry information.
 */
WPI_DEPRECATED("use NT_Inst function instead")
std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types);

/**
 * @copydoc GetEntryInfo(StringRef, unsigned int)
 *
 * @param inst    instance handle
 */
std::vector<EntryInfo> GetEntryInfo(NT_Inst inst, const Twine& prefix,
                                    unsigned int types);

/**
 * Get Entry Information.
 *
 * Returns information about an entry (name, entry type,
 * and timestamp of last change to type/value).
 *
 * @param entry         entry handle
 * @return Entry information.
 */
EntryInfo GetEntryInfo(NT_Entry entry);

/** @} */

/**
 * @defgroup ntcore_entrylistener_func Entry Listener Functions
 * @{
 */

/**
 * Entry listener callback function.
 * Called when a key-value pair is changed.
 *
 * @param entry_listener  entry listener handle returned by callback creation
 *                        function
 * @param name            entry name
 * @param value           the new value
 * @param flags           update flags; for example, NT_NOTIFY_NEW if the key
 *                        did not previously exist
 */
typedef std::function<void(NT_EntryListener entry_listener, StringRef name,
                           std::shared_ptr<Value> value, unsigned int flags)>
    EntryListenerCallback;

/**
 * Add a listener for all entries starting with a certain prefix.
 *
 * @param prefix            UTF-8 string prefix
 * @param callback          listener to add
 * @param flags             NotifyKind bitmask
 * @return Listener handle
 */
WPI_DEPRECATED("use NT_Inst function instead")
NT_EntryListener AddEntryListener(StringRef prefix,
                                  EntryListenerCallback callback,
                                  unsigned int flags);

/**
 * @copydoc AddEntryListener(StringRef, EntryListenerCallback, unsigned int)
 *
 * @param inst              instance handle
 */
NT_EntryListener AddEntryListener(
    NT_Inst inst, const Twine& prefix,
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags);

/**
 * Add a listener for a single entry.
 *
 * @param entry             entry handle
 * @param callback          listener to add
 * @param flags             NotifyKind bitmask
 * @return Listener handle
 */
NT_EntryListener AddEntryListener(
    NT_Entry entry,
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags);

/**
 * Create a entry listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using AddPolledEntryListener()) will be stored in the queue and
 * must be collected by calling PollEntryListener().
 * The returned handle must be destroyed with DestroyEntryListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_EntryListenerPoller CreateEntryListenerPoller(NT_Inst inst);

/**
 * Destroy a entry listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void DestroyEntryListenerPoller(NT_EntryListenerPoller poller);

/**
 * Create a polled entry listener.
 * The caller is responsible for calling PollEntryListener() to poll.
 *
 * @param poller            poller handle
 * @param prefix            UTF-8 string prefix
 * @param flags             NotifyKind bitmask
 * @return Listener handle
 */
NT_EntryListener AddPolledEntryListener(NT_EntryListenerPoller poller,
                                        const Twine& prefix,
                                        unsigned int flags);

/**
 * Create a polled entry listener.
 * The caller is responsible for calling PollEntryListener() to poll.
 *
 * @param poller            poller handle
 * @param prefix            UTF-8 string prefix
 * @param flags             NotifyKind bitmask
 * @return Listener handle
 */
NT_EntryListener AddPolledEntryListener(NT_EntryListenerPoller poller,
                                        NT_Entry entry, unsigned int flags);

/**
 * Get the next entry listener event.  This blocks until the next event occurs.
 * This is intended to be used with AddPolledEntryListener(); entry listeners
 * created using AddEntryListener() will not be serviced through this function.
 *
 * @param poller    poller handle
 * @return Information on the entry listener events.  Only returns empty if an
 *         error occurred (e.g. the instance was invalid or is shutting down).
 */
std::vector<EntryNotification> PollEntryListener(NT_EntryListenerPoller poller);

/**
 * Get the next entry listener event.  This blocks until the next event occurs
 * or it times out.  This is intended to be used with AddPolledEntryListener();
 * entry listeners created using AddEntryListener() will not be serviced
 * through this function.
 *
 * @param poller      poller handle
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Information on the entry listener events.  If empty is returned and
 *         and timed_out is also false, an error occurred (e.g. the instance
 *         was invalid or is shutting down).
 */
std::vector<EntryNotification> PollEntryListener(NT_EntryListenerPoller poller,
                                                 double timeout,
                                                 bool* timed_out);

/**
 * Cancel a PollEntryListener call.  This wakes up a call to
 * PollEntryListener for this poller and causes it to immediately return
 * an empty array.
 *
 * @param poller  poller handle
 */
void CancelPollEntryListener(NT_EntryListenerPoller poller);

/**
 * Remove an entry listener.
 *
 * @param entry_listener Listener handle to remove
 */
void RemoveEntryListener(NT_EntryListener entry_listener);

/**
 * Wait for the entry listener queue to be empty.  This is primarily useful
 * for deterministic testing.  This blocks until either the entry listener
 * queue is empty (e.g. there are no more events that need to be passed along
 * to callbacks or poll queues) or the timeout expires.
 *
 * @param inst      instance handle
 * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
 *                  or a negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
bool WaitForEntryListenerQueue(NT_Inst inst, double timeout);

/** @} */

/**
 * @defgroup ntcore_connectionlistener_func Connection Listener Functions
 * @{
 */

/**
 * Connection listener callback function.
 * Called when a network connection is made or lost.
 *
 * @param conn_listener   connection listener handle returned by callback
 *                        creation function
 * @param connected       true if event is due to connection being established
 * @param conn            connection info
 */
typedef std::function<void(NT_ConnectionListener conn_listener, bool connected,
                           const ConnectionInfo& conn)>
    ConnectionListenerCallback;

/**
 * Add a connection listener.
 *
 * @param callback          listener to add
 * @param immediate_notify  notify listener of all existing connections
 * @return Listener handle
 */
WPI_DEPRECATED("use NT_Inst function instead")
NT_ConnectionListener AddConnectionListener(ConnectionListenerCallback callback,
                                            bool immediate_notify);

/**
 * @copydoc AddConnectionListener(ConnectionListenerCallback, bool)
 *
 * @param inst              instance handle
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
std::vector<ConnectionNotification> PollConnectionListener(
    NT_ConnectionListenerPoller poller);

/**
 * Get the next connection event.  This blocks until the next connect or
 * disconnect occurs or it times out.  This is intended to be used with
 * AddPolledConnectionListener(); connection listeners created using
 * AddConnectionListener() will not be serviced through this function.
 *
 * @param poller      poller handle
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Information on the connection events.  If empty is returned and
 *         timed_out is also false, an error occurred (e.g. the instance was
 *         invalid or is shutting down).
 */
std::vector<ConnectionNotification> PollConnectionListener(
    NT_ConnectionListenerPoller poller, double timeout, bool* timed_out);

/**
 * Cancel a PollConnectionListener call.  This wakes up a call to
 * PollConnectionListener for this poller and causes it to immediately return
 * an empty array.
 *
 * @param poller  poller handle
 */
void CancelPollConnectionListener(NT_ConnectionListenerPoller poller);

/**
 * Remove a connection listener.
 *
 * @param conn_listener Listener handle to remove
 */
void RemoveConnectionListener(NT_ConnectionListener conn_listener);

/**
 * Wait for the connection listener queue to be empty.  This is primarily useful
 * for deterministic testing.  This blocks until either the connection listener
 * queue is empty (e.g. there are no more events that need to be passed along
 * to callbacks or poll queues) or the timeout expires.
 *
 * @param inst      instance handle
 * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
 *                  or a negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
bool WaitForConnectionListenerQueue(NT_Inst inst, double timeout);

/** @} */

/**
 * @defgroup ntcore_rpc_func Remote Procedure Call Functions
 * @{
 */

/**
 * Create a callback-based RPC entry point.  Only valid to use on the server.
 * The callback function will be called when the RPC is called.
 *
 * @param entry     entry handle of RPC entry
 * @param def       RPC definition
 * @param callback  callback function; note the callback function must call
 *                  PostRpcResponse() to provide a response to the call
 */
void CreateRpc(NT_Entry entry, StringRef def,
               std::function<void(const RpcAnswer& answer)> callback);

/**
 * Create a RPC call poller.  Only valid to use on the server.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using CreatePolledRpc()) will be stored in the queue and must be
 * collected by calling PollRpc().
 * The returned handle must be destroyed with DestroyRpcCallPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_RpcCallPoller CreateRpcCallPoller(NT_Inst inst);

/**
 * Destroy a RPC call poller.  This will abort any blocked polling call and
 * prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void DestroyRpcCallPoller(NT_RpcCallPoller poller);

/**
 * Create a polled RPC entry point.  Only valid to use on the server.
 * The caller is responsible for calling PollRpc() to poll for servicing
 * incoming RPC calls.
 *
 * @param entry     entry handle of RPC entry
 * @param def       RPC definition
 * @param poller    poller handle
 */
void CreatePolledRpc(NT_Entry entry, StringRef def, NT_RpcCallPoller poller);

/**
 * Get the next incoming RPC call.  This blocks until the next incoming RPC
 * call is received.  This is intended to be used with CreatePolledRpc();
 * RPC calls created using CreateRpc() will not be serviced through this
 * function.  Upon successful return, PostRpcResponse() must be called to
 * send the return value to the caller.
 *
 * @param poller      poller handle
 * @return Information on the next RPC calls.  Only returns empty if an error
 *         occurred (e.g. the instance was invalid or is shutting down).
 */
std::vector<RpcAnswer> PollRpc(NT_RpcCallPoller poller);

/**
 * Get the next incoming RPC call.  This blocks until the next incoming RPC
 * call is received or it times out.  This is intended to be used with
 * CreatePolledRpc(); RPC calls created using CreateRpc() will not be
 * serviced through this function.  Upon successful return,
 * PostRpcResponse() must be called to send the return value to the caller.
 *
 * @param poller      poller handle
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Information on the next RPC calls.  If empty and timed_out is also
 *         false, an error occurred (e.g. the instance was invalid or is
 *         shutting down).
 */
std::vector<RpcAnswer> PollRpc(NT_RpcCallPoller poller, double timeout,
                               bool* timed_out);

/**
 * Cancel a PollRpc call.  This wakes up a call to PollRpc for this poller
 * and causes it to immediately return an empty array.
 *
 * @param poller  poller handle
 */
void CancelPollRpc(NT_RpcCallPoller poller);

/**
 * Wait for the incoming RPC call queue to be empty.  This is primarily useful
 * for deterministic testing.  This blocks until either the RPC call
 * queue is empty (e.g. there are no more events that need to be passed along
 * to callbacks or poll queues) or the timeout expires.
 *
 * @param inst      instance handle
 * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
 *                  or a negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
bool WaitForRpcCallQueue(NT_Inst inst, double timeout);

/**
 * Post RPC response (return value) for a polled RPC.
 * The rpc and call parameters should come from the RpcAnswer returned
 * by PollRpc().
 *
 * @param entry       entry handle of RPC entry (from RpcAnswer)
 * @param call        RPC call handle (from RpcAnswer)
 * @param result      result raw data that will be provided to remote caller
 * @return            true if the response was posted, otherwise false
 */
bool PostRpcResponse(NT_Entry entry, NT_RpcCall call, StringRef result);

/**
 * Call a RPC function.  May be used on either the client or server.
 * This function is non-blocking.  Either GetRpcResult() or
 * CancelRpcResult() must be called to either get or ignore the result of
 * the call.
 *
 * @param entry       entry handle of RPC entry
 * @param params      parameter
 * @return RPC call handle (for use with GetRpcResult() or
 *         CancelRpcResult()).
 */
NT_RpcCall CallRpc(NT_Entry entry, StringRef params);

/**
 * Get the result (return value) of a RPC call.  This function blocks until
 * the result is received.
 *
 * @param entry       entry handle of RPC entry
 * @param call        RPC call handle returned by CallRpc()
 * @param result      received result (output)
 * @return False on error, true otherwise.
 */
bool GetRpcResult(NT_Entry entry, NT_RpcCall call, std::string* result);

/**
 * Get the result (return value) of a RPC call.  This function blocks until
 * the result is received or it times out.
 *
 * @param entry       entry handle of RPC entry
 * @param call        RPC call handle returned by CallRpc()
 * @param result      received result (output)
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return False on error or timeout, true otherwise.
 */
bool GetRpcResult(NT_Entry entry, NT_RpcCall call, std::string* result,
                  double timeout, bool* timed_out);

/**
 * Ignore the result of a RPC call.  This function is non-blocking.
 *
 * @param entry       entry handle of RPC entry
 * @param call        RPC call handle returned by CallRpc()
 */
void CancelRpcResult(NT_Entry entry, NT_RpcCall call);

/**
 * Pack a RPC version 1 definition.
 *
 * @param def         RPC version 1 definition
 * @return Raw packed bytes.
 */
std::string PackRpcDefinition(const RpcDefinition& def);

/**
 * Unpack a RPC version 1 definition.  This can be used for introspection or
 * validation.
 *
 * @param packed      raw packed bytes
 * @param def         RPC version 1 definition (output)
 * @return True if successfully unpacked, false otherwise.
 */
bool UnpackRpcDefinition(StringRef packed, RpcDefinition* def);

/**
 * Pack RPC values as required for RPC version 1 definition messages.
 *
 * @param values      array of values to pack
 * @return Raw packed bytes.
 */
std::string PackRpcValues(ArrayRef<std::shared_ptr<Value>> values);

/**
 * Unpack RPC values as required for RPC version 1 definition messages.
 *
 * @param packed      raw packed bytes
 * @param types       array of data types (as provided in the RPC definition)
 * @return Array of values.
 */
std::vector<std::shared_ptr<Value>> UnpackRpcValues(StringRef packed,
                                                    ArrayRef<NT_Type> types);

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
 * @param name      identity to advertise
 */
WPI_DEPRECATED("use NT_Inst function instead")
void SetNetworkIdentity(StringRef name);

/**
 * @copydoc SetNetworkIdentity(StringRef)
 *
 * @param inst      instance handle
 */
void SetNetworkIdentity(NT_Inst inst, const Twine& name);

/**
 * Get the current network mode.
 *
 * @return Bitmask of NT_NetworkMode.
 */
WPI_DEPRECATED("use NT_Inst function instead")
unsigned int GetNetworkMode();

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
 * @param persist_filename  the name of the persist file to use (UTF-8 string,
 *                          null terminated)
 * @param listen_address    the address to listen on, or null to listen on any
 *                          address. (UTF-8 string, null terminated)
 * @param port              port to communicate over.
 */
WPI_DEPRECATED("use NT_Inst function instead")
void StartServer(StringRef persist_filename, const char* listen_address,
                 unsigned int port);

/**
 * @copydoc StartServer(StringRef, const char*, unsigned int)
 *
 * @param inst              instance handle
 */
void StartServer(NT_Inst inst, const Twine& persist_filename,
                 const char* listen_address, unsigned int port);

/**
 * Stops the server if it is running.
 */
WPI_DEPRECATED("use NT_Inst function instead")
void StopServer();

/**
 * @copydoc StopServer()
 *
 * @param inst  instance handle
 */
void StopServer(NT_Inst inst);

/**
 * Starts a client.  Use SetServer to set the server name and port.
 */
WPI_DEPRECATED("use NT_Inst function instead")
void StartClient();

/**
 * Starts a client using the specified server and port
 *
 * @param server_name server name (UTF-8 string, null terminated)
 * @param port        port to communicate over
 */
WPI_DEPRECATED("use NT_Inst function instead")
void StartClient(const char* server_name, unsigned int port);

/**
 * Starts a client using the specified (server, port) combinations.  The
 * client will attempt to connect to each server in round robin fashion.
 *
 * @param servers   array of server name and port pairs
 */
WPI_DEPRECATED("use NT_Inst function instead")
void StartClient(ArrayRef<std::pair<StringRef, unsigned int>> servers);

/**
 * @copydoc StartClient()
 *
 * @param inst  instance handle
 */
void StartClient(NT_Inst inst);

/**
 * @copydoc StartClient(const char*, unsigned int)
 *
 * @param inst        instance handle
 */
void StartClient(NT_Inst inst, const char* server_name, unsigned int port);

/**
 * @copydoc StartClient(ArrayRef<std::pair<StringRef, unsigned int>>)
 *
 * @param inst      instance handle
 */
void StartClient(NT_Inst inst,
                 ArrayRef<std::pair<StringRef, unsigned int>> servers);

/**
 * Starts a client using commonly known robot addresses for the specified
 * team.
 *
 * @param inst        instance handle
 * @param team        team number
 * @param port        port to communicate over
 */
void StartClientTeam(NT_Inst inst, unsigned int team, unsigned int port);

/**
 * Stops the client if it is running.
 */
WPI_DEPRECATED("use NT_Inst function instead")
void StopClient();

/**
 * @copydoc StopClient()
 * @param inst  instance handle
 */
void StopClient(NT_Inst inst);

/**
 * Sets server address and port for client (without restarting client).
 *
 * @param server_name server name (UTF-8 string, null terminated)
 * @param port        port to communicate over
 */
WPI_DEPRECATED("use NT_Inst function instead")
void SetServer(const char* server_name, unsigned int port);

/**
 * Sets server addresses for client (without restarting client).
 * The client will attempt to connect to each server in round robin fashion.
 *
 * @param servers   array of server name and port pairs
 */
WPI_DEPRECATED("use NT_Inst function instead")
void SetServer(ArrayRef<std::pair<StringRef, unsigned int>> servers);

/**
 * @copydoc SetServer(const char*, unsigned int)
 *
 * @param inst        instance handle
 */
void SetServer(NT_Inst inst, const char* server_name, unsigned int port);

/**
 * @copydoc SetServer(ArrayRef<std::pair<StringRef, unsigned int>>)
 *
 * @param inst      instance handle
 */
void SetServer(NT_Inst inst,
               ArrayRef<std::pair<StringRef, unsigned int>> servers);

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
 * @param port server port to use in combination with IP from DS
 */
WPI_DEPRECATED("use NT_Inst function instead")
void StartDSClient(unsigned int port);

/**
 * @copydoc StartDSClient(unsigned int)
 * @param inst  instance handle
 */
void StartDSClient(NT_Inst inst, unsigned int port);

/** Stops requesting server address from Driver Station. */
WPI_DEPRECATED("use NT_Inst function instead")
void StopDSClient();

/**
 * @copydoc StopDSClient()
 *
 * @param inst  instance handle
 */
void StopDSClient(NT_Inst inst);

/** Stops the RPC server if it is running. */
WPI_DEPRECATED("use NT_Inst function instead")
void StopRpcServer();

/**
 * Set the periodic update rate.
 * Sets how frequently updates are sent to other nodes over the network.
 *
 * @param interval  update interval in seconds (range 0.01 to 1.0)
 */
WPI_DEPRECATED("use NT_Inst function instead")
void SetUpdateRate(double interval);

/**
 * @copydoc SetUpdateRate(double)
 *
 * @param inst      instance handle
 */
void SetUpdateRate(NT_Inst inst, double interval);

/**
 * Flush Entries.
 *
 * Forces an immediate flush of all local entry changes to network.
 * Normally this is done on a regularly scheduled interval (see
 * SetUpdateRate()).
 *
 * Note: flushes are rate limited to avoid excessive network traffic.  If
 * the time between calls is too short, the flush will occur after the minimum
 * time elapses (rather than immediately).
 */
WPI_DEPRECATED("use NT_Inst function instead")
void Flush();

/**
 * @copydoc Flush()
 *
 * @param inst      instance handle
 */
void Flush(NT_Inst inst);

/**
 * Get information on the currently established network connections.
 * If operating as a client, this will return either zero or one values.
 *
 * @return      array of connection information
 */
WPI_DEPRECATED("use NT_Inst function instead")
std::vector<ConnectionInfo> GetConnections();

/**
 * @copydoc GetConnections()
 *
 * @param inst  instance handle
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
 * @defgroup ntcore_file_func File Save/Load Functions
 * @{
 */

/**
 * Save persistent values to a file.  The server automatically does this,
 * but this function provides a way to save persistent values in the same
 * format to a file on either a client or a server.
 *
 * @param filename  filename
 * @return error string, or nullptr if successful
 */
WPI_DEPRECATED("use NT_Inst function instead")
const char* SavePersistent(StringRef filename);

/**
 * @copydoc SavePersistent(StringRef)
 * @param inst      instance handle
 */
const char* SavePersistent(NT_Inst inst, const Twine& filename);

/**
 * Load persistent values from a file.  The server automatically does this
 * at startup, but this function provides a way to restore persistent values
 * in the same format from a file at any time on either a client or a server.
 *
 * @param filename  filename
 * @param warn      callback function for warnings
 * @return error string, or nullptr if successful
 */
WPI_DEPRECATED("use NT_Inst function instead")
const char* LoadPersistent(
    StringRef filename, std::function<void(size_t line, const char* msg)> warn);

/**
 * @copydoc LoadPersistent(StringRef, std::function<void(size_t, const
 * char*)>)
 *
 * @param inst      instance handle
 */
const char* LoadPersistent(
    NT_Inst inst, const Twine& filename,
    std::function<void(size_t line, const char* msg)> warn);

/**
 * Save table values to a file.  The file format used is identical to
 * that used for SavePersistent.
 *
 * @param inst      instance handle
 * @param filename  filename
 * @param prefix    save only keys starting with this prefix
 * @return error string, or nullptr if successful
 */
const char* SaveEntries(NT_Inst inst, const Twine& filename,
                        const Twine& prefix);

/**
 * Load table values from a file.  The file format used is identical to
 * that used for SavePersistent / LoadPersistent.
 *
 * @param inst      instance handle
 * @param filename  filename
 * @param prefix    load only keys starting with this prefix
 * @param warn      callback function for warnings
 * @return error string, or nullptr if successful
 */
const char* LoadEntries(NT_Inst inst, const Twine& filename,
                        const Twine& prefix,
                        std::function<void(size_t line, const char* msg)> warn);

/** @} */

/**
 * @defgroup ntcore_utility_func Utility Functions
 * @{
 */

/**
 * Returns monotonic current time in 1 us increments.
 * This is the same time base used for entry and connection timestamps.
 * This function is a compatibility wrapper around wpi::Now().
 *
 * @return Timestamp
 */
uint64_t Now();

/** @} */

/**
 * @defgroup ntcore_logger_func Logger Functions
 * @{
 */

/**
 * Log function.
 *
 * @param level   log level of the message (see NT_LogLevel)
 * @param file    origin source filename
 * @param line    origin source line number
 * @param msg     message
 */
typedef std::function<void(unsigned int level, const char* file,
                           unsigned int line, const char* msg)>
    LogFunc;

/**
 * Set logger callback function.  By default, log messages are sent to stderr;
 * this function changes the log level and sends log messages to the provided
 * callback function instead.  The callback function will only be called for
 * log messages with level greater than or equal to min_level; messages lower
 * than this level will be silently ignored.
 *
 * @param func        log callback function
 * @param min_level   minimum log level
 */
WPI_DEPRECATED("use NT_Inst function instead")
void SetLogger(LogFunc func, unsigned int min_level);

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
std::vector<LogMessage> PollLogger(NT_LoggerPoller poller);

/**
 * Get the next log event.  This blocks until the next log occurs or it times
 * out.
 *
 * @param poller      poller handle
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Information on the log events.  If empty is returned and timed_out
 *         is also false, an error occurred (e.g. the instance was invalid or
 *         is shutting down).
 */
std::vector<LogMessage> PollLogger(NT_LoggerPoller poller, double timeout,
                                   bool* timed_out);

/**
 * Cancel a PollLogger call.  This wakes up a call to PollLogger for this
 * poller and causes it to immediately return an empty array.
 *
 * @param poller  poller handle
 */
void CancelPollLogger(NT_LoggerPoller poller);

/**
 * Remove a logger.
 *
 * @param logger Logger handle to remove
 */
void RemoveLogger(NT_Logger logger);

/**
 * Wait for the incoming log event queue to be empty.  This is primarily useful
 * for deterministic testing.  This blocks until either the log event
 * queue is empty (e.g. there are no more events that need to be passed along
 * to callbacks or poll queues) or the timeout expires.
 *
 * @param inst      instance handle
 * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
 *                  or a negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
bool WaitForLoggerQueue(NT_Inst inst, double timeout);

/** @} */
/** @} */

inline bool RpcAnswer::PostResponse(StringRef result) const {
  auto ret = PostRpcResponse(entry, call, result);
  call = 0;
  return ret;
}

}  // namespace nt

#endif  // NTCORE_NTCORE_CPP_H_
