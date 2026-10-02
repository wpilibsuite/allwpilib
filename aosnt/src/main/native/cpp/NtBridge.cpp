// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/aosnt/NtBridge.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <new>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <upb/base/status.h>
#include <upb/mem/arena.h>
#include <upb/reflection/def.h>
#include <upb/reflection/stage0/google/protobuf/descriptor.upb.h>

#include "aosnt/types/boolean_array_generated.h"
#include "aosnt/types/boolean_generated.h"
#include "aosnt/types/double_array_generated.h"
#include "aosnt/types/double_generated.h"
#include "aosnt/types/float_array_generated.h"
#include "aosnt/types/float_generated.h"
#include "aosnt/types/integer_array_generated.h"
#include "aosnt/types/integer_generated.h"
#include "aosnt/types/string_array_generated.h"
#include "aosnt/types/string_generated.h"
#include "wpi/nt/NetworkTableValue.hpp"

namespace wpi::aosnt {
namespace {

// Initial encode buffer size. A flatbuffer and its protobuf encoding are not
// related in size, so this is a guess, not a bound. Messages that do not fit
// grow the buffer.
constexpr size_t ENCODE_SIZE_FACTOR = 2;
constexpr size_t ENCODE_SIZE_SLACK = 512;

struct ArenaDeleter {
  void operator()(upb_Arena* arena) const { upb_Arena_Free(arena); }
};

struct DefPoolDeleter {
  void operator()(upb_DefPool* pool) const { upb_DefPool_Free(pool); }
};

wpi::nt::PubSubOptions BridgeOptions(const aos::Channel* channel) {
  wpi::nt::PubSubOptions options;
  // AOS delivers every message and never collapses duplicates. Without this,
  // NT's default batching drops messages published faster than it sends.
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 1.0 / channel->frequency();
  return options;
}

// Copies a flatbuffer vector, which is null when the field is unset.
template <typename Element, typename T>
std::vector<Element> ToVector(const flatbuffers::Vector<T>* vector) {
  if (vector == nullptr) {
    return {};
  }
  return std::vector<Element>(vector->begin(), vector->end());
}

// A table in aosnt/types/ and the NetworkTables type it is published as.
struct PrimitiveType {
  std::string_view aosType;
  std::string_view ntType;
  wpi::nt::Value (*toValue)(const uint8_t* message);
};

const std::array<PrimitiveType, 10> PRIMITIVE_TYPES{{
    {"wpi.aosnt.Boolean", "boolean",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeBoolean(
           flatbuffers::GetRoot<Boolean>(message)->value());
     }},
    {"wpi.aosnt.Integer", "int",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeInteger(
           flatbuffers::GetRoot<Integer>(message)->value());
     }},
    {"wpi.aosnt.Float", "float",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeFloat(
           flatbuffers::GetRoot<Float>(message)->value());
     }},
    {"wpi.aosnt.Double", "double",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeDouble(
           flatbuffers::GetRoot<Double>(message)->value());
     }},
    {"wpi.aosnt.String", "string",
     [](const uint8_t* message) {
       const flatbuffers::String* value =
           flatbuffers::GetRoot<String>(message)->value();
       return wpi::nt::Value::MakeString(
           value == nullptr ? std::string_view{} : value->string_view());
     }},
    {"wpi.aosnt.BooleanArray", "boolean[]",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeBooleanArray(
           ToVector<int>(flatbuffers::GetRoot<BooleanArray>(message)->value()));
     }},
    {"wpi.aosnt.IntegerArray", "int[]",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeIntegerArray(ToVector<int64_t>(
           flatbuffers::GetRoot<IntegerArray>(message)->value()));
     }},
    {"wpi.aosnt.FloatArray", "float[]",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeFloatArray(
           ToVector<float>(flatbuffers::GetRoot<FloatArray>(message)->value()));
     }},
    {"wpi.aosnt.DoubleArray", "double[]",
     [](const uint8_t* message) {
       return wpi::nt::Value::MakeDoubleArray(ToVector<double>(
           flatbuffers::GetRoot<DoubleArray>(message)->value()));
     }},
    {"wpi.aosnt.StringArray", "string[]",
     [](const uint8_t* message) {
       const auto* value = flatbuffers::GetRoot<StringArray>(message)->value();
       std::vector<std::string> strings;
       if (value != nullptr) {
         strings.reserve(value->size());
         for (const flatbuffers::String* string : *value) {
           strings.emplace_back(string->string_view());
         }
       }
       return wpi::nt::Value::MakeStringArray(std::move(strings));
     }},
}};

