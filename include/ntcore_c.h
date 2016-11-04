/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_C_H_
#define NTCORE_C_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/** NetworkTables notififier kinds. */
enum NT_NotifyKind {
  NT_NOTIFY_NONE = 0,
  NT_NOTIFY_IMMEDIATE = 0x01, /* initial listener addition */
  NT_NOTIFY_LOCAL = 0x02,     /* changed locally */
  NT_NOTIFY_NEW = 0x04,       /* newly created entry */
  NT_NOTIFY_DELETE = 0x08,    /* deleted */
  NT_NOTIFY_UPDATE = 0x10,    /* value changed */
  NT_NOTIFY_FLAGS = 0x20      /* flags changed */
};

/*
 * Structures
 */

/** A NetworkTables string. */
struct NT_String {
  /** String contents (UTF-8).
   * The string is NOT required to be zero-terminated.
   * When returned by the library, this is zero-terminated and allocated with
   * malloc().
   */
  char* str;

  /** Length of the string in bytes.  If the string happens to be zero
   * terminated, this does not include the zero-termination.
   */
  size_t len;
};

/** NetworkTables Entry Value.  Note this is a typed union. */
struct NT_Value {
  enum NT_Type type;
  unsigned long long last_change;
  union {
    int v_boolean;
    double v_double;
    struct NT_String v_string;
    struct NT_String v_raw;
    struct {
      int* arr;
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
  /** Entry name */
  struct NT_String name;

  /** Entry type */
  enum NT_Type type;

  /** Entry flags */
  unsigned int flags;

