// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string_view>

#include <wpi/MemAlloc.h>
#include <wpi/SmallVector.h>
#include <wpi/json.h>
#include <wpi/timestamp.h>

#include "Value_internal.h"
#include "ntcore.h"

using namespace nt;

// Conversion helpers

static void ConvertToC(const TopicInfo& in, NT_TopicInfo* out) {
  out->topic = in.topic;
  ConvertToC(in.name, &out->name);
  out->type = in.type;
  ConvertToC(in.type_str, &out->type_str);
  ConvertToC(in.properties, &out->properties);
}

static void ConvertToC(const ConnectionInfo& in, NT_ConnectionInfo* out) {
  ConvertToC(in.remote_id, &out->remote_id);
  ConvertToC(in.remote_ip, &out->remote_ip);
  out->remote_port = in.remote_port;
  out->last_update = in.last_update;
  out->protocol_version = in.protocol_version;
}

static void ConvertToC(const TopicNotification& in, NT_TopicNotification* out) {
  out->listener = in.listener;
  ConvertToC(in.info, &out->info);
  out->flags = in.flags;
}

static void ConvertToC(const ValueNotification& in, NT_ValueNotification* out) {
  out->listener = in.listener;
  out->topic = in.topic;
  out->subentry = in.subentry;
  ConvertToC(in.value, &out->value);
  out->flags = in.flags;
}

static void ConvertToC(const ConnectionNotification& in,
                       NT_ConnectionNotification* out) {
  out->listener = in.listener;
  out->connected = in.connected;
  ConvertToC(in.conn, &out->conn);
}

static void ConvertToC(const LogMessage& in, NT_LogMessage* out) {
  out->logger = in.logger;
  out->level = in.level;
  ConvertToC(in.filename, &out->filename);
  out->line = in.line;
  ConvertToC(in.message, &out->message);
}

static void DisposeConnectionInfo(NT_ConnectionInfo* info) {
  std::free(info->remote_id.str);
  std::free(info->remote_ip.str);
}

static void DisposeTopicInfo(NT_TopicInfo* info) {
  std::free(info->name.str);
  std::free(info->type_str.str);
  std::free(info->properties.str);
}

static void DisposeTopicNotification(NT_TopicNotification* info) {
  DisposeTopicInfo(&info->info);
}

static void DisposeValueNotification(NT_ValueNotification* info) {
  NT_DisposeValue(&info->value);
}

static void DisposeConnectionNotification(NT_ConnectionNotification* info) {
  DisposeConnectionInfo(&info->conn);
}

