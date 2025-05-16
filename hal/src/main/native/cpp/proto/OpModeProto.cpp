#include "hal/proto/OpMode.h"

#include "wpi/protobuf/ProtobufCallbacks.h"

std::optional<mrc::OpMode> wpi::Protobuf<mrc::OpMode>::Unpack(
    InputStream& Stream) {
  wpi::UnpackCallback<std::string> NameCb;

  mrc_proto_ProtobufOpMode Msg;
  Msg.Name = NameCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Name = NameCb.Items();

  if (Name.empty()) {
    return {};
  }

  mrc::OpMode OutputData;
  OutputData.MoveName(std::move(Name[0]));

  OutputData.Hash = Msg.Hash;

  return OutputData;
}

bool wpi::Protobuf<mrc::OpMode>::Pack(OutputStream& Stream,
                                      const mrc::OpMode& Value) {
  std::string_view EventNameStr = Value.GetName();
  wpi::PackCallback EventName{&EventNameStr};

  mrc_proto_ProtobufOpMode Msg{
      .Hash = Value.Hash,
      .Name = EventName.Callback(),
  };

  return Stream.Encode(Msg);
}

std::optional<std::vector<mrc::OpMode>>
wpi::Protobuf<std::vector<mrc::OpMode>>::Unpack(InputStream& Stream) {
  wpi::StdVectorUnpackCallback<mrc::OpMode> ModesCb;
  ModesCb.SetLimits(DecodeLimits::Add);

  mrc_proto_ProtobufAvailableOpModes Msg;
  Msg.Modes = ModesCb.Callback();

  if (!Stream.Decode(Msg)) {
    return {};
  }

  return ModesCb.Vec();
}

bool wpi::Protobuf<std::vector<mrc::OpMode>>::Pack(
    OutputStream& Stream, const std::vector<mrc::OpMode>& Value) {
  std::span<const mrc::OpMode> ModesSpan = Value;
  wpi::PackCallback Modes{ModesSpan};

  mrc_proto_ProtobufAvailableOpModes Msg{
      .Modes = Modes.Callback(),
  };

  return Stream.Encode(Msg);
}
