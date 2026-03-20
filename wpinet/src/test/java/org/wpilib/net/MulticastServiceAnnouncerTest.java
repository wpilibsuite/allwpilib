// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.net;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;

class MulticastServiceAnnouncerTest {
  @Test
  void emptyText() throws InterruptedException, UnknownHostException {
    final String serviceName = "Foaksdfjasklfj";
    final String serviceType = "_wpinotxt._tcp";
    final int port = 12345;

    try (MulticastServiceAnnouncer announcer =
            new MulticastServiceAnnouncer(serviceName, serviceType, port);
        MulticastServiceResolver resolver = new MulticastServiceResolver(serviceType)) {
      assertTrue(announcer.hasImplementation() && resolver.hasImplementation());

      announcer.start();
      resolver.start();

      List<ServiceData> allData = new ArrayList<>();

      for (int i = 0; i < 10; i++) {
        ServiceData[] data = resolver.getData();
        if (data == null) {
          continue;
        }

        allData.addAll(List.of(data));

        if (!allData.isEmpty()) {
          break;
        }

        Thread.sleep(1000);
      }

      assertFalse(allData.isEmpty(), "Expected at least one service entry, but got none");

      resolver.stop();
      announcer.stop();
    }
  }
}
