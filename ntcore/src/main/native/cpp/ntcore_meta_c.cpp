// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <optional>
#include <vector>

#include "Value_internal.h"
#include "ntcore_cpp.h"

using namespace nt;
using namespace nt::meta;

static void ConvertToC(const SubscriberOptions& in,
                       NT_Meta_SubscriberOptions* out) {
  out->periodic = in.periodic;
  out->topicsOnly = in.topicsOnly;
  out->sendAll = in.sendAll;
  out->prefixMatch = in.prefixMatch;
}

static void ConvertToC(const TopicPublisher& in, NT_Meta_TopicPublisher* out) {
  ConvertToC(in.client, &out->client);
  out->pubuid = in.pubuid;
}

static void ConvertToC(const TopicSubscriber& in,
                       NT_Meta_TopicSubscriber* out) {
  ConvertToC(in.client, &out->client);
  out->subuid = in.subuid;
  ConvertToC(in.options, &out->options);
}

static void ConvertToC(const ClientPublisher& in,
                       NT_Meta_ClientPublisher* out) {
  out->uid = in.uid;
  ConvertToC(in.topic, &out->topic);
}

static void ConvertToC(const ClientSubscriber& in,
                       NT_Meta_ClientSubscriber* out) {
  out->uid = in.uid;
  out->topics = ConvertToC<WPI_String>(in.topics, &out->topicsCount);
  ConvertToC(in.options, &out->options);
}

static void ConvertToC(const Client& in, NT_Meta_Client* out) {
  ConvertToC(in.id, &out->id);
  ConvertToC(in.conn, &out->conn);
  out->version = in.version;
}

template <typename O, typename I>
static O* ConvertToC(const std::optional<std::vector<I>>& in, size_t* out_len) {
  if (in) {
    if (O* rv = ConvertToC<O>(*in, out_len)) {
      return rv;
    } else {
      return static_cast<O*>(wpi::safe_malloc(0));  // return non-NULL
    }
  } else {
    *out_len = 0;
    return nullptr;
  }
}

extern "C" {

struct NT_Meta_TopicPublisher* NT_Meta_DecodeTopicPublishers(
    const uint8_t* data, size_t size, size_t* count) {
  return ConvertToC<NT_Meta_TopicPublisher>(DecodeTopicPublishers({data, size}),
                                            count);
}

struct NT_Meta_TopicSubscriber* NT_Meta_DecodeTopicSubscribers(
    const uint8_t* data, size_t size, size_t* count) {
  return ConvertToC<NT_Meta_TopicSubscriber>(
      DecodeTopicSubscribers({data, size}), count);
}

struct NT_Meta_ClientPublisher* NT_Meta_DecodeClientPublishers(
    const uint8_t* data, size_t size, size_t* count) {
  return ConvertToC<NT_Meta_ClientPublisher>(
      DecodeClientPublishers({data, size}), count);
}

struct NT_Meta_ClientSubscriber* NT_Meta_DecodeClientSubscribers(
    const uint8_t* data, size_t size, size_t* count) {
  return ConvertToC<NT_Meta_ClientSubscriber>(
      DecodeClientSubscribers({data, size}), count);
}

struct NT_Meta_Client* NT_Meta_DecodeClients(const uint8_t* data, size_t size,
                                             size_t* count) {
  return ConvertToC<NT_Meta_Client>(DecodeClients({data, size}), count);
}

void NT_Meta_FreeTopicPublishers(struct NT_Meta_TopicPublisher* arr,
                                 size_t count) {
  for (size_t i = 0; i < count; ++i) {
    WPI_FreeString(&arr[i].client);
  }
  std::free(arr);
}

void NT_Meta_FreeTopicSubscribers(struct NT_Meta_TopicSubscriber* arr,
                                  size_t count) {
  for (size_t i = 0; i < count; ++i) {
    WPI_FreeString(&arr[i].client);
  }
  std::free(arr);
}

void NT_Meta_FreeClientPublishers(struct NT_Meta_ClientPublisher* arr,
                                  size_t count) {
  for (size_t i = 0; i < count; ++i) {
    WPI_FreeString(&arr[i].topic);
  }
  std::free(arr);
}

void NT_Meta_FreeClientSubscribers(struct NT_Meta_ClientSubscriber* arr,
                                   size_t count) {
  for (size_t i = 0; i < count; ++i) {
    WPI_FreeStringArray(arr[i].topics, arr[i].topicsCount);
  }
  std::free(arr);
}

void NT_Meta_FreeClients(struct NT_Meta_Client* arr, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    WPI_FreeString(&arr[i].id);
    WPI_FreeString(&arr[i].conn);
  }
  std::free(arr);
}

}  // extern "C"
