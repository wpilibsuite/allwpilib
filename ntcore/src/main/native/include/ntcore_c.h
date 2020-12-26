// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NTCORE_C_H_
#define NTCORE_NTCORE_C_H_

#include <stdint.h>

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include <wpi/deprecated.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ntcore_c_api ntcore C API
 *
 * Handle-based interface for C.
 *
 * @{
 */

/** Typedefs */
typedef int NT_Bool;

typedef unsigned int NT_Handle;
typedef NT_Handle NT_ConnectionListener;
typedef NT_Handle NT_ConnectionListenerPoller;
typedef NT_Handle NT_Entry;
typedef NT_Handle NT_EntryListener;
typedef NT_Handle NT_EntryListenerPoller;
typedef NT_Handle NT_Inst;
typedef NT_Handle NT_Logger;
typedef NT_Handle NT_LoggerPoller;
typedef NT_Handle NT_RpcCall;
typedef NT_Handle NT_RpcCallPoller;

/** Default network tables port number */
#define NT_DEFAULT_PORT 1735

/** NetworkTables data types. */
enum NT_Type {
  NT_UNASSIGNED = 0,
  NT_BOOLEAN = 0x01,
  NT_DOUBLE = 0x02,
  NT_STRING = 0x04,
  NT_RAW = 0x08,
  NT_BOOLEAN_ARRAY = 0x10,
  NT_DOUBLE_ARRAY = 0x20,
  NT_STRING_ARRAY = 0x40,
  NT_RPC = 0x80
};

/** NetworkTables entry flags. */
enum NT_EntryFlags { NT_PERSISTENT = 0x01 };

/** NetworkTables logging levels. */
enum NT_LogLevel {
  NT_LOG_CRITICAL = 50,
  NT_LOG_ERROR = 40,
  NT_LOG_WARNING = 30,
  NT_LOG_INFO = 20,
  NT_LOG_DEBUG = 10,
  NT_LOG_DEBUG1 = 9,
  NT_LOG_DEBUG2 = 8,
  NT_LOG_DEBUG3 = 7,
  NT_LOG_DEBUG4 = 6
};

/** NetworkTables notifier kinds. */
enum NT_NotifyKind {
  NT_NOTIFY_NONE = 0,
  NT_NOTIFY_IMMEDIATE = 0x01, /* initial listener addition */
  NT_NOTIFY_LOCAL = 0x02,     /* changed locally */
  NT_NOTIFY_NEW = 0x04,       /* newly created entry */
  NT_NOTIFY_DELETE = 0x08,    /* deleted */
  NT_NOTIFY_UPDATE = 0x10,    /* value changed */
  NT_NOTIFY_FLAGS = 0x20      /* flags changed */
};

/** Client/server modes */
enum NT_NetworkMode {
  NT_NET_MODE_NONE = 0x00,     /* not running */
  NT_NET_MODE_SERVER = 0x01,   /* running in server mode */
  NT_NET_MODE_CLIENT = 0x02,   /* running in client mode */
  NT_NET_MODE_STARTING = 0x04, /* flag for starting (either client or server) */
  NT_NET_MODE_FAILURE = 0x08,  /* flag for failure (either client or server) */
  NT_NET_MODE_LOCAL = 0x10,    /* running in local-only mode */
};

/*
 * Structures
 */

/** A NetworkTables string. */
struct NT_String {
  /**
   * String contents (UTF-8).
   * The string is NOT required to be zero-terminated.
   * When returned by the library, this is zero-terminated and allocated with
   * std::malloc().
   */
  char* str;

  /**
   * Length of the string in bytes.  If the string happens to be zero
   * terminated, this does not include the zero-termination.
   */
  size_t len;
};

/** NetworkTables Entry Value.  Note this is a typed union. */
struct NT_Value {
  enum NT_Type type;
  uint64_t last_change;
  union {
    NT_Bool v_boolean;
    double v_double;
    struct NT_String v_string;
    struct NT_String v_raw;
    struct {
      NT_Bool* arr;
      size_t size;
    } arr_boolean;
    struct {
      double* arr;
      size_t size;
    } arr_double;
    struct {
      struct NT_String* arr;
      size_t size;
    } arr_string;
  } data;
};

/** NetworkTables Entry Information */
struct NT_EntryInfo {
  /** Entry handle */
  NT_Entry entry;

  /** Entry name */
  struct NT_String name;

  /** Entry type */
  enum NT_Type type;

  /** Entry flags */
  unsigned int flags;

  /** Timestamp of last change to entry (type or value). */
  uint64_t last_change;
};

/** NetworkTables Connection Information */
struct NT_ConnectionInfo {
  /**
   * The remote identifier (as set on the remote node by
   * NetworkTableInstance::SetNetworkIdentity() or nt::SetNetworkIdentity()).
   */
  struct NT_String remote_id;

  /** The IP address of the remote node. */
  struct NT_String remote_ip;

  /** The port number of the remote node. */
  unsigned int remote_port;

  /**
   * The last time any update was received from the remote node (same scale as
   * returned by nt::Now()).
   */
  uint64_t last_update;

  /**
   * The protocol version being used for this connection.  This in protocol
   * layer format, so 0x0200 = 2.0, 0x0300 = 3.0).
   */
  unsigned int protocol_version;
};

/** NetworkTables RPC Version 1 Definition Parameter */
struct NT_RpcParamDef {
  struct NT_String name;
  struct NT_Value def_value;
};

/** NetworkTables RPC Version 1 Definition Result */
struct NT_RpcResultDef {
  struct NT_String name;
  enum NT_Type type;
};

/** NetworkTables RPC Version 1 Definition */
struct NT_RpcDefinition {
  unsigned int version;
  struct NT_String name;
  size_t num_params;
  struct NT_RpcParamDef* params;
  size_t num_results;
  struct NT_RpcResultDef* results;
};

/** NetworkTables RPC Call Data */
struct NT_RpcAnswer {
  NT_Entry entry;
  NT_RpcCall call;
  struct NT_String name;
  struct NT_String params;
  struct NT_ConnectionInfo conn;
};

/** NetworkTables Entry Notification */
struct NT_EntryNotification {
  /** Listener that was triggered. */
  NT_EntryListener listener;

  /** Entry handle. */
  NT_Entry entry;

  /** Entry name. */
  struct NT_String name;

  /** The new value. */
  struct NT_Value value;

  /**
   * Update flags.  For example, NT_NOTIFY_NEW if the key did not previously
   * exist.
   */
  unsigned int flags;
};

/** NetworkTables Connection Notification */
struct NT_ConnectionNotification {
  /** Listener that was triggered. */
  NT_ConnectionListener listener;

  /** True if event is due to connection being established. */
  NT_Bool connected;

  /** Connection info. */
  struct NT_ConnectionInfo conn;
};

/** NetworkTables log message. */
struct NT_LogMessage {
  /** The logger that generated the message. */
  NT_Logger logger;

  /** Log level of the message.  See NT_LogLevel. */
  unsigned int level;

  /** The filename of the source file that generated the message. */
  const char* filename;

  /** The line number in the source file that generated the message. */
  unsigned int line;

  /** The message. */
  char* message;
};

/**
 * @defgroup ntcore_instance_cfunc Instance Functions
 * @{
 */

/**
 * Get default instance.
 * This is the instance used by non-handle-taking functions.
 *
 * @return Instance handle
 */
NT_Inst NT_GetDefaultInstance(void);

/**
 * Create an instance.
 *
 * @return Instance handle
 */
NT_Inst NT_CreateInstance(void);

/**
 * Destroy an instance.
 * The default instance cannot be destroyed.
 *
 * @param inst Instance handle
 */
void NT_DestroyInstance(NT_Inst inst);

/**
 * Get instance handle from another handle.
 *
 * @param handle    handle
 * @return Instance handle
 */
NT_Inst NT_GetInstanceFromHandle(NT_Handle handle);

/** @} */

/**
 * @defgroup ntcore_table_cfunc Table Functions
 * @{
 */

/**
 * Get Entry Handle.
 *
 * @param inst      instance handle
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @return entry handle
 */
NT_Entry NT_GetEntry(NT_Inst inst, const char* name, size_t name_len);

/**
 * Get Entry Handles.
 *
 * Returns an array of entry handles.  The results are optionally
 * filtered by string prefix and entry type to only return a subset of all
 * entries.
 *
 * @param prefix        entry name required prefix; only entries whose name
 *                      starts with this string are returned
 * @param prefix_len    length of prefix in bytes
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @return Array of entry handles.
 */
NT_Entry* NT_GetEntries(NT_Inst inst, const char* prefix, size_t prefix_len,
                        unsigned int types, size_t* count);

/**
 * Gets the name of the specified entry.
 * Returns an empty string if the handle is invalid.
 *
 * @param entry     entry handle
 * @param name_len  length of the returned string (output parameter)
 * @return Entry name
 */
char* NT_GetEntryName(NT_Entry entry, size_t* name_len);

/**
 * Gets the type for the specified key, or unassigned if non existent.
 *
 * @param entry   entry handle
 * @return Entry type
 */
enum NT_Type NT_GetEntryType(NT_Entry entry);

/**
 * Gets the last time the entry was changed.
 * Returns 0 if the handle is invalid.
 *
 * @param entry   entry handle
 * @return Entry last change time
 */
uint64_t NT_GetEntryLastChange(NT_Entry entry);

/**
 * Get Entry Value.
 *
 * Returns copy of current entry value.
 * Note that one of the type options is "unassigned".
 *
 * @param entry     entry handle
 * @param value     storage for returned entry value
 *
 * It is the caller's responsibility to free value once it's no longer
 * needed (the utility function NT_DisposeValue() is useful for this
 * purpose).
 */
void NT_GetEntryValue(NT_Entry entry, struct NT_Value* value);

/**
 * Set Default Entry Value.
 *
 * Returns copy of current entry value if it exists.
 * Otherwise, sets passed in value, and returns set value.
 * Note that one of the type options is "unassigned".
 *
 * @param entry     entry handle
 * @param default_value     value to be set if name does not exist
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryValue(NT_Entry entry,
                                const struct NT_Value* default_value);

/**
 * Set Entry Value.
 *
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, returns error and does not update value.
 *
 * @param entry     entry handle
 * @param value     new entry value
 * @return 0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryValue(NT_Entry entry, const struct NT_Value* value);

/**
 * Set Entry Type and Value.
 *
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, the currently stored entry type is overridden
 * (generally this will generate an Entry Assignment message).
 *
 * This is NOT the preferred method to update a value; generally
 * NT_SetEntryValue() should be used instead, with appropriate error handling.
 *
 * @param entry     entry handle
 * @param value     new entry value
 */
void NT_SetEntryTypeValue(NT_Entry entry, const struct NT_Value* value);

/**
 * Set Entry Flags.
 *
 * @param entry     entry handle
 * @param flags     flags value (bitmask of NT_EntryFlags)
 */
void NT_SetEntryFlags(NT_Entry entry, unsigned int flags);

/**
 * Get Entry Flags.
 *
 * @param entry     entry handle
 * @return Flags value (bitmask of NT_EntryFlags)
 */
unsigned int NT_GetEntryFlags(NT_Entry entry);

/**
 * Delete Entry.
 *
 * Deletes an entry.  This is a new feature in version 3.0 of the protocol,
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: NT_GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 *
 * @param entry     entry handle
 */
void NT_DeleteEntry(NT_Entry entry);

/**
 * Delete All Entries.
 *
 * Deletes ALL table entries.  This is a new feature in version 3.0 of the
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: NT_GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 *
 * @param inst      instance handle
 */
void NT_DeleteAllEntries(NT_Inst inst);

/**
 * Get Entry Information.
 *
 * Returns an array of entry information (entry handle, name, entry type,
 * and timestamp of last change to type/value).  The results are optionally
 * filtered by string prefix and entry type to only return a subset of all
 * entries.
 *
 * @param inst          instance handle
 * @param prefix        entry name required prefix; only entries whose name
 *                      starts with this string are returned
 * @param prefix_len    length of prefix in bytes
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @param count         output parameter; set to length of returned array
 * @return Array of entry information.
 */
struct NT_EntryInfo* NT_GetEntryInfo(NT_Inst inst, const char* prefix,
                                     size_t prefix_len, unsigned int types,
                                     size_t* count);

/**
 * Get Entry Information.
 *
 * Returns information about an entry (name, entry type,
 * and timestamp of last change to type/value).
 *
 * @param entry         entry handle
 * @param info          entry information (output)
 * @return True if successful, false on error.
 */
NT_Bool NT_GetEntryInfoHandle(NT_Entry entry, struct NT_EntryInfo* info);

/** @} */

/**
 * @defgroup ntcore_entrylistener_cfunc Entry Listener Functions
 * @{
 */

/**
 * Entry listener callback function.
 * Called when a key-value pair is changed.
 *
 * @param data            data pointer provided to callback creation function
 * @param event           event information
 */
typedef void (*NT_EntryListenerCallback)(
    void* data, const struct NT_EntryNotification* event);

/**
 * Add a listener for all entries starting with a certain prefix.
 *
 * @param inst              instance handle
 * @param prefix            UTF-8 string prefix
 * @param prefix_len        length of prefix in bytes
 * @param data              data pointer to pass to callback
 * @param callback          listener to add
 * @param flags             NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_EntryListener NT_AddEntryListener(NT_Inst inst, const char* prefix,
                                     size_t prefix_len, void* data,
                                     NT_EntryListenerCallback callback,
                                     unsigned int flags);

/**
 * Add a listener for a single entry.
 *
 * @param entry             entry handle
 * @param data              data pointer to pass to callback
 * @param callback          listener to add
 * @param flags             NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_EntryListener NT_AddEntryListenerSingle(NT_Entry entry, void* data,
                                           NT_EntryListenerCallback callback,
                                           unsigned int flags);

/**
 * Create a entry listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using NT_AddPolledEntryListener()) will be stored in the queue and
 * must be collected by calling NT_PollEntryListener().
 * The returned handle must be destroyed with NT_DestroyEntryListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_EntryListenerPoller NT_CreateEntryListenerPoller(NT_Inst inst);

/**
 * Destroy a entry listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void NT_DestroyEntryListenerPoller(NT_EntryListenerPoller poller);

/**
 * Create a polled entry listener.
 * The caller is responsible for calling NT_PollEntryListener() to poll.
 *
 * @param poller            poller handle
 * @param prefix            UTF-8 string prefix
 * @param flags             NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_EntryListener NT_AddPolledEntryListener(NT_EntryListenerPoller poller,
                                           const char* prefix,
                                           size_t prefix_len,
                                           unsigned int flags);

/**
 * Create a polled entry listener.
 * The caller is responsible for calling NT_PollEntryListener() to poll.
 *
 * @param poller            poller handle
 * @param prefix            UTF-8 string prefix
 * @param flags             NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_EntryListener NT_AddPolledEntryListenerSingle(NT_EntryListenerPoller poller,
                                                 NT_Entry entry,
                                                 unsigned int flags);

/**
 * Get the next entry listener event.  This blocks until the next event occurs.
 *
 * This is intended to be used with NT_AddPolledEntryListener(void); entry
 * listeners created using NT_AddEntryListener() will not be serviced through
 * this function.
 *
 * @param poller    poller handle
 * @param len       length of returned array (output)
 * @return Array of information on the entry listener events.  Returns NULL if
 *         an erroroccurred (e.g. the instance was invalid or is shutting down).
 */
struct NT_EntryNotification* NT_PollEntryListener(NT_EntryListenerPoller poller,
                                                  size_t* len);

/**
 * Get the next entry listener event.  This blocks until the next event occurs
 * or it times out.  This is intended to be used with
 * NT_AddPolledEntryListener(); entry listeners created using
 * NT_AddEntryListener() will not be serviced through this function.
 *
 * @param poller      poller handle
 * @param len         length of returned array (output)
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Array of information on the entry listener events.  If NULL is
 *         returned and timed_out is also false, an error occurred (e.g. the
 *         instance was invalid or is shutting down).
 */
struct NT_EntryNotification* NT_PollEntryListenerTimeout(
    NT_EntryListenerPoller poller, size_t* len, double timeout,
    NT_Bool* timed_out);

/**
 * Cancel a PollEntryListener call.  This wakes up a call to
 * PollEntryListener for this poller and causes it to immediately return
 * an empty array.
 *
 * @param poller  poller handle
 */
void NT_CancelPollEntryListener(NT_EntryListenerPoller poller);

/**
 * Remove an entry listener.
 *
 * @param entry_listener Listener handle to remove
 */
void NT_RemoveEntryListener(NT_EntryListener entry_listener);

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
NT_Bool NT_WaitForEntryListenerQueue(NT_Inst inst, double timeout);

/** @} */

/**
 * @defgroup ntcore_connectionlistener_cfunc Connection Listener Functions
 * @{
 */

/**
 * Connection listener callback function.
 * Called when a network connection is made or lost.
 *
 * @param data            data pointer provided to callback creation function
 * @param event           event info
 */
typedef void (*NT_ConnectionListenerCallback)(
    void* data, const struct NT_ConnectionNotification* event);

/**
 * Add a connection listener.
 *
 * @param inst              instance handle
 * @param data              data pointer to pass to callback
 * @param callback          listener to add
 * @param immediate_notify  notify listener of all existing connections
 * @return Listener handle
 */
NT_ConnectionListener NT_AddConnectionListener(
    NT_Inst inst, void* data, NT_ConnectionListenerCallback callback,
    NT_Bool immediate_notify);

/**
 * Create a connection listener poller.
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using NT_AddPolledConnectionListener()) will be stored in the queue
 * and must be collected by calling NT_PollConnectionListener().
 * The returned handle must be destroyed with
 * NT_DestroyConnectionListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_ConnectionListenerPoller NT_CreateConnectionListenerPoller(NT_Inst inst);

/**
 * Destroy a connection listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void NT_DestroyConnectionListenerPoller(NT_ConnectionListenerPoller poller);

/**
 * Create a polled connection listener.
 * The caller is responsible for calling NT_PollConnectionListener() to poll.
 *
 * @param poller            poller handle
 * @param immediate_notify  notify listener of all existing connections
 */
NT_ConnectionListener NT_AddPolledConnectionListener(
    NT_ConnectionListenerPoller poller, NT_Bool immediate_notify);

/**
 * Get the next connection event.  This blocks until the next connect or
 * disconnect occurs.  This is intended to be used with
 * NT_AddPolledConnectionListener(); connection listeners created using
 * NT_AddConnectionListener() will not be serviced through this function.
 *
 * @param poller    poller handle
 * @param len       length of returned array (output)
 * @return Array of information on the connection events.  Only returns NULL
 *         if an error occurred (e.g. the instance was invalid or is shutting
 *         down).
 */
struct NT_ConnectionNotification* NT_PollConnectionListener(
    NT_ConnectionListenerPoller poller, size_t* len);

/**
 * Get the next connection event.  This blocks until the next connect or
 * disconnect occurs or it times out.  This is intended to be used with
 * NT_AddPolledConnectionListener(); connection listeners created using
 * NT_AddConnectionListener() will not be serviced through this function.
 *
 * @param poller      poller handle
 * @param len         length of returned array (output)
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Array of information on the connection events.  If NULL is returned
 *         and timed_out is also false, an error occurred (e.g. the instance
 *         was invalid or is shutting down).
 */
struct NT_ConnectionNotification* NT_PollConnectionListenerTimeout(
    NT_ConnectionListenerPoller poller, size_t* len, double timeout,
    NT_Bool* timed_out);

/**
 * Cancel a PollConnectionListener call.  This wakes up a call to
 * PollConnectionListener for this poller and causes it to immediately return
 * an empty array.
 *
 * @param poller  poller handle
 */
void NT_CancelPollConnectionListener(NT_ConnectionListenerPoller poller);

/**
 * Remove a connection listener.
 *
 * @param conn_listener Listener handle to remove
 */
void NT_RemoveConnectionListener(NT_ConnectionListener conn_listener);

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
NT_Bool NT_WaitForConnectionListenerQueue(NT_Inst inst, double timeout);

/** @} */

/**
 * @defgroup ntcore_rpc_cfunc Remote Procedure Call Functions
 * @{
 */

/**
 * Remote Procedure Call (RPC) callback function.
 *
 * @param data        data pointer provided to NT_CreateRpc()
 * @param call        call information
 *
 * Note: NT_PostRpcResponse() must be called by the callback to provide a
 * response to the call.
 */
typedef void (*NT_RpcCallback)(void* data, const struct NT_RpcAnswer* call);

/**
 * Create a callback-based RPC entry point.  Only valid to use on the server.
 * The callback function will be called when the RPC is called.
 *
 * @param entry     entry handle of RPC entry
 * @param def       RPC definition
 * @param def_len   length of def in bytes
 * @param data      data pointer to pass to callback function
 * @param callback  callback function
 */
void NT_CreateRpc(NT_Entry entry, const char* def, size_t def_len, void* data,
                  NT_RpcCallback callback);

/**
 * Create a RPC call poller.  Only valid to use on the server.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using NT_CreatePolledRpc()) will be stored in the queue and must be
 * collected by calling NT_PollRpc() or NT_PollRpcTimeout().
 * The returned handle must be destroyed with NT_DestroyRpcCallPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_RpcCallPoller NT_CreateRpcCallPoller(NT_Inst inst);

/**
 * Destroy a RPC call poller.  This will abort any blocked polling call and
 * prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void NT_DestroyRpcCallPoller(NT_RpcCallPoller poller);

/**
 * Create a polled RPC entry point.  Only valid to use on the server.
 *
 * The caller is responsible for calling NT_PollRpc() or NT_PollRpcTimeout()
 * to poll for servicing incoming RPC calls.
 *
 * @param entry     entry handle of RPC entry
 * @param def       RPC definition
 * @param def_len   length of def in bytes
 * @param poller    poller handle
 */
void NT_CreatePolledRpc(NT_Entry entry, const char* def, size_t def_len,
                        NT_RpcCallPoller poller);

/**
 * Get the next incoming RPC call.  This blocks until the next incoming RPC
 * call is received.  This is intended to be used with NT_CreatePolledRpc(void);
 * RPC calls created using NT_CreateRpc() will not be serviced through this
 * function.  Upon successful return, NT_PostRpcResponse() must be called to
 * send the return value to the caller.  The returned array must be freed
 * using NT_DisposeRpcAnswerArray().
 *
 * @param poller      poller handle
 * @param len         length of returned array (output)
 * @return Array of RPC call information.  Only returns NULL if an error
 *         occurred (e.g. the instance was invalid or is shutting down).
 */
struct NT_RpcAnswer* NT_PollRpc(NT_RpcCallPoller poller, size_t* len);

/**
 * Get the next incoming RPC call.  This blocks until the next incoming RPC
 * call is received or it times out.  This is intended to be used with
 * NT_CreatePolledRpc(); RPC calls created using NT_CreateRpc() will not be
 * serviced through this function.  Upon successful return,
 * NT_PostRpcResponse() must be called to send the return value to the caller.
 * The returned array must be freed using NT_DisposeRpcAnswerArray().
 *
 * @param poller      poller handle
 * @param len         length of returned array (output)
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Array of RPC call information.  If NULL is returned and timed_out
 *         is also false, an error occurred (e.g. the instance was invalid or
 *         is shutting down).
 */
struct NT_RpcAnswer* NT_PollRpcTimeout(NT_RpcCallPoller poller, size_t* len,
                                       double timeout, NT_Bool* timed_out);

/**
 * Cancel a PollRpc call.  This wakes up a call to PollRpc for this poller
 * and causes it to immediately return an empty array.
 *
 * @param poller  poller handle
 */
void NT_CancelPollRpc(NT_RpcCallPoller poller);

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
NT_Bool NT_WaitForRpcCallQueue(NT_Inst inst, double timeout);

/**
 * Post RPC response (return value) for a polled RPC.
 *
 * The rpc and call parameters should come from the NT_RpcAnswer returned
 * by NT_PollRpc().
 *
 * @param entry       entry handle of RPC entry (from NT_RpcAnswer)
 * @param call        RPC call handle (from NT_RpcAnswer)
 * @param result      result raw data that will be provided to remote caller
 * @param result_len  length of result in bytes
 * @return            true if the response was posted, otherwise false
 */
NT_Bool NT_PostRpcResponse(NT_Entry entry, NT_RpcCall call, const char* result,
                           size_t result_len);

/**
 * Call a RPC function.  May be used on either the client or server.
 *
 * This function is non-blocking.  Either NT_GetRpcResult() or
 * NT_CancelRpcResult() must be called to either get or ignore the result of
 * the call.
 *
 * @param entry       entry handle of RPC entry
 * @param params      parameter
 * @param params_len  length of param in bytes
 * @return RPC call handle (for use with NT_GetRpcResult() or
 *         NT_CancelRpcResult()).
 */
NT_RpcCall NT_CallRpc(NT_Entry entry, const char* params, size_t params_len);

/**
 * Get the result (return value) of a RPC call.  This function blocks until
 * the result is received.
 *
 * @param entry       entry handle of RPC entry
 * @param call        RPC call handle returned by NT_CallRpc()
 * @param result_len  length of returned result in bytes
 * @return NULL on error, or result.
 */
char* NT_GetRpcResult(NT_Entry entry, NT_RpcCall call, size_t* result_len);

/**
 * Get the result (return value) of a RPC call.  This function blocks until
 * the result is received or it times out.
 *
 * @param entry       entry handle of RPC entry
 * @param call        RPC call handle returned by NT_CallRpc()
 * @param result_len  length of returned result in bytes
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return NULL on error or timeout, or result.
 */
char* NT_GetRpcResultTimeout(NT_Entry entry, NT_RpcCall call,
                             size_t* result_len, double timeout,
                             NT_Bool* timed_out);

/**
 * Ignore the result of a RPC call.  This function is non-blocking.
 *
 * @param entry       entry handle of RPC entry
 * @param call        RPC call handle returned by NT_CallRpc()
 */
void NT_CancelRpcResult(NT_Entry entry, NT_RpcCall call);

/**
 * Pack a RPC version 1 definition.
 *
 * @param def         RPC version 1 definition
 * @param packed_len  length of return value in bytes
 * @return Raw packed bytes.  Use C standard library std::free() to release.
 */
char* NT_PackRpcDefinition(const struct NT_RpcDefinition* def,
                           size_t* packed_len);

/**
 * Unpack a RPC version 1 definition.  This can be used for introspection or
 * validation.
 *
 * @param packed      raw packed bytes
 * @param packed_len  length of packed in bytes
 * @param def         RPC version 1 definition (output)
 * @return True if successfully unpacked, false otherwise.
 */
NT_Bool NT_UnpackRpcDefinition(const char* packed, size_t packed_len,
                               struct NT_RpcDefinition* def);

/**
 * Pack RPC values as required for RPC version 1 definition messages.
 *
 * @param values      array of values to pack
 * @param values_len  length of values
 * @param packed_len  length of return value in bytes
 * @return Raw packed bytes.  Use C standard library std::free() to release.
 */
char* NT_PackRpcValues(const struct NT_Value** values, size_t values_len,
                       size_t* packed_len);

/**
 * Unpack RPC values as required for RPC version 1 definition messages.
 *
 * @param packed      raw packed bytes
 * @param packed_len  length of packed in bytes
 * @param types       array of data types (as provided in the RPC definition)
 * @param types_len   length of types
 * @return Array of NT_Value's.
 */
struct NT_Value** NT_UnpackRpcValues(const char* packed, size_t packed_len,
                                     const enum NT_Type* types,
                                     size_t types_len);

/** @} */

/**
 * @defgroup ntcore_network_cfunc Client/Server Functions
 * @{
 */

/**
 * Set the network identity of this node.
 * This is the name used during the initial connection handshake, and is
 * visible through NT_ConnectionInfo on the remote node.
 *
 * @param inst      instance handle
 * @param name      identity to advertise
 * @param name_len  length of name in bytes
 */
void NT_SetNetworkIdentity(NT_Inst inst, const char* name, size_t name_len);

/**
 * Get the current network mode.
 *
 * @param inst  instance handle
 * @return Bitmask of NT_NetworkMode.
 */
unsigned int NT_GetNetworkMode(NT_Inst inst);

/**
 * Starts local-only operation.  Prevents calls to NT_StartServer or
 * NT_StartClient from taking effect.  Has no effect if NT_StartServer or
 * NT_StartClient has already been called.
 */
void NT_StartLocal(NT_Inst inst);

/**
 * Stops local-only operation.  NT_StartServer or NT_StartClient can be called
 * after this call to start a server or client.
 */
void NT_StopLocal(NT_Inst inst);

/**
 * Starts a server using the specified filename, listening address, and port.
 *
 * @param inst              instance handle
 * @param persist_filename  the name of the persist file to use (UTF-8 string,
 *                          null terminated)
 * @param listen_address    the address to listen on, or null to listen on any
 *                          address. (UTF-8 string, null terminated)
 * @param port              port to communicate over.
 */
void NT_StartServer(NT_Inst inst, const char* persist_filename,
                    const char* listen_address, unsigned int port);

/**
 * Stops the server if it is running.
 *
 * @param inst  instance handle
 */
void NT_StopServer(NT_Inst inst);

/**
 * Starts a client.  Use NT_SetServer to set the server name and port.
 *
 * @param inst  instance handle
 */
void NT_StartClientNone(NT_Inst inst);

/**
 * Starts a client using the specified server and port
 *
 * @param inst        instance handle
 * @param server_name server name (UTF-8 string, null terminated)
 * @param port        port to communicate over
 */
void NT_StartClient(NT_Inst inst, const char* server_name, unsigned int port);

/**
 * Starts a client using the specified (server, port) combinations.  The
 * client will attempt to connect to each server in round robin fashion.
 *
 * @param inst         instance handle
 * @param count        length of the server_names and ports arrays
 * @param server_names array of server names (each a UTF-8 string, null
 *                     terminated)
 * @param ports        array of ports to communicate over (one for each server)
 */
void NT_StartClientMulti(NT_Inst inst, size_t count, const char** server_names,
                         const unsigned int* ports);

/**
 * Starts a client using commonly known robot addresses for the specified team.
 *
 * @param inst        instance handle
 * @param team        team number
 * @param port        port to communicate over
 */
void NT_StartClientTeam(NT_Inst inst, unsigned int team, unsigned int port);

/**
 * Stops the client if it is running.
 *
 * @param inst  instance handle
 */
void NT_StopClient(NT_Inst inst);

/**
 * Sets server address and port for client (without restarting client).
 *
 * @param inst        instance handle
 * @param server_name server name (UTF-8 string, null terminated)
 * @param port        port to communicate over
 */
void NT_SetServer(NT_Inst inst, const char* server_name, unsigned int port);

/**
 * Sets server addresses for client (without restarting client).
 * The client will attempt to connect to each server in round robin fashion.
 *
 * @param inst         instance handle
 * @param count        length of the server_names and ports arrays
 * @param server_names array of server names (each a UTF-8 string, null
 *                     terminated)
 * @param ports        array of ports to communicate over (one for each server)
 */
void NT_SetServerMulti(NT_Inst inst, size_t count, const char** server_names,
                       const unsigned int* ports);

/**
 * Sets server addresses and port for client (without restarting client).
 * Connects using commonly known robot addresses for the specified team.
 *
 * @param inst        instance handle
 * @param team        team number
 * @param port        port to communicate over
 */
void NT_SetServerTeam(NT_Inst inst, unsigned int team, unsigned int port);

/**
 * Starts requesting server address from Driver Station.
 * This connects to the Driver Station running on localhost to obtain the
 * server IP address.
 *
 * @param inst  instance handle
 * @param port server port to use in combination with IP from DS
 */
void NT_StartDSClient(NT_Inst inst, unsigned int port);

/**
 * Stops requesting server address from Driver Station.
 *
 * @param inst  instance handle
 */
void NT_StopDSClient(NT_Inst inst);

/**
 * Set the periodic update rate.
 * Sets how frequently updates are sent to other nodes over the network.
 *
 * @param inst      instance handle
 * @param interval  update interval in seconds (range 0.01 to 1.0)
 */
void NT_SetUpdateRate(NT_Inst inst, double interval);

/**
 * Flush Entries.
 *
 * Forces an immediate flush of all local entry changes to network.
 * Normally this is done on a regularly scheduled interval (see
 * NT_SetUpdateRate()).
 *
 * Note: flushes are rate limited to avoid excessive network traffic.  If
 * the time between calls is too short, the flush will occur after the minimum
 * time elapses (rather than immediately).
 *
 * @param inst      instance handle
 */
void NT_Flush(NT_Inst inst);

/**
 * Get information on the currently established network connections.
 * If operating as a client, this will return either zero or one values.
 *
 * @param inst  instance handle
 * @param count returns the number of elements in the array
 * @return      array of connection information
 *
 * It is the caller's responsibility to free the array. The
 * NT_DisposeConnectionInfoArray function is useful for this purpose.
 */
struct NT_ConnectionInfo* NT_GetConnections(NT_Inst inst, size_t* count);

/**
 * Return whether or not the instance is connected to another node.
 *
 * @param inst  instance handle
 * @return True if connected.
 */
NT_Bool NT_IsConnected(NT_Inst inst);

/** @} */

/**
 * @defgroup ntcore_file_cfunc File Save/Load Functions
 * @{
 */

/**
 * Save persistent values to a file.  The server automatically does this,
 * but this function provides a way to save persistent values in the same
 * format to a file on either a client or a server.
 *
 * @param inst      instance handle
 * @param filename  filename
 * @return error string, or NULL if successful
 */
const char* NT_SavePersistent(NT_Inst inst, const char* filename);

/**
 * Load persistent values from a file.  The server automatically does this
 * at startup, but this function provides a way to restore persistent values
 * in the same format from a file at any time on either a client or a server.
 *
 * @param inst      instance handle
 * @param filename  filename
 * @param warn      callback function for warnings
 * @return error string, or NULL if successful
 */
const char* NT_LoadPersistent(NT_Inst inst, const char* filename,
                              void (*warn)(size_t line, const char* msg));

/**
 * Save table values to a file.  The file format used is identical to
 * that used for SavePersistent.
 *
 * @param inst        instance handle
 * @param filename    filename
 * @param prefix      save only keys starting with this prefix
 * @param prefix_len  length of prefix in bytes
 * @return error string, or nullptr if successful
 */
const char* NT_SaveEntries(NT_Inst inst, const char* filename,
                           const char* prefix, size_t prefix_len);

/**
 * Load table values from a file.  The file format used is identical to
 * that used for SavePersistent / LoadPersistent.
 *
 * @param inst        instance handle
 * @param filename    filename
 * @param prefix      load only keys starting with this prefix
 * @param prefix_len  length of prefix in bytes
 * @param warn        callback function for warnings
 * @return error string, or nullptr if successful
 */
const char* NT_LoadEntries(NT_Inst inst, const char* filename,
                           const char* prefix, size_t prefix_len,
                           void (*warn)(size_t line, const char* msg));

/** @} */

/**
 * @defgroup ntcore_utility_cfunc Utility Functions
 * @{
 */

/**
 * Frees value memory.
 *
 * @param value   value to free
 */
void NT_DisposeValue(struct NT_Value* value);

/**
 * Initializes a NT_Value.
 * Sets type to NT_UNASSIGNED and clears rest of struct.
 *
 * @param value value to initialize
 */
void NT_InitValue(struct NT_Value* value);

/**
 * Frees string memory.
 *
 * @param str   string to free
 */
void NT_DisposeString(struct NT_String* str);

/**
 * Initializes a NT_String.
 * Sets length to zero and pointer to null.
 *
 * @param str   string to initialize
 */
void NT_InitString(struct NT_String* str);

/**
 * Disposes an entry handle array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeEntryArray(NT_Entry* arr, size_t count);

/**
 * Disposes a connection info array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeConnectionInfoArray(struct NT_ConnectionInfo* arr, size_t count);

/**
 * Disposes an entry info array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeEntryInfoArray(struct NT_EntryInfo* arr, size_t count);

/**
 * Disposes a single entry info (as returned by NT_GetEntryInfoHandle).
 *
 * @param info  pointer to the info to dispose
 */
void NT_DisposeEntryInfo(struct NT_EntryInfo* info);

/**
 * Disposes a Rpc Definition structure.
 *
 * @param def  pointer to the struct to dispose
 */
void NT_DisposeRpcDefinition(struct NT_RpcDefinition* def);

/**
 * Disposes a Rpc Answer array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeRpcAnswerArray(struct NT_RpcAnswer* arr, size_t count);

/**
 * Disposes a Rpc Answer structure.
 *
 * @param answer     pointer to the struct to dispose
 */
void NT_DisposeRpcAnswer(struct NT_RpcAnswer* answer);

/**
 * Disposes an entry notification array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeEntryNotificationArray(struct NT_EntryNotification* arr,
                                      size_t count);

/**
 * Disposes a single entry notification.
 *
 * @param info  pointer to the info to dispose
 */
void NT_DisposeEntryNotification(struct NT_EntryNotification* info);

/**
 * Disposes a connection notification array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeConnectionNotificationArray(
    struct NT_ConnectionNotification* arr, size_t count);

/**
 * Disposes a single connection notification.
 *
 * @param info  pointer to the info to dispose
 */
void NT_DisposeConnectionNotification(struct NT_ConnectionNotification* info);

/**
 * Disposes a log message array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeLogMessageArray(struct NT_LogMessage* arr, size_t count);

/**
 * Disposes a single log message.
 *
 * @param info  pointer to the info to dispose
 */
void NT_DisposeLogMessage(struct NT_LogMessage* info);

/**
 * Returns monotonic current time in 1 us increments.
 * This is the same time base used for entry and connection timestamps.
 * This function is a compatibility wrapper around WPI_Now().
 *
 * @return Timestamp
 */
uint64_t NT_Now(void);

/** @} */

/**
 * @defgroup ntcore_logger_cfunc Logger Functions
 * @{
 */

/**
 * Log function.
 *
 * @param data    data pointer passed to NT_AddLogger()
 * @param msg     message information
 */
typedef void (*NT_LogFunc)(void* data, const struct NT_LogMessage* msg);

/**
 * Add logger callback function.  By default, log messages are sent to stderr;
 * this function sends log messages to the provided callback function instead.
 * The callback function will only be called for log messages with level
 * greater than or equal to min_level and less than or equal to max_level;
 * messages outside this range will be silently ignored.
 *
 * @param inst        instance handle
 * @param data        data pointer to pass to func
 * @param func        log callback function
 * @param min_level   minimum log level
 * @param max_level   maximum log level
 * @return Logger handle
 */
NT_Logger NT_AddLogger(NT_Inst inst, void* data, NT_LogFunc func,
                       unsigned int min_level, unsigned int max_level);

/**
 * Create a log poller.  A poller provides a single queue of poll events.
 * The returned handle must be destroyed with NT_DestroyLoggerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_LoggerPoller NT_CreateLoggerPoller(NT_Inst inst);

/**
 * Destroy a log poller.  This will abort any blocked polling call and prevent
 * additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void NT_DestroyLoggerPoller(NT_LoggerPoller poller);

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
NT_Logger NT_AddPolledLogger(NT_LoggerPoller poller, unsigned int min_level,
                             unsigned int max_level);

/**
 * Get the next log event.  This blocks until the next log occurs.
 *
 * @param poller    poller handle
 * @param len       length of returned array (output)
 * @return Array of information on the log events.  Only returns NULL if an
 *         error occurred (e.g. the instance was invalid or is shutting down).
 */
struct NT_LogMessage* NT_PollLogger(NT_LoggerPoller poller, size_t* len);

/**
 * Get the next log event.  This blocks until the next log occurs or it times
 * out.
 *
 * @param poller      poller handle
 * @param len         length of returned array (output)
 * @param timeout     timeout, in seconds
 * @param timed_out   true if the timeout period elapsed (output)
 * @return Array of information on the log events.  If NULL is returned and
 *         timed_out is also false, an error occurred (e.g. the instance was
 *         invalid or is shutting down).
 */
struct NT_LogMessage* NT_PollLoggerTimeout(NT_LoggerPoller poller, size_t* len,
                                           double timeout, NT_Bool* timed_out);

/**
 * Cancel a PollLogger call.  This wakes up a call to PollLogger for this
 * poller and causes it to immediately return an empty array.
 *
 * @param poller  poller handle
 */
void NT_CancelPollLogger(NT_LoggerPoller poller);

/**
 * Remove a logger.
 *
 * @param logger Logger handle to remove
 */
void NT_RemoveLogger(NT_Logger logger);

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
NT_Bool NT_WaitForLoggerQueue(NT_Inst inst, double timeout);

/** @} */

/**
 * @defgroup ntcore_interop_cfunc Interop Utility Functions
 * @{
 */

/**
 * @defgroup ntcore_memoryallocators_cfunc Memory Allocators
 * @{
 */

/**
 * Allocates an array of chars.
 * Note that the size is the number of elements, and not the
 * specific number of bytes to allocate. That is calculated internally.
 *
 * @param size  the number of elements the array will contain
 * @return      the allocated char array
 *
 * After use, the array should be freed using the NT_FreeCharArray()
 * function.
 */
char* NT_AllocateCharArray(size_t size);

/**
 * Allocates an array of booleans.
 * Note that the size is the number of elements, and not the
 * specific number of bytes to allocate. That is calculated internally.
 *
 * @param size  the number of elements the array will contain
 * @return      the allocated boolean array
 *
 * After use, the array should be freed using the NT_FreeBooleanArray()
 * function.
 */
NT_Bool* NT_AllocateBooleanArray(size_t size);

/**
 * Allocates an array of doubles.
 * Note that the size is the number of elements, and not the
 * specific number of bytes to allocate. That is calculated internally.
 *
 * @param size  the number of elements the array will contain
 * @return      the allocated double array
 *
 * After use, the array should be freed using the NT_FreeDoubleArray()
 * function.
 */
double* NT_AllocateDoubleArray(size_t size);

/**
 * Allocates an array of NT_Strings.
 * Note that the size is the number of elements, and not the
 * specific number of bytes to allocate. That is calculated internally.
 *
 * @param size  the number of elements the array will contain
 * @return      the allocated NT_String array
 *
 * After use, the array should be freed using the NT_FreeStringArray()
 * function.
 */
struct NT_String* NT_AllocateStringArray(size_t size);

/**
 * Frees an array of chars.
 *
 * @param v_boolean  pointer to the char array to free
 */
void NT_FreeCharArray(char* v_char);

/**
 * Frees an array of doubles.
 *
 * @param v_boolean  pointer to the double array to free
 */
void NT_FreeDoubleArray(double* v_double);

/**
 * Frees an array of booleans.
 *
 * @param v_boolean  pointer to the boolean array to free
 */
void NT_FreeBooleanArray(NT_Bool* v_boolean);

/**
 * Frees an array of NT_Strings.
 *
 * @param v_string  pointer to the string array to free
 * @param arr_size  size of the string array to free
 *
 * Note that the individual NT_Strings in the array should NOT be
 * freed before calling this. This function will free all the strings
 * individually.
 */
void NT_FreeStringArray(struct NT_String* v_string, size_t arr_size);

/** @} */

/**
 * @defgroup ntcore_typedgetters_cfunc Typed Getters
 * @{
 */

/**
 * Returns the type of an NT_Value struct.
 * Note that one of the type options is "unassigned".
 *
 * @param value  The NT_Value struct to get the type from.
 * @return       The type of the value, or unassigned if null.
 */
enum NT_Type NT_GetValueType(const struct NT_Value* value);

/**
 * Returns the boolean from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the boolean from
 * @param last_change returns time in ms since the last change in the value
 * @param v_boolean   returns the boolean assigned to the name
 * @return            1 if successful, or 0 if value is null or not a boolean
 */
NT_Bool NT_GetValueBoolean(const struct NT_Value* value, uint64_t* last_change,
                           NT_Bool* v_boolean);

/**
 * Returns the double from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the double from
 * @param last_change returns time in ms since the last change in the value
 * @param v_double    returns the boolean assigned to the name
 * @return            1 if successful, or 0 if value is null or not a double
 */
NT_Bool NT_GetValueDouble(const struct NT_Value* value, uint64_t* last_change,
                          double* v_double);

/**
 * Returns a copy of the string from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the string from
 * @param last_change returns time in ms since the last change in the value
 * @param str_len     returns the length of the string
 * @return            pointer to the string (UTF-8), or null if error
 *
 * It is the caller's responsibility to free the string once its no longer
 * needed. The NT_FreeCharArray() function is useful for this purpose. The
 * returned string is a copy of the string in the value, and must be freed
 * separately.
 */
char* NT_GetValueString(const struct NT_Value* value, uint64_t* last_change,
                        size_t* str_len);

/**
 * Returns a copy of the raw value from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns null.
 *
 * @param value       NT_Value struct to get the string from
 * @param last_change returns time in ms since the last change in the value
 * @param raw_len     returns the length of the string
 * @return            pointer to the raw value (UTF-8), or null if error
 *
 * It is the caller's responsibility to free the raw value once its no longer
 * needed. The NT_FreeCharArray() function is useful for this purpose. The
 * returned string is a copy of the string in the value, and must be freed
 * separately.
 */
char* NT_GetValueRaw(const struct NT_Value* value, uint64_t* last_change,
                     size_t* raw_len);

/**
 * Returns a copy of the boolean array from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns null.
 *
 * @param value       NT_Value struct to get the boolean array from
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the boolean array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeBooleanArray() function is useful for this purpose.
 * The returned array is a copy of the array in the value, and must be
 * freed separately.
 */
NT_Bool* NT_GetValueBooleanArray(const struct NT_Value* value,
                                 uint64_t* last_change, size_t* arr_size);

/**
 * Returns a copy of the double array from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns null.
 *
 * @param value       NT_Value struct to get the double array from
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the double array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeDoubleArray() function is useful for this purpose.
 * The returned array is a copy of the array in the value, and must be
 * freed separately.
 */
double* NT_GetValueDoubleArray(const struct NT_Value* value,
                               uint64_t* last_change, size_t* arr_size);

/**
 * Returns a copy of the NT_String array from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns null.
 *
 * @param value       NT_Value struct to get the NT_String array from
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the NT_String array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeStringArray() function is useful for this purpose.
 * The returned array is a copy of the array in the value, and must be
 * freed separately. Note that the individual NT_Strings should not be freed,
 * but the entire array should be freed at once. The NT_FreeStringArray()
 * function will free all the NT_Strings.
 */
struct NT_String* NT_GetValueStringArray(const struct NT_Value* value,
                                         uint64_t* last_change,
                                         size_t* arr_size);

/**
 * Returns the boolean currently assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns 0.
 *
 * @param entry       entry handle
 * @param last_change returns time in ms since the last change in the value
 * @param v_boolean   returns the boolean assigned to the name
 * @return            1 if successful, or 0 if value is unassigned or not a
 *                    boolean
 */
NT_Bool NT_GetEntryBoolean(NT_Entry entry, uint64_t* last_change,
                           NT_Bool* v_boolean);

/**
 * Returns the double currently assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns 0.
 *
 * @param entry       entry handle
 * @param last_change returns time in ms since the last change in the value
 * @param v_double    returns the double assigned to the name
 * @return            1 if successful, or 0 if value is unassigned or not a
 *                    double
 */
NT_Bool NT_GetEntryDouble(NT_Entry entry, uint64_t* last_change,
                          double* v_double);

/**
 * Returns a copy of the string assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param entry       entry handle
 * @param last_change returns time in ms since the last change in the value
 * @param str_len     returns the length of the string
 * @return            pointer to the string (UTF-8), or null if error
 *
 * It is the caller's responsibility to free the string once its no longer
 * needed. The NT_FreeCharArray() function is useful for this purpose.
 */
char* NT_GetEntryString(NT_Entry entry, uint64_t* last_change, size_t* str_len);

/**
 * Returns a copy of the raw value assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param entry       entry handle
 * @param last_change returns time in ms since the last change in the value
 * @param raw_len     returns the length of the string
 * @return            pointer to the raw value (UTF-8), or null if error
 *
 * It is the caller's responsibility to free the raw value once its no longer
 * needed. The NT_FreeCharArray() function is useful for this purpose.
 */
char* NT_GetEntryRaw(NT_Entry entry, uint64_t* last_change, size_t* raw_len);

/**
 * Returns a copy of the boolean array assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param entry       entry handle
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the boolean array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeBooleanArray() function is useful for this purpose.
 */
NT_Bool* NT_GetEntryBooleanArray(NT_Entry entry, uint64_t* last_change,
                                 size_t* arr_size);

/**
 * Returns a copy of the double array assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param entry       entry handle
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the double array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeDoubleArray() function is useful for this purpose.
 */
double* NT_GetEntryDoubleArray(NT_Entry entry, uint64_t* last_change,
                               size_t* arr_size);

/**
 * Returns a copy of the NT_String array assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param entry       entry handle
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the NT_String array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeStringArray() function is useful for this purpose. Note
 * that the individual NT_Strings should not be freed, but the entire array
 * should be freed at once. The NT_FreeStringArray() function will free all the
 * NT_Strings.
 */
struct NT_String* NT_GetEntryStringArray(NT_Entry entry, uint64_t* last_change,
                                         size_t* arr_size);

/** @} */

/**
 * @defgroup ntcore_setdefault_cfunc Set Default Values
 * @{
 */

/** Set Default Entry Boolean.
 * Sets the default for the specified key to be a boolean.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param default_boolean     value to be set if name does not exist
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryBoolean(NT_Entry entry, uint64_t time,
                                  NT_Bool default_boolean);

/** Set Default Entry Double.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param default_double     value to be set if name does not exist
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryDouble(NT_Entry entry, uint64_t time,
                                 double default_double);

/** Set Default Entry String.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param default_value     value to be set if name does not exist
 * @param default_len       length of value
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryString(NT_Entry entry, uint64_t time,
                                 const char* default_value, size_t default_len);

/** Set Default Entry Raw.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param default_value     value to be set if name does not exist
 * @param default_len       length of value array
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryRaw(NT_Entry entry, uint64_t time,
                              const char* default_value, size_t default_len);

/** Set Default Entry Boolean Array.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param default_value     value to be set if name does not exist
 * @param default_size      size of value array
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryBooleanArray(NT_Entry entry, uint64_t time,
                                       const int* default_value,
                                       size_t default_size);

/** Set Default Entry Double Array.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param default_value     value to be set if name does not exist
 * @param default_size      size of value array
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryDoubleArray(NT_Entry entry, uint64_t time,
                                      const double* default_value,
                                      size_t default_size);

/** Set Default Entry String Array.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param default_value     value to be set if name does not exist
 * @param default_size      size of value array
 * @return 0 on error (value not set), 1 on success
 */
NT_Bool NT_SetDefaultEntryStringArray(NT_Entry entry, uint64_t time,
                                      const struct NT_String* default_value,
                                      size_t default_size);

/** @} */

/**
 * @defgroup ntcore_valuesetters_cfunc Entry Value Setters
 * @{
 */

/** Set Entry Boolean
 * Sets an entry boolean. If the entry name is not currently assigned to a
 * boolean, returns error unless the force parameter is set.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param v_boolean boolean value to set
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryBoolean(NT_Entry entry, uint64_t time, NT_Bool v_boolean,
                           NT_Bool force);

/** Set Entry Double
 * Sets an entry double. If the entry name is not currently assigned to a
 * double, returns error unless the force parameter is set.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param v_double  double value to set
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryDouble(NT_Entry entry, uint64_t time, double v_double,
                          NT_Bool force);

/** Set Entry String
 * Sets an entry string. If the entry name is not currently assigned to a
 * string, returns error unless the force parameter is set.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param str       string to set (UTF-8 string)
 * @param str_len   length of string to write in bytes
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryString(NT_Entry entry, uint64_t time, const char* str,
                          size_t str_len, NT_Bool force);

/** Set Entry Raw
 * Sets the raw value of an entry. If the entry name is not currently assigned
 * to a raw value, returns error unless the force parameter is set.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param raw       raw string to set (UTF-8 string)
 * @param raw_len   length of raw string to write in bytes
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryRaw(NT_Entry entry, uint64_t time, const char* raw,
                       size_t raw_len, NT_Bool force);

/** Set Entry Boolean Array
 * Sets an entry boolean array. If the entry name is not currently assigned to
 * a boolean array, returns error unless the force parameter is set.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param arr       boolean array to write
 * @param size      number of elements in the array
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryBooleanArray(NT_Entry entry, uint64_t time, const int* arr,
                                size_t size, NT_Bool force);

/** Set Entry Double Array
 * Sets an entry double array. If the entry name is not currently assigned to
 * a double array, returns error unless the force parameter is set.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param arr       double array to write
 * @param size      number of elements in the array
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryDoubleArray(NT_Entry entry, uint64_t time, const double* arr,
                               size_t size, NT_Bool force);

/** Set Entry String Array
 * Sets an entry string array. If the entry name is not currently assigned to
 * a string array, returns error unless the force parameter is set.
 *
 * @param entry     entry handle
 * @param time      timestamp
 * @param arr       NT_String array to write
 * @param size      number of elements in the array
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
NT_Bool NT_SetEntryStringArray(NT_Entry entry, uint64_t time,
                               const struct NT_String* arr, size_t size,
                               NT_Bool force);

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // NTCORE_NTCORE_C_H_
