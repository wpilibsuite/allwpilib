// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/aosnt/NtBridge.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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
constexpr size_t kEncodeSizeFactor = 2;
constexpr size_t kEncodeSizeSlack = 512;

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
  std::string_view aos_type;
  std::string_view nt_type;
  wpi::nt::Value (*to_value)(const uint8_t* message);
};

const std::array<PrimitiveType, 10> kPrimitiveTypes{{
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
       const flatbuffers::String* const value =
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
       const auto* const value =
           flatbuffers::GetRoot<StringArray>(message)->value();
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
  for (const PrimitiveType& primitive : kPrimitiveTypes) {
    if (primitive.aos_type == type) {
      return &primitive;
    }
  }
  return nullptr;
}

}  // namespace

std::string NtBridge::NtName(const aos::Channel* channel) {
  return std::string{channel->name()->string_view()};
}

std::string NtBridge::NtTypeString(const aos::Channel* channel) {
  if (const PrimitiveType* const primitive = FindPrimitiveType(channel)) {
    return std::string{primitive->nt_type};
  }
  return std::format("proto:{}", channel->type()->string_view());
}

NtBridge::NtBridge(aos::EventLoop* event_loop,
                   wpi::nt::NetworkTableInstance instance)
    : event_loop_(event_loop), instance_(instance) {
  const aos::Configuration* const config = event_loop_->configuration();

  // Check every tagged channel before registering anything, so a bad
  // configuration throws without leaving watchers behind.
  std::vector<const aos::Channel*> channels;
  for (const aos::Channel* channel : *config->channels()) {
    if (!aos::configuration::ChannelHasTag(channel, kPublishTag)) {
      continue;
    }
    if (!channel->has_schema()) {
      throw std::invalid_argument(std::format(
          "Channel {} carries no schema, so there is nothing to describe it "
          "with. A flattened configuration always has one.",
          aos::configuration::CleanedChannelToString(channel)));
    }
    for (const aos::Channel* other : channels) {
      if (other->name()->string_view() == channel->name()->string_view()) {
        throw std::invalid_argument(std::format(
            "Channels {} and {} are both tagged {} and share a name. A "
            "NetworkTables topic has one type, so they cannot share a topic.",
            aos::configuration::CleanedChannelToString(other),
            aos::configuration::CleanedChannelToString(channel), kPublishTag));
      }
    }
    channels.push_back(channel);
  }

  for (const aos::Channel* channel : channels) {
    auto publisher = std::make_unique<Publisher>();
    publisher->channel = channel;
    Publisher* const raw = publisher.get();

    if (const PrimitiveType* const primitive = FindPrimitiveType(channel)) {
      publisher->primitive =
          instance_.GetTopic(NtName(channel))
              .GenericPublish(primitive->nt_type, BridgeOptions(channel));
      event_loop_->MakeRawWatcher(
          channel, [this, raw, primitive](const aos::Context& context,
                                          const void* data) {
            raw->primitive.Set(
                primitive->to_value(static_cast<const uint8_t*>(data)));
            ++published_messages_;
          });
      publishers_.push_back(std::move(publisher));
      continue;
    }

    publisher->translator =
        std::make_unique<aos::FlatbufferToProto>(channel->schema());

    // The watcher grows this if a message needs more.
    publisher->buffer.resize(static_cast<size_t>(channel->max_size()) *
                                 kEncodeSizeFactor +
                             kEncodeSizeSlack);

    AddSchema(channel);

    publisher->publisher =
        instance_.GetRawTopic(NtName(channel))
            .Publish(NtTypeString(channel), BridgeOptions(channel));

    event_loop_->MakeRawWatcher(
        channel, [this, raw](const aos::Context& context, const void* data) {
          const uint8_t* const message = static_cast<const uint8_t*>(data);
          const size_t needed = raw->translator->EncodedSize(message);
          if (needed > raw->buffer.size()) {
            // Grow once and keep it.
            raw->buffer.resize(needed);
          }
          const size_t size = raw->translator->Encode(message, raw->buffer);
          assert(size == needed);
          raw->publisher.Set(
              std::span<const uint8_t>(raw->buffer.data(), size));
          ++published_messages_;
        });

    publishers_.push_back(std::move(publisher));
  }
}

void NtBridge::AddSchema(const aos::Channel* channel) {
  // NetworkTables names a protobuf schema after the file it came from. AOS
  // schemas have no file of their own, so the message name stands in, with the
  // package spelled as directories the way protobuf files are.
  std::string file_name{channel->type()->string_view()};
  std::replace(file_name.begin(), file_name.end(), '.', '/');
  file_name += ".proto";
  const std::string schema_name = std::format("proto:{}", file_name);
  if (instance_.HasSchema(schema_name)) {
    return;
  }

  const std::vector<uint8_t> descriptor =
      aos::FileDescriptorProtoForSchema(channel->schema(), file_name);
  instance_.AddSchema(schema_name, "proto:FileDescriptorProto", descriptor);
}

}  // namespace wpi::aosnt
