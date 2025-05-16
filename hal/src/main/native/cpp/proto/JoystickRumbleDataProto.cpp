#include "hal/proto/JoystickRumbleData.h"

#include "wpi/protobuf/ProtobufCallbacks.h"

std::optional<mrc::JoystickRumbleData>
wpi::Protobuf<mrc::JoystickRumbleData>::Unpack(InputStream& Stream) {
  wpi::UnpackCallback<uint16_t, MRC_MAX_NUM_RUMBLE> RumbleCb;

  mrc_proto_ProtobufJoystickRumbleData Msg{
      .Value = RumbleCb.Callback(),
  };

  if (!Stream.Decode(Msg)) {
    return {};
  }

  auto Rumbles = RumbleCb.Items();

  mrc::JoystickRumbleData Rumble;
  Rumble.SetCount(Rumbles.size());

  for (size_t i = 0; i < Rumble.GetCount(); i++) {
    Rumble.Rumbles()[i] = Rumbles[i];
  }

  return Rumble;
}

bool wpi::Protobuf<mrc::JoystickRumbleData>::Pack(
    OutputStream& Stream, const mrc::JoystickRumbleData& Value) {
  wpi::PackCallback RumbleCb{Value.Rumbles()};

  mrc_proto_ProtobufJoystickRumbleData Msg{
      .Value = RumbleCb.Callback(),
  };

  return Stream.Encode(Msg);
}
