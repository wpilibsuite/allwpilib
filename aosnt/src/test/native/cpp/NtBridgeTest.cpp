// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/aosnt/NtBridge.hpp"

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <upb/base/status.h>
#include <upb/mem/arena.h>
#include <upb/message/message.h>
#include <upb/reflection/def.h>
#include <upb/reflection/message.h>
#include <upb/reflection/stage0/google/protobuf/descriptor.upb.h>
#include <upb/wire/decode.h>

#include "aos/configuration.h"
#include "aos/events/simulated_event_loop.h"
#include "aos/testing/ping_pong/ping_static.h"
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
#include "tools/cpp/runfiles/runfiles.h"
#include "wpi/nt/BooleanArrayTopic.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/DoubleArrayTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/FloatArrayTopic.hpp"
#include "wpi/nt/FloatTopic.hpp"
#include "wpi/nt/IntegerArrayTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/RawTopic.hpp"
#include "wpi/nt/StringArrayTopic.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace wpi::aosnt {
namespace {

namespace chrono = std::chrono;

using bazel::tools::cpp::runfiles::Runfiles;

// Resolves a data dependency through the runfiles manifest. Windows has no
// runfiles tree to walk.
std::string RunfilePath(const std::string& path) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles{Runfiles::CreateForTest(&error)};
  if (!runfiles) {
    throw std::runtime_error(error);
  }
  std::string resolved = runfiles->Rlocation("_main/" + path);
  if (resolved.empty()) {
    resolved = runfiles->Rlocation("__main__/" + path);
  }
  if (resolved.empty()) {
    throw std::runtime_error("No runfile " + path + ".");
  }
  return resolved;
}

// Sends the message `create` builds.
template <typename T, typename Create>
void Send(aos::Sender<T>* sender, Create create) {
  typename aos::Sender<T>::Builder builder = sender->MakeBuilder();
  REQUIRE(builder.Send(create(builder.fbb())) == aos::RawSender::Error::kOk);
}

const aos::FlatbufferDetachedBuffer<aos::Configuration>& Config() {
  static const aos::FlatbufferDetachedBuffer<aos::Configuration> result =
      aos::configuration::ReadConfig(RunfilePath("aosnt/aos_config.bfbs"));
  return result;
}

const aos::Channel* PingChannel() {
  return aos::configuration::GetChannel(&Config().message(), "/bridge",
                                        "aos.examples.Ping", "", nullptr);
}

class NtBridgeTest {
 public:
  NtBridgeTest()
      : factory{&Config().message()},
        bridgeLoop{factory.MakeEventLoop("bridge")},
        testLoop{factory.MakeEventLoop("test")},
        instance{wpi::nt::NetworkTableInstance::Create()} {
    instance.StartLocal();
  }

  ~NtBridgeTest() {
    bridge.reset();
    wpi::nt::NetworkTableInstance::Destroy(instance);
  }

 protected:
  void MakeBridge() {
    bridge = std::make_unique<NtBridge>(bridgeLoop.get(), instance);
  }

  aos::SimulatedEventLoopFactory factory;
  std::unique_ptr<aos::EventLoop> bridgeLoop;
  std::unique_ptr<aos::EventLoop> testLoop;
  wpi::nt::NetworkTableInstance instance;
  std::unique_ptr<NtBridge> bridge;
};

}  // namespace

// Only tagged channels are bridged.
TEST_CASE_METHOD(NtBridgeTest, "NtBridgeTest OnlyBridgesTaggedChannels",
                 "[aosnt][nt-bridge]") {
  MakeBridge();
  REQUIRE(bridge->GetPublishedChannelCount() == 11u);
}

// A channel of a type in aosnt/types/ gets that NetworkTables type.
TEST_CASE("NtBridgeTest NamesPrimitiveTypes", "[aosnt][nt-bridge]") {
  const auto typeString = [](std::string_view name, std::string_view type) {
    const aos::Channel* channel = aos::configuration::GetChannel(
        &Config().message(), name, type, "", nullptr);
    REQUIRE(channel != nullptr);
    return NtBridge::GetTypeString(channel);
  };

  REQUIRE(typeString("/types/boolean", "wpi.aosnt.Boolean") == "boolean");
  REQUIRE(typeString("/types/integer", "wpi.aosnt.Integer") == "int");
  REQUIRE(typeString("/types/float", "wpi.aosnt.Float") == "float");
  REQUIRE(typeString("/types/double", "wpi.aosnt.Double") == "double");
  REQUIRE(typeString("/types/string", "wpi.aosnt.String") == "string");
  REQUIRE(typeString("/types/boolean_array", "wpi.aosnt.BooleanArray") ==
          "boolean[]");
  REQUIRE(typeString("/types/integer_array", "wpi.aosnt.IntegerArray") ==
          "int[]");
  REQUIRE(typeString("/types/float_array", "wpi.aosnt.FloatArray") ==
          "float[]");
  REQUIRE(typeString("/types/double_array", "wpi.aosnt.DoubleArray") ==
          "double[]");
  REQUIRE(typeString("/types/string_array", "wpi.aosnt.StringArray") ==
          "string[]");
}