// Returns the primitive type a channel carries, or nullptr for any other type.
const PrimitiveType* FindPrimitiveType(const aos::Channel* channel) {
  const std::string_view type = channel->type()->string_view();
  for (const PrimitiveType& primitive : PRIMITIVE_TYPES) {
    if (primitive.aosType == type) {
      return &primitive;
    }
  }
  return nullptr;
}

}  // namespace

std::string NtBridge::GetTopicName(const aos::Channel* channel) {
  return std::string{channel->name()->string_view()};
}

std::string NtBridge::GetTypeString(const aos::Channel* channel) {
  if (const PrimitiveType* primitive = FindPrimitiveType(channel)) {
    return std::string{primitive->ntType};
  }
  return std::format("proto:{}", channel->type()->string_view());
}

NtBridge::NtBridge(aos::EventLoop* eventLoop,
                   wpi::nt::NetworkTableInstance instance)
    : m_eventLoop(eventLoop), m_instance(instance) {
  const aos::Configuration* config = m_eventLoop->configuration();

  // Check every tagged channel, including that its schema translates, before
  // registering anything, so a bad configuration throws without leaving
  // watchers behind.
  struct Pending {
    const aos::Channel* channel;
    std::unique_ptr<FlatbufferToProto> translator;
    std::vector<ProtoFile> files;
  };
  std::vector<Pending> channels;
  // NetworkTables keeps the first schema registered under a name, so two
  // channels that describe one message differently would leave the second
  // decoded with the first's fields.
  struct Described {
    const aos::Channel* channel;
    std::span<const uint8_t> descriptor;
  };
  std::map<std::string, Described, std::less<>> described;
  for (const aos::Channel* channel : *config->channels()) {
    if (!aos::configuration::ChannelHasTag(channel, PUBLISH_TAG)) {
      continue;
    }
    if (!channel->has_schema()) {
      throw std::invalid_argument(std::format(
          "Channel {} carries no schema, so there is nothing to describe it "
          "with. A flattened configuration always has one.",
          aos::configuration::CleanedChannelToString(channel)));
    }
    for (const Pending& other : channels) {
      if (other.channel->name()->string_view() ==
          channel->name()->string_view()) {
        throw std::invalid_argument(std::format(
            "Channels {} and {} are both tagged {} and share a name. A "
            "NetworkTables topic has one type, so they cannot share a topic.",
            aos::configuration::CleanedChannelToString(other.channel),
            aos::configuration::CleanedChannelToString(channel), PUBLISH_TAG));
      }
    }
    Pending pending{channel, nullptr, {}};
    if (!FindPrimitiveType(channel)) {
      pending.translator =
          std::make_unique<FlatbufferToProto>(channel->schema());
      pending.files = BuildFileDescriptorProtos(channel->schema());
    }
    channels.push_back(std::move(pending));
  }

  // The descriptors live in channels, which does not move again.
  for (const Pending& pending : channels) {
    for (const ProtoFile& file : pending.files) {
      auto [it, inserted] =
          described.try_emplace(file.name, pending.channel, file.descriptor);
      if (!inserted &&
          !std::ranges::equal(it->second.descriptor, file.descriptor)) {
        throw std::invalid_argument(std::format(
            "Channels {} and {} are both tagged {} and describe {} "
            "differently. NetworkTables registers a schema once, so one of "
            "them could not be decoded.",
            aos::configuration::CleanedChannelToString(it->second.channel),
            aos::configuration::CleanedChannelToString(pending.channel),
            PUBLISH_TAG, file.name));
      }
    }
  }

  // Glass and DataLog load every descriptor into one pool, which refuses things
  // no single schema shows, such as enums in two channels' schemas that share a
  // package and a value name. Load them the same way first.
  {
    std::unique_ptr<upb_Arena, ArenaDeleter> arena{upb_Arena_New()};
    std::unique_ptr<upb_DefPool, DefPoolDeleter> pool{upb_DefPool_New()};
    if (!arena || !pool) {
      throw std::bad_alloc{};
    }
    std::set<std::string_view> loaded;
    for (const Pending& pending : channels) {
      for (const ProtoFile& file : pending.files) {
        if (!loaded.insert(file.name).second) {
          continue;
        }
        const google_protobuf_FileDescriptorProto* parsed =
            google_protobuf_FileDescriptorProto_parse(
                reinterpret_cast<const char*>(file.descriptor.data()),
                file.descriptor.size(), arena.get());
        if (!parsed) {
          throw std::bad_alloc{};
        }
        upb_Status status;
        upb_Status_Clear(&status);
        if (!upb_DefPool_AddFile(pool.get(), parsed, &status)) {
          throw std::invalid_argument(std::format(
              "Channel {} is tagged {}, but {} does not load alongside the "
              "other tagged channels' descriptors, as Glass and DataLog load "
              "them: {}",
              aos::configuration::CleanedChannelToString(pending.channel),
              PUBLISH_TAG, file.name, upb_Status_ErrorMessage(&status)));
        }
      }
    }
  }

  for (Pending& pending : channels) {
    const aos::Channel* channel = pending.channel;
    auto publisher = std::make_unique<Publisher>();
    publisher->channel = channel;
    Publisher* raw = publisher.get();

    if (const PrimitiveType* primitive = FindPrimitiveType(channel)) {
      publisher->primitive =
          m_instance.GetTopic(GetTopicName(channel))
              .GenericPublish(primitive->ntType, BridgeOptions(channel));
      m_eventLoop->MakeRawWatcher(
          channel, [this, raw, primitive](const aos::Context& context,
                                          const void* data) {
            raw->primitive.Set(
                primitive->toValue(static_cast<const uint8_t*>(data)));
            ++m_publishedMessages;
          });
      m_publishers.push_back(std::move(publisher));
      continue;
    }

    publisher->translator = std::move(pending.translator);

    // The watcher grows this if a message needs more.
    publisher->buffer.resize(static_cast<size_t>(channel->max_size()) *
                                 ENCODE_SIZE_FACTOR +
                             ENCODE_SIZE_SLACK);

    for (const ProtoFile& file : pending.files) {
      AddSchema(file);
    }

    publisher->publisher =
        m_instance.GetRawTopic(GetTopicName(channel))
            .Publish(GetTypeString(channel), BridgeOptions(channel));

    m_eventLoop->MakeRawWatcher(
        channel, [this, raw](const aos::Context& context, const void* data) {
          const uint8_t* message = static_cast<const uint8_t*>(data);
          const size_t needed = raw->translator->GetEncodedSize(message);
          if (needed > raw->buffer.size()) {
            // Grow once and keep it.
            raw->buffer.resize(needed);
          }
          const size_t size = raw->translator->Encode(message, raw->buffer);
          assert(size == needed);
          raw->publisher.Set(
              std::span<const uint8_t>(raw->buffer.data(), size));
          ++m_publishedMessages;
        });

    m_publishers.push_back(std::move(publisher));
  }
}

void NtBridge::AddSchema(const ProtoFile& file) {
  const std::string schemaName = std::format("proto:{}", file.name);
  if (m_instance.HasSchema(schemaName)) {
    return;
  }
  m_instance.AddSchema(schemaName, "proto:FileDescriptorProto",
                       file.descriptor);
}

}  // namespace wpi::aosnt
