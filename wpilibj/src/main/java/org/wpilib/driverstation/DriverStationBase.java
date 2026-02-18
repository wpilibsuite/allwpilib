package org.wpilib.driverstation;

import java.util.Optional;
import java.util.OptionalInt;

import org.wpilib.driverstation.backend.DriverStationBackend;
import org.wpilib.driverstation.backend.DriverStationBackend.Alliance;

public abstract class DriverStationBase {
  public Optional<Alliance> getAlliance() {
    return DriverStationBackend.getAlliance();
  }

  public OptionalInt getLocation() {
    return DriverStationBackend.getLocation();
  }

  public Optional<String> getGameData() {
    return DriverStationBackend.getGameData();
  }
}
