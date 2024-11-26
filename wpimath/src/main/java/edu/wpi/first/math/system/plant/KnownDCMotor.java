package edu.wpi.first.math.system.plant;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RPM;
import static edu.wpi.first.units.Units.Volts;

/** Holds the constants for known DC motors. */
public enum KnownDCMotor {
  /** CIM motor constants. */
  CIM(12, 2.42, 133, 2.7, 5310),
  /** VEX 775 Pro motor constants. */
  Vex775Pro(12, 0.71, 134, 0.7, 18730),
  /** NEO motor constants. */
  NEO(12, 2.6, 105, 1.8, 5676),
  /** MiniCIM motor constants. */
  MiniCIM(12, 1.41, 89, 3, 5840),
  /** Bag motor constants. */
  Bag(12, 0.43, 53, 1.8, 13180),
  /** Andymark RS775_125 motor constants. */
  AndymarkRs775_125(12, 0.28, 18, 1.6, 5800),
  /** Banebots RS775 motor constants. */
  BanebotsRs775(12, 0.72, 97, 2.7, 13050),
  /** Andymark9015 motor constants. */
  Andymark9015(12, 0.36, 71, 3.7, 14270),
  /** Banebots RS550 motor constants. */
  BanebotsRS550(12, 0.38, 84, 0.4, 19000),
  /** Neo 550 motor constants. */
  Neo550(12, 0.97, 100, 1.4, 11000),

  /**
   * Falcon500 constants.
   *
   * @see <a href= "https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/">Falcon 500
   *     Documentation</a>
   */
  Falcon500(12, 4.69, 257, 1.5, 6380),

  /**
   * Falcon 500 FOC constants.
   *
   * @see <a href= "https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/">Falcon 500
   *     FOC (Field-Oriented Control) Documentation</a>
   */
  Falcon500Foc(12, 5.84, 304, 1.5, 6080),

  /**
   * Romi BuiltIn constants.
   *
   * @see <a href="https://www.pololu.com/product/1520/specs">Romi Built-In Documentation</a>
   */
  RomiBuiltIn(4.5, 0.1765, 1.25, 0.13, 150),

  /**
   * Kraken X60 constants.
   *
   * @see <a href= "https://store.ctr-electronics.com/announcing-kraken-x60/">Kraken X60
   *     Documentation</a>
   */
  KrakenX60(12, 7.09, 366, 2, 6000),

  /**
   * Kraken X60 FOC constants.
   *
   * @see <a href= "https://store.ctr-electronics.com/announcing-kraken-x60/">Kraken X60 FOC
   *     (Field-Oriented Control) Documentation</a>
   */
  KrakenX60Foc(12, 9.37, 483, 2, 5800),

  /**
   * NEO Vortex constants.
   *
   * @see <a href="https://www.revrobotics.com/next-generation-spark-neo/">NEO Vortex
   *     Documentation</a>
   */
  NeoVortex(12, 3.60, 211, 3.6, 6784);

  /** DCMotor object for this known type. */
  public final DCMotor dcMotor;

  KnownDCMotor(
      double nominalVoltageVolts,
      double stallTorqueNewtonMeters,
      double stallCurrentAmps,
      double freeCurrentAmps,
      double freeSpeedRPM) {
    dcMotor =
        new DCMotor(
            Volts.of(nominalVoltageVolts),
            NewtonMeters.of(stallTorqueNewtonMeters),
            Amps.of(stallCurrentAmps),
            Amps.of(freeCurrentAmps),
            RPM.of(freeSpeedRPM));
  }
}