extern "C" {

/*
 * Instance Functions
 */

NT_Inst NT_GetDefaultInstance(void) {
  return nt::GetDefaultInstance();
}

NT_Inst NT_CreateInstance(void) {
  return nt::CreateInstance();
}

void NT_DestroyInstance(NT_Inst inst) {
  return nt::DestroyInstance(inst);
}

NT_Inst NT_GetInstanceFromHandle(NT_Handle handle) {
  return nt::GetInstanceFromHandle(handle);
}

/*
 * Table Functions
 */

NT_Entry NT_GetEntry(NT_Inst inst, const char* name, size_t name_len) {
  return nt::GetEntry(inst, {name, name_len});
}

char* NT_GetEntryName(NT_Entry entry, size_t* name_len) {
  struct NT_String v_name;
  nt::ConvertToC(nt::GetEntryName(entry), &v_name);
  *name_len = v_name.len;
  return v_name.str;
}

enum NT_Type NT_GetEntryType(NT_Entry entry) {
  return nt::GetEntryType(entry);
}

uint64_t NT_GetEntryLastChange(NT_Entry entry) {
  return nt::GetEntryLastChange(entry);
}

void NT_GetEntryValue(NT_Entry entry, struct NT_Value* value) {
  NT_InitValue(value);
  auto v = nt::GetEntryValue(entry);
  if (!v) {
    return;
  }
  ConvertToC(v, value);
}

int NT_SetDefaultEntryValue(NT_Entry entry,
                            const struct NT_Value* default_value) {
  return nt::SetDefaultEntryValue(entry, ConvertFromC(*default_value));
}

int NT_SetEntryValue(NT_Entry entry, const struct NT_Value* value) {
  return nt::SetEntryValue(entry, ConvertFromC(*value));
}

void NT_SetEntryFlags(NT_Entry entry, unsigned int flags) {
  nt::SetEntryFlags(entry, flags);
}

unsigned int NT_GetEntryFlags(NT_Entry entry) {
  return nt::GetEntryFlags(entry);
}

struct NT_Value* NT_ReadQueueValue(NT_Handle subentry, size_t* count) {
  return ConvertToC<NT_Value>(nt::ReadQueueValue(subentry), count);
}

NT_Topic* NT_GetTopics(NT_Inst inst, const char* prefix, size_t prefix_len,
                       unsigned int types, size_t* count) {
  auto info_v = nt::GetTopics(inst, {prefix, prefix_len}, types);
  return ConvertToC<NT_Topic>(info_v, count);
}

NT_Topic* NT_GetTopicsStr(NT_Inst inst, const char* prefix, size_t prefix_len,
                          const char* const* types, size_t types_len,
                          size_t* count) {
  wpi::SmallVector<std::string_view, 4> typesCpp;
  typesCpp.reserve(types_len);
  for (size_t i = 0; i < types_len; ++i) {
    typesCpp.emplace_back(types[i]);
  }
  auto info_v = nt::GetTopics(inst, {prefix, prefix_len}, typesCpp);
  return ConvertToC<NT_Topic>(info_v, count);
}

struct NT_TopicInfo* NT_GetTopicInfos(NT_Inst inst, const char* prefix,
                                      size_t prefix_len, unsigned int types,
                                      size_t* count) {
  auto info_v = nt::GetTopicInfo(inst, {prefix, prefix_len}, types);
  return ConvertToC<NT_TopicInfo>(info_v, count);
}

struct NT_TopicInfo* NT_GetTopicInfosStr(NT_Inst inst, const char* prefix,
                                         size_t prefix_len,
                                         const char* const* types,
                                         size_t types_len, size_t* count) {
  wpi::SmallVector<std::string_view, 4> typesCpp;
  typesCpp.reserve(types_len);
  for (size_t i = 0; i < types_len; ++i) {
    typesCpp.emplace_back(types[i]);
  }
  auto info_v = nt::GetTopicInfo(inst, {prefix, prefix_len}, typesCpp);
  return ConvertToC<NT_TopicInfo>(info_v, count);
}

NT_Bool NT_GetTopicInfo(NT_Topic topic, struct NT_TopicInfo* info) {
  auto info_v = nt::GetTopicInfo(topic);
  if (info_v.name.empty()) {
    return false;
  }
  ConvertToC(info_v, info);
  return true;
}

NT_Topic NT_GetTopic(NT_Inst inst, const char* name, size_t name_len) {
  return nt::GetTopic(inst, std::string_view{name, name_len});
}

char* NT_GetTopicName(NT_Topic topic, size_t* name_len) {
  auto name = nt::GetTopicName(topic);
  if (name.empty()) {
    *name_len = 0;
    return nullptr;
  }
  struct NT_String v_name;
  nt::ConvertToC(name, &v_name);
  *name_len = v_name.len;
  return v_name.str;
}

NT_Type NT_GetTopicType(NT_Topic topic) {
  return nt::GetTopicType(topic);
}

char* NT_GetTopicTypeString(NT_Topic topic, size_t* type_len) {
  auto type = nt::GetTopicTypeString(topic);
  struct NT_String v_type;
  nt::ConvertToC(type, &v_type);
  *type_len = v_type.len;
  return v_type.str;
}

void NT_SetTopicPersistent(NT_Topic topic, NT_Bool value) {
  nt::SetTopicPersistent(topic, value);
}

NT_Bool NT_GetTopicPersistent(NT_Topic topic) {
  return nt::GetTopicPersistent(topic);
}

void NT_SetTopicRetained(NT_Topic topic, NT_Bool value) {
  nt::SetTopicRetained(topic, value);
}

NT_Bool NT_GetTopicRetained(NT_Topic topic) {
  return nt::GetTopicRetained(topic);
}

NT_Bool NT_GetTopicExists(NT_Handle handle) {
  return nt::GetTopicExists(handle);
}

char* NT_GetTopicProperty(NT_Topic topic, const char* name, size_t* len) {
  wpi::json j = nt::GetTopicProperty(topic, name);
  struct NT_String v;
  nt::ConvertToC(j.dump(), &v);
  *len = v.len;
  return v.str;
}

NT_Bool NT_SetTopicProperty(NT_Topic topic, const char* name,
                            const char* value) {
  wpi::json j;
  try {
    j = wpi::json::parse(value);
  } catch (wpi::json::parse_error&) {
    return false;
  }
  nt::SetTopicProperty(topic, name, j);
  return true;
}

void NT_DeleteTopicProperty(NT_Topic topic, const char* name) {
  nt::DeleteTopicProperty(topic, name);
}

char* NT_GetTopicProperties(NT_Topic topic, size_t* len) {
  wpi::json j = nt::GetTopicProperties(topic);
  struct NT_String v;
  nt::ConvertToC(j.dump(), &v);
  *len = v.len;
  return v.str;
}

NT_Bool NT_SetTopicProperties(NT_Topic topic, const char* properties) {
  wpi::json j;
  try {
    j = wpi::json::parse(properties);
  } catch (wpi::json::parse_error&) {
    return false;
  }
  return nt::SetTopicProperties(topic, j);
}

NT_Subscriber NT_Subscribe(NT_Topic topic, NT_Type type, const char* typeStr,
                           const struct NT_PubSubOption* options,
                           size_t options_len) {
  wpi::SmallVector<nt::PubSubOption> o;
  o.reserve(options_len);
  for (size_t i = 0; i < options_len; ++i) {
    o.emplace_back(options[i].type, options[i].value);
  }
  return nt::Subscribe(topic, type, typeStr, o);
}

void NT_Unsubscribe(NT_Subscriber sub) {
  return nt::Unsubscribe(sub);
}

NT_Publisher NT_Publish(NT_Topic topic, NT_Type type, const char* typeStr,
                        const struct NT_PubSubOption* options,
                        size_t options_len) {
  wpi::SmallVector<nt::PubSubOption> o;
  o.reserve(options_len);
  for (size_t i = 0; i < options_len; ++i) {
    o.emplace_back(options[i].type, options[i].value);
  }
  return nt::Publish(topic, type, typeStr, o);
}

NT_Publisher NT_PublishEx(NT_Topic topic, NT_Type type, const char* typeStr,
                          const char* properties,
                          const struct NT_PubSubOption* options,
                          size_t options_len) {
  wpi::json j;
  if (properties[0] == '\0') {
    // gracefully handle empty string
    j = wpi::json::object();
  } else {
    try {
      j = wpi::json::parse(properties);
    } catch (wpi::json::parse_error&) {
      return {};
    }
  }

  wpi::SmallVector<nt::PubSubOption> o;
  o.reserve(options_len);
  for (size_t i = 0; i < options_len; ++i) {
    o.emplace_back(options[i].type, options[i].value);
  }

  return nt::PublishEx(topic, type, typeStr, j, o);
}

void NT_Unpublish(NT_Handle pubentry) {
  return nt::Unpublish(pubentry);
}

NT_Entry NT_GetEntryEx(NT_Topic topic, NT_Type type, const char* typeStr,
                       const struct NT_PubSubOption* options,
                       size_t options_len) {
  wpi::SmallVector<nt::PubSubOption> o;
  o.reserve(options_len);
  for (size_t i = 0; i < options_len; ++i) {
    o.emplace_back(options[i].type, options[i].value);
  }
  return nt::GetEntry(topic, type, typeStr, o);
}

void NT_ReleaseEntry(NT_Entry entry) {
  nt::ReleaseEntry(entry);
}

void NT_Release(NT_Handle pubsubentry) {
  nt::Release(pubsubentry);
}

NT_Topic NT_GetTopicFromHandle(NT_Handle pubsubentry) {
  return nt::GetTopicFromHandle(pubsubentry);
}

/*
 * Callback Creation Functions
 */

NT_TopicListener NT_AddTopicListener(NT_Inst inst, const char* prefix,
                                     size_t prefix_len, unsigned int mask,
                                     void* data,
                                     NT_TopicListenerCallback callback) {
  std::string_view p{prefix, prefix_len};
  return nt::AddTopicListener(inst, {{p}}, mask, [=](auto& event) {
    NT_TopicNotification event_c;
    ConvertToC(event, &event_c);
    callback(data, &event_c);
    DisposeTopicNotification(&event_c);
  });
}

NT_TopicListener NT_AddTopicListenerMultiple(
    NT_Inst inst, const NT_String* prefixes, size_t prefixes_len,
    unsigned int mask, void* data, NT_TopicListenerCallback callback) {
  wpi::SmallVector<std::string_view, 8> p;
  p.reserve(prefixes_len);
  for (size_t i = 0; i < prefixes_len; ++i) {
    p.emplace_back(prefixes[i].str, prefixes[i].len);
  }
  return nt::AddTopicListener(inst, p, mask, [=](auto& event) {
    NT_TopicNotification event_c;
    ConvertToC(event, &event_c);
    callback(data, &event_c);
    DisposeTopicNotification(&event_c);
  });
}

NT_TopicListener NT_AddTopicListenerSingle(NT_Topic topic, unsigned int mask,
                                           void* data,
                                           NT_TopicListenerCallback callback) {
  return nt::AddTopicListener(topic, mask, [=](auto& event) {
    NT_TopicNotification event_c;
    ConvertToC(event, &event_c);
    callback(data, &event_c);
    DisposeTopicNotification(&event_c);
  });
}

NT_Bool NT_WaitForTopicListenerQueue(NT_Handle handle, double timeout) {
  return nt::WaitForTopicListenerQueue(handle, timeout);
}

NT_TopicListenerPoller NT_CreateTopicListenerPoller(NT_Inst inst) {
  return nt::CreateTopicListenerPoller(inst);
}

void NT_DestroyTopicListenerPoller(NT_TopicListenerPoller poller) {
  nt::DestroyTopicListenerPoller(poller);
}

NT_TopicListener NT_AddPolledTopicListener(NT_TopicListenerPoller poller,
                                           const char* prefix,
                                           size_t prefix_len,
                                           unsigned int mask) {
  std::string_view p{prefix, prefix_len};
  return nt::AddPolledTopicListener(poller, {{p}}, mask);
}

NT_TopicListener NT_AddPolledTopicListenerMultiple(
    NT_TopicListenerPoller poller, const NT_String* prefixes,
    size_t prefixes_len, unsigned int mask) {
  wpi::SmallVector<std::string_view, 8> p;
  p.reserve(prefixes_len);
  for (size_t i = 0; i < prefixes_len; ++i) {
    p.emplace_back(prefixes[i].str, prefixes[i].len);
  }
  return nt::AddPolledTopicListener(poller, p, mask);
}

NT_TopicListener NT_AddPolledTopicListenerSingle(NT_TopicListenerPoller poller,
                                                 NT_Topic topic,
                                                 unsigned int mask) {
  return nt::AddPolledTopicListener(poller, topic, mask);
}

struct NT_TopicNotification* NT_ReadTopicListenerQueue(
    NT_TopicListenerPoller poller, size_t* len) {
  auto arr_cpp = nt::ReadTopicListenerQueue(poller);
  return ConvertToC<NT_TopicNotification>(arr_cpp, len);
}

void NT_RemoveTopicListener(NT_TopicListener topic_listener) {
  nt::RemoveTopicListener(topic_listener);
}

NT_ValueListener NT_AddValueListener(NT_Handle subentry, unsigned int mask,
                                     void* data,
                                     NT_ValueListenerCallback callback) {
  return nt::AddValueListener(subentry, mask, [=](auto& event) {
    NT_ValueNotification event_c;
    ConvertToC(event, &event_c);
    callback(data, &event_c);
    DisposeValueNotification(&event_c);
  });
}

NT_Bool NT_WaitForValueListenerQueue(NT_Handle handle, double timeout) {
  return nt::WaitForValueListenerQueue(handle, timeout);
}

NT_ValueListenerPoller NT_CreateValueListenerPoller(NT_Inst inst) {
  return nt::CreateValueListenerPoller(inst);
}

void NT_DestroyValueListenerPoller(NT_ValueListenerPoller poller) {
  nt::DestroyValueListenerPoller(poller);
}

NT_ValueListener NT_AddPolledValueListener(NT_ValueListenerPoller poller,
                                           NT_Handle subentry,
                                           unsigned int mask) {
  return nt::AddPolledValueListener(poller, subentry, mask);
}

struct NT_ValueNotification* NT_ReadValueListenerQueue(
    NT_ValueListenerPoller poller, size_t* len) {
  auto arr_cpp = nt::ReadValueListenerQueue(poller);
  return ConvertToC<NT_ValueNotification>(arr_cpp, len);
}

void NT_RemoveValueListener(NT_ValueListener value_listener) {
  nt::RemoveValueListener(value_listener);
}

NT_ConnectionListener NT_AddConnectionListener(
    NT_Inst inst, NT_Bool immediate_notify, void* data,
    NT_ConnectionListenerCallback callback) {
  return nt::AddConnectionListener(inst, immediate_notify != 0,
                                   [=](const ConnectionNotification& event) {
                                     NT_ConnectionNotification event_c;
                                     ConvertToC(event, &event_c);
                                     callback(data, &event_c);
                                     DisposeConnectionNotification(&event_c);
                                   });
}

NT_Bool NT_WaitForConnectionListenerQueue(NT_Handle handle, double timeout) {
  return nt::WaitForConnectionListenerQueue(handle, timeout);
}

NT_ConnectionListenerPoller NT_CreateConnectionListenerPoller(NT_Inst inst) {
  return nt::CreateConnectionListenerPoller(inst);
}

void NT_DestroyConnectionListenerPoller(NT_ConnectionListenerPoller poller) {
  nt::DestroyConnectionListenerPoller(poller);
}

NT_ConnectionListener NT_AddPolledConnectionListener(
    NT_ConnectionListenerPoller poller, NT_Bool immediate_notify) {
  return nt::AddPolledConnectionListener(poller, immediate_notify);
}

struct NT_ConnectionNotification* NT_ReadConnectionListenerQueue(
    NT_ConnectionListenerPoller poller, size_t* len) {
  auto arr_cpp = nt::ReadConnectionListenerQueue(poller);
  return ConvertToC<NT_ConnectionNotification>(arr_cpp, len);
}

void NT_RemoveConnectionListener(NT_ConnectionListener conn_listener) {
  nt::RemoveConnectionListener(conn_listener);
}

/*
 * Client/Server Functions
 */

unsigned int NT_GetNetworkMode(NT_Inst inst) {
  return nt::GetNetworkMode(inst);
}

void NT_StartLocal(NT_Inst inst) {
  nt::StartLocal(inst);
}

void NT_StopLocal(NT_Inst inst) {
  nt::StopLocal(inst);
}

void NT_StartServer(NT_Inst inst, const char* persist_filename,
                    const char* listen_address, unsigned int port3,
                    unsigned int port4) {
  nt::StartServer(inst, persist_filename, listen_address, port3, port4);
}

void NT_StopServer(NT_Inst inst) {
  nt::StopServer(inst);
}

void NT_StartClient3(NT_Inst inst, const char* identity) {
  nt::StartClient3(inst, identity);
}

void NT_StartClient4(NT_Inst inst, const char* identity) {
  nt::StartClient4(inst, identity);
}

void NT_StopClient(NT_Inst inst) {
  nt::StopClient(inst);
}

void NT_SetServer(NT_Inst inst, const char* server_name, unsigned int port) {
  nt::SetServer(inst, server_name, port);
}

void NT_SetServerMulti(NT_Inst inst, size_t count, const char** server_names,
                       const unsigned int* ports) {
  std::vector<std::pair<std::string_view, unsigned int>> servers;
  servers.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    servers.emplace_back(std::make_pair(server_names[i], ports[i]));
  }
  nt::SetServer(inst, servers);
}

