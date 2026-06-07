// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/NetworkOutgoingQueue.hpp"

#include <stdint.h>

#include <deque>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "net/Message.hpp"
#include "net/WireConnection.hpp"
#include "net/WireDecoder.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace std::string_literals;

namespace wpi::nt::net {
namespace {

class RecordingWireConnection : public WireConnection {
 public:
  unsigned int GetVersion() const override { return version; }
  void SendPing(uint64_t time) override { pings.emplace_back(time); }
  bool Ready() const override { return ready; }

  int WriteText(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                    writer) override {
    std::string text;
    wpi::util::raw_string_ostream os{text};
    writer(os);
    textWrites.emplace_back(os.str());
    return PopReturn(&textWriteReturns);
  }

  int WriteBinary(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                      writer) override {
    std::vector<uint8_t> binary;
    wpi::util::raw_uvector_ostream os{binary};
    writer(os);
    binaryWrites.emplace_back(std::move(binary));
    return PopReturn(&binaryWriteReturns);
  }

  int Flush() override {
    ++flushCount;
    return PopReturn(&flushReturns);
  }

  void SendText(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                    writer) override {
    std::string text;
    wpi::util::raw_string_ostream os{text};
    writer(os);
    textSends.emplace_back(os.str());
  }

  void SendBinary(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                      writer) override {
    std::vector<uint8_t> binary;
    wpi::util::raw_uvector_ostream os{binary};
    writer(os);
    binarySends.emplace_back(std::move(binary));
  }

  uint64_t GetLastFlushTime() const override { return lastFlushTime; }
  uint64_t GetLastReceivedTime() const override { return lastReceivedTime; }

  void StopRead() override { readStopped = true; }
  void StartRead() override { readStopped = false; }

  void Disconnect(std::string_view reason) override {
    disconnectReason = std::string{reason};
  }

  int PopReturn(std::deque<int>* returns) {
    if (returns->empty()) {
      return 0;
    }
    int rv = returns->front();
    returns->pop_front();
    return rv;
  }

  unsigned int version = 0x0401;
  bool ready = true;
  bool readStopped = false;
  uint64_t lastFlushTime = 0;
  uint64_t lastReceivedTime = 0;
  std::string disconnectReason;
  std::vector<uint64_t> pings;
  std::vector<std::string> textWrites;
  std::vector<std::vector<uint8_t>> binaryWrites;
  std::vector<std::string> textSends;
  std::vector<std::vector<uint8_t>> binarySends;
  std::deque<int> textWriteReturns;
  std::deque<int> binaryWriteReturns;
  std::deque<int> flushReturns;
  int flushCount = 0;
};

std::pair<int, Value> DecodeBinary(std::span<const uint8_t> data,
                                   int64_t localTimeOffset = 0) {
  int id = 0;
  Value value;
  std::string error;
  EXPECT_TRUE(WireDecodeBinary(&data, &id, &value, &error, localTimeOffset))
      << error;
  EXPECT_TRUE(data.empty());
  return {id, value};
}

ClientMessage Publish(int pubuid, std::string_view name) {
  return ClientMessage{PublishMsg{
      pubuid, std::string{name}, "double"s, wpi::util::json::object(), {}}};
}

}  // namespace

TEST(NetworkOutgoingQueueTest, UpdatePeriodCalcUsesGcdAndMinimum) {
  EXPECT_EQ(UpdatePeriodCalc(UINT32_MAX, 100), 100u);
  EXPECT_EQ(UpdatePeriodCalc(100, 40), 20u);
  EXPECT_EQ(UpdatePeriodCalc(6, 4), kMinPeriodMs);
}

TEST(NetworkOutgoingQueueTest, CalculatePeriodUsesGcdAndMinimum) {
  std::vector<uint32_t> periods{100, 40, 60};
  EXPECT_EQ(CalculatePeriod(periods, [](uint32_t period) { return period; }),
            20u);

  periods = {2, 4};
  EXPECT_EQ(CalculatePeriod(periods, [](uint32_t period) { return period; }),
            kMinPeriodMs);
}

TEST(NetworkOutgoingQueueTest, RemoteTextMessageWaitsForReadyAndMinimumPeriod) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ClientMessage> queue{wire, false};

