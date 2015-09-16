package edu.wpi.first.wpilibj.networktables;

public class EntryInfo {
  final String name;
  final int type;
  final int flags;
  final long last_change;

  EntryInfo(String name, int type, int flags, long last_change) {
    this.name = name;
    this.type = type;
    this.flags = flags;
    this.last_change = last_change;
  }
}
