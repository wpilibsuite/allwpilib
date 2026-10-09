// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifdef __linux__

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "../../../main/native/linux/BlueZInternal.hpp"

using namespace wpi::net::linuxbluetooth;

TEST_CASE("Bluetooth pairing agent only authorizes its requested device",
          "[bluetooth]") {
  std::string error;
  const auto* api = DBusApi::Get(&error);
  if (!api) {
    SKIP(error);
  }
  constexpr const char* DEVICE = "/org/bluez/hci0/dev_AA_BB_CC_DD_EE_01";
  for (const char* method :
       {"RequestConfirmation", "RequestAuthorization", "AuthorizeService",
        "DisplayPinCode", "DisplayPasskey"}) {
    for (int argument : {0, 1, 2, 3}) {
      CAPTURE(method, argument);
      DBusMessagePtr request{api->dbus_message_new_method_call(
          "org.wpilib.Test", "/agent", "org.bluez.Agent1", method)};
      REQUIRE(request);
      api->dbus_message_set_serial(request.Get(), 1);
      if (argument != 0) {
        DBusMessageIter iter;
        api->dbus_message_iter_init_append(request.Get(), &iter);
        const char* device =
            argument == 2 ? "/org/bluez/hci0/dev_AA_BB_CC_DD_EE_02" : DEVICE;
        REQUIRE(api->dbus_message_iter_append_basic(
            &iter, argument == 3 ? DBUS_TYPE_STRING : DBUS_TYPE_OBJECT_PATH,
            &device));
      }

      DBusMessagePtr reply;
      CHECK(CreateBlueZPairingReply(request.Get(), DEVICE, &reply) ==
            DBUS_HANDLER_RESULT_HANDLED);
      REQUIRE(reply);
      if (argument == 1) {
        CHECK(api->dbus_message_get_type(reply.Get()) !=
              DBUS_MESSAGE_TYPE_ERROR);
      } else {
        CHECK(api->dbus_message_get_type(reply.Get()) ==
              DBUS_MESSAGE_TYPE_ERROR);
        CHECK(std::string{api->dbus_message_get_error_name(reply.Get())} ==
              "org.bluez.Error.Rejected");
      }
    }
  }
}

#endif
