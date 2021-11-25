// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/mDNSAnnouncer.h"

using namespace wpi;

struct mDNSAnnouncer::Impl {};

mDNSAnnouncer::mDNSAnnouncer(
    std::string_view serviceName, std::string_view serviceType,
    wpi::span<std::pair<std::string, std::string>> txt) {}

mDNSAnnouncer::~mDNSAnnouncer() noexcept {}

void mDNSAnnouncer::Start() {}

void mDNSAnnouncer::Stop() {}
