// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string_view>

#include <wpi/function_ref.h>

namespace wpi {
class raw_ostream;
}  // namespace wpi

namespace nt::net {

class WireConnection {
 public:
  virtual ~WireConnection() = default;

  virtual unsigned int GetVersion() const = 0;

  virtual void SendPing(uint64_t time) = 0;

  virtual bool Ready() const = 0;

  // These return <0 on error, 0 on success. On buffer full, a positive number
  // is is returned indicating the number of previous messages (including this
  // call) that were NOT sent, e.g. 1 if just this call to WriteText or
  // WriteBinary was not sent, 2 if the this call and the *previous* call were
  // not sent.
  [[nodiscard]]
  virtual int WriteText(
      wpi::function_ref<void(wpi::raw_ostream& os)> writer) = 0;
  [[nodiscard]]
  virtual int WriteBinary(
      wpi::function_ref<void(wpi::raw_ostream& os)> writer) = 0;

  // Flushes any pending buffers. Return value equivalent to
  // WriteText/WriteBinary (e.g. 1 means the last WriteX call was not sent).
  [[nodiscard]]
  virtual int Flush() = 0;

  // These immediately send the data even if the buffer is full.
  virtual void SendText(
      wpi::function_ref<void(wpi::raw_ostream& os)> writer) = 0;
  virtual void SendBinary(
      wpi::function_ref<void(wpi::raw_ostream& os)> writer) = 0;

  virtual uint64_t GetLastFlushTime() const = 0;  // in microseconds

  // Gets the timestamp of the last incoming data
  virtual uint64_t GetLastReceivedTime() const = 0;  // in microseconds

  virtual void StopRead() = 0;
  virtual void StartRead() = 0;

  virtual void Disconnect(std::string_view reason) = 0;
};

}  // namespace nt::net
