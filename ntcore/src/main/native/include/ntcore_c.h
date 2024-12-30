// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <wpi/string.h>

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct WPI_DataLog;

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
typedef NT_Handle NT_DataLogger;
typedef NT_Handle NT_Entry;
typedef NT_Handle NT_Inst;
typedef NT_Handle NT_Listener;
typedef NT_Handle NT_ListenerPoller;
typedef NT_Handle NT_MultiSubscriber;
typedef NT_Handle NT_Topic;
typedef NT_Handle NT_Subscriber;
typedef NT_Handle NT_Publisher;

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
enum NT_EntryFlags {
  NT_PERSISTENT = 0x01,
  NT_RETAINED = 0x02,
  NT_UNCACHED = 0x04
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

/** Client/server modes */
enum NT_NetworkMode {
  NT_NET_MODE_NONE = 0x00,     /* not running */
  NT_NET_MODE_SERVER = 0x01,   /* running in server mode */
  NT_NET_MODE_CLIENT3 = 0x02,  /* running in NT3 client mode */
  NT_NET_MODE_CLIENT4 = 0x04,  /* running in NT4 client mode */
  NT_NET_MODE_STARTING = 0x08, /* flag for starting (either client or server) */
  NT_NET_MODE_LOCAL = 0x10,    /* running in local-only mode */
};

/** Event notification flags. */
enum NT_EventFlags {
  NT_EVENT_NONE = 0,
  /** Initial listener addition. */
  NT_EVENT_IMMEDIATE = 0x01,
  /** Client connected (on server, any client connected). */
  NT_EVENT_CONNECTED = 0x02,
  /** Client disconnected (on server, any client disconnected). */
  NT_EVENT_DISCONNECTED = 0x04,
  /** Any connection event (connect or disconnect). */
  NT_EVENT_CONNECTION = NT_EVENT_CONNECTED | NT_EVENT_DISCONNECTED,
  /** New topic published. */
  NT_EVENT_PUBLISH = 0x08,
  /** Topic unpublished. */
  NT_EVENT_UNPUBLISH = 0x10,
  /** Topic properties changed. */
  NT_EVENT_PROPERTIES = 0x20,
  /** Any topic event (publish, unpublish, or properties changed). */
  NT_EVENT_TOPIC = NT_EVENT_PUBLISH | NT_EVENT_UNPUBLISH | NT_EVENT_PROPERTIES,
  /** Topic value updated (via network). */
  NT_EVENT_VALUE_REMOTE = 0x40,
  /** Topic value updated (local). */
  NT_EVENT_VALUE_LOCAL = 0x80,
  /** Topic value updated (network or local). */
  NT_EVENT_VALUE_ALL = NT_EVENT_VALUE_REMOTE | NT_EVENT_VALUE_LOCAL,
  /** Log message. */
  NT_EVENT_LOGMESSAGE = 0x100,
  /** Time synchronized with server. */
  NT_EVENT_TIMESYNC = 0x200,
};

/*
 * Structures
 */

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
    struct WPI_String v_string;
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
      struct WPI_String* arr;
      size_t size;
    } arr_string;
  } data;
};

/** NetworkTables Topic Information */
struct NT_TopicInfo {
  /** Topic handle */
  NT_Topic topic;

  /** Topic name */
  struct WPI_String name;

  /** Topic type */
  enum NT_Type type;

  /** Topic type string */
  struct WPI_String type_str;

  /** Topic properties JSON string */
  struct WPI_String properties;
};

/** NetworkTables Connection Information */
struct NT_ConnectionInfo {
  /**
   * The remote identifier (as set on the remote node by NT_StartClient4().
   */
  struct WPI_String remote_id;

  /** The IP address of the remote node. */
  struct WPI_String remote_ip;

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

/** NetworkTables value event data. */
struct NT_ValueEventData {
  /** Topic handle. */
  NT_Topic topic;

  /** Subscriber/entry handle. */
  NT_Handle subentry;

  /** The new value. */
  struct NT_Value value;
};

/** NetworkTables log message. */
struct NT_LogMessage {
  /** Log level of the message.  See NT_LogLevel. */
  unsigned int level;

  /** The filename of the source file that generated the message. */
  struct WPI_String filename;

