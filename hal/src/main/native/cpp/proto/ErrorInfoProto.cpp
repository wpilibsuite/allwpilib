// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>

#include "wpi/hal/proto/ErrorInfo.h"
#include "wpi/util/protobuf/ProtobufCallbacks.hpp"

std::optional<mrc::ErrorInfo> wpi::util::Protobuf<mrc::ErrorInfo>::Unpack(
    InputStream& Stream) {
  wpi::util::UnpackCallback<std::string> DetailsCb;
  wpi::util::UnpackCallback<std::string> LocationCb;
  wpi::util::UnpackCallback<std::string> CallStackCb;

  mrc_proto_ProtobufErrorInfo Msg{
      .IsError = false,
      .ErrorCode = 0,
      .Details = DetailsCb.Callback(),
      .Location = LocationCb.Callback(),
      .CallStack = CallStackCb.Callback(),
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Details = DetailsCb.Items();
  auto Location = LocationCb.Items();
  auto CallStack = CallStackCb.Items();

  mrc::ErrorInfo ToRet;
  ToRet.IsError = Msg.IsError;
  ToRet.ErrorCode = Msg.ErrorCode;

  if (!Details.empty()) {
    ToRet.MoveDetails(std::move(Details[0]));
  }

  if (!Location.empty()) {
    ToRet.MoveLocation(std::move(Location[0]));
  }

  if (!CallStack.empty()) {
    ToRet.MoveCallStack(std::move(CallStack[0]));
  }

  return ToRet;
}

bool wpi::util::Protobuf<mrc::ErrorInfo>::Pack(OutputStream& Stream,
                                               const mrc::ErrorInfo& Value) {
  std::string_view DetailsView = Value.GetDetails();
  std::string_view LocationView = Value.GetLocation();
  std::string_view CallStackView = Value.GetCallStack();
  wpi::util::PackCallback DetailsCb{&DetailsView};
  wpi::util::PackCallback LocationCb{&LocationView};
  wpi::util::PackCallback CallStackCb{&CallStackView};

  mrc_proto_ProtobufErrorInfo Msg{
      .IsError = Value.IsError,
      .ErrorCode = Value.ErrorCode,
      .Details = DetailsCb.Callback(),
      .Location = LocationCb.Callback(),
      .CallStack = CallStackCb.Callback(),
  };

  return Stream.Encode(Msg);
}
