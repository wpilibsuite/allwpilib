// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <span>
#include <string>

namespace nt {
class Value;
}  // namespace nt

namespace nt::net3 {

class MessageHandler3 {
 public:
  virtual void KeepAlive() = 0;
  virtual void ServerHelloDone() = 0;
  virtual void ClientHelloDone() = 0;
  virtual void ClearEntries() = 0;
  virtual void ProtoUnsup(unsigned int proto_rev) = 0;
  virtual void ClientHello(std::string_view self_id,
                           unsigned int proto_rev) = 0;
  virtual void ServerHello(unsigned int flags, std::string_view self_id) = 0;
  virtual void EntryAssign(std::string_view name, unsigned int id,
                           unsigned int seq_num, const Value& value,
                           unsigned int flags) = 0;
  virtual void EntryUpdate(unsigned int id, unsigned int seq_num,
                           const Value& value) = 0;
  virtual void FlagsUpdate(unsigned int id, unsigned int flags) = 0;
  virtual void EntryDelete(unsigned int id) = 0;
  virtual void ExecuteRpc(unsigned int id, unsigned int uid,
                          std::span<const uint8_t> params) = 0;
  virtual void RpcResponse(unsigned int id, unsigned int uid,
                           std::span<const uint8_t> result) = 0;
};

/* Decodes NT3 protocol into native representation. */
class WireDecoder3 {
 public:
  explicit WireDecoder3(MessageHandler3& out);
  ~WireDecoder3();

  /**
   * Executes the decoder.  All input data will be consumed unless an error
   * occurs.
   * @param in input data (updated during parse)
   * @return false if error occurred
   */
  bool Execute(std::span<const uint8_t>* in);

  void SetError(std::string_view message);
  std::string GetError() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace nt::net3
