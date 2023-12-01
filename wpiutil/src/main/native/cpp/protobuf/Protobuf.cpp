// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/protobuf/Protobuf.h"

#include <fmt/format.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message.h>

#include "wpi/SmallVector.h"

using namespace wpi;

using google::protobuf::Arena;
using google::protobuf::FileDescriptor;
using google::protobuf::FileDescriptorProto;

namespace {
class VectorOutputStream final
    : public google::protobuf::io::ZeroCopyOutputStream {
 public:
  // Create a StringOutputStream which appends bytes to the given string.
  // The string remains property of the caller, but it is mutated in arbitrary
  // ways and MUST NOT be accessed in any way until you're done with the
  // stream. Either be sure there's no further usage, or (safest) destroy the
  // stream before using the contents.
  //
  // Hint:  If you call target->reserve(n) before creating the stream,
  //   the first call to Next() will return at least n bytes of buffer
  //   space.
  explicit VectorOutputStream(std::vector<uint8_t>& target) : target_{target} {}
  VectorOutputStream(const VectorOutputStream&) = delete;
  ~VectorOutputStream() override = default;

  VectorOutputStream& operator=(const VectorOutputStream&) = delete;

  // implements ZeroCopyOutputStream ---------------------------------
  bool Next(void** data, int* size) override;
  void BackUp(int count) override { target_.resize(target_.size() - count); }
  int64_t ByteCount() const override { return target_.size(); }

 private:
  static constexpr size_t kMinimumSize = 16;

  std::vector<uint8_t>& target_;
};

class SmallVectorOutputStream final
    : public google::protobuf::io::ZeroCopyOutputStream {
 public:
  // Create a StringOutputStream which appends bytes to the given string.
  // The string remains property of the caller, but it is mutated in arbitrary
  // ways and MUST NOT be accessed in any way until you're done with the
  // stream. Either be sure there's no further usage, or (safest) destroy the
  // stream before using the contents.
  //
  // Hint:  If you call target->reserve(n) before creating the stream,
  //   the first call to Next() will return at least n bytes of buffer
  //   space.
  explicit SmallVectorOutputStream(wpi::SmallVectorImpl<uint8_t>& target)
      : target_{target} {
    target.resize(0);
  }
  SmallVectorOutputStream(const SmallVectorOutputStream&) = delete;
  ~SmallVectorOutputStream() override = default;

  SmallVectorOutputStream& operator=(const SmallVectorOutputStream&) = delete;

  // implements ZeroCopyOutputStream ---------------------------------
  bool Next(void** data, int* size) override;
  void BackUp(int count) override { target_.resize(target_.size() - count); }
  int64_t ByteCount() const override { return target_.size(); }

 private:
  static constexpr size_t kMinimumSize = 16;

  wpi::SmallVectorImpl<uint8_t>& target_;
};
}  // namespace

bool VectorOutputStream::Next(void** data, int* size) {
  size_t old_size = target_.size();

  // Grow the string.
  size_t new_size;
  if (old_size < target_.capacity()) {
    // Resize to match its capacity, since we can get away
    // without a memory allocation this way.
    new_size = target_.capacity();
  } else {
    // Size has reached capacity, try to double it.
    new_size = old_size * 2;
  }
  // Avoid integer overflow in returned '*size'.
  new_size = (std::min)(new_size, old_size + (std::numeric_limits<int>::max)());
  // Increase the size, also make sure that it is at least kMinimumSize.
  target_.resize((std::max)(new_size, kMinimumSize));

  *data = target_.data() + old_size;
  *size = target_.size() - old_size;
  return true;
}

bool SmallVectorOutputStream::Next(void** data, int* size) {
  size_t old_size = target_.size();

  // Grow the string.
  size_t new_size;
  if (old_size < target_.capacity()) {
    // Resize to match its capacity, since we can get away
    // without a memory allocation this way.
    new_size = target_.capacity();
  } else {
    // Size has reached capacity, try to double it.
    new_size = old_size * 2;
  }
  // Avoid integer overflow in returned '*size'.
  new_size = (std::min)(new_size, old_size + (std::numeric_limits<int>::max)());
  // Increase the size, also make sure that it is at least kMinimumSize.
  target_.resize_for_overwrite((std::max)(new_size, kMinimumSize));

  *data = target_.data() + old_size;
  *size = target_.size() - old_size;
  return true;
}

void detail::DeleteProtobuf(google::protobuf::Message* msg) {
  if (msg && !msg->GetArena()) {
    delete msg;
  }
}

bool detail::ParseProtobuf(google::protobuf::Message* msg,
                           std::span<const uint8_t> data) {
  return msg->ParseFromArray(data.data(), data.size());
}

bool detail::SerializeProtobuf(wpi::SmallVectorImpl<uint8_t>& out,
                               const google::protobuf::Message& msg) {
  SmallVectorOutputStream stream{out};
  return msg.SerializeToZeroCopyStream(&stream);
}

bool detail::SerializeProtobuf(std::vector<uint8_t>& out,
                               const google::protobuf::Message& msg) {
  VectorOutputStream stream{out};
  return msg.SerializeToZeroCopyStream(&stream);
}

std::string detail::GetTypeString(const google::protobuf::Message& msg) {
  return fmt::format("proto:{}", msg.GetDescriptor()->full_name());
}

static void ForEachProtobufDescriptorImpl(
    const FileDescriptor* desc,
    function_ref<bool(std::string_view typeString)> exists,
    function_ref<void(std::string_view typeString,
                      std::span<const uint8_t> schema)>
        fn,
    Arena* arena, FileDescriptorProto** descproto) {
  std::string name = fmt::format("proto:{}", desc->name());
  if (exists(name)) {
    return;
  }
  for (int i = 0, ndep = desc->dependency_count(); i < ndep; ++i) {
    ForEachProtobufDescriptorImpl(desc->dependency(i), exists, fn, arena,
                                  descproto);
  }
  if (!*descproto) {
    *descproto = Arena::CreateMessage<FileDescriptorProto>(arena);
  }
  (*descproto)->Clear();
  desc->CopyTo(*descproto);
  SmallVector<uint8_t, 128> buf;
  detail::SerializeProtobuf(buf, **descproto);
  fn(name, buf);
}

void detail::ForEachProtobufDescriptor(
    const google::protobuf::Message& msg,
    function_ref<bool(std::string_view filename)> exists,
    function_ref<void(std::string_view filename,
                      std::span<const uint8_t> descriptor)>
        fn) {
  FileDescriptorProto* descproto = nullptr;
  ForEachProtobufDescriptorImpl(msg.GetDescriptor()->file(), exists, fn,
                                msg.GetArena(), &descproto);
  if (descproto && !msg.GetArena()) {
    delete descproto;
  }
}
