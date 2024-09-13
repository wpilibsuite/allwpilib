// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>

#include "wpi/StringMap.h"

namespace wpi {

/**
 * Database of protobuf dynamic messages. Unlike the protobuf descriptor pools
 * and databases, this gracefully handles adding descriptors out of order and
 * descriptors being replaced.
 */
class ProtobufMessageDatabase {
 public:
  /**
   * Adds a file descriptor to the database. If the file references other
   * files that have not yet been added, no messages in that file will be
   * available until those files are added. Replaces any existing file with
   * the same name.
   *
   * @param filename filename
   * @param data FileDescriptorProto serialized data
   * @return False if could not parse data
   */
  bool Add(std::string_view filename, std::span<const uint8_t> data);

  /**
   * Finds a message in the database by name.
   *
   * @param name type name
   * @return protobuf message, or nullptr if not found
   */
  google::protobuf::Message* Find(std::string_view name) const;

  /**
   * Gets message factory.
   *
   * @return message factory
   */
  google::protobuf::MessageFactory* GetMessageFactory() {
    return m_factory.get();
  }

 private:
  struct ProtoFile {
    std::unique_ptr<google::protobuf::FileDescriptorProto> proto;
    std::vector<std::string> uses;
    bool complete = false;
    bool inPool = false;
  };

  void Build(std::string_view filename, ProtoFile& file);
  bool Rebuild(ProtoFile& file);

  std::unique_ptr<google::protobuf::DescriptorPool> m_pool =
      std::make_unique<google::protobuf::DescriptorPool>();
  std::unique_ptr<google::protobuf::DynamicMessageFactory> m_factory =
      std::make_unique<google::protobuf::DynamicMessageFactory>();
  wpi::StringMap<ProtoFile> m_files;  // indexed by filename
  // indexed by type string
  mutable wpi::StringMap<std::unique_ptr<google::protobuf::Message>> m_msgs;
};

}  // namespace wpi