  queue.SendMessage(5, Publish(5, "test"));

  queue.SendOutgoing(4, false);
  EXPECT_TRUE(wire.textWrites.empty());

  wire.ready = false;
  queue.SendOutgoing(5, false);
  EXPECT_TRUE(wire.textWrites.empty());

  wire.ready = true;
  queue.SendOutgoing(5, false);
  ASSERT_EQ(wire.textWrites.size(), 1u);
  EXPECT_EQ(wire.textWrites[0],
            "{\"method\":\"publish\",\"params\":{\"name\":\"test\","
            "\"properties\":{},\"pubuid\":5,\"type\":\"double\"}}");
  EXPECT_EQ(wire.flushCount, 1);

  queue.SendOutgoing(10, false);
  EXPECT_EQ(wire.textWrites.size(), 1u);
}

TEST(NetworkOutgoingQueueTest, FlushWaitsForMinimumPeriod) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ClientMessage> queue{wire, false};

  queue.SendMessage(1, Publish(1, "first"));
  queue.SendOutgoing(5, false);
  ASSERT_EQ(wire.textWrites.size(), 1u);

  queue.SendMessage(2, Publish(2, "second"));
  queue.SendOutgoing(6, true);
  ASSERT_EQ(wire.textWrites.size(), 1u);

  queue.SendOutgoing(10, true);
  ASSERT_EQ(wire.textWrites.size(), 2u);
  EXPECT_EQ(wire.textWrites[1],
            "{\"method\":\"publish\",\"params\":{\"name\":\"second\","
            "\"properties\":{},\"pubuid\":2,\"type\":\"double\"}}");
}

TEST(NetworkOutgoingQueueTest, NormalValueKeepsOnlyNewestQueuedTimestamp) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SendValue(7, Value::MakeDouble(1.0, 100), ValueSendMode::kNormal);
  queue.SendValue(7, Value::MakeDouble(2.0, 200), ValueSendMode::kNormal);
  queue.SendValue(7, Value::MakeDouble(3.0, 150), ValueSendMode::kNormal);

  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.binaryWrites.size(), 1u);
  auto [id, value] = DecodeBinary(wire.binaryWrites[0]);
  EXPECT_EQ(id, 7);
  EXPECT_EQ(value.time(), 200);
  EXPECT_EQ(value.GetDouble(), 2.0);
}

TEST(NetworkOutgoingQueueTest, NormalValueShrinkUpdatesTotalSize) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SendValue(1, Value::MakeString(std::string(900 * 1024, 'x'), 10),
                  ValueSendMode::kNormal);
  queue.SendValue(1, Value::MakeDouble(1.0, 20), ValueSendMode::kNormal);
  queue.SendValue(1, Value::MakeDouble(2.0, 30), ValueSendMode::kAll);
  queue.SendValue(1, Value::MakeDouble(3.0, 40), ValueSendMode::kAll);

  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.binaryWrites.size(), 3u);

  auto [id0, value0] = DecodeBinary(wire.binaryWrites[0]);
  EXPECT_EQ(id0, 1);
  EXPECT_EQ(value0.GetDouble(), 1.0);

  auto [id1, value1] = DecodeBinary(wire.binaryWrites[1]);
  EXPECT_EQ(id1, 1);
  EXPECT_EQ(value1.GetDouble(), 2.0);

  auto [id2, value2] = DecodeBinary(wire.binaryWrites[2]);
  EXPECT_EQ(id2, 1);
  EXPECT_EQ(value2.GetDouble(), 3.0);
}

TEST(NetworkOutgoingQueueTest, NormalValueReplacesAfterPeriodChange) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SetPeriod(1, 100);
  queue.SendValue(1, Value::MakeDouble(1.0, 10), ValueSendMode::kNormal);

  queue.SetPeriod(2, 50);
  queue.SendValue(2, Value::MakeDouble(2.0, 20), ValueSendMode::kNormal);

  queue.SetPeriod(1, 50);
  queue.SendValue(1, Value::MakeDouble(3.0, 30), ValueSendMode::kNormal);

  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.binaryWrites.size(), 2u);
  auto [id0, value0] = DecodeBinary(wire.binaryWrites[0]);
  EXPECT_EQ(id0, 2);
  EXPECT_EQ(value0.GetDouble(), 2.0);

  auto [id1, value1] = DecodeBinary(wire.binaryWrites[1]);
  EXPECT_EQ(id1, 1);
  EXPECT_EQ(value1.time(), 30);
  EXPECT_EQ(value1.GetDouble(), 3.0);
}

