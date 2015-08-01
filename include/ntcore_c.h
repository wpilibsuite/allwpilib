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
enum NT_EntryFlags {
  NT_PERSISTENT = 0x01
};

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
  char *str;

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
      int *arr;
      size_t size;
    } arr_boolean;
    struct {
      double *arr;
      size_t size;
    } arr_double;
    struct {
      struct NT_String *arr;
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
  char *remote_name;
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
  NT_RpcParamDef *params;
  size_t num_results;
  NT_RpcResultDef *results;
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
void NT_GetEntryValue(const char *name, size_t name_len,
                      struct NT_Value *value);

/** Set Entry Value.
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, returns error and does not update value.
 *
 * @param name      entry name (UTF-8 string)
 * @param name_len  length of name in bytes
 * @param value     new entry value
 * @return 0 on error (type mismatch), 1 on success
 */
int NT_SetEntryValue(const char *name, size_t name_len,
                     const struct NT_Value *value);

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
void NT_SetEntryTypeValue(const char *name, size_t name_len,
                          const struct NT_Value *value);

/** Set Entry Flags.
 */
void NT_SetEntryFlags(const char *name, size_t name_len, unsigned int flags);

/** Get Entry Flags.
 */
unsigned int NT_GetEntryFlags(const char *name, size_t name_len);

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
void NT_DeleteEntry(const char *name, size_t name_len);

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
struct NT_EntryInfo *NT_GetEntryInfo(const char *prefix, size_t prefix_len,
                                     unsigned int types, size_t *count);

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

typedef void (*NT_EntryListenerCallback)(
    unsigned int uid, void *data, const char *name, size_t name_len,
    const struct NT_Value *value, int is_new);

typedef void (*NT_ConnectionListenerCallback)(
    unsigned int uid, void *data, int connected,
    const struct NT_ConnectionInfo *conn);

unsigned int NT_AddEntryListener(const char *prefix, size_t prefix_len,
                                 void *data, NT_EntryListenerCallback callback,
                                 int immediate_notify);
void NT_RemoveEntryListener(unsigned int entry_listener_uid);
unsigned int NT_AddConnectionListener(void *data,
                                      NT_ConnectionListenerCallback callback);
void NT_RemoveConnectionListener(unsigned int conn_listener_uid);

/*
 * Remote Procedure Call Functions
 */

typedef NT_Value **(*NT_RpcCallback)(unsigned int uid, void *data,
                                     const char *name, size_t name_len,
                                     const struct NT_Value **params,
                                     size_t params_len, size_t *results_len);

unsigned int NT_CreateRpc(const char *name, size_t name_len,
                          const struct NT_RpcDefinition *def, void *data,
                          NT_RpcCallback callback);
void NT_DeleteRpc(unsigned int rpc_uid);
unsigned int NT_CallRpc(const char *name, size_t name_len,
                        const struct NT_Value **params, size_t params_len);
struct NT_Value **NT_GetRpcResult(unsigned int result_uid, size_t *results_len);

/*
 * Client/Server Functions
 */
void NT_SetNetworkIdentity(const char *name, size_t name_len);
void NT_StartServer(const char *persist_filename, const char *listen_address,
                    unsigned int port);
void NT_StopServer(void);
void NT_StartClient(const char *server_name, unsigned int port);
void NT_StopClient(void);
void NT_SetUpdateRate(double interval);
struct NT_ConnectionInfo *NT_GetConnections(size_t *count);

/*
 * Persistent Functions
 */
/* return error string, or NULL if successful */
const char *NT_SavePersistent(const char *filename);
const char *NT_LoadPersistent(const char *filename,
                              void (*warn)(size_t line, const char *msg));

/*
 * Utility Functions
 */

/* frees value memory */
void NT_DisposeValue(struct NT_Value *value);

/* sets type to unassigned and clears rest of struct */
void NT_InitValue(struct NT_Value *value);

/* frees string memory */
void NT_DisposeString(struct NT_String *str);

/* sets length to zero and pointer to null */
void NT_InitString(struct NT_String *str);

void NT_DisposeConnectionInfoArray(struct NT_ConnectionInfo *arr, size_t count);

/* timestamp */
unsigned long long NT_Now(void);

/* logging */
typedef void (*NT_LogFunc)(unsigned int level, const char *file,
                           unsigned int line, const char *msg);
void NT_SetLogger(NT_LogFunc func, unsigned int min_level);

#ifdef __cplusplus
}
#endif

#endif /* NTCORE_C_H_ */
