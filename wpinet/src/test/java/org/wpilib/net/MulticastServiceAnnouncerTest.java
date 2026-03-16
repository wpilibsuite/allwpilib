// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.net;

import static org.junit.jupiter.api.Assertions.*;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import org.junit.jupiter.api.Test;

public class MulticastServiceAnnouncerTest {
  @Test
  public void emptyText() throws InterruptedException, UnknownHostException {
    final String serviceName = "Foaksdfjasklfj";
    final String serviceType = "_wpinotxt._tcp";
    final int port = new Random().nextInt(65535);

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
          System.out.println("null data");
          continue;
        }

        System.out.printf("Got %d data%n", data.length);

        for (ServiceData it : data) {
          String ipString =
              InetAddress.getByAddress(
                      ByteBuffer.allocate(4).putInt((int) it.getIpv4Address()).array())
                  .getHostAddress();

          System.out.printf(
              "service %s at host %s ipv4 %s%n", it.getServiceName(), it.getHostName(), ipString);
          allData.add(it);
        }

        if (!allData.isEmpty()) {
          break;
        }

        Thread.sleep(1000);
      }

      System.out.printf("Ending with %d%n", allData.size());
      assertFalse(allData.isEmpty(), "Expected at least one service entry, but got none");

      resolver.stop();
      announcer.stop();
    }
  }
}
