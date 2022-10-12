// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MockWireConnection.h"

using namespace nt::net;

void MockWireConnection::StartSendText() {
  if (m_in_text) {
    m_text_os << ',';
  } else {
    m_text_os << '[';
    m_in_text = true;
  }
}

void MockWireConnection::FinishSendText() {
  if (m_in_text) {
    m_text_os << ']';
    m_in_text = false;
  }
  m_text_os.flush();
  Text(m_text);
  m_text.clear();
}