TEST(NetworkOutgoingQueueTest, NormalValueReplacesLastValueAfterPeriodChange) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SetPeriod(1, 100);
  queue.SendValue(1, Value::MakeDouble(1.0, 10), ValueSendMode::kAll);
  queue.SendValue(1, Value::MakeDouble(2.0, 20), ValueSendMode::kAll);

  queue.SetPeriod(2, 50);
  queue.SendValue(2, Value::MakeDouble(4.0, 40), ValueSendMode::kNormal);

  queue.SetPeriod(1, 50);
  queue.SendValue(1, Value::MakeDouble(3.0, 30), ValueSendMode::kNormal);

  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.binaryWrites.size(), 3u);
  auto [id0, value0] = DecodeBinary(wire.binaryWrites[0]);
  EXPECT_EQ(id0, 2);
  EXPECT_EQ(value0.GetDouble(), 4.0);

  auto [id1, value1] = DecodeBinary(wire.binaryWrites[1]);
  EXPECT_EQ(id1, 1);
  EXPECT_EQ(value1.time(), 10);
  EXPECT_EQ(value1.GetDouble(), 1.0);

  auto [id2, value2] = DecodeBinary(wire.binaryWrites[2]);
  EXPECT_EQ(id2, 1);
  EXPECT_EQ(value2.time(), 30);
  EXPECT_EQ(value2.GetDouble(), 3.0);
}

TEST(NetworkOutgoingQueueTest, PeriodChangeMovesMixedMessagesInStableOrder) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ClientMessage> queue{wire, false};

  queue.SetPeriod(1, 100);
  queue.SendMessage(1, Publish(1, "first"));
  queue.SendValue(1, Value::MakeDouble(1.0, 10), ValueSendMode::kAll);
  queue.SendMessage(1, Publish(1, "second"));

  queue.SetPeriod(2, 50);
  queue.SendMessage(2, Publish(2, "dest"));

  queue.SetPeriod(1, 50);
  queue.SendValue(1, Value::MakeDouble(2.0, 20), ValueSendMode::kNormal);

  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.textWrites.size(), 3u);
  EXPECT_EQ(wire.textWrites[0],
            "{\"method\":\"publish\",\"params\":{\"name\":\"dest\","
            "\"properties\":{},\"pubuid\":2,\"type\":\"double\"}}");
  EXPECT_EQ(wire.textWrites[1],
            "{\"method\":\"publish\",\"params\":{\"name\":\"first\","
            "\"properties\":{},\"pubuid\":1,\"type\":\"double\"}}");
  EXPECT_EQ(wire.textWrites[2],
            "{\"method\":\"publish\",\"params\":{\"name\":\"second\","
            "\"properties\":{},\"pubuid\":1,\"type\":\"double\"}}");

  ASSERT_EQ(wire.binaryWrites.size(), 1u);
  auto [id, value] = DecodeBinary(wire.binaryWrites[0]);
  EXPECT_EQ(id, 1);
  EXPECT_EQ(value.time(), 20);
  EXPECT_EQ(value.GetDouble(), 2.0);
}

TEST(NetworkOutgoingQueueTest, SendAllValuesAppendBelowBackpressureLimit) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SendValue(1, Value::MakeDouble(1.0, 10), ValueSendMode::kAll);
  queue.SendValue(1, Value::MakeDouble(2.0, 20), ValueSendMode::kAll);

  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.binaryWrites.size(), 2u);
  auto [id0, value0] = DecodeBinary(wire.binaryWrites[0]);
  EXPECT_EQ(id0, 1);
  EXPECT_EQ(value0.GetDouble(), 1.0);

  auto [id1, value1] = DecodeBinary(wire.binaryWrites[1]);
  EXPECT_EQ(id1, 1);
  EXPECT_EQ(value1.GetDouble(), 2.0);
}

