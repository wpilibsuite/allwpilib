// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

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
typedef NT_Handle NT_ConnectionDataLogger;
typedef NT_Handle NT_ConnectionListener;
typedef NT_Handle NT_ConnectionListenerPoller;
typedef NT_Handle NT_DataLogger;
typedef NT_Handle NT_Entry;
typedef NT_Handle NT_Inst;
typedef NT_Handle NT_Logger;
typedef NT_Handle NT_LoggerPoller;
typedef NT_Handle NT_MultiSubscriber;
typedef NT_Handle NT_Topic;
typedef NT_Handle NT_TopicListener;
typedef NT_Handle NT_TopicListenerPoller;
typedef NT_Handle NT_Subscriber;
typedef NT_Handle NT_Publisher;
typedef NT_Handle NT_ValueListener;
typedef NT_Handle NT_ValueListenerPoller;

/** Default network tables port number (NT3) */
#define NT_DEFAULT_PORT3 1735

/** Default network tables port number (NT4) */
#define NT_DEFAULT_PORT4 5810

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
  NT_RPC = 0x80,
  NT_INTEGER = 0x100,
  NT_FLOAT = 0x200,
  NT_INTEGER_ARRAY = 0x400,
  NT_FLOAT_ARRAY = 0x800
};

/** NetworkTables entry flags. */
enum NT_EntryFlags { NT_PERSISTENT = 0x01, NT_RETAINED = 0x02 };

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

/** Client/server modes */
enum NT_NetworkMode {
  NT_NET_MODE_NONE = 0x00,     /* not running */
  NT_NET_MODE_SERVER = 0x01,   /* running in server mode */
  NT_NET_MODE_CLIENT3 = 0x02,  /* running in NT3 client mode */
  NT_NET_MODE_CLIENT4 = 0x04,  /* running in NT4 client mode */
  NT_NET_MODE_STARTING = 0x08, /* flag for starting (either client or server) */
  NT_NET_MODE_LOCAL = 0x10,    /* running in local-only mode */
};

/** Pub/sub option types */
enum NT_PubSubOptionType {
  NT_PUBSUB_PERIODIC = 1,   /* period between transmissions */
  NT_PUBSUB_SENDALL,        /* all value changes are sent */
  NT_PUBSUB_TOPICSONLY,     /* only send topic changes, no value changes */
  NT_PUBSUB_POLLSTORAGE,    /* polling storage for subscription */
  NT_PUBSUB_KEEPDUPLICATES, /* preserve duplicate values */
};

/** Topic notification flags. */
enum NT_TopicNotifyKind {
  NT_TOPIC_NOTIFY_NONE = 0,
  NT_TOPIC_NOTIFY_IMMEDIATE = 0x01,  /* initial listener addition */
  NT_TOPIC_NOTIFY_PUBLISH = 0x02,    /* initially published */
  NT_TOPIC_NOTIFY_UNPUBLISH = 0x04,  /* no more publishers */
  NT_TOPIC_NOTIFY_PROPERTIES = 0x08, /* properties changed */
};

/** Value notification flags. */
enum NT_ValueNotifyKind {
  NT_VALUE_NOTIFY_NONE = 0,
  NT_VALUE_NOTIFY_IMMEDIATE = 0x01, /* initial listener addition */
  NT_VALUE_NOTIFY_LOCAL = 0x02,     /* changed locally */
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
  int64_t last_change;
  int64_t server_time;
  union {
    NT_Bool v_boolean;
    int64_t v_int;
    float v_float;
    double v_double;
    struct NT_String v_string;
    struct {
      uint8_t* data;
      size_t size;
    } v_raw;
    struct {
      NT_Bool* arr;
      size_t size;
    } arr_boolean;
    struct {
      double* arr;
      size_t size;
    } arr_double;
    struct {
      float* arr;
      size_t size;
    } arr_float;
    struct {
      int64_t* arr;
      size_t size;
    } arr_int;
    struct {
      struct NT_String* arr;
      size_t size;
    } arr_string;
  } data;
};

struct NT_TopicInfo {
  /** Topic handle */
  NT_Topic topic;

  /** Topic name */
  struct NT_String name;

  /** Topic type */
  enum NT_Type type;

  /** Topic type string */
  struct NT_String type_str;

  /** Topic properties JSON string */
  struct NT_String properties;
};