TEST_CASE_METHOD(NtBridgeTest, "NtBridgeTest PublishesPrimitiveValues",
                 "[aosnt][nt-bridge]") {
  MakeBridge();

  wpi::nt::BooleanSubscriber boolean =
      instance.GetBooleanTopic("/types/boolean").Subscribe(false);
  wpi::nt::IntegerSubscriber integer =
      instance.GetIntegerTopic("/types/integer").Subscribe(0);
  wpi::nt::FloatSubscriber floating =
      instance.GetFloatTopic("/types/float").Subscribe(0);
  wpi::nt::DoubleSubscriber real =
      instance.GetDoubleTopic("/types/double").Subscribe(0);
  wpi::nt::StringSubscriber string =
      instance.GetStringTopic("/types/string").Subscribe("");
  wpi::nt::BooleanArraySubscriber booleans =
      instance.GetBooleanArrayTopic("/types/boolean_array").Subscribe({});
  wpi::nt::IntegerArraySubscriber integers =
      instance.GetIntegerArrayTopic("/types/integer_array").Subscribe({});
  wpi::nt::FloatArraySubscriber floats =
      instance.GetFloatArrayTopic("/types/float_array").Subscribe({});
  wpi::nt::DoubleArraySubscriber reals =
      instance.GetDoubleArrayTopic("/types/double_array").Subscribe({});
  wpi::nt::StringArraySubscriber strings =
      instance.GetStringArrayTopic("/types/string_array").Subscribe({});

  aos::Sender<Boolean> booleanSender =
      testLoop->MakeSender<Boolean>("/types/boolean");
  aos::Sender<Integer> integerSender =
      testLoop->MakeSender<Integer>("/types/integer");
  aos::Sender<Float> floatSender = testLoop->MakeSender<Float>("/types/float");
  aos::Sender<Double> doubleSender =
      testLoop->MakeSender<Double>("/types/double");
  aos::Sender<String> stringSender =
      testLoop->MakeSender<String>("/types/string");
  aos::Sender<BooleanArray> booleanArraySender =
      testLoop->MakeSender<BooleanArray>("/types/boolean_array");
  aos::Sender<IntegerArray> integerArraySender =
      testLoop->MakeSender<IntegerArray>("/types/integer_array");
  aos::Sender<FloatArray> floatArraySender =
      testLoop->MakeSender<FloatArray>("/types/float_array");
  aos::Sender<DoubleArray> doubleArraySender =
      testLoop->MakeSender<DoubleArray>("/types/double_array");
  aos::Sender<StringArray> stringArraySender =
      testLoop->MakeSender<StringArray>("/types/string_array");

  testLoop->OnRun([&]() {
    Send(&booleanSender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateBoolean(*fbb, true);
    });
    Send(&integerSender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateInteger(*fbb, 1234);
    });
    Send(&floatSender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateFloat(*fbb, 2.5f);
    });
    Send(&doubleSender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateDouble(*fbb, 3.25);
    });
    Send(&stringSender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateStringDirect(*fbb, "text");
    });
    Send(&booleanArraySender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<uint8_t> value{1, 0, 1};
      return CreateBooleanArrayDirect(*fbb, &value);
    });
    Send(&integerArraySender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<int64_t> value{1, -2, 3};
      return CreateIntegerArrayDirect(*fbb, &value);
    });
    Send(&floatArraySender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<float> value{1.5f, -2.5f};
      return CreateFloatArrayDirect(*fbb, &value);
    });
    Send(&doubleArraySender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<double> value{0.125, -0.25};
      return CreateDoubleArrayDirect(*fbb, &value);
    });
    Send(&stringArraySender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<flatbuffers::Offset<flatbuffers::String>> value{
          fbb->CreateString("a"), fbb->CreateString("bc")};
      return CreateStringArrayDirect(*fbb, &value);
    });
  });

  factory.RunFor(chrono::milliseconds(100));
  REQUIRE(bridge->GetPublishedMessageCount() == 10u);

  REQUIRE(boolean.Get());
  REQUIRE(integer.Get() == 1234);
  REQUIRE(floating.Get() == 2.5f);
  REQUIRE(real.Get() == 3.25);
  REQUIRE(string.Get() == "text");
  REQUIRE(booleans.Get() == std::vector<int>{1, 0, 1});
  REQUIRE(integers.Get() == std::vector<int64_t>{1, -2, 3});
  REQUIRE(floats.Get() == std::vector<float>{1.5f, -2.5f});
  REQUIRE(reals.Get() == std::vector<double>{0.125, -0.25});
  REQUIRE(strings.Get() == std::vector<std::string>{"a", "bc"});
}

