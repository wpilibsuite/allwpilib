// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <utility>

#include <upb/mem/arena.h>
#include <upb/reflection/def.h>

namespace wpi::util {
/**
 * Database of protobuf dynamic messages. This automatically cleans up the
 * underlying upb pointers.
 */
class ProtobufMessageDatabase {
 public:
  ProtobufMessageDatabase() {}
  ~ProtobufMessageDatabase();
  ProtobufMessageDatabase(const ProtobufMessageDatabase&) = delete;
  ProtobufMessageDatabase& operator=(const ProtobufMessageDatabase&) = delete;
  ProtobufMessageDatabase(ProtobufMessageDatabase&& rhs)
      : m_protoPool(rhs.m_protoPool), m_arena(rhs.m_arena) {
    rhs.m_protoPool = nullptr;
    rhs.m_arena = nullptr;
  }
  ProtobufMessageDatabase& operator=(ProtobufMessageDatabase&& rhs) {
    std::swap(m_arena, rhs.m_arena);
    std::swap(m_protoPool, rhs.m_protoPool);
    return *this;
  }

  /**
   * Adds a file descriptor to the database. The message names are inferred from
   * the data.
   *
   * @param data FileDescriptorProto serialized data
   * @return False if data could not be parsed
   */
  bool Add(std::span<const uint8_t> data);

  /**
   * Finds a message definition in the database by name.
   *
   * @param name Type name
   * @return Protobuf message definition, or nullptr if not found
   */
  const upb_MessageDef* Find(const char* name) const;

  /**
   * Decodes a message.
   *
   * @param messageDef The protobuf message definition (typically from Find).
   * @param data The message data (typically transmitted over the wire).
   * @return The decoded message, or nullptr.
   */
  upb_Message* Decode(const upb_MessageDef* messageDef,
                      std::span<const uint8_t> data);

  /**
   * Gets the underlying arena.
   *
   * @return The arena.
   */
  upb_Arena* GetArena() { return m_arena; };

 private:
  upb_DefPool* m_protoPool = upb_DefPool_New();
  upb_Arena* m_arena = upb_Arena_New();
};
}  // namespace wpi::util
