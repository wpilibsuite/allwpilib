// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/protobuf/ProtobufMessageDatabase.hpp"

#include "upb/reflection/def.h"

using namespace wpi::util;

ProtobufMessageDatabase::~ProtobufMessageDatabase() {
  if (m_protoPool) {
    upb_DefPool_Free(m_protoPool);
  }
  if (m_arena) {
    upb_Arena_Free(m_arena);
  }
}

bool ProtobufMessageDatabase::Add(std::span<const uint8_t> data) {
  // protobuf descriptor handling
  if (!m_protoPool || !m_arena) {
    return false;
  }
  auto* descriptor = google_protobuf_FileDescriptorProto_parse(
      reinterpret_cast<const char*>(data.data()), data.size(), m_arena);
  if (!descriptor) {
    return false;
  }
  upb_Status status;
  upb_Status_Clear(&status);
  upb_DefPool_AddFile(m_protoPool, descriptor, &status);
  return upb_Status_IsOk(&status);
}

const upb_MessageDef* ProtobufMessageDatabase::Find(const char* name) const {
  return upb_DefPool_FindMessageByName(m_protoPool, name);
}

upb_Message* ProtobufMessageDatabase::Decode(const upb_MessageDef* messageDef,
                                             std::span<const uint8_t> data) {
  const upb_MiniTable* miniTable = upb_MessageDef_MiniTable(messageDef);

  upb_Message* message = upb_Message_New(miniTable, m_arena);
  upb_DecodeStatus status =
      upb_Decode(reinterpret_cast<const char*>(data.data()), data.size(),
                 message, miniTable, nullptr, 0, m_arena);
  if (status == kUpb_DecodeStatus_Ok) {
    return message;
  } else {
    return nullptr;
  }
}