void NT_SetServerTeam(NT_Inst inst, unsigned int team, unsigned int port) {
  nt::SetServerTeam(inst, team, port);
}

void NT_StartDSClient(NT_Inst inst, unsigned int port) {
  nt::StartDSClient(inst, port);
}

void NT_StopDSClient(NT_Inst inst) {
  nt::StopDSClient(inst);
}

void NT_FlushLocal(NT_Inst inst) {
  nt::FlushLocal(inst);
}

void NT_Flush(NT_Inst inst) {
  nt::Flush(inst);
}

NT_Bool NT_IsConnected(NT_Inst inst) {
  return nt::IsConnected(inst);
}

struct NT_ConnectionInfo* NT_GetConnections(NT_Inst inst, size_t* count) {
  auto conn_v = nt::GetConnections(inst);
  return ConvertToC<NT_ConnectionInfo>(conn_v, count);
}

/*
 * Utility Functions
 */

uint64_t NT_Now(void) {
  return nt::Now();
}

void NT_SetNow(uint64_t timestamp) {
  nt::SetNow(timestamp);
}

NT_Logger NT_AddLogger(NT_Inst inst, void* data, NT_LogFunc func,
                       unsigned int min_level, unsigned int max_level) {
  return nt::AddLogger(
      inst,
      [=](const LogMessage& msg) {
        NT_LogMessage msg_c;
        ConvertToC(msg, &msg_c);
        func(data, &msg_c);
        NT_DisposeLogMessage(&msg_c);
      },
      min_level, max_level);
}

