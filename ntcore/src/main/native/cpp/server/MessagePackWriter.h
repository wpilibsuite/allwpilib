// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <vector>

#include <wpi/MessagePack.h>
#include <wpi/raw_ostream.h>

namespace nt::server {

struct Writer : public mpack::mpack_writer_t {
  Writer() {
    mpack::mpack_writer_init(this, buf, sizeof(buf));
    mpack::mpack_writer_set_context(this, &os);
    mpack::mpack_writer_set_flush(
        this, [](mpack::mpack_writer_t* w, const char* buffer, size_t count) {
          static_cast<wpi::raw_ostream*>(w->context)->write(buffer, count);
        });
  }

  std::vector<uint8_t> bytes;
  wpi::raw_uvector_ostream os{bytes};
  char buf[128];
};

}  // namespace nt::server
