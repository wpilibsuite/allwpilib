// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/mDNSResolver.h"

using namespace wpi;

struct mDNSResolver::Impl {};

mDNSResolver::mDNSResolver(std::string_view serviceType,
                           mDnsRevolveCompletionFunc onFound) {}

mDNSResolver::~mDNSResolver() noexcept {}

void mDNSResolver::Start() {}

void mDNSResolver::Stop() {}