NT_LoggerPoller NT_CreateLoggerPoller(NT_Inst inst) {
  return nt::CreateLoggerPoller(inst);
}

void NT_DestroyLoggerPoller(NT_LoggerPoller poller) {
  nt::DestroyLoggerPoller(poller);
}

NT_Logger NT_AddPolledLogger(NT_LoggerPoller poller, unsigned int min_level,
                             unsigned int max_level) {
  return nt::AddPolledLogger(poller, min_level, max_level);
}

struct NT_LogMessage* NT_ReadLoggerQueue(NT_LoggerPoller poller, size_t* len) {
  auto arr_cpp = nt::ReadLoggerQueue(poller);
  return ConvertToC<NT_LogMessage>(arr_cpp, len);
}

void NT_RemoveLogger(NT_Logger logger) {
  nt::RemoveLogger(logger);
}

void NT_DisposeValue(NT_Value* value) {
  switch (value->type) {
    case NT_UNASSIGNED:
    case NT_BOOLEAN:
    case NT_INTEGER:
    case NT_FLOAT:
    case NT_DOUBLE:
      break;
    case NT_STRING:
      std::free(value->data.v_string.str);
      break;
    case NT_RAW:
      std::free(value->data.v_raw.data);
      break;
    case NT_BOOLEAN_ARRAY:
      std::free(value->data.arr_boolean.arr);
      break;
    case NT_INTEGER_ARRAY:
      std::free(value->data.arr_int.arr);
      break;
    case NT_FLOAT_ARRAY:
      std::free(value->data.arr_float.arr);
      break;
    case NT_DOUBLE_ARRAY:
      std::free(value->data.arr_double.arr);
      break;
    case NT_STRING_ARRAY: {
      for (size_t i = 0; i < value->data.arr_string.size; i++) {
        std::free(value->data.arr_string.arr[i].str);
      }
      std::free(value->data.arr_string.arr);
      break;
    }
    default:
      assert(false && "unknown value type");
  }
  value->type = NT_UNASSIGNED;
  value->last_change = 0;
  value->server_time = 0;
}

