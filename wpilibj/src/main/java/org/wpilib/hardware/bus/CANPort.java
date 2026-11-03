// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.bus;

/**
 * CAN port mapping.
 *
 * <p>S0-S4 are Systemcore CAN buses. D0-D19 are Motioncore CAN buses.
 */
public enum CANPort {
  /** CAN port S0. */
  CAN_S0(0),
  /** CAN port S1. */
  CAN_S1(1),
  /** CAN port S2. */
  CAN_S2(2),
  /** CAN port S3. */
  CAN_S3(3),
  /** CAN port S4. */
  CAN_S4(4),
  /** CAN port D0. */
  CAN_D0(5),
  /** CAN port D1. */
  CAN_D1(6),
  /** CAN port D2. */
  CAN_D2(7),
  /** CAN port D3. */
  CAN_D3(8),
  /** CAN port D4. */
  CAN_D4(9),
  /** CAN port D5. */
  CAN_D5(10),
  /** CAN port D6. */
  CAN_D6(11),
  /** CAN port D7. */
  CAN_D7(12),
  /** CAN port D8. */
  CAN_D8(13),
  /** CAN port D9. */
  CAN_D9(14),
  /** CAN port D10. */
  CAN_D10(15),
  /** CAN port D11. */
  CAN_D11(16),
  /** CAN port D12. */
  CAN_D12(17),
  /** CAN port D13. */
  CAN_D13(18),
  /** CAN port D14. */
  CAN_D14(19),
  /** CAN port D15. */
  CAN_D15(20),
  /** CAN port D16. */
  CAN_D16(21),
  /** CAN port D17. */
  CAN_D17(22),
  /** CAN port D18. */
  CAN_D18(23),
  /** CAN port D19. */
  CAN_D19(24);

  /** CAN port ID. */
  public final int value;

  CANPort(int value) {
    this.value = value;
  }
}
