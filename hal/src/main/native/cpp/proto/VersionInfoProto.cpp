// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>

#include <wpi/protobuf/ProtobufCallbacks.h>

#include "hal/proto/VersionInfo.h"

std::optional<mrc::VersionInfo> wpi::Protobuf<mrc::VersionInfo>::Unpack(
    InputStream& Stream) {
  wpi::UnpackCallback<std::string> NameCb;
  wpi::UnpackCallback<std::string> VersionCb;

  mrc_proto_ProtobufVersionInfo Msg{
      .DeviceId = 0,
      .Name = NameCb.Callback(),
      .Version = VersionCb.Callback(),
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Name = NameCb.Items();
  auto Version = VersionCb.Items();

  if (Name.empty() || Version.empty()) {
    return {};
  }

  mrc::VersionInfo ToRet;

  ToRet.DeviceId = Msg.DeviceId;
  ToRet.MoveName(std::move(Name[0]));
  ToRet.MoveVersion(std::move(Version[0]));

  return ToRet;
}

bool wpi::Protobuf<mrc::VersionInfo>::Pack(OutputStream& Stream,
                                           const mrc::VersionInfo& Value) {
  std::string_view NameView = Value.GetName();
  std::string_view VersionView = Value.GetVersion();
  wpi::PackCallback NameCb{&NameView};
  wpi::PackCallback VersionCb{&VersionView};

  mrc_proto_ProtobufVersionInfo Msg{
      .DeviceId = Value.DeviceId,
      .Name = NameCb.Callback(),
      .Version = VersionCb.Callback(),
  };
  return Stream.Encode(Msg);
}
