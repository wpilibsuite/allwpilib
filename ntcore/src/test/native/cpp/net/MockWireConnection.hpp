// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <deque>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "net/WireConnection.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace wpi::nt::net {

class MockWireConnection : public WireConnection {
 public:
  struct WriteTextCall {
    std::string contents;

    bool operator==(const WriteTextCall&) const = default;
  };

  struct WriteBinaryCall {
    std::vector<uint8_t> contents;

    bool operator==(const WriteBinaryCall&) const = default;
  };

  struct FlushCall {
    bool operator==(const FlushCall&) const = default;
  };

  using WriteCall = std::variant<WriteTextCall, WriteBinaryCall, FlushCall>;

  unsigned int GetVersion() const override { return version; }

  void SendPing(uint64_t time) override { sendPingCalls.emplace_back(time); }

  bool Ready() const override {
    ++readyCalls;
    if (!readyReturns.empty()) {
      bool rv = readyReturns.front();
      readyReturns.pop_front();
      return rv;
    }
    return defaultReady;
  }

  int WriteText(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                    writer) override {
    std::string text;
    wpi::util::raw_string_ostream os{text};
    writer(os);
    return DoWriteText(text);
  }
  int WriteBinary(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                      writer) override {
    std::vector<uint8_t> binary;
    wpi::util::raw_uvector_ostream os{binary};
    writer(os);
    return DoWriteBinary(binary);
  }

  void SendText(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                    writer) override {
    std::string text;
    wpi::util::raw_string_ostream os{text};
    writer(os);
    DoSendText(text);
  }
  void SendBinary(wpi::util::function_ref<void(wpi::util::raw_ostream& os)>
                      writer) override {
    std::vector<uint8_t> binary;
    wpi::util::raw_uvector_ostream os{binary};
    writer(os);
    DoSendBinary(binary);
  }

  int DoWriteText(std::string_view contents) {
    writeTextCalls.emplace_back(contents);
    writeCalls.emplace_back(WriteTextCall{writeTextCalls.back()});
    if (onWriteText) {
      return onWriteText(contents);
    }
    if (!writeTextReturns.empty()) {
      int rv = writeTextReturns.front();
      writeTextReturns.pop_front();
      return rv;
    }
    return defaultWriteTextReturn;
  }

  int DoWriteBinary(std::span<const uint8_t> contents) {
    writeBinaryCalls.emplace_back(contents.begin(), contents.end());
    writeCalls.emplace_back(WriteBinaryCall{writeBinaryCalls.back()});
    if (onWriteBinary) {
      return onWriteBinary(contents);
    }
    if (!writeBinaryReturns.empty()) {
      int rv = writeBinaryReturns.front();
      writeBinaryReturns.pop_front();
      return rv;
    }
    return defaultWriteBinaryReturn;
  }

  void DoSendText(std::string_view contents) {
    sendTextCalls.emplace_back(contents);
  }

  void DoSendBinary(std::span<const uint8_t> contents) {
    sendBinaryCalls.emplace_back(contents.begin(), contents.end());
  }

  int Flush() override {
    ++flushCalls;
    writeCalls.emplace_back(FlushCall{});
    if (!flushReturns.empty()) {
      int rv = flushReturns.front();
      flushReturns.pop_front();
      return rv;
    }
    return defaultFlushReturn;
  }

  uint64_t GetLastFlushTime() const override { return lastFlushTime; }

  uint64_t GetLastReceivedTime() const override {
    ++lastReceivedTimeCalls;
    if (!lastReceivedTimeReturns.empty()) {
      uint64_t rv = lastReceivedTimeReturns.front();
      lastReceivedTimeReturns.pop_front();
      return rv;
    }
    return defaultLastReceivedTime;
  }

  void StopRead() override { ++stopReadCalls; }

  void StartRead() override { ++startReadCalls; }

  void Disconnect(std::string_view reason) override {
    disconnectCalls.emplace_back(reason);
  }

  unsigned int version = 0x0401;
  mutable std::deque<bool> readyReturns;
  bool defaultReady = true;
  std::deque<int> writeTextReturns;
  int defaultWriteTextReturn = 0;
  std::deque<int> writeBinaryReturns;
  int defaultWriteBinaryReturn = 0;
  std::deque<int> flushReturns;
  int defaultFlushReturn = 0;
  uint64_t lastFlushTime = 0;
  mutable std::deque<uint64_t> lastReceivedTimeReturns;
  uint64_t defaultLastReceivedTime = 0;
  std::function<int(std::string_view)> onWriteText;
  std::function<int(std::span<const uint8_t>)> onWriteBinary;

  std::vector<uint64_t> sendPingCalls;
  mutable int readyCalls = 0;
  std::vector<std::string> writeTextCalls;
  std::vector<std::vector<uint8_t>> writeBinaryCalls;
  std::vector<WriteCall> writeCalls;
  std::vector<std::string> sendTextCalls;
  std::vector<std::vector<uint8_t>> sendBinaryCalls;
  int flushCalls = 0;
  mutable int lastReceivedTimeCalls = 0;
  int stopReadCalls = 0;
  int startReadCalls = 0;
  std::vector<std::string> disconnectCalls;
};

}  // namespace wpi::nt::net
