package edu.wpi.first.wpilibj.networktables;

public class ConnectionInfo {
  final String remote_id;
  final String remote_name;
  final int remote_port;
  final long last_update;
  final int protocol_version;

  ConnectionInfo(String remote_id, String remote_name, int remote_port, long last_update, int protocol_version) {
    this.remote_id = remote_id;
    this.remote_name = remote_name;
    this.remote_port = remote_port;
    this.last_update = last_update;
    this.protocol_version = protocol_version;
  }
}
