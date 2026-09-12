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
#include "google/protobuf/descriptor.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/dynamic_message.h"
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
      : factory_{&Config().message()},
        bridge_loop_{factory_.MakeEventLoop("bridge")},
        test_loop_{factory_.MakeEventLoop("test")},
        instance_{wpi::nt::NetworkTableInstance::Create()} {
    instance_.StartLocal();
  }

  ~NtBridgeTest() {
    bridge_.reset();
    wpi::nt::NetworkTableInstance::Destroy(instance_);
  }

 protected:
  void MakeBridge() {
    bridge_ = std::make_unique<NtBridge>(bridge_loop_.get(), instance_);
  }

  aos::SimulatedEventLoopFactory factory_;
  std::unique_ptr<aos::EventLoop> bridge_loop_;
  std::unique_ptr<aos::EventLoop> test_loop_;
  wpi::nt::NetworkTableInstance instance_;
  std::unique_ptr<NtBridge> bridge_;
};

}  // namespace

// Only tagged channels are bridged.
TEST_CASE_METHOD(NtBridgeTest, "NtBridge only bridges tagged channels",
                 "[NtBridge]") {
  MakeBridge();
  REQUIRE(bridge_->published_channels() == 11u);
}

// A channel of a type in aosnt/types/ gets that NetworkTables type.
TEST_CASE("NtBridge names primitive types", "[NtBridge]") {
  const auto type_string = [](std::string_view name, std::string_view type) {
    const aos::Channel* const channel = aos::configuration::GetChannel(
        &Config().message(), name, type, "", nullptr);
    REQUIRE(channel != nullptr);
    return NtBridge::NtTypeString(channel);
  };

  REQUIRE(type_string("/types/boolean", "wpi.aosnt.Boolean") == "boolean");
  REQUIRE(type_string("/types/integer", "wpi.aosnt.Integer") == "int");
  REQUIRE(type_string("/types/float", "wpi.aosnt.Float") == "float");
  REQUIRE(type_string("/types/double", "wpi.aosnt.Double") == "double");
  REQUIRE(type_string("/types/string", "wpi.aosnt.String") == "string");
  REQUIRE(type_string("/types/boolean_array", "wpi.aosnt.BooleanArray") ==
          "boolean[]");
  REQUIRE(type_string("/types/integer_array", "wpi.aosnt.IntegerArray") ==
          "int[]");
  REQUIRE(type_string("/types/float_array", "wpi.aosnt.FloatArray") ==
          "float[]");
  REQUIRE(type_string("/types/double_array", "wpi.aosnt.DoubleArray") ==
          "double[]");
  REQUIRE(type_string("/types/string_array", "wpi.aosnt.StringArray") ==
          "string[]");
}

TEST_CASE_METHOD(NtBridgeTest, "NtBridge publishes primitive values",
                 "[NtBridge]") {
  MakeBridge();

  wpi::nt::BooleanSubscriber boolean =
      instance_.GetBooleanTopic("/types/boolean").Subscribe(false);
  wpi::nt::IntegerSubscriber integer =
      instance_.GetIntegerTopic("/types/integer").Subscribe(0);
  wpi::nt::FloatSubscriber floating =
      instance_.GetFloatTopic("/types/float").Subscribe(0);
  wpi::nt::DoubleSubscriber real =
      instance_.GetDoubleTopic("/types/double").Subscribe(0);
  wpi::nt::StringSubscriber string =
      instance_.GetStringTopic("/types/string").Subscribe("");
  wpi::nt::BooleanArraySubscriber booleans =
      instance_.GetBooleanArrayTopic("/types/boolean_array").Subscribe({});
  wpi::nt::IntegerArraySubscriber integers =
      instance_.GetIntegerArrayTopic("/types/integer_array").Subscribe({});
  wpi::nt::FloatArraySubscriber floats =
      instance_.GetFloatArrayTopic("/types/float_array").Subscribe({});
  wpi::nt::DoubleArraySubscriber reals =
      instance_.GetDoubleArrayTopic("/types/double_array").Subscribe({});
  wpi::nt::StringArraySubscriber strings =
      instance_.GetStringArrayTopic("/types/string_array").Subscribe({});

  aos::Sender<Boolean> boolean_sender =
      test_loop_->MakeSender<Boolean>("/types/boolean");
  aos::Sender<Integer> integer_sender =
      test_loop_->MakeSender<Integer>("/types/integer");
  aos::Sender<Float> float_sender =
      test_loop_->MakeSender<Float>("/types/float");
  aos::Sender<Double> double_sender =
      test_loop_->MakeSender<Double>("/types/double");
  aos::Sender<String> string_sender =
      test_loop_->MakeSender<String>("/types/string");
  aos::Sender<BooleanArray> boolean_array_sender =
      test_loop_->MakeSender<BooleanArray>("/types/boolean_array");
  aos::Sender<IntegerArray> integer_array_sender =
      test_loop_->MakeSender<IntegerArray>("/types/integer_array");
  aos::Sender<FloatArray> float_array_sender =
      test_loop_->MakeSender<FloatArray>("/types/float_array");
  aos::Sender<DoubleArray> double_array_sender =
      test_loop_->MakeSender<DoubleArray>("/types/double_array");
  aos::Sender<StringArray> string_array_sender =
      test_loop_->MakeSender<StringArray>("/types/string_array");

  test_loop_->OnRun([&]() {
    Send(&boolean_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateBoolean(*fbb, true);
    });
    Send(&integer_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateInteger(*fbb, 1234);
    });
    Send(&float_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateFloat(*fbb, 2.5f);
    });
    Send(&double_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateDouble(*fbb, 3.25);
    });
    Send(&string_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      return CreateStringDirect(*fbb, "text");
    });
    Send(&boolean_array_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<uint8_t> value{1, 0, 1};
      return CreateBooleanArrayDirect(*fbb, &value);
    });
    Send(&integer_array_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<int64_t> value{1, -2, 3};
      return CreateIntegerArrayDirect(*fbb, &value);
    });
    Send(&float_array_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<float> value{1.5f, -2.5f};
      return CreateFloatArrayDirect(*fbb, &value);
    });
    Send(&double_array_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<double> value{0.125, -0.25};
      return CreateDoubleArrayDirect(*fbb, &value);
    });
    Send(&string_array_sender, [](flatbuffers::FlatBufferBuilder* fbb) {
      const std::vector<flatbuffers::Offset<flatbuffers::String>> value{
          fbb->CreateString("a"), fbb->CreateString("bc")};
      return CreateStringArrayDirect(*fbb, &value);
    });
  });

  factory_.RunFor(chrono::milliseconds(100));
  REQUIRE(bridge_->published_messages() == 10u);

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
TEST_CASE_METHOD(NtBridgeTest, "NtBridge publishes a protobuf topic",
                 "[NtBridge]") {
  MakeBridge();

  REQUIRE(NtBridge::NtName(PingChannel()) == "/bridge");
  REQUIRE(NtBridge::NtTypeString(PingChannel()) == "proto:aos.examples.Ping");

  INFO("the descriptor should be in NetworkTables' schema registry");
  REQUIRE(instance_.HasSchema("proto:aos/examples/Ping.proto"));
}

