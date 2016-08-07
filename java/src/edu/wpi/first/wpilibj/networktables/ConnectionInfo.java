package edu.wpi.first.wpilibj.networktables;

public class ConnectionInfo {
  public final String remote_id;
  public final String remote_ip;
  public final int remote_port;
  public final long last_update;
  public final int protocol_version;

  ConnectionInfo(String remote_id, String remote_ip, int remote_port, long last_update, int protocol_version) {
    this.remote_id = remote_id;
    this.remote_ip = remote_ip;
    this.remote_port = remote_port;
    this.last_update = last_update;
    this.protocol_version = protocol_version;
  }
}
