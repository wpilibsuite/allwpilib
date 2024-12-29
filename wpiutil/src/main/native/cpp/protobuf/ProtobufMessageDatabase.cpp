// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/protobuf/ProtobufMessageDatabase.h"

#include <memory>
#include <string>

#include <google/protobuf/descriptor.h>

#include "wpi/ProtoHelper.h"

using namespace wpi;

using google::protobuf::Arena;
using google::protobuf::FileDescriptorProto;
using google::protobuf::Message;

bool ProtobufMessageDatabase::Add(std::string_view filename,
                                  std::span<const uint8_t> data) {
  auto& file = m_files[filename];
  bool needsRebuild = false;
  if (file.complete) {
    file.complete = false;

    m_msgs.clear();
    m_factory.reset();

    // rebuild the pool EXCEPT for this descriptor
    m_pool = std::make_unique<google::protobuf::DescriptorPool>();

    for (auto&& p : m_files) {
      p.second.inPool = false;
    }

    needsRebuild = true;
  }

  if (!file.proto) {
    file.proto = std::unique_ptr<FileDescriptorProto>{
        wpi::CreateMessage<FileDescriptorProto>(nullptr)};
  } else {
    // replacing an existing one; remove any previously existing refs
    for (auto&& dep : file.proto->dependency()) {
      auto& depFile = m_files[dep];
      std::erase(depFile.uses, filename);
    }
    file.proto->Clear();
  }

  // parse data
  if (!file.proto->ParseFromArray(data.data(), data.size())) {
    return false;
  }

  // rebuild if necessary; we do this after the parse due to dependencies
  if (needsRebuild) {
    for (auto&& p : m_files) {
      if (p.second.complete && !p.second.inPool) {
        Rebuild(p.second);
      }
    }

    // clear messages and reset factory; Find() will recreate as needed
    m_factory = std::make_unique<google::protobuf::DynamicMessageFactory>();
  }

  // build this one
  Build(filename, file);
  return true;
}

Message* ProtobufMessageDatabase::Find(std::string_view name) const {
  // cached
  auto& msg = m_msgs[name];
  if (msg) {
    return msg.get();
  }

  // need to create it
  auto desc = m_pool->FindMessageTypeByName(std::string{name});
  if (!desc) {
    return nullptr;
  }
  msg = std::unique_ptr<Message>{m_factory->GetPrototype(desc)->New(nullptr)};
  return msg.get();
}

void ProtobufMessageDatabase::Build(std::string_view filename,
                                    ProtoFile& file) {
  if (file.complete) {
    return;
  }
  // are all of the dependencies complete?
  bool complete = true;
  for (auto&& dep : file.proto->dependency()) {
    auto& depFile = m_files[dep];
    if (!depFile.complete) {
      complete = false;
    }
    depFile.uses.emplace_back(filename);
  }
  if (!complete) {
    return;
  }

  // add to pool
  if (!m_pool->BuildFile(*file.proto)) {
    return;
  }
  file.inPool = true;
  file.complete = true;

  // recursively validate all uses
  for (auto&& use : file.uses) {
    Build(use, m_files[use]);
  }
}

bool ProtobufMessageDatabase::Rebuild(ProtoFile& file) {
  for (auto&& dep : file.proto->dependency()) {
    auto& depFile = m_files[dep];
    if (!depFile.inPool) {
      if (!Rebuild(depFile)) {
        return false;
      }
    }
  }
  if (!m_pool->BuildFile(*file.proto)) {
    return false;
  }
  file.inPool = true;
  file.complete = true;
  return true;
}
