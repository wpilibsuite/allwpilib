#include "hal/proto/JoystickOutputData.h"

#include "wpi/protobuf/ProtobufCallbacks.h"

std::optional<mrc::JoystickOutputData>
wpi::Protobuf<mrc::JoystickOutputData>::Unpack(InputStream& Stream) {
    mrc_proto_ProtobufJoystickOutputData Msg;

    if (!Stream.Decode(Msg)) {
        return {};
    }

    return mrc::JoystickOutputData{
        .HidOutputs = Msg.HidOutputs,
        .LeftRumble = Msg.LeftRumble,
        .RightRumble = Msg.RightRumble,
    };
}

bool wpi::Protobuf<mrc::JoystickOutputData>::Pack(
    OutputStream& Stream, const mrc::JoystickOutputData& Value) {
    mrc_proto_ProtobufJoystickOutputData Msg{
        .HidOutputs = Value.HidOutputs,
        .LeftRumble = Value.LeftRumble,
        .RightRumble = Value.RightRumble,
    };

    return Stream.Encode(Msg);
}
