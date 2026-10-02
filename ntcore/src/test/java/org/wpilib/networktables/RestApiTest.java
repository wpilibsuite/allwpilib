// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.URI;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

class RestApiTest {
  @TempDir Path m_tempDir;

  @Test
  void restUpdatesJavaSubscriber() throws IOException, InterruptedException {
    int port;
    try (var socket = new ServerSocket(0)) {
      port = socket.getLocalPort();
    }
    try (var inst = NetworkTableInstance.create();
        var sub = inst.getIntegerTopic("/rest/java").subscribe(0)) {
      inst.startServer(m_tempDir.resolve("persistent.json").toString(), "127.0.0.1", "", port);
      long deadline = System.nanoTime() + 5_000_000_000L;
      while (true) {
        try (var socket = new Socket("127.0.0.1", port)) {
          assertTrue(socket.isConnected());
          break;
        } catch (IOException ex) {
          if (System.nanoTime() >= deadline) {
            throw ex;
          }
          Thread.sleep(10);
        }
      }
      var url = URI.create("http://127.0.0.1:" + port + "/nt/v1/topics/%2Frest%2Fjava").toURL();
      var conn = (HttpURLConnection) url.openConnection();
      try {
        conn.setConnectTimeout(5000);
        conn.setReadTimeout(5000);
        conn.setRequestMethod("PUT");
        conn.setRequestProperty("Content-Type", "application/json");
        conn.setDoOutput(true);
        try (var output = conn.getOutputStream()) {
          output.write("{\"type\":\"int\",\"value\":42}".getBytes(StandardCharsets.UTF_8));
        }
        assertEquals(201, conn.getResponseCode());
        try (var input = conn.getInputStream()) {
          assertTrue(
              new String(input.readAllBytes(), StandardCharsets.UTF_8).contains("\"value\":42"));
        }
        assertEquals(42, sub.get());
        assertTrue(sub.getTopic().isRetained());
      } finally {
        conn.disconnect();
      }
      String resource = url.toString();
      assertArrayEquals(
          "\"int\"".getBytes(StandardCharsets.UTF_8),
          request(resource + "/type", "GET", null, "application/json", "application/json", 200));
      assertArrayEquals(
          new byte[] {42},
          request(
              resource + "/value", "GET", null, "application/json", "application/msgpack", 200));
      request(
          resource + "/value",
          "PUT",
          new byte[] {43},
          "application/msgpack",
          "application/json",
          204);
      assertEquals(43, sub.get());
      assertArrayEquals(
          "43".getBytes(StandardCharsets.UTF_8),
          request(resource + "/value", "GET", null, "application/json", "application/json", 200));
      assertArrayEquals(
          "[]".getBytes(StandardCharsets.UTF_8),
          request(
              "http://127.0.0.1:" + port + "/nt/v1/topics/%24pub%24%2Frest%2Fjava/value",
              "GET",
              null,
              "application/json",
              "application/json",
              200));
      String persistentUrl = "http://127.0.0.1:" + port + "/nt/v1/persistent.json";
      try (var keep = inst.getIntegerTopic("/rest/keep").subscribe(0)) {
        String upload =
            """
            [
              {"name":"/rest/java","type":"int","value":50,"properties":{"persistent":true}},
              {"name":"/rest/keep","type":"int","value":7,"properties":{"persistent":true}}
            ]
            """;
        request(
            persistentUrl,
            "PUT",
            upload.getBytes(StandardCharsets.UTF_8),
            "application/json",
            "application/json",
            204);
        assertEquals(50, sub.get());
        assertEquals(7, keep.get());
        upload =
            """
            [{"name":"/rest/java","type":"int","value":51,"properties":{"persistent":true}}]
            """;
        request(
            persistentUrl,
            "PUT",
            upload.getBytes(StandardCharsets.UTF_8),
            "application/json",
            "application/json",
            204);
        assertEquals(51, sub.get());
        assertEquals(7, keep.get());
        assertTrue(keep.getTopic().isPersistent());
        request(
            persistentUrl,
            "PUT",
            "[]".getBytes(StandardCharsets.UTF_8),
            "application/json",
            "application/json",
            204);
        byte[] downloaded =
            request(persistentUrl, "GET", null, "application/json", "application/json", 200);
        request(persistentUrl, "PUT", downloaded, "application/json", "application/json", 204);
        assertEquals(51, sub.get());
        assertEquals(7, keep.get());
        assertArrayEquals(
            downloaded,
            request(
                "http://127.0.0.1:" + port + "/nt/persistent.json",
                "GET",
                null,
                "application/json",
                "application/json",
                200));
      }
      conn = (HttpURLConnection) url.openConnection();
      try {
        conn.setConnectTimeout(5000);
        conn.setReadTimeout(5000);
        conn.setRequestMethod("DELETE");
        assertEquals(204, conn.getResponseCode());
        assertFalse(sub.getTopic().exists());
      } finally {
        conn.disconnect();
      }
    }
  }

  private static byte[] request(
      String url, String method, byte[] body, String contentType, String accept, int expectedStatus)
      throws IOException {
    var conn = (HttpURLConnection) URI.create(url).toURL().openConnection();
    try {
      conn.setConnectTimeout(5000);
      conn.setReadTimeout(5000);
      conn.setRequestMethod(method);
      conn.setRequestProperty("Content-Type", contentType);
      conn.setRequestProperty("Accept", accept);
      if (body != null) {
        conn.setDoOutput(true);
        try (var output = conn.getOutputStream()) {
          output.write(body);
        }
      }
      assertEquals(expectedStatus, conn.getResponseCode());
      try (var input = conn.getInputStream()) {
        return input.readAllBytes();
      }
    } finally {
      conn.disconnect();
    }
  }
}