// Decodes a message the way a dashboard would: read the descriptor off
// NetworkTables, build the message type from it, then parse the topic's bytes.
// Nothing here is compiled against aos.examples.Ping as a protobuf. It is a
// flatbuffer, described at runtime.
TEST_CASE_METHOD(NtBridgeTest, "NtBridge publishes a decodable message",
                 "[NtBridge]") {
  MakeBridge();

  wpi::nt::RawSubscriber data =
      instance_.GetRawTopic(NtBridge::NtName(PingChannel()))
          .Subscribe(NtBridge::NtTypeString(PingChannel()), {});
  wpi::nt::RawSubscriber schema =
      instance_.GetRawTopic("/.schema/proto:aos/examples/Ping.proto")
          .Subscribe("proto:FileDescriptorProto", {});

  aos::Sender<aos::examples::PingStatic> sender =
      test_loop_->MakeSender<aos::examples::PingStatic>("/bridge");
  test_loop_->OnRun([&]() {
    aos::Sender<aos::examples::PingStatic>::StaticBuilder builder =
        sender.MakeStaticBuilder();
    builder->set_value(1868);
    builder->set_send_time(1234567);
    builder.CheckOk(builder.Send());
  });

  factory_.RunFor(chrono::milliseconds(100));
  REQUIRE(bridge_->published_messages() == 1u);

  const std::vector<uint8_t> schema_bytes = schema.Get();
  INFO("no descriptor was published");
  REQUIRE_FALSE(schema_bytes.empty());

  google::protobuf::FileDescriptorProto file;
  REQUIRE(file.ParseFromArray(schema_bytes.data(), schema_bytes.size()));
  google::protobuf::DescriptorPool pool;
  REQUIRE(pool.BuildFile(file) != nullptr);
  const google::protobuf::Descriptor* const ping =
      pool.FindMessageTypeByName("aos.examples.Ping");
  REQUIRE(ping != nullptr);

  const std::vector<uint8_t> message_bytes = data.Get();
  INFO("nothing arrived on the topic");
  REQUIRE_FALSE(message_bytes.empty());

  google::protobuf::DynamicMessageFactory factory{&pool};
  std::unique_ptr<google::protobuf::Message> message{
      factory.GetPrototype(ping)->New()};
  REQUIRE(message->ParseFromArray(message_bytes.data(), message_bytes.size()));

  const google::protobuf::Reflection* const reflection =
      message->GetReflection();
  REQUIRE(reflection->GetInt32(*message, ping->FindFieldByName("value")) ==
          1868);
  REQUIRE(reflection->GetInt64(*message, ping->FindFieldByName("send_time")) ==
          1234567);
}

// A topic has one type, so two tagged channels cannot share a name.
TEST_CASE("NtBridge rejects tagged channels that share a name", "[NtBridge]") {
  const aos::FlatbufferDetachedBuffer<aos::Configuration> config =
      aos::configuration::ReadConfig(
          RunfilePath("aosnt/aos_duplicate_config.bfbs"));
  aos::SimulatedEventLoopFactory factory{&config.message()};
  std::unique_ptr<aos::EventLoop> event_loop = factory.MakeEventLoop("bridge");
  wpi::nt::NetworkTableInstance instance =
      wpi::nt::NetworkTableInstance::Create();

  REQUIRE_THROWS_AS(NtBridge(event_loop.get(), instance),
                    std::invalid_argument);

  wpi::nt::NetworkTableInstance::Destroy(instance);
}

}  // namespace wpi::aosnt