TEST(NetworkOutgoingQueueTest, SendAllValuesCoalesceAfterBackpressureLimit) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SendValue(1, Value::MakeString(std::string(1024 * 1024, 'x'), 10),
                  ValueSendMode::kAll);
  queue.SendValue(1, Value::MakeString(std::string(1024 * 1024, 'y'), 20),
                  ValueSendMode::kAll);
  queue.SendValue(1, Value::MakeDouble(3.0, 30), ValueSendMode::kAll);

  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.binaryWrites.size(), 1u);
  auto [id, value] = DecodeBinary(wire.binaryWrites[0]);
  EXPECT_EQ(id, 1);
  EXPECT_EQ(value.GetDouble(), 3.0);
}

TEST(NetworkOutgoingQueueTest, PartialWriteRetainsUnsentValueForReplacement) {
  RecordingWireConnection wire;
  wire.binaryWriteReturns = {0, 1};
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SendValue(1, Value::MakeDouble(1.0, 10), ValueSendMode::kNormal);
  queue.SendValue(2, Value::MakeDouble(2.0, 20), ValueSendMode::kNormal);

  queue.SendOutgoing(5, true);
  ASSERT_EQ(wire.binaryWrites.size(), 2u);

  queue.SendValue(2, Value::MakeDouble(3.0, 30), ValueSendMode::kNormal);
  queue.SendOutgoing(10, true);

  ASSERT_EQ(wire.binaryWrites.size(), 3u);
  auto [id, value] = DecodeBinary(wire.binaryWrites[2]);
  EXPECT_EQ(id, 2);
  EXPECT_EQ(value.time(), 30);
  EXPECT_EQ(value.GetDouble(), 3.0);
}

TEST(NetworkOutgoingQueueTest, FlushBackpressureRetainsLastUnsentValue) {
  RecordingWireConnection wire;
  wire.flushReturns = {1};
  NetworkOutgoingQueue<ServerMessage> queue{wire, false};

  queue.SendValue(1, Value::MakeDouble(1.0, 10), ValueSendMode::kNormal);
  queue.SendValue(2, Value::MakeDouble(2.0, 20), ValueSendMode::kNormal);

  queue.SendOutgoing(5, true);
  ASSERT_EQ(wire.binaryWrites.size(), 2u);

  queue.SendOutgoing(10, true);

  ASSERT_EQ(wire.binaryWrites.size(), 3u);
  auto [id, value] = DecodeBinary(wire.binaryWrites[2]);
  EXPECT_EQ(id, 2);
  EXPECT_EQ(value.GetDouble(), 2.0);
}

TEST(NetworkOutgoingQueueTest, ClientImmediateValueAppliesTimeOffset) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ClientMessage> queue{wire, false};
  queue.SetTimeOffset(5);

  queue.SendValue(3, Value::MakeDouble(1.0, 10), ValueSendMode::kImm);

  ASSERT_EQ(wire.binarySends.size(), 1u);
  auto [id, value] = DecodeBinary(wire.binarySends[0], -5);
  EXPECT_EQ(id, 3);
  EXPECT_EQ(value.time(), 10);
  EXPECT_EQ(value.server_time(), 15);
  EXPECT_EQ(value.GetDouble(), 1.0);
}

TEST(NetworkOutgoingQueueTest, LocalQueueSendsImmediately) {
  RecordingWireConnection wire;
  NetworkOutgoingQueue<ClientMessage> queue{wire, true};

  queue.SendMessage(5, Publish(5, "local"));
  queue.SendValue(5, Value::MakeDouble(1.0, 10), ValueSendMode::kNormal);
  queue.SendOutgoing(5, true);

  ASSERT_EQ(wire.textSends.size(), 1u);
  EXPECT_EQ(wire.textSends[0],
            "{\"method\":\"publish\",\"params\":{\"name\":\"local\","
            "\"properties\":{},\"pubuid\":5,\"type\":\"double\"}}");
  ASSERT_EQ(wire.binarySends.size(), 1u);
  EXPECT_TRUE(wire.textWrites.empty());
  EXPECT_TRUE(wire.binaryWrites.empty());
}

}  // namespace wpi::nt::net
