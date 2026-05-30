// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/NetworkOutgoingQueue.hpp"

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

#include "MockWireConnection.hpp"
#include "net/Message.hpp"
#include "net/WireEncoder.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace wpi::nt;
using namespace wpi::nt::net;
using ::testing::_;
using ::testing::Return;

// Encode a single binary value update into bytes for comparison.
static std::vector<uint8_t> EncodeValue(int id, const Value& v) {
  std::vector<uint8_t> buf;
  wpi::util::raw_uvector_ostream os{buf};
  WireEncodeBinary(os, id, v.time(), v);
  return buf;
}

// Returns true if 'needle' appears as a contiguous substring of 'haystack'.
static bool Contains(const std::vector<uint8_t>& haystack,
                     const std::vector<uint8_t>& needle) {
  return std::search(haystack.begin(), haystack.end(), needle.begin(),
                     needle.end()) != haystack.end();
}

class NetworkOutgoingQueueTest : public ::testing::Test {
 protected:
  ::testing::NiceMock<MockWireConnection> m_wire;
  NetworkOutgoingQueue<ClientMessage> m_queue{m_wire, false};

  // Capture all binary writes from SendOutgoing (flush=true to force all
  // queues regardless of their nextSendMs).
  std::vector<uint8_t> Flush(uint64_t curTimeMs) {
    std::vector<uint8_t> captured;
    ON_CALL(m_wire, DoWriteBinary(_))
        .WillByDefault([&](std::span<const uint8_t> data) {
          captured.insert(captured.end(), data.begin(), data.end());
          return 0;
        });
    ON_CALL(m_wire, Ready()).WillByDefault(Return(true));
    ON_CALL(m_wire, Flush()).WillByDefault(Return(0));
    m_queue.SendOutgoing(curTimeMs, true);
    return captured;
  }
};

// When publisher B shares a queue with publisher A and the queue looks like
// [A, B, A, C], stable_partition produces [B, C, A, A] with partIdx=2.
//
// The original fix invalidates valuePos >= partIdx. B.valuePos=1 < 2 so it
// is NOT invalidated — but B has shifted from original index 1 to new index
// 0. The subsequent kNormal SendValue(B, newB) checks msgs[1] = C, fails the
// id check, and APPENDs a second B entry instead of replacing the first.
// At flush time B's old value is sent before B's new value.
//
// The correct fix recomputes positions by counting removed elements before
// each saved index:
//   B.valuePos = 1 - count(A before index 1) = 1 - 1 = 0  (correct)
//   C.valuePos = 3 - count(A before index 3) = 3 - 2 = 1  (correct)
// so kNormal SendValue(B, newB) replaces at index 0 and no stale entry
// remains.
//
// This test FAILS on the PR-only fix (B's stale value appears in the flush
// output) and PASSES once the position-recomputation fix is applied.
TEST_F(NetworkOutgoingQueueTest,
       SetPeriodCorrectlyUpdatesValuePosForOtherPublishers) {
  m_queue.SetPeriod(1, 100);
  m_queue.SetPeriod(2, 100);
  m_queue.SetPeriod(3, 100);

  Value vA1 = Value::MakeDouble(1.0, 10);
  Value vB = Value::MakeDouble(2.0, 10);  // B's original value
  Value vA2 = Value::MakeDouble(1.5, 20);
  Value vC = Value::MakeDouble(3.0, 10);
  Value vBnew = Value::MakeDouble(99.0, 30);  // B's updated value

  // Build queue [A(1), B(2), A(1), C(3)] so that B.valuePos(1) < partIdx(2)
  // after partition, triggering the stale-valuePos bug.
  m_queue.SendValue(1, vA1, ValueSendMode::kAll);    // index 0 (A)
  m_queue.SendValue(2, vB, ValueSendMode::kNormal);  // index 1 (B)
  m_queue.SendValue(1, vA2, ValueSendMode::kAll);    // index 2 (A)
  m_queue.SendValue(3, vC, ValueSendMode::kNormal);  // index 3 (C)

  // Move A to a different period — triggers stable_partition.
  // Result: old queue = [B(old), C]; A entries go to the 200 ms queue.
  m_queue.SetPeriod(1, 200);

  // Update B with kNormal.
  // Correct: replaces B(old) at index 0 → old queue = [B(new), C].
  // Buggy:   B.valuePos=1 points to C's slot; appends instead
  //          → old queue = [B(old), C, B(new)].
  m_queue.SendValue(2, vBnew, ValueSendMode::kNormal);

  auto data = Flush(200);

  // With the fix B's old value must NOT appear; B's new value must appear.
  EXPECT_FALSE(Contains(data, EncodeValue(2, vB)))
      << "B's stale value (2.0) was flushed — stale B.valuePos caused a "
         "duplicate entry instead of a replacement";
  EXPECT_TRUE(Contains(data, EncodeValue(2, vBnew)))
      << "B's updated value (99.0) was not flushed";
}
