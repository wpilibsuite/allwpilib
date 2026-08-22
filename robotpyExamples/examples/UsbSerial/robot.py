#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import sys
import threading

import wpilib
import wpinet


class MyRobot(wpilib.TimedRobot):
    def __init__(self) -> None:
        super().__init__()

        self._serial_lock = threading.Lock()
        self._serial: wpilib.SerialPort | None = None
        self._serial_path = ""
        self._detector = wpinet.UsbDeviceDetector(
            0,  # Physical USB port; valid values are 0-3.
            self._device_connected,
            self._device_disconnected,
        )
        if not self._detector.start():
            print("USB device detection is unavailable", file=sys.stderr)

    def robot_periodic(self) -> None:
        # Detector callbacks execute on a worker thread, so protect SerialPort
        # use with the same lock used when callbacks open and close it.
        with self._serial_lock:
            if self._serial is None:
                return

            try:
                available = self._serial.get_bytes_received()
                if available > 0:
                    data = self._serial.read(min(available, 256))
                    sys.stdout.buffer.write(data)
                    sys.stdout.buffer.flush()
            except RuntimeError as error:
                print(f"Serial read failed: {error}", file=sys.stderr)

    def _device_connected(self, device: wpinet.UsbDevice) -> None:
        # The detector is generic. A USB serial adapter has a child device in
        # the tty subsystem; cameras, HID devices, and storage devices have
        # different subsystem values and can be handled by other consumers.
        if device.subsystem != "tty" or not device.device_node:
            return

        with self._serial_lock:
            if self._serial is not None:
                return

            try:
                self._serial = wpilib.SerialPort(115200, device.device_node)
                self._serial_path = device.device_node
                print(f"Opened serial device {self._serial_path}")
            except RuntimeError as error:
                print(f"Could not open {device.device_node}: {error}", file=sys.stderr)

    def _device_disconnected(self, device: wpinet.UsbDevice) -> None:
        with self._serial_lock:
            if self._serial is None or device.device_node != self._serial_path:
                return

            # Releasing SerialPort closes its native handle.
            self._serial = None
            self._serial_path = ""
            print("Closed disconnected serial device")

    def end_competition(self) -> None:
        # Stop the callback thread before releasing state used by the callbacks.
        self._detector.stop()
        with self._serial_lock:
            self._serial = None
        super().end_competition()
