// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/uv/Handle.h"

using namespace wpi::uv;

Handle::~Handle() noexcept {
  if (!m_closed && m_uv_handle->type != UV_UNKNOWN_HANDLE) {
    uv_close(m_uv_handle, [](uv_handle_t* uv_handle) { delete uv_handle; });
  } else {
    delete m_uv_handle;
  }
}

void Handle::Close() noexcept {
  if (!IsClosing()) {
    uv_close(m_uv_handle, [](uv_handle_t* handle) {
      Handle& h = *static_cast<Handle*>(handle->data);
      h.closed();
      h.Release();  // free ourselves
    });
    m_closed = true;
  }
}

void Handle::AllocBuf(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
  auto& h = *static_cast<Handle*>(handle->data);
  *buf = h.m_allocBuf(size);
}

void Handle::DefaultFreeBuf(Buffer& buf) {
  buf.Deallocate();
}