void NT_InitValue(NT_Value* value) {
  value->type = NT_UNASSIGNED;
  value->last_change = 0;
  value->server_time = 0;
}

void NT_DisposeString(NT_String* str) {
  std::free(str->str);
  str->str = nullptr;
  str->len = 0;
}

void NT_InitString(NT_String* str) {
  str->str = nullptr;
  str->len = 0;
}

void NT_DisposeValueArray(struct NT_Value* arr, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    NT_DisposeValue(&arr[i]);
  }
  std::free(arr);
}

void NT_DisposeConnectionInfoArray(NT_ConnectionInfo* arr, size_t count) {
  for (size_t i = 0; i < count; i++) {
    DisposeConnectionInfo(&arr[i]);
  }
  std::free(arr);
}

void NT_DisposeTopicInfoArray(NT_TopicInfo* arr, size_t count) {
  for (size_t i = 0; i < count; i++) {
    DisposeTopicInfo(&arr[i]);
  }
  std::free(arr);
}

void NT_DisposeTopicInfo(NT_TopicInfo* info) {
  DisposeTopicInfo(info);
}

void NT_DisposeTopicNotificationArray(NT_TopicNotification* arr, size_t count) {
  for (size_t i = 0; i < count; i++) {
    DisposeTopicNotification(&arr[i]);
  }
  std::free(arr);
}