  /** Timestamp of last change to entry (type or value). */
  unsigned long long last_change;
};

/** NetworkTables Connection Information */
struct NT_ConnectionInfo {
  struct NT_String remote_id;
  struct NT_String remote_ip;
  unsigned int remote_port;
  unsigned long long last_update;
  unsigned int protocol_version;
};

/** NetworkTables RPC Parameter Definition */
struct NT_RpcParamDef {
  struct NT_String name;
  struct NT_Value def_value;
};

/** NetworkTables RPC Result Definition */
struct NT_RpcResultDef {
  struct NT_String name;
  enum NT_Type type;
};

/** NetworkTables RPC Definition */
struct NT_RpcDefinition {
  unsigned int version;
  struct NT_String name;
  size_t num_params;
  NT_RpcParamDef* params;
  size_t num_results;
  NT_RpcResultDef* results;
};

/** NetworkTables RPC Call Data */
struct NT_RpcCallInfo {
  unsigned int rpc_id;
  unsigned int call_uid;
  struct NT_String name;
  struct NT_String params;
};

/*
 * Table Functions
 */

/** Get Entry Value.
 * Returns copy of current entry value.
 * Note that one of the type options is "unassigned".
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param value     storage for returned entry value
 *
 * It is the caller's responsibility to free value once it's no longer
 * needed (the utility function NT_DisposeValue() is useful for this
 * purpose).
 */
void NT_GetEntryValue(const char* name, size_t name_len,
                      struct NT_Value* value);

/** Set Default Entry Value.
 * Returns copy of current entry value if it exists.
 * Otherwise, sets passed in value, and returns set value.
 * Note that one of the type options is "unassigned".
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_value     value to be set if name does not exist
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryValue(const char* name, size_t name_len,
                            const struct NT_Value* default_value);

/** Set Entry Value.
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, returns error and does not update value.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param value     new entry value
 * @return 0 on error (type mismatch), 1 on success
 */
int NT_SetEntryValue(const char* name, size_t name_len,
                     const struct NT_Value* value);

/** Set Entry Type and Value.
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, the currently stored entry type is overridden
 * (generally this will generate an Entry Assignment message).
 *
 * This is NOT the preferred method to update a value; generally
 * NT_SetEntryValue() should be used instead, with appropriate error handling.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param value     new entry value
 */
void NT_SetEntryTypeValue(const char* name, size_t name_len,
                          const struct NT_Value* value);

/** Set Entry Flags.
 */
void NT_SetEntryFlags(const char* name, size_t name_len, unsigned int flags);

/** Get Entry Flags.
 */
unsigned int NT_GetEntryFlags(const char* name, size_t name_len);

/** Delete Entry.
 * Deletes an entry.  This is a new feature in version 3.0 of the protocol,
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: NT_GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 */
void NT_DeleteEntry(const char* name, size_t name_len);

/** Delete All Entries.
 * Deletes ALL table entries.  This is a new feature in version 3.0 of the
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: NT_GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 */
void NT_DeleteAllEntries(void);

/** Get Entry Information.
 * Returns an array of entry information (name, entry type,
 * and timestamp of last change to type/value).  The results are optionally
 * filtered by string prefix and entry type to only return a subset of all
 * entries.
 *
 * @param prefix        entry name required prefix; only entries whose name
 *                      starts with this string are returned
 * @param prefix_len    length of prefix in bytes
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @param count         output parameter; set to length of returned array
 * @return Array of entry information.
 */
struct NT_EntryInfo* NT_GetEntryInfo(const char* prefix, size_t prefix_len,
                                     unsigned int types, size_t* count);

/** Flush Entries.
 * Forces an immediate flush of all local entry changes to network.
 * Normally this is done on a regularly scheduled interval (see
 * NT_SetUpdateRate()).
 *
 * Note: flushes are rate limited to avoid excessive network traffic.  If
 * the time between calls is too short, the flush will occur after the minimum
 * time elapses (rather than immediately).
 */
void NT_Flush(void);

/*
 * Callback Creation Functions
 */

void NT_SetListenerOnStart(void (*on_start)(void* data), void* data);
void NT_SetListenerOnExit(void (*on_exit)(void* data), void* data);

typedef void (*NT_EntryListenerCallback)(unsigned int uid, void* data,
                                         const char* name, size_t name_len,
                                         const struct NT_Value* value,
                                         unsigned int flags);

typedef void (*NT_ConnectionListenerCallback)(
    unsigned int uid, void* data, int connected,
    const struct NT_ConnectionInfo* conn);

unsigned int NT_AddEntryListener(const char* prefix, size_t prefix_len,
                                 void* data, NT_EntryListenerCallback callback,
                                 unsigned int flags);
void NT_RemoveEntryListener(unsigned int entry_listener_uid);
unsigned int NT_AddConnectionListener(void* data,
                                      NT_ConnectionListenerCallback callback,
                                      int immediate_notify);
void NT_RemoveConnectionListener(unsigned int conn_listener_uid);

int NT_NotifierDestroyed();

/*
 * Remote Procedure Call Functions
 */

void NT_SetRpcServerOnStart(void (*on_start)(void* data), void* data);
void NT_SetRpcServerOnExit(void (*on_exit)(void* data), void* data);

typedef char* (*NT_RpcCallback)(void* data, const char* name, size_t name_len,
                                const char* params, size_t params_len,
                                size_t* results_len,
                                const struct NT_ConnectionInfo* conn_info);

void NT_CreateRpc(const char* name, size_t name_len, const char* def,
                  size_t def_len, void* data, NT_RpcCallback callback);
void NT_CreatePolledRpc(const char* name, size_t name_len, const char* def,
                        size_t def_len);

int NT_PollRpc(int blocking, struct NT_RpcCallInfo* call_info);
int NT_PollRpcTimeout(int blocking, double time_out,
                      struct NT_RpcCallInfo* call_info);
void NT_PostRpcResponse(unsigned int rpc_id, unsigned int call_uid,
                        const char* result, size_t result_len);

unsigned int NT_CallRpc(const char* name, size_t name_len, const char* params,
                        size_t params_len);
char* NT_GetRpcResult(int blocking, unsigned int call_uid, size_t* result_len);
char* NT_GetRpcResultTimeout(int blocking, unsigned int call_uid,
                             double time_out, size_t* result_len);
void NT_CancelBlockingRpcResult(unsigned int call_uid);

char* NT_PackRpcDefinition(const struct NT_RpcDefinition* def,
                           size_t* packed_len);
int NT_UnpackRpcDefinition(const char* packed, size_t packed_len,
                           struct NT_RpcDefinition* def);
char* NT_PackRpcValues(const struct NT_Value** values, size_t values_len,
                       size_t* packed_len);
struct NT_Value** NT_UnpackRpcValues(const char* packed, size_t packed_len,
                                     const NT_Type* types, size_t types_len);

/*
 * Client/Server Functions
 */
void NT_SetNetworkIdentity(const char* name, size_t name_len);

/** Start Server
 * Starts a server using the specified filename, listening address, and port.
 *
 * @param persist_filename  the name of the persist file to use (UTF-8 string,
 *                          null terminated)
 * @param listen_address    the address to listen on, or null to listen on any
 *                          address. (UTF-8 string, null terminated)
 * @param port              port to communicate over.
 */
void NT_StartServer(const char* persist_filename, const char* listen_address,
                    unsigned int port);

/** Stop Server
 * Stops the server if it is running.
 */
void NT_StopServer(void);

/** Starts Client
 * Starts a client.  Use NT_SetServer to set the server name and port.
 */
void NT_StartClientNone(void);

/** Starts Client
 * Starts a client using the specified server and port
 *
 * @param server_name server name (UTF-8 string, null terminated)
 * @param port        port to communicate over
 *
 */
void NT_StartClient(const char* server_name, unsigned int port);

/** Starts Client
 * Starts a client using the specified (server, port) combinations.  The
 * client will attempt to connect to each server in round robin fashion.
 *
 * @param count        length of the server_names and ports arrays
 * @param server_names array of server names (each a UTF-8 string, null
 *                     terminated)
 * @param ports        array of ports to communicate over (one for each server)
 *
 */
void NT_StartClientMulti(size_t count, const char** server_names,
                         const unsigned int* ports);

/** Stop Client
 * Stops the client if it is running.
 */
void NT_StopClient(void);

/** Sets server address for client (without restarting client).
 *
 * @param server_name server name (UTF-8 string, null terminated)
 * @param port        port to communicate over
 *
 */
void NT_SetServer(const char* server_name, unsigned int port);

/** Sets server addresses for client (without restarting client).
 * The client will attempt to connect to each server in round robin fashion.
 *
 * @param count        length of the server_names and ports arrays
 * @param server_names array of server names (each a UTF-8 string, null
 *                     terminated)
 * @param ports        array of ports to communicate over (one for each server)
 *
 */
void NT_SetServerMulti(size_t count, const char** server_names,
                       const unsigned int* ports);

/** Starts requesting server address from Driver Station.
 * This connects to the Driver Station running on localhost to obtain the
 * server IP address.
 *
 * @param port server port to use in combination with IP from DS
 */
void NT_StartDSClient(unsigned int port);

/** Stops requesting server address from Driver Station. */
void NT_StopDSClient(void);

/** Stop Rpc Server
 * Stops the Rpc server if it is running.
 */
void NT_StopRpcServer(void);

/** Stop Notifier
 * Stops the Notifier (Entry and Connection Listener) thread if it is running.
 */
void NT_StopNotifier(void);

/** Set Update Rate
 * Sets the update rate of the table
 *
 * @param interval  the interval to update the table at (in seconds)
 *
 */
void NT_SetUpdateRate(double interval);

/** Get Connections
 * Gets an array of all the connections in the table.
 *
 * @param count returns the number of elements in the array
 * @return      an array containing all the connections.
 *
 * It is the caller's responsibility to free the array. The
 * NT_DisposeConnectionInfoArray function is useful for this purpose.
 */
struct NT_ConnectionInfo* NT_GetConnections(size_t* count);

/*
 * Persistent Functions
 */
/* return error string, or NULL if successful */
const char* NT_SavePersistent(const char* filename);
const char* NT_LoadPersistent(const char* filename,
                              void (*warn)(size_t line, const char* msg));

/*
 * Utility Functions
 */

/* frees value memory */
void NT_DisposeValue(struct NT_Value* value);

/* sets type to unassigned and clears rest of struct */
void NT_InitValue(struct NT_Value* value);

/* frees string memory */
void NT_DisposeString(struct NT_String* str);

/* sets length to zero and pointer to null */
void NT_InitString(struct NT_String* str);

/* Gets the type for the specified key, or unassigned if non existent. */
enum NT_Type NT_GetType(const char* name, size_t name_len);

/** Dispose Connection Info Array
 * Disposes a connection info array
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 *
 */
void NT_DisposeConnectionInfoArray(struct NT_ConnectionInfo* arr, size_t count);

/** Dispose Entry Info Array
 * Disposes an entry info array
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 *
 */
void NT_DisposeEntryInfoArray(struct NT_EntryInfo* arr, size_t count);

/** Dispose Rpc Definition
 * Disposes a Rpc Definition structure
 *
 * @param def  pointer to the struct to dispose
 *
 */
void NT_DisposeRpcDefinition(struct NT_RpcDefinition* def);

/** Dispose Rpc Call Info
 * Disposes a Rpc Call Info structure
 *
 * @param def  pointer to the struct to dispose
 *
 */
void NT_DisposeRpcCallInfo(struct NT_RpcCallInfo* call_info);

/* timestamp */
unsigned long long NT_Now(void);

/* logging */
typedef void (*NT_LogFunc)(unsigned int level, const char* file,
                           unsigned int line, const char* msg);
void NT_SetLogger(NT_LogFunc func, unsigned int min_level);

/*
 * Interop Utility Functions
 */

/* Memory Allocators */

/** Allocate Character Array
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

/** Allocate Boolean Array
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
int* NT_AllocateBooleanArray(size_t size);

/** Allocate Double Array
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

/** Allocate NT_String Array
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

/** Free Char Array
 * Frees an array of chars.
 *
 * @param v_boolean  pointer to the char array to free
 */
void NT_FreeCharArray(char* v_char);

/** Free Double Array
 * Frees an array of doubles.
 *
 * @param v_boolean  pointer to the double array to free
 */
void NT_FreeDoubleArray(double* v_double);

/** Free Boolean Array
 * Frees an array of booleans.
 *
 * @param v_boolean  pointer to the boolean array to free
 */
void NT_FreeBooleanArray(int* v_boolean);

/** Free String Array
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

/** Get Value Type
 * Returns the type of an NT_Value struct.
 * Note that one of the type options is "unassigned".
 *
 * @param value  The NT_Value struct to get the type from.
 * @return       The type of the value, or unassigned if null.
 */
enum NT_Type NT_GetValueType(const struct NT_Value* value);

/** Get Value Boolean
 * Returns the boolean from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the boolean from
 * @param last_change returns time in ms since the last change in the value
 * @param v_boolean   returns the boolean assigned to the name
 * @return            1 if successful, or 0 if value is null or not a boolean
 */
int NT_GetValueBoolean(const struct NT_Value* value,
                       unsigned long long* last_change, int* v_boolean);

/** Get Value Double
 * Returns the double from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the double from
 * @param last_change returns time in ms since the last change in the value
 * @param v_double    returns the boolean assigned to the name
 * @return            1 if successful, or 0 if value is null or not a double
 */
int NT_GetValueDouble(const struct NT_Value* value,
                      unsigned long long* last_change, double* v_double);

/** Get Value String
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
char* NT_GetValueString(const struct NT_Value* value,
                        unsigned long long* last_change, size_t* str_len);

/** Get Value Raw
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
char* NT_GetValueRaw(const struct NT_Value* value,
                     unsigned long long* last_change, size_t* raw_len);

/** Get Value Boolean Array
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
int* NT_GetValueBooleanArray(const struct NT_Value* value,
                             unsigned long long* last_change, size_t* arr_size);

/** Get Value Double Array
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
                               unsigned long long* last_change,
                               size_t* arr_size);

/** Get Value String Array
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
 * freed seperately. Note that the individual NT_Strings should not be freed,
 * but the entire array should be freed at once. The NT_FreeStringArray()
 * function will free all the NT_Strings.
 */
NT_String* NT_GetValueStringArray(const struct NT_Value* value,
                                  unsigned long long* last_change,
                                  size_t* arr_size);

/** Get Entry Boolean
 * Returns the boolean currently assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns 0.
 *
 * @param name        entry name (UTF-8 string)
 * @param name_len    length of name in bytes
 * @param last_change returns time in ms since the last change in the value
 * @param v_boolean   returns the boolean assigned to the name
 * @return            1 if successful, or 0 if value is unassigned or not a
 *                    boolean
 */
int NT_GetEntryBoolean(const char* name, size_t name_len,
                       unsigned long long* last_change, int* v_boolean);

/** Get Entry Double
 * Returns the double currently assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns 0.
 *
 * @param name        entry name (UTF-8 string)
 * @param name_len    length of name in bytes
 * @param last_change returns time in ms since the last change in the value
 * @param v_double    returns the double assigned to the name
 * @return            1 if successful, or 0 if value is unassigned or not a
 *                    double
 */
int NT_GetEntryDouble(const char* name, size_t name_len,
                      unsigned long long* last_change, double* v_double);

/** Get Entry String
 * Returns a copy of the string assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param name        entry name (UTF-8 string)
 * @param name_len    length of name in bytes
 * @param last_change returns time in ms since the last change in the value
 * @param str_len     returns the length of the string
 * @return            pointer to the string (UTF-8), or null if error
 *
 * It is the caller's responsibility to free the string once its no longer
 * needed. The NT_FreeCharArray() function is useful for this purpose.
 */
char* NT_GetEntryString(const char* name, size_t name_len,
                        unsigned long long* last_change, size_t* str_len);

/** Get Entry Raw
 * Returns a copy of the raw value assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param name        entry name (UTF-8 string)
 * @param name_len    length of name in bytes
 * @param last_change returns time in ms since the last change in the value
 * @param raw_len     returns the length of the string
 * @return            pointer to the raw value (UTF-8), or null if error
 *
 * It is the caller's responsibility to free the raw value once its no longer
 * needed. The NT_FreeCharArray() function is useful for this purpose.
 */
char* NT_GetEntryRaw(const char* name, size_t name_len,
                     unsigned long long* last_change, size_t* raw_len);

/** Get Entry Boolean Array
 * Returns a copy of the boolean array assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param name        entry name (UTF-8 string)
 * @param name_len    length of name in bytes
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the boolean array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeBooleanArray() function is useful for this purpose.
 */
int* NT_GetEntryBooleanArray(const char* name, size_t name_len,
                             unsigned long long* last_change, size_t* arr_size);

/** Get Entry Double Array
 * Returns a copy of the double array assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param name        entry name (UTF-8 string)
 * @param name_len    length of name in bytes
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the double array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeDoubleArray() function is useful for this purpose.
 */
double* NT_GetEntryDoubleArray(const char* name, size_t name_len,
                               unsigned long long* last_change,
                               size_t* arr_size);

/** Get Entry String Array
 * Returns a copy of the NT_String array assigned to the entry name.
 * If the entry name is not currently assigned, or is assigned to a
 * different type, returns null.
 *
 * @param name        entry name (UTF-8 string)
 * @param name_len    length of name in bytes
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
NT_String* NT_GetEntryStringArray(const char* name, size_t name_len,
                                  unsigned long long* last_change,
                                  size_t* arr_size);

/* Set Default Values */

/** Set Default Entry Boolean.
 * Sets the default for the specified key to be a boolean.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_boolean     value to be set if name does not exist
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryBoolean(const char* name, size_t name_len,
                              int default_boolean);

/** Set Default Entry Double.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_double     value to be set if name does not exist
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryDouble(const char* name, size_t name_len,
                             double default_double);

/** Set Default Entry String.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_value     value to be set if name does not exist
 * @param default_len       length of value
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryString(const char* name, size_t name_len,
                             const char* default_value, size_t default_len);

/** Set Default Entry Raw.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_value     value to be set if name does not exist
 * @param default_len       length of value array
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryRaw(const char* name, size_t name_len,
                          const char* default_value, size_t default_len);

/** Set Default Entry Boolean Array.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_value     value to be set if name does not exist
 * @param default_size      size of value array
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryBooleanArray(const char* name, size_t name_len,
                                   const int* default_value,
                                   size_t default_size);

/** Set Default Entry Double Array.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_value     value to be set if name does not exist
 * @param default_size      size of value array
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryDoubleArray(const char* name, size_t name_len,
                                  const double* default_value,
                                  size_t default_size);

/** Set Default Entry String Array.
 * Sets the default for the specified key.
 * If key exists with same type, does not set value. Otherwise
 * sets value to the default.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param default_value     value to be set if name does not exist
 * @param default_size      size of value array
 * @return 0 on error (value not set), 1 on success
 */
int NT_SetDefaultEntryStringArray(const char* name, size_t name_len,
                                  const struct NT_String* default_value,
                                  size_t default_size);

/* Entry Value Setters */

/** Set Entry Boolean
 * Sets an entry boolean. If the entry name is not currently assigned to a
 * boolean, returns error unless the force parameter is set.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param v_boolean boolean value to set
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
int NT_SetEntryBoolean(const char* name, size_t name_len, int v_boolean,
                       int force);

/** Set Entry Double
 * Sets an entry double. If the entry name is not currently assigned to a
 * double, returns error unless the force parameter is set.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param v_double  double value to set
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
int NT_SetEntryDouble(const char* name, size_t name_len, double v_double,
                      int force);

/** Set Entry String
 * Sets an entry string. If the entry name is not currently assigned to a
 * string, returns error unless the force parameter is set.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param str       string to set (UTF-8 string)
 * @param str_len   length of string to write in bytes
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
int NT_SetEntryString(const char* name, size_t name_len, const char* str,
                      size_t str_len, int force);

/** Set Entry Raw
 * Sets the raw value of an entry. If the entry name is not currently assigned
 * to a raw value, returns error unless the force parameter is set.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param raw       raw string to set (UTF-8 string)
 * @param raw_len   length of raw string to write in bytes
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
int NT_SetEntryRaw(const char* name, size_t name_len, const char* raw,
                   size_t raw_len, int force);

/** Set Entry Boolean Array
 * Sets an entry boolean array. If the entry name is not currently assigned to
 * a boolean array, returns error unless the force parameter is set.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param arr       boolean array to write
 * @param size      number of elements in the array
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
int NT_SetEntryBooleanArray(const char* name, size_t name_len, const int* arr,
                            size_t size, int force);

/** Set Entry Double Array
 * Sets an entry double array. If the entry name is not currently assigned to
 * a double array, returns error unless the force parameter is set.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param arr       double array to write
 * @param size      number of elements in the array
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
int NT_SetEntryDoubleArray(const char* name, size_t name_len, const double* arr,
                           size_t size, int force);

/** Set Entry String Array
 * Sets an entry string array. If the entry name is not currently assigned to
 * a string array, returns error unless the force parameter is set.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param arr       NT_String array to write
 * @param size      number of elements in the array
 * @param force     1 to force the entry to get overwritten, otherwise 0
 * @return          0 on error (type mismatch), 1 on success
 */
int NT_SetEntryStringArray(const char* name, size_t name_len,
                           const struct NT_String* arr, size_t size, int force);

#ifdef __cplusplus
}
#endif

#endif /* NTCORE_C_H_ */
