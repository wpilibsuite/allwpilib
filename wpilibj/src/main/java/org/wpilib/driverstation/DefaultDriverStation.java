package org.wpilib.driverstation;

import org.wpilib.driverstation.backend.DriverStationBackend;

public class DefaultDriverStation extends DriverStationBase {
    private final Gamepad[] m_gamepads;

    public DefaultDriverStation() {
        m_gamepads = new Gamepad[DriverStationBackend.kJoystickPorts];
        for (int i = 0; i < m_gamepads.length; i++) {
            m_gamepads[i] = new Gamepad(i);
        }
    }

    public Gamepad getGamepad(int port) {
        return m_gamepads[port];
    }
}