  /** The line number in the source file that generated the message. */
  unsigned int line;

  /** The message. */
  struct WPI_String message;
};

/** NetworkTables time sync event data. */
struct NT_TimeSyncEventData {
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
  NT_Bool valid;
};

/** NetworkTables event */
struct NT_Event {
  /** Listener that triggered this event. */
  NT_Handle listener;

  /**
   * Event flags (NT_EventFlags). Also indicates the data included with the
   * event:
   * - NT_EVENT_CONNECTED or NT_EVENT_DISCONNECTED: connInfo
   * - NT_EVENT_PUBLISH, NT_EVENT_UNPUBLISH, or NT_EVENT_PROPERTIES: topicInfo
   * - NT_EVENT_VALUE_REMOTE, NT_NOTIFY_VALUE_LOCAL: valueData
   * - NT_EVENT_LOGMESSAGE: logMessage
   * - NT_EVENT_TIMESYNC: timeSyncData
   */
  unsigned int flags;

  /** Event data; content depends on flags. */
  union {
    struct NT_ConnectionInfo connInfo;
    struct NT_TopicInfo topicInfo;
    struct NT_ValueEventData valueData;
    struct NT_LogMessage logMessage;
    struct NT_TimeSyncEventData timeSyncData;
  } data;
};

/** NetworkTables publish/subscribe options. */
struct NT_PubSubOptions {
  /**
   * Structure size. Must be set to sizeof(NT_PubSubOptions).
   */
  unsigned int structSize;

  /**
   * Polling storage size for a subscription. Specifies the maximum number of
   * updates NetworkTables should store between calls to the subscriber's
   * ReadQueue() function. If zero, defaults to 1 if sendAll is false, 20 if
   * sendAll is true.
   */
  unsigned int pollStorage;

  /**
   * How frequently changes will be sent over the network, in seconds.
   * NetworkTables may send more frequently than this (e.g. use a combined
   * minimum period for all values) or apply a restricted range to this value.
   * The default is 100 ms.
   */
  double periodic;

  /**
   * For subscriptions, if non-zero, value updates for ReadQueue() are not
   * queued for this publisher.
   */
  NT_Publisher excludePublisher;

  /**
   * Send all value changes over the network.
   */
  NT_Bool sendAll;

  /**
   * For subscriptions, don't ask for value changes (only topic announcements).
   */
  NT_Bool topicsOnly;

  /**
   * Perform prefix match on subscriber topic names. Is ignored/overridden by
   * Subscribe() functions; only present in struct for the purposes of getting
   * information about subscriptions.
   */
  NT_Bool prefixMatch;

  /**
   * Preserve duplicate value changes (rather than ignoring them).
   */
  NT_Bool keepDuplicates;

  /**
   * For subscriptions, if remote value updates should not be queued for
   * ReadQueue(). See also disableLocal.
   */
  NT_Bool disableRemote;

  /**
   * For subscriptions, if local value updates should not be queued for
   * ReadQueue(). See also disableRemote.
   */
  NT_Bool disableLocal;

  /**
   * For entries, don't queue (for ReadQueue) value updates for the entry's
   * internal publisher.
   */
  NT_Bool excludeSelf;

