// Copyright (c) Sleipnir contributors

#pragma once

namespace slp::detail {

/// Type tag used to designate an uninitialized VariableMatrix.
struct empty_t {};

/// Designates an uninitialized VariableMatrix.
static constexpr empty_t empty{};

}  // namespace slp::detail