void NT_DisposeTopicNotification(NT_TopicNotification* info) {
  DisposeTopicNotification(info);
}

void NT_DisposeValueNotificationArray(NT_ValueNotification* arr, size_t count) {
  for (size_t i = 0; i < count; i++) {
    DisposeValueNotification(&arr[i]);
  }
  std::free(arr);
}

void NT_DisposeValueNotification(NT_ValueNotification* info) {
  DisposeValueNotification(info);
}

void NT_DisposeConnectionNotificationArray(NT_ConnectionNotification* arr,
                                           size_t count) {
  for (size_t i = 0; i < count; i++) {
    DisposeConnectionNotification(&arr[i]);
  }
  std::free(arr);
}

void NT_DisposeConnectionNotification(NT_ConnectionNotification* info) {
  DisposeConnectionNotification(info);
}

void NT_DisposeLogMessageArray(NT_LogMessage* arr, size_t count) {
  for (size_t i = 0; i < count; i++) {
    NT_DisposeLogMessage(&arr[i]);
  }
  std::free(arr);
}

void NT_DisposeLogMessage(NT_LogMessage* info) {
  std::free(info->filename);
  std::free(info->message);
}

/* Interop Utility Functions */

/* Array and Struct Allocations */

char* NT_AllocateCharArray(size_t size) {
  char* retVal = static_cast<char*>(wpi::safe_malloc(size * sizeof(char)));
  return retVal;
}