// The topic looks like any other protobuf topic.
TEST_CASE_METHOD(NtBridgeTest, "NtBridgeTest PublishesAProtobufTopic",
                 "[aosnt][nt-bridge]") {
  MakeBridge();

  REQUIRE(NtBridge::GetTopicName(PingChannel()) == "/bridge");
  REQUIRE(NtBridge::GetTypeString(PingChannel()) == "proto:aos.examples.Ping");

  INFO("the descriptor should be in NetworkTables' schema registry");
  REQUIRE(instance.HasSchema("proto:aos/examples/Ping.proto"));
}

// Decodes a message the way a dashboard would: read the descriptor off
// NetworkTables, build the message type from it, then parse the topic's bytes.
// Nothing here is compiled against aos.examples.Ping as a protobuf. It is a
// flatbuffer, described at runtime.
TEST_CASE_METHOD(NtBridgeTest, "NtBridgeTest PublishesADecodableMessage",
                 "[aosnt][nt-bridge]") {
  MakeBridge();

  wpi::nt::RawSubscriber data =
      instance.GetRawTopic(NtBridge::GetTopicName(PingChannel()))
          .Subscribe(NtBridge::GetTypeString(PingChannel()), {});
  wpi::nt::RawSubscriber schema =
      instance.GetRawTopic("/.schema/proto:aos/examples/Ping.proto")
          .Subscribe("proto:FileDescriptorProto", {});

  aos::Sender<aos::examples::PingStatic> sender =
      testLoop->MakeSender<aos::examples::PingStatic>("/bridge");
  testLoop->OnRun([&]() {
    aos::Sender<aos::examples::PingStatic>::StaticBuilder builder =
        sender.MakeStaticBuilder();
    builder->set_value(1868);
    builder->set_send_time(1234567);
    builder.CheckOk(builder.Send());
  });

  factory.RunFor(chrono::milliseconds(100));
  REQUIRE(bridge->GetPublishedMessageCount() == 1u);

  const std::vector<uint8_t> schemaBytes = schema.Get();
  INFO("no descriptor was published");
  REQUIRE_FALSE(schemaBytes.empty());

  std::unique_ptr<upb_Arena, decltype(&upb_Arena_Free)> arena{upb_Arena_New(),
                                                              &upb_Arena_Free};
  std::unique_ptr<upb_DefPool, decltype(&upb_DefPool_Free)> pool{
      upb_DefPool_New(), &upb_DefPool_Free};
  const google_protobuf_FileDescriptorProto* file =
      google_protobuf_FileDescriptorProto_parse(
          reinterpret_cast<const char*>(schemaBytes.data()), schemaBytes.size(),
          arena.get());
  REQUIRE(file != nullptr);
  upb_Status status;
  upb_Status_Clear(&status);
  REQUIRE(upb_DefPool_AddFile(pool.get(), file, &status) != nullptr);
  const upb_MessageDef* ping =
      upb_DefPool_FindMessageByName(pool.get(), "aos.examples.Ping");
  REQUIRE(ping != nullptr);

  const std::vector<uint8_t> messageBytes = data.Get();
  INFO("nothing arrived on the topic");
  REQUIRE_FALSE(messageBytes.empty());

  const upb_MiniTable* table = upb_MessageDef_MiniTable(ping);
  upb_Message* message = upb_Message_New(table, arena.get());
  REQUIRE(upb_Decode(reinterpret_cast<const char*>(messageBytes.data()),
                     messageBytes.size(), message, table, nullptr, 0,
                     arena.get()) == kUpb_DecodeStatus_Ok);

  REQUIRE(upb_Message_GetFieldByDef(
              message, upb_MessageDef_FindFieldByName(ping, "value"))
              .int32_val == 1868);
  REQUIRE(upb_Message_GetFieldByDef(
              message, upb_MessageDef_FindFieldByName(ping, "send_time"))
              .int64_val == 1234567);
}

// A topic has one type, so two tagged channels cannot share a name.
TEST_CASE("NtBridgeTest RejectsTaggedChannelsThatShareAName",
          "[aosnt][nt-bridge]") {
  const aos::FlatbufferDetachedBuffer<aos::Configuration> config =
      aos::configuration::ReadConfig(
          RunfilePath("aosnt/aos_duplicate_config.bfbs"));
  aos::SimulatedEventLoopFactory factory{&config.message()};
  std::unique_ptr<aos::EventLoop> eventLoop = factory.MakeEventLoop("bridge");
  wpi::nt::NetworkTableInstance instance =
      wpi::nt::NetworkTableInstance::Create();

  REQUIRE_THROWS_AS(NtBridge(eventLoop.get(), instance), std::invalid_argument);

  wpi::nt::NetworkTableInstance::Destroy(instance);
}

}  // namespace wpi::aosnt