  /**
   * For subscriptions, don't share the existence of the subscription with the
   * network. Note this means updates will not be received from the network
   * unless another subscription overlaps with this one, and the subscription
   * will not appear in metatopics.
   */
  NT_Bool hidden;
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
 * @return entry handle
 */
NT_Entry NT_GetEntry(NT_Inst inst, const struct WPI_String* name);

/**
 * Gets the name of the specified entry.
 * Returns an empty string if the handle is invalid.
 *
 * @param entry     entry handle
 * @param name      entry name (output parameter)
 */
void NT_GetEntryName(NT_Entry entry, struct WPI_String* name);

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
 * Get Entry Value.
 *
 * Returns copy of current entry value.
 * Note that one of the type options is "unassigned".
 *
 * @param entry     entry handle
 * @param types     bitmask of NT_Type values; 0 is treated specially
 *                  as a "don't care"
 * @param value     storage for returned entry value
 *
 * It is the caller's responsibility to free value once it's no longer
 * needed (the utility function NT_DisposeValue() is useful for this
 * purpose).
 */
void NT_GetEntryValueType(NT_Entry entry, unsigned int types,
                          struct NT_Value* value);

/**
 * Set Default Entry Value.
 *
 * Returns 0 if name exists.
 * Otherwise, sets passed in value, and returns 1.
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

/**
 * Read Entry Queue.
 *
 * Returns new entry values since last call. The returned array must be freed
 * using NT_DisposeValueArray().
 *
 * @param subentry     subscriber or entry handle
 * @param types        bitmask of NT_Type values; 0 is treated specially
 *                     as a "don't care"
 * @param count        count of items in returned array (output)
 * @return entry value array; returns NULL and count=0 if no new values
 */
struct NT_Value* NT_ReadQueueValueType(NT_Handle subentry, unsigned int types,
                                       size_t* count);

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
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @param count         output parameter; set to length of returned array
 * @return Array of topic handles.
 */
NT_Topic* NT_GetTopics(NT_Inst inst, const struct WPI_String* prefix,
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
 * @param types         array of type strings
 * @param types_len     number of elements in types array
 * @param count         output parameter; set to length of returned array
 * @return Array of topic handles.
 */
NT_Topic* NT_GetTopicsStr(NT_Inst inst, const struct WPI_String* prefix,
                          const struct WPI_String* types, size_t types_len,
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
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @param count         output parameter; set to length of returned array
 * @return Array of topic information.
 */
struct NT_TopicInfo* NT_GetTopicInfos(NT_Inst inst,
                                      const struct WPI_String* prefix,
                                      unsigned int types, size_t* count);

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
 * @param types         array of type strings
 * @param types_len     number of elements in types array
 * @param count         output parameter; set to length of returned array
 * @return Array of topic information.
 */
struct NT_TopicInfo* NT_GetTopicInfosStr(NT_Inst inst,
                                         const struct WPI_String* prefix,
                                         const struct WPI_String* types,
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
 * @return Topic handle.
 */
NT_Topic NT_GetTopic(NT_Inst inst, const struct WPI_String* name);

/**
 * Gets the name of the specified topic.
 *
 * @param topic     topic handle
 * @param name  topic name (output); return length of 0 and nullptr if
 * handle is invalid.
 */
void NT_GetTopicName(NT_Topic topic, struct WPI_String* name);

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
 * @param topic topic handle
 * @param type  topic type string (output)
 */
void NT_GetTopicTypeString(NT_Topic topic, struct WPI_String* type);

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
 * Sets the cached property of a topic.  If true, the server and clients will
 * store the latest value, allowing the value to be read (and not just accessed
 * through event queues and listeners).
 *
 * @param topic topic handle
 * @param value True for cached, false for not cached
 */
void NT_SetTopicCached(NT_Topic topic, NT_Bool value);

/**
 * Gets the cached property of a topic.
 *
 * @param topic topic handle
 * @return cached property value
 */
NT_Bool NT_GetTopicCached(NT_Topic topic);

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
 * @param property JSON string (output)
 */
void NT_GetTopicProperty(NT_Topic topic, const struct WPI_String* name,
                         struct WPI_String* property);

/**
 * Sets a property value.
 *
 * @param topic topic handle
 * @param name property name
 * @param value property value (JSON string)
 */
NT_Bool NT_SetTopicProperty(NT_Topic topic, const struct WPI_String* name,
                            const struct WPI_String* value);

/**
 * Deletes a property.  Has no effect if the property does not exist.
 *
 * @param topic topic handle
 * @param name property name
 */
void NT_DeleteTopicProperty(NT_Topic topic, const struct WPI_String* name);

/**
 * Gets all topic properties as a JSON string.  Each key in the object
 * is the property name, and the corresponding value is the property value.
 *
 * @param topic topic handle
 * @param properties JSON string (output)
 */
void NT_GetTopicProperties(NT_Topic topic, struct WPI_String* properties);

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
NT_Bool NT_SetTopicProperties(NT_Topic topic,
                              const struct WPI_String* properties);

/**
 * Creates a new subscriber to value changes on a topic.
 *
 * @param topic topic handle
 * @param type expected type
 * @param typeStr expected type string
 * @param options subscription options
 * @return Subscriber handle
 */
NT_Subscriber NT_Subscribe(NT_Topic topic, enum NT_Type type,
                           const struct WPI_String* typeStr,
                           const struct NT_PubSubOptions* options);

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
 * @return Publisher handle
 */
NT_Publisher NT_Publish(NT_Topic topic, enum NT_Type type,
                        const struct WPI_String* typeStr,
                        const struct NT_PubSubOptions* options);

/**
 * Creates a new publisher to a topic.
 *
 * @param topic topic handle
 * @param type type
 * @param typeStr type string
 * @param properties initial properties (JSON object)
 * @param options publish options
 * @return Publisher handle
 */
NT_Publisher NT_PublishEx(NT_Topic topic, enum NT_Type type,
                          const struct WPI_String* typeStr,
                          const struct WPI_String* properties,
                          const struct NT_PubSubOptions* options);

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
 * @return Entry handle
 */
NT_Entry NT_GetEntryEx(NT_Topic topic, enum NT_Type type,
                       const struct WPI_String* typeStr,
                       const struct NT_PubSubOptions* options);

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
 * @return subscriber handle
 */
NT_MultiSubscriber NT_SubscribeMultiple(NT_Inst inst,
                                        const struct WPI_String* prefixes,
                                        size_t prefixes_len,
                                        const struct NT_PubSubOptions* options);

/**
 * Unsubscribes a multi-subscriber.
 *
 * @param sub multi-subscriber handle
 */
void NT_UnsubscribeMultiple(NT_MultiSubscriber sub);

/** @} */

/**
 * @defgroup ntcore_listener_cfunc Listener Functions
 * @{
 */

/**
 * Event listener callback function.
 *
 * @param data            data pointer provided to callback creation function
 * @param event           event info
 */
typedef void (*NT_ListenerCallback)(void* data, const struct NT_Event* event);

/**
 * Creates a listener poller.
 *
 * A poller provides a single queue of poll events.  Events linked to this
 * poller (using NT_AddPolledXListener()) will be stored in the queue and
 * must be collected by calling NT_ReadListenerQueue().
 * The returned handle must be destroyed with NT_DestroyListenerPoller().
 *
 * @param inst      instance handle
 * @return poller handle
 */
NT_ListenerPoller NT_CreateListenerPoller(NT_Inst inst);

/**
 * Destroys a listener poller.  This will abort any blocked polling
 * call and prevent additional events from being generated for this poller.
 *
 * @param poller    poller handle
 */
void NT_DestroyListenerPoller(NT_ListenerPoller poller);

/**
 * Read notifications.
 *
 * @param poller    poller handle
 * @param len       length of returned array (output)
 * @return Array of events.  Returns NULL and len=0 if no events since last
 *         call.
 */
struct NT_Event* NT_ReadListenerQueue(NT_ListenerPoller poller, size_t* len);

/**
 * Removes a listener.
 *
 * @param listener Listener handle to remove
 */
void NT_RemoveListener(NT_Listener listener);

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
NT_Bool NT_WaitForListenerQueue(NT_Handle handle, double timeout);

/**
 * Create a listener for changes to topics with names that start with
 * the given prefix. This creates a corresponding internal subscriber with the
 * lifetime of the listener.
 *
 * @param inst Instance handle
 * @param prefix Topic name string prefix
 * @param mask Bitmask of NT_EventFlags values (only topic and value events will
 *             be generated)
 * @param data Data passed to callback function
 * @param callback Listener function
 * @return Listener handle
 */
NT_Listener NT_AddListenerSingle(NT_Inst inst, const struct WPI_String* prefix,
                                 unsigned int mask, void* data,
                                 NT_ListenerCallback callback);

/**
 * Create a listener for changes to topics with names that start with any of
 * the given prefixes. This creates a corresponding internal subscriber with the
 * lifetime of the listener.
 *
 * @param inst Instance handle
 * @param prefixes Topic name string prefixes
 * @param prefixes_len Number of elements in prefixes array
 * @param mask Bitmask of NT_EventFlags values (only topic and value events will
 *             be generated)
 * @param data Data passed to callback function
 * @param callback Listener function
 * @return Listener handle
 */
NT_Listener NT_AddListenerMultiple(NT_Inst inst,
                                   const struct WPI_String* prefixes,
                                   size_t prefixes_len, unsigned int mask,
                                   void* data, NT_ListenerCallback callback);

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
 * NT_AddLogger().
 *
 * @param handle Handle
 * @param mask Bitmask of NT_EventFlags values
 * @param data Data passed to callback function
 * @param callback Listener function
 * @return Listener handle
 */
NT_Listener NT_AddListener(NT_Handle handle, unsigned int mask, void* data,
                           NT_ListenerCallback callback);

/**
 * Creates a polled topic listener. This creates a corresponding internal
 * subscriber with the lifetime of the listener.
 * The caller is responsible for calling NT_ReadListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param prefix            UTF-8 string prefix
 * @param mask              NT_EventFlags bitmask (only topic and value events
 * will be generated)
 * @return Listener handle
 */
NT_Listener NT_AddPolledListenerSingle(NT_ListenerPoller poller,
                                       const struct WPI_String* prefix,
                                       unsigned int mask);

/**
 * Creates a polled topic listener. This creates a corresponding internal
 * subscriber with the lifetime of the listener.
 * The caller is responsible for calling NT_ReadListenerQueue() to poll.
 *
 * @param poller            poller handle
 * @param prefixes          array of UTF-8 string prefixes
 * @param prefixes_len      Length of prefixes array
 * @param mask              NT_EventFlags bitmask (only topic and value events
 * will be generated)
 * @return Listener handle
 */
NT_Listener NT_AddPolledListenerMultiple(NT_ListenerPoller poller,
                                         const struct WPI_String* prefixes,
                                         size_t prefixes_len,
                                         unsigned int mask);

/**
 * Creates a polled listener.
 * The caller is responsible for calling NT_ReadListenerQueue() to poll.
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
 * NT_AddPolledLogger().
 *
 * @param poller            poller handle
 * @param handle            handle
 * @param mask              NT_NotifyKind bitmask
 * @return Listener handle
 */
NT_Listener NT_AddPolledListener(NT_ListenerPoller poller, NT_Handle handle,
                                 unsigned int mask);

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
void NT_StartServer(NT_Inst inst, const struct WPI_String* persist_filename,
                    const struct WPI_String* listen_address, unsigned int port3,
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
void NT_StartClient3(NT_Inst inst, const struct WPI_String* identity);

/**
 * Starts a NT4 client.  Use NT_SetServer or NT_SetServerTeam to set the server
 * name and port.
 *
 * @param inst      instance handle
 * @param identity  network identity to advertise (cannot be empty string)
 */
void NT_StartClient4(NT_Inst inst, const struct WPI_String* identity);

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
void NT_SetServer(NT_Inst inst, const struct WPI_String* server_name,
                  unsigned int port);

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
void NT_SetServerMulti(NT_Inst inst, size_t count,
                       const struct WPI_String* server_names,
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
 * Disconnects the client if it's running and connected. This will automatically
 * start reconnection attempts to the current server list.
 *
 * @param inst instance handle
 */
void NT_Disconnect(NT_Inst inst);

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

/**
 * Get the time offset between server time and local time. Add this value to
 * local time to get the estimated equivalent server time. In server mode, this
 * always returns a valid value of 0. In client mode, this returns the time
 * offset only if the client and server are connected and have exchanged
 * synchronization messages. Note the time offset may change over time as it is
 * periodically updated; to receive updates as events, add a listener to the
 * "time sync" event.
 *
 * @param inst instance handle
 * @param valid set to true if the return value is valid, false otherwise
 *              (output)
 * @return Time offset in microseconds (if valid is set to true)
 */
int64_t NT_GetServerTimeOffset(NT_Inst inst, NT_Bool* valid);

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
 * Disposes an event array.
 *
 * @param arr   pointer to the array to dispose
 * @param count number of elements in the array
 */
void NT_DisposeEventArray(struct NT_Event* arr, size_t count);

/**
 * Disposes a single event.
 *
 * @param event  pointer to the event to dispose
 */
void NT_DisposeEvent(struct NT_Event* event);

/**
 * Returns monotonic current time in 1 us increments.
 * This is the same time base used for entry and connection timestamps.
 * This function by default simply wraps WPI_Now(), but if NT_SetNow() is
 * called, this function instead returns the value passed to NT_SetNow();
 * this can be used to reduce overhead.
 *
 * @return Timestamp
 */
int64_t NT_Now(void);

/**
 * Sets the current timestamp used for timestamping values that do not
 * provide a timestamp (e.g. a value of 0 is passed).  For consistency,
 * it also results in NT_Now() returning the set value.  This should generally
 * be used only if the overhead of calling WPI_Now() is a concern.
 * If used, it should be called periodically with the value of WPI_Now().
 *
 * @param timestamp timestamp (1 us increments)
 */
void NT_SetNow(int64_t timestamp);

/** @} */

/**
 * @defgroup ntcore_data_logger_cfunc Data Logger Functions
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
NT_DataLogger NT_StartEntryDataLog(NT_Inst inst, struct WPI_DataLog* log,
                                   const struct WPI_String* prefix,
                                   const struct WPI_String* logPrefix);

/**
 * Stops logging entry changes to a DataLog.
 *
 * @param logger data logger handle
 */
void NT_StopEntryDataLog(NT_DataLogger logger);

/**
 * Starts logging connection changes to a DataLog.
 *
 * @param inst instance handle
 * @param log data log object; lifetime must extend until StopConnectionDataLog
 *            is called or the instance is destroyed
 * @param name data log entry name
 * @return Data logger handle
 */
NT_ConnectionDataLogger NT_StartConnectionDataLog(
    NT_Inst inst, struct WPI_DataLog* log, const struct WPI_String* name);

/**
 * Stops logging connection changes to a DataLog.
 *
 * @param logger data logger handle
 */
void NT_StopConnectionDataLog(NT_ConnectionDataLogger logger);

/** @} */

/**
 * @defgroup ntcore_logger_cfunc Logger Functions
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
 * @param data        data pointer to pass to func
 * @param func        listener callback function
 * @return Listener handle
 */
NT_Listener NT_AddLogger(NT_Inst inst, unsigned int min_level,
                         unsigned int max_level, void* data,
                         NT_ListenerCallback func);

/**
 * Set the log level for a listener poller.  Events will only be generated for
 * log messages with level greater than or equal to min_level and less than or
 * equal to max_level; messages outside this range will be silently ignored.
 *
 * @param poller        poller handle
 * @param min_level     minimum log level
 * @param max_level     maximum log level
 * @return Listener handle
 */
NT_Listener NT_AddPolledLogger(NT_ListenerPoller poller, unsigned int min_level,
                               unsigned int max_level);

/** @} */

/**
 * @defgroup ntcore_schema_cfunc Schema Functions
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
NT_Bool NT_HasSchema(NT_Inst inst, const struct WPI_String* name);

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
 * @param schemaSize Size of schema data
 */
void NT_AddSchema(NT_Inst inst, const struct WPI_String* name,
                  const struct WPI_String* type, const uint8_t* schema,
                  size_t schemaSize);

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
 * Returns a copy of the struct WPI_String array from the NT_Value.
 * If the NT_Value is null, or is assigned to a different type, returns null.
 *
 * @param value       NT_Value struct to get the struct WPI_String array from
 * @param last_change returns time in ms since the last change in the value
 * @param arr_size    returns the number of elements in the array
 * @return            pointer to the struct WPI_String array, or null if error
 *
 * It is the caller's responsibility to free the array once its no longer
 * needed. The WPI_FreeStringArray() function is useful for this purpose.
 * The returned array is a copy of the array in the value, and must be
 * freed separately. Note that the individual struct WPI_Strings should not be
 * freed, but the entire array should be freed at once. The
 * WPI_FreeStringArray() function will free all the struct WPI_Strings.
 */
struct WPI_String* NT_GetValueStringArray(const struct NT_Value* value,
                                          uint64_t* last_change,
                                          size_t* arr_size);

/** @} */
/** @} */
/** @} */

/**
 * @defgroup ntcore_c_meta_api ntcore C meta-topic API
 *
 * Meta-topic decoders for C.
 *
 * @{
 */

/**
 * Subscriber options. Different from PubSubOptions in this reflects only
 * options that are sent over the network.
 */
struct NT_Meta_SubscriberOptions {
  double periodic;
  NT_Bool topicsOnly;
  NT_Bool sendAll;
  NT_Bool prefixMatch;
};

/**
 * Topic publisher (as published via `$pub$<topic>`).
 */
struct NT_Meta_TopicPublisher {
  struct WPI_String client;
  uint64_t pubuid;
};

/**
 * Topic subscriber (as published via `$sub$<topic>`).
 */
struct NT_Meta_TopicSubscriber {
  struct WPI_String client;
  uint64_t subuid;
  struct NT_Meta_SubscriberOptions options;
};

/**
 * Client publisher (as published via `$clientpub$<client>` or `$serverpub`).
 */
struct NT_Meta_ClientPublisher {
  int64_t uid;
  struct WPI_String topic;
};

/**
 * Client subscriber (as published via `$clientsub$<client>` or `$serversub`).
 */
struct NT_Meta_ClientSubscriber {
  int64_t uid;
  size_t topicsCount;
  struct WPI_String* topics;
  struct NT_Meta_SubscriberOptions options;
};

/**
 * Client (as published via `$clients`).
 */
struct NT_Meta_Client {
  struct WPI_String id;
  struct WPI_String conn;
  uint16_t version;
};

/**
 * Decodes `$pub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @param size size of data contents
 * @param count number of elements in returned array (output)
 * @return Array of TopicPublishers, or NULL on decoding error.
 */
struct NT_Meta_TopicPublisher* NT_Meta_DecodeTopicPublishers(
    const uint8_t* data, size_t size, size_t* count);

/**
 * Decodes `$sub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @param size size of data contents
 * @param count number of elements in returned array (output)
 * @return Array of TopicSubscribers, or NULL on decoding error.
 */
struct NT_Meta_TopicSubscriber* NT_Meta_DecodeTopicSubscribers(
    const uint8_t* data, size_t size, size_t* count);

/**
 * Decodes `$clientpub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @param size size of data contents
 * @param count number of elements in returned array (output)
 * @return Array of ClientPublishers, or NULL on decoding error.
 */
struct NT_Meta_ClientPublisher* NT_Meta_DecodeClientPublishers(
    const uint8_t* data, size_t size, size_t* count);

/**
 * Decodes `$clientsub$<topic>` meta-topic data.
 *
 * @param data data contents
 * @param size size of data contents
 * @param count number of elements in returned array (output)
 * @return Array of ClientSubscribers, or NULL on decoding error.
 */
struct NT_Meta_ClientSubscriber* NT_Meta_DecodeClientSubscribers(
    const uint8_t* data, size_t size, size_t* count);

/**
 * Decodes `$clients` meta-topic data.
 *
 * @param data data contents
 * @param size size of data contents
 * @param count number of elements in returned array (output)
 * @return Array of Clients, or NULL on decoding error.
 */
struct NT_Meta_Client* NT_Meta_DecodeClients(const uint8_t* data, size_t size,
                                             size_t* count);

/**
 * Frees an array of NT_Meta_TopicPublisher.
 *
 * @param arr   pointer to the array to free
 * @param count size of the array to free
 */
void NT_Meta_FreeTopicPublishers(struct NT_Meta_TopicPublisher* arr,
                                 size_t count);

/**
 * Frees an array of NT_Meta_TopicSubscriber.
 *
 * @param arr   pointer to the array to free
 * @param count size of the array to free
 */
void NT_Meta_FreeTopicSubscribers(struct NT_Meta_TopicSubscriber* arr,
                                  size_t count);

/**
 * Frees an array of NT_Meta_ClientPublisher.
 *
 * @param arr   pointer to the array to free
 * @param count size of the array to free
 */
void NT_Meta_FreeClientPublishers(struct NT_Meta_ClientPublisher* arr,
                                  size_t count);

/**
 * Frees an array of NT_Meta_ClientSubscriber.
 *
 * @param arr   pointer to the array to free
 * @param count size of the array to free
 */
void NT_Meta_FreeClientSubscribers(struct NT_Meta_ClientSubscriber* arr,
                                   size_t count);

/**
 * Frees an array of NT_Meta_Client.
 *
 * @param arr   pointer to the array to free
 * @param count size of the array to free
 */
void NT_Meta_FreeClients(struct NT_Meta_Client* arr, size_t count);

/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif

#include "ntcore_c_types.h"