int* NT_AllocateBooleanArray(size_t size) {
  int* retVal = static_cast<int*>(wpi::safe_malloc(size * sizeof(int)));
  return retVal;
}

int64_t* NT_AllocateIntegerArray(size_t size) {
  int64_t* retVal =
      static_cast<int64_t*>(wpi::safe_malloc(size * sizeof(int64_t)));
  return retVal;
}

float* NT_AllocateFloatArray(size_t size) {
  float* retVal = static_cast<float*>(wpi::safe_malloc(size * sizeof(float)));
  return retVal;
}

double* NT_AllocateDoubleArray(size_t size) {
  double* retVal =
      static_cast<double*>(wpi::safe_malloc(size * sizeof(double)));
  return retVal;
}

struct NT_String* NT_AllocateStringArray(size_t size) {
  NT_String* retVal =
      static_cast<NT_String*>(wpi::safe_malloc(size * sizeof(NT_String)));
  return retVal;
}

void NT_FreeCharArray(char* v_char) {
  std::free(v_char);
}
void NT_FreeBooleanArray(int* v_boolean) {
  std::free(v_boolean);
}
void NT_FreeIntegerArray(int64_t* v_int) {
  std::free(v_int);
}
void NT_FreeFloatArray(float* v_float) {
  std::free(v_float);
}
void NT_FreeDoubleArray(double* v_double) {
  std::free(v_double);
}
void NT_FreeStringArray(struct NT_String* v_string, size_t arr_size) {
  for (size_t i = 0; i < arr_size; ++i) {
    std::free(v_string[i].str);
  }
  std::free(v_string);
}

enum NT_Type NT_GetValueType(const struct NT_Value* value) {
  if (!value) {
    return NT_Type::NT_UNASSIGNED;
  }
  return value->type;
}

NT_Bool NT_GetValueBoolean(const struct NT_Value* value, uint64_t* last_change,
                           NT_Bool* v_boolean) {
  if (!value || value->type != NT_Type::NT_BOOLEAN) {
    return 0;
  }
  *v_boolean = value->data.v_boolean;
  *last_change = value->last_change;
  return 1;
}

NT_Bool NT_GetValueInteger(const struct NT_Value* value, uint64_t* last_change,
                           int64_t* v_int) {
  if (!value || value->type != NT_Type::NT_INTEGER) {
    return 0;
  }
  *last_change = value->last_change;
  *v_int = value->data.v_int;
  return 1;
}

NT_Bool NT_GetValueFloat(const struct NT_Value* value, uint64_t* last_change,
                         float* v_float) {
  if (!value || value->type != NT_Type::NT_FLOAT) {
    return 0;
  }
  *last_change = value->last_change;
  *v_float = value->data.v_float;
  return 1;
}