/** NetworkTables Connection Information */
struct NT_ConnectionInfo {
  /**
   * The remote identifier (as set on the remote node by NT_StartClient4().
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

/** NetworkTables Topic Notification */
struct NT_TopicNotification {
  /** Listener that was triggered. */
  NT_TopicListener listener;

  /** Topic info. */
  struct NT_TopicInfo info;

  /** Update flags. */
  unsigned int flags;
};

/** NetworkTables Value Notification */
struct NT_ValueNotification {
  /** Listener that was triggered. */
  NT_ValueListener listener;

  /** Topic handle. */
  NT_Topic topic;

  /** Subscriber/entry handle. */
  NT_Handle subentry;

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
  char* filename;

  /** The line number in the source file that generated the message. */
  unsigned int line;

  /** The message. */
  char* message;
};

/** NetworkTables publish/subscribe option. */
struct NT_PubSubOption {
  /** Option type. */
  enum NT_PubSubOptionType type;

  /**
   * Option value.  1 (true) or 0 (false) for immediate and logging options,
   * time between updates, in seconds, for periodic option.
   */
  double value;
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
 * Read Entry Queue.
 *
 * Returns new entry values since last call. The returned array must be freed
 * using NT_DisposeValueArray().
 *
 * @param subentry     subscriber or entry handle
 * @param count        count of items in returned array (output)
 * @return entry value array; returns NULL and count=0 if no new values
 */
struct NT_Value* NT_ReadQueueValue(NT_Handle subentry, size_t* count);

/** @} */

/**
 * @defgroup ntcore_topic_cfunc Topic Functions
 * @{
 */

/**
 * Get Published Topic Handles.
 *
 * Returns an array of topic handles.  The results are optionally
 * filtered by string prefix and type to only return a subset of all
 * topics.
 *
 * @param inst          instance handle
 * @param prefix        name required prefix; only topics whose name
 *                      starts with this string are returned
 * @param prefix_len    length of prefix in bytes
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @param count         output parameter; set to length of returned array
 * @return Array of topic handles.
 */
NT_Topic* NT_GetTopics(NT_Inst inst, const char* prefix, size_t prefix_len,
                       unsigned int types, size_t* count);

/**
 * Get Published Topic Handles.
 *
 * Returns an array of topic handles.  The results are optionally
 * filtered by string prefix and type to only return a subset of all
 * topics.
 *
 * @param inst          instance handle
 * @param prefix        name required prefix; only topics whose name
 *                      starts with this string are returned
 * @param prefix_len    length of prefix in bytes
 * @param types         array of type strings
 * @param types_len     number of elements in types array
 * @param count         output parameter; set to length of returned array
 * @return Array of topic handles.
 */
NT_Topic* NT_GetTopicsStr(NT_Inst inst, const char* prefix, size_t prefix_len,
                          const char* const* types, size_t types_len,
                          size_t* count);

/**
 * Get Topics.
 *
 * Returns an array of topic information (handle, name, type).  The results are
 * optionally filtered by string prefix and type to only return a subset
 * of all topics.
 *
 * @param inst          instance handle
 * @param prefix        name required prefix; only topics whose name
 *                      starts with this string are returned
 * @param prefix_len    length of prefix in bytes
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @param count         output parameter; set to length of returned array
 * @return Array of topic information.
 */
struct NT_TopicInfo* NT_GetTopicInfos(NT_Inst inst, const char* prefix,
                                      size_t prefix_len, unsigned int types,
                                      size_t* count);

/**
 * Get Topics.
 *
 * Returns an array of topic information (handle, name, type).  The results are
 * optionally filtered by string prefix and type to only return a subset
 * of all topics.
 *
 * @param inst          instance handle
 * @param prefix        name required prefix; only topics whose name
 *                      starts with this string are returned
 * @param prefix_len    length of prefix in bytes
 * @param types         array of type strings
 * @param types_len     number of elements in types array
 * @param count         output parameter; set to length of returned array
 * @return Array of topic information.
 */
struct NT_TopicInfo* NT_GetTopicInfosStr(NT_Inst inst, const char* prefix,
                                         size_t prefix_len,
                                         const char* const* types,
                                         size_t types_len, size_t* count);

/**
 * Gets Topic Information.
 *
 * Returns information about a topic (name and type).
 *
 * @param topic         handle
 * @param info          information (output)
 * @return True if successful, false on error.
 */
NT_Bool NT_GetTopicInfo(NT_Topic topic, struct NT_TopicInfo* info);

/**
 * Gets Topic Handle.
 *
 * Returns topic handle.
 *
 * @param inst      instance handle
 * @param name      topic name
 * @param name_len  length of topic name in bytes
 * @return Topic handle.
 */
NT_Topic NT_GetTopic(NT_Inst inst, const char* name, size_t name_len);

/**
 * Gets the name of the specified topic.
 *
 * @param topic     topic handle
 * @param name_len  length of topic name (output)
 * @return Topic name; returns NULL and name_len=0 if the handle is invalid.
 */
char* NT_GetTopicName(NT_Topic topic, size_t* name_len);

/**
 * Gets the type for the specified topic, or unassigned if non existent.
 *
 * @param topic   topic handle
 * @return Topic type
 */
enum NT_Type NT_GetTopicType(NT_Topic topic);

/**
 * Gets the type string for the specified topic.  This may have more information
 * than the numeric type (especially for raw values).
 *
 * @param topic     topic handle
 * @param type_len  length of type string (output)
 * @return Topic type string; returns NULL if non-existent
 */
char* NT_GetTopicTypeString(NT_Topic topic, size_t* type_len);

/**
 * Sets the persistent property of a topic.  If true, the stored value is
 * persistent through server restarts.
 *
 * @param topic topic handle
 * @param value True for persistent, false for not persistent.
 */
void NT_SetTopicPersistent(NT_Topic topic, NT_Bool value);

/**
 * Gets the persistent property of a topic.
 *
 * @param topic topic handle
 * @return persistent property value
 */
NT_Bool NT_GetTopicPersistent(NT_Topic topic);

/**
 * Sets the retained property of a topic.  If true, the server retains the
 * topic even when there are no publishers.
 *
 * @param topic topic handle
 * @param value new retained property value
 */
void NT_SetTopicRetained(NT_Topic topic, NT_Bool value);

/**
 * Gets the retained property of a topic.
 *
 * @param topic topic handle
 * @return retained property value
 */
NT_Bool NT_GetTopicRetained(NT_Topic topic);

/**
 * Determine if topic exists (e.g. has at least one publisher).
 *
 * @param handle Topic, entry, or subscriber handle.
 * @return True if topic exists.
 */
NT_Bool NT_GetTopicExists(NT_Handle handle);

/**
 * Gets the current value of a property (as a JSON string).
 *
 * @param topic topic handle
 * @param name property name
 * @param len length of returned string (output)
 * @return JSON string; empty string if the property does not exist.
 */
char* NT_GetTopicProperty(NT_Topic topic, const char* name, size_t* len);

/**
 * Sets a property value.
 *
 * @param topic topic handle
 * @param name property name
 * @param value property value (JSON string)
 */
NT_Bool NT_SetTopicProperty(NT_Topic topic, const char* name,
                            const char* value);

/**
 * Deletes a property.  Has no effect if the property does not exist.
 *
 * @param topic topic handle
 * @param name property name
 */
void NT_DeleteTopicProperty(NT_Topic topic, const char* name);

/**
 * Gets all topic properties as a JSON string.  Each key in the object
 * is the property name, and the corresponding value is the property value.
 *
 * @param topic topic handle
 * @param len length of returned string (output)
 * @return JSON string
 */
char* NT_GetTopicProperties(NT_Topic topic, size_t* len);

/**
 * Updates multiple topic properties.  Each key in the passed-in JSON object is
 * the name of the property to add/update, and the corresponding value is the
 * property value to set for that property.  Null values result in deletion
 * of the corresponding property.
 *
 * @param topic topic handle
 * @param properties JSON object string with keys to add/update/delete
 * @return False if properties are not a valid JSON object
 */
NT_Bool NT_SetTopicProperties(NT_Topic topic, const char* properties);

/**
 * Creates a new subscriber to value changes on a topic.
 *
 * @param topic topic handle
 * @param type expected type
 * @param typeStr expected type string
 * @param options subscription options
 * @param options_len number of elements in options array
 * @return Subscriber handle
 */
NT_Subscriber NT_Subscribe(NT_Topic topic, enum NT_Type type,
                           const char* typeStr,
                           const struct NT_PubSubOption* options,
                           size_t options_len);

/**
 * Stops subscriber.
 *
 * @param sub subscriber handle
 */
void NT_Unsubscribe(NT_Subscriber sub);

/**
 * Creates a new publisher to a topic.
 *
 * @param topic topic handle
 * @param type type
 * @param typeStr type string
 * @param options publish options
 * @param options_len number of elements in options array
 * @return Publisher handle
 */
NT_Publisher NT_Publish(NT_Topic topic, enum NT_Type type, const char* typeStr,
                        const struct NT_PubSubOption* options,
                        size_t options_len);

/**
 * Creates a new publisher to a topic.
 *
 * @param topic topic handle
 * @param type type
 * @param typeStr type string
 * @param properties initial properties (JSON object)
 * @param options publish options
 * @param options_len number of elements in options array
 * @return Publisher handle
 */
NT_Publisher NT_PublishEx(NT_Topic topic, enum NT_Type type,
                          const char* typeStr, const char* properties,
                          const struct NT_PubSubOption* options,
                          size_t options_len);

/**
 * Stops publisher.
 *
 * @param pubentry publisher/entry handle
 */
void NT_Unpublish(NT_Handle pubentry);

/**
 * @brief Creates a new entry (subscriber and weak publisher) to a topic.
 *
 * @param topic topic handle
 * @param type type
 * @param typeStr type string
 * @param options publish options
 * @param options_len number of elements in options array
 * @return Entry handle
 */
NT_Entry NT_GetEntryEx(NT_Topic topic, enum NT_Type type, const char* typeStr,
                       const struct NT_PubSubOption* options,
                       size_t options_len);

/**
 * Stops entry subscriber/publisher.
 *
 * @param entry entry handle
 */
void NT_ReleaseEntry(NT_Entry entry);

/**
 * Stops entry/subscriber/publisher.
 *
 * @param pubsubentry entry/subscriber/publisher handle
 */
void NT_Release(NT_Handle pubsubentry);

/**
 * Gets the topic handle from an entry/subscriber/publisher handle.
 *
 * @param pubsubentry entry/subscriber/publisher handle
 * @return Topic handle
 */
NT_Topic NT_GetTopicFromHandle(NT_Handle pubsubentry);

/** @} */

/**
 * @defgroup ntcore_advancedsub_cfunc Advanced Subscriber Functions
 * @{
 */

/**
 * Subscribes to multiple topics based on one or more topic name prefixes. Can
 * be used in combination with a Value Listener or ReadQueueValue() to get value
 * changes across all matching topics.
 *
 * @param inst instance handle
 * @param prefixes topic name prefixes
 * @param prefixes_len number of elements in prefixes array
 * @param options subscriber options
 * @param options_len number of elements in options array
 * @return subscriber handle
 */
NT_MultiSubscriber NT_SubscribeMultiple(NT_Inst inst,
                                        const struct NT_String* prefixes,
                                        size_t prefixes_len,
                                        const struct NT_PubSubOption* options,
                                        size_t options_len);

/**
 * Unsubscribes a multi-subscriber.
 *
 * @param sub multi-subscriber handle
 */
void NT_UnsubscribeMultiple(NT_MultiSubscriber sub);

/** @} */

/**
 * @defgroup ntcore_topiclistener_cfunc Topic Listener Functions
 * @{
 */

/**
 * Topic listener callback function.
 *
 * @param data            data pointer provided to callback creation function
 * @param event           event info
 */
typedef void (*NT_TopicListenerCallback)(
    void* data, const struct NT_TopicNotification* event);

/**
 * Create a listener for changes to topics with names that start with
 * the given prefix.
 *
 * @param inst Instance handle
 * @param prefix Topic name string prefix
 * @param prefix_len Length of topic name string prefix
 * @param mask Bitmask of NT_TopicListenerFlags values
 * @param data Data passed to callback function
 * @param callback Listener function
 */
NT_TopicListener NT_AddTopicListener(NT_Inst inst, const char* prefix,
                                     size_t prefix_len, unsigned int mask,
                                     void* data,
                                     NT_TopicListenerCallback callback);

/**
 * Create a listener for changes to topics with names that start with any of
 * the given prefixes.
 *
 * @param inst Instance handle
 * @param prefixes Topic name string prefixes
 * @param prefixes_len Number of elements in prefixes array
 * @param mask Bitmask of NT_TopicListenerFlags values
 * @param data Data passed to callback function
 * @param callback Listener function
 */
NT_TopicListener NT_AddTopicListenerMultiple(NT_Inst inst,
                                             const struct NT_String* prefixes,
                                             size_t prefixes_len,
                                             unsigned int mask, void* data,
                                             NT_TopicListenerCallback callback);

/**
 * Create a listener for changes on a particular topic.
 *
 * @param topic Topic handle
 * @param mask Bitmask of NT_TopicListenerFlags values
 * @param data Data passed to callback function
 * @param callback Listener function
 */
NT_TopicListener NT_AddTopicListenerSingle(NT_Topic topic, unsigned int mask,
                                           void* data,
                                           NT_TopicListenerCallback callback);

/**
 * Wait for the topic listener queue to be empty. This is primarily useful
 * for deterministic testing. This blocks until either the topic listener
 * queue is empty (e.g. there are no more events that need to be passed along to
 * callbacks or poll queues) or the timeout expires.
 *
 * @param handle  handle
 * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or a
 *                negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
NT_Bool NT_WaitForTopicListenerQueue(NT_Handle handle, double timeout);

/**
 * Creates a topic listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using NT_AddPolledTopicListener()) will be stored in the queue and
 * must be collected by calling NT_ReadTopicListenerQueue().
 * The returned handle must be destroyed with NT_DestroyTopicListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_TopicListenerPoller NT_CreateTopicListenerPoller(NT_Inst inst);

/**
 * Destroys a topic listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void NT_DestroyTopicListenerPoller(NT_TopicListenerPoller poller);

/**
 * Read topic notifications.
 *
 * @param poller    poller handle
 * @param len       length of returned array (output)
 * @return Array of topic notifications.  Returns NULL and len=0 if no
 *         notifications since last call.
 */
struct NT_TopicNotification* NT_ReadTopicListenerQueue(
    NT_TopicListenerPoller poller, size_t* len);

/**
 * Creates a polled topic listener.
 * The caller is responsible for calling NT_ReadTopicListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param prefix            UTF-8 string prefix
 * @param prefix_len        Length of UTF-8 string prefix
 * @param mask              NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_TopicListener NT_AddPolledTopicListener(NT_TopicListenerPoller poller,
                                           const char* prefix,
                                           size_t prefix_len,
                                           unsigned int mask);

/**
 * Creates a polled topic listener.
 * The caller is responsible for calling NT_ReadTopicListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param prefixes          array of UTF-8 string prefixes
 * @param prefixes_len      Length of prefixes array
 * @param mask              NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_TopicListener NT_AddPolledTopicListenerMultiple(
    NT_TopicListenerPoller poller, const struct NT_String* prefixes,
    size_t prefixes_len, unsigned int mask);

/**
 * Creates a polled topic listener.
 * The caller is responsible for calling NT_ReadTopicListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param topic             topic
 * @param mask              NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_TopicListener NT_AddPolledTopicListenerSingle(NT_TopicListenerPoller poller,
                                                 NT_Topic topic,
                                                 unsigned int mask);

/**
 * Removes a topic listener.
 *
 * @param topic_listener Listener handle to remove
 */
void NT_RemoveTopicListener(NT_TopicListener topic_listener);

/** @} */

/**
 * @defgroup ntcore_valuelistener_cfunc Value Listener Functions
 * @{
 */

/**
 * Value listener callback function.
 *
 * @param data            data pointer provided to callback creation function
 * @param event           event info
 */
typedef void (*NT_ValueListenerCallback)(
    void* data, const struct NT_ValueNotification* event);

/**
 * Create a listener for value changes on a subscriber.
 *
 * @param subentry Subscriber/entry
 * @param mask Bitmask of NT_ValueListenerFlags values
 * @param data Data passed to listener function
 * @param callback Listener function
 */
NT_ValueListener NT_AddValueListener(NT_Handle subentry, unsigned int mask,
                                     void* data,
                                     NT_ValueListenerCallback callback);

/**
 * Wait for the value listener queue to be empty. This is primarily useful
 * for deterministic testing. This blocks until either the value listener
 * queue is empty (e.g. there are no more events that need to be passed along to
 * callbacks or poll queues) or the timeout expires.
 *
 * @param handle  handle
 * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or a
 *                negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
NT_Bool NT_WaitForValueListenerQueue(NT_Handle handle, double timeout);

/**
 * Create a value listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using NT_AddPolledValueListener()) will be stored in the queue and
 * must be collected by calling NT_ReadValueListenerQueue().
 * The returned handle must be destroyed with NT_DestroyValueListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_ValueListenerPoller NT_CreateValueListenerPoller(NT_Inst inst);

/**
 * Destroy a value listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void NT_DestroyValueListenerPoller(NT_ValueListenerPoller poller);

/**
 * Reads value listener queue (all value changes since last call).
 *
 * @param poller    poller handle
 * @param len       length of returned array (output)
 * @return Array of value notifications.  Returns NULL and len=0 if no
 *         notifications since last call.
 */
struct NT_ValueNotification* NT_ReadValueListenerQueue(
    NT_ValueListenerPoller poller, size_t* len);

/**
 * Create a polled value listener.
 * The caller is responsible for calling NT_ReadValueListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param subentry          subscriber or entry handle
 * @param flags             NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_ValueListener NT_AddPolledValueListener(NT_ValueListenerPoller poller,
                                           NT_Handle subentry,
                                           unsigned int flags);

/**
 * Remove a value listener.
 *
 * @param value_listener Listener handle to remove
 */
void NT_RemoveValueListener(NT_ValueListener value_listener);

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
    NT_Inst inst, NT_Bool immediate_notify, void* data,
    NT_ConnectionListenerCallback callback);

/**
 * Wait for the connection listener queue to be empty. This is primarily useful
 * for deterministic testing. This blocks until either the connection listener
 * queue is empty (e.g. there are no more events that need to be passed along to
 * callbacks or poll queues) or the timeout expires.
 *
 * @param handle  handle
 * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or a
 *                negative value to block indefinitely
 * @return False if timed out, otherwise true.
 */
NT_Bool NT_WaitForConnectionListenerQueue(NT_Handle handle, double timeout);

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
struct NT_ConnectionNotification* NT_ReadConnectionListenerQueue(
    NT_ConnectionListenerPoller poller, size_t* len);

/**
 * Remove a connection listener.
 *
 * @param conn_listener Listener handle to remove
 */
void NT_RemoveConnectionListener(NT_ConnectionListener conn_listener);

/** @} */

/**
 * @defgroup ntcore_network_cfunc Client/Server Functions
 * @{
 */

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
 * @param port3             port to communicate over (NT3)
 * @param port4             port to communicate over (NT4)
 */
void NT_StartServer(NT_Inst inst, const char* persist_filename,
                    const char* listen_address, unsigned int port3,
                    unsigned int port4);

/**
 * Stops the server if it is running.
 *
 * @param inst  instance handle
 */
void NT_StopServer(NT_Inst inst);

/**
 * Starts a NT3 client.  Use NT_SetServer or NT_SetServerTeam to set the server
 * name and port.
 *
 * @param inst      instance handle
 * @param identity  network identity to advertise (cannot be empty string)
 */
void NT_StartClient3(NT_Inst inst, const char* identity);

/**
 * Starts a NT4 client.  Use NT_SetServer or NT_SetServerTeam to set the server
 * name and port.
 *
 * @param inst      instance handle
 * @param identity  network identity to advertise (cannot be empty string)
 */
void NT_StartClient4(NT_Inst inst, const char* identity);

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
 * @param port  server port to use in combination with IP from DS
 */
void NT_StartDSClient(NT_Inst inst, unsigned int port);

/**
 * Stops requesting server address from Driver Station.
 *
 * @param inst  instance handle
 */
void NT_StopDSClient(NT_Inst inst);

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
void NT_FlushLocal(NT_Inst inst);

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
 * Frees an array of NT_Values.
 *
 * @param arr   pointer to the value array to free
 * @param count number of elements in the array
 *
 * Note that the individual NT_Values in the array should NOT be
 * freed before calling this. This function will free all the values
 * individually.
 */
void NT_DisposeValueArray(struct NT_Value* arr, size_t count);

/**
 * Disposes a connection info array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeConnectionInfoArray(struct NT_ConnectionInfo* arr, size_t count);

/**
 * Disposes a topic info array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeTopicInfoArray(struct NT_TopicInfo* arr, size_t count);

/**
 * Disposes a single topic info (as returned by NT_GetTopicInfo).
 *
 * @param info  pointer to the info to dispose
 */
void NT_DisposeTopicInfo(struct NT_TopicInfo* info);

/**
 * Disposes an topic notification array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeTopicNotificationArray(struct NT_TopicNotification* arr,
                                      size_t count);

/**
 * Disposes a single topic notification.
 *
 * @param info  pointer to the info to dispose
 */
void NT_DisposeTopicNotification(struct NT_TopicNotification* info);

/**
 * Disposes an value notification array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeValueNotificationArray(struct NT_ValueNotification* arr,
                                      size_t count);

/**
 * Disposes a single value notification.
 *
 * @param info  pointer to the info to dispose
 */
void NT_DisposeValueNotification(struct NT_ValueNotification* info);

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
 * This function by default simply wraps WPI_Now(), but if NT_SetNow() is
 * called, this function instead returns the value passed to NT_SetNow();
 * this can be used to reduce overhead.
 *
 * @return Timestamp
 */
uint64_t NT_Now(void);

/**
 * Sets the current timestamp used for timestamping values that do not
 * provide a timestamp (e.g. a value of 0 is passed).  For consistency,
 * it also results in NT_Now() returning the set value.  This should generally
 * be used only if the overhead of calling WPI_Now() is a concern.
 * If used, it should be called periodically with the value of WPI_Now().
 *
 * @param timestamp timestamp (1 us increments)
 */
void NT_SetNow(uint64_t timestamp);

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
struct NT_LogMessage* NT_ReadLoggerQueue(NT_LoggerPoller poller, size_t* len);

/**
 * Remove a logger.
 *
 * @param logger Logger handle to remove
 */
void NT_RemoveLogger(NT_Logger logger);

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
 * Allocates an array of ints.
 * Note that the size is the number of elements, and not the
 * specific number of bytes to allocate. That is calculated internally.
 *
 * @param size  the number of elements the array will contain
 * @return      the allocated double array
 *
 * After use, the array should be freed using the NT_FreeIntArray()
 * function.
 */
int64_t* NT_AllocateIntegerArray(size_t size);

/**
 * Allocates an array of floats.
 * Note that the size is the number of elements, and not the
 * specific number of bytes to allocate. That is calculated internally.
 *
 * @param size  the number of elements the array will contain
 * @return      the allocated double array
 *
 * After use, the array should be freed using the NT_FreeFloatArray()
 * function.
 */
float* NT_AllocateFloatArray(size_t size);

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
 * @param v_char pointer to the char array to free
 */
void NT_FreeCharArray(char* v_char);

/**
 * Frees an array of booleans.
 *
 * @param v_boolean pointer to the boolean array to free
 */
void NT_FreeBooleanArray(NT_Bool* v_boolean);

/**
 * Frees an array of ints.
 *
 * @param v_int pointer to the int array to free
 */
void NT_FreeIntegerArray(int64_t* v_int);

/**
 * Frees an array of floats.
 *
 * @param v_float pointer to the float array to free
 */
void NT_FreeFloatArray(float* v_float);

/**
 * Frees an array of doubles.
 *
 * @param v_double pointer to the double array to free
 */
void NT_FreeDoubleArray(double* v_double);

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
 * Returns the int from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the int from
 * @param last_change returns time in ms since the last change in the value
 * @param v_int       returns the int assigned to the name
 * @return            1 if successful, or 0 if value is null or not an int
 */
NT_Bool NT_GetValueInteger(const struct NT_Value* value, uint64_t* last_change,
                           int64_t* v_int);

/**
 * Returns the float from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the float from
 * @param last_change returns time in ms since the last change in the value
 * @param v_float     returns the float assigned to the name
 * @return            1 if successful, or 0 if value is null or not a float
 */
NT_Bool NT_GetValueFloat(const struct NT_Value* value, uint64_t* last_change,
                         float* v_float);

/**
 * Returns the double from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns 0.
 *
 * @param value       NT_Value struct to get the double from
 * @param last_change returns time in ms since the last change in the value
 * @param v_double    returns the double assigned to the name
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
uint8_t* NT_GetValueRaw(const struct NT_Value* value, uint64_t* last_change,
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
 * Returns a copy of the int array from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns null.
 *
 * @param value       NT_Value struct to get the int array from
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the int array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeIntArray() function is useful for this purpose.
 * The returned array is a copy of the array in the value, and must be
 * freed separately.
 */
int64_t* NT_GetValueIntegerArray(const struct NT_Value* value,
                                 uint64_t* last_change, size_t* arr_size);

/**
 * Returns a copy of the float array from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns null.
 *
 * @param value       NT_Value struct to get the float array from
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the float array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The NT_FreeFloatArray() function is useful for this purpose.
 * The returned array is a copy of the array in the value, and must be
 * freed separately.
 */
float* NT_GetValueFloatArray(const struct NT_Value* value,
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

/** @} */
/** @} */
/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif

#include "ntcore_c_types.h"