NT_Bool NT_GetValueDouble(const struct NT_Value* value, uint64_t* last_change,
                          double* v_double) {
  if (!value || value->type != NT_Type::NT_DOUBLE) {
    return 0;
  }
  *last_change = value->last_change;
  *v_double = value->data.v_double;
  return 1;
}

char* NT_GetValueString(const struct NT_Value* value, uint64_t* last_change,
                        size_t* str_len) {
  if (!value || value->type != NT_Type::NT_STRING) {
    return nullptr;
  }
  *last_change = value->last_change;
  *str_len = value->data.v_string.len;
  char* str =
      static_cast<char*>(wpi::safe_malloc(value->data.v_string.len + 1));
  std::memcpy(str, value->data.v_string.str, value->data.v_string.len + 1);
  return str;
}

uint8_t* NT_GetValueRaw(const struct NT_Value* value, uint64_t* last_change,
                        size_t* raw_len) {
  if (!value || value->type != NT_Type::NT_RAW) {
    return nullptr;
  }
  *last_change = value->last_change;
  *raw_len = value->data.v_raw.size;
  uint8_t* raw =
      static_cast<uint8_t*>(wpi::safe_malloc(value->data.v_raw.size));
  std::memcpy(raw, value->data.v_raw.data, value->data.v_raw.size);
  return raw;
}

NT_Bool* NT_GetValueBooleanArray(const struct NT_Value* value,
                                 uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_BOOLEAN_ARRAY) {
    return nullptr;
  }
  *last_change = value->last_change;
  *arr_size = value->data.arr_boolean.size;
  NT_Bool* arr = static_cast<int*>(
      wpi::safe_malloc(value->data.arr_boolean.size * sizeof(NT_Bool)));
  std::memcpy(arr, value->data.arr_boolean.arr,
              value->data.arr_boolean.size * sizeof(NT_Bool));
  return arr;
}

int64_t* NT_GetValueIntegerArray(const struct NT_Value* value,
                                 uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_INTEGER_ARRAY) {
    return nullptr;
  }
  *last_change = value->last_change;
  *arr_size = value->data.arr_int.size;
  int64_t* arr = static_cast<int64_t*>(
      wpi::safe_malloc(value->data.arr_int.size * sizeof(int64_t)));
  std::memcpy(arr, value->data.arr_int.arr,
              value->data.arr_int.size * sizeof(int64_t));
  return arr;
}

float* NT_GetValueFloatArray(const struct NT_Value* value,
                             uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_FLOAT_ARRAY) {
    return nullptr;
  }
  *last_change = value->last_change;
  *arr_size = value->data.arr_float.size;
  float* arr = static_cast<float*>(
      wpi::safe_malloc(value->data.arr_float.size * sizeof(float)));
  std::memcpy(arr, value->data.arr_float.arr,
              value->data.arr_float.size * sizeof(float));
  return arr;
}

double* NT_GetValueDoubleArray(const struct NT_Value* value,
                               uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_DOUBLE_ARRAY) {
    return nullptr;
  }
  *last_change = value->last_change;
  *arr_size = value->data.arr_double.size;
  double* arr = static_cast<double*>(
      wpi::safe_malloc(value->data.arr_double.size * sizeof(double)));
  std::memcpy(arr, value->data.arr_double.arr,
              value->data.arr_double.size * sizeof(double));
  return arr;
}

NT_String* NT_GetValueStringArray(const struct NT_Value* value,
                                  uint64_t* last_change, size_t* arr_size) {
  if (!value || value->type != NT_Type::NT_STRING_ARRAY) {
    return nullptr;
  }
  *last_change = value->last_change;
  *arr_size = value->data.arr_string.size;
  NT_String* arr = static_cast<NT_String*>(
      wpi::safe_malloc(value->data.arr_string.size * sizeof(NT_String)));
  for (size_t i = 0; i < value->data.arr_string.size; ++i) {
    size_t len = value->data.arr_string.arr[i].len;
    arr[i].len = len;
    arr[i].str = static_cast<char*>(wpi::safe_malloc(len + 1));
    std::memcpy(arr[i].str, value->data.arr_string.arr[i].str, len + 1);
  }
  return arr;
}

}  // extern "C"
