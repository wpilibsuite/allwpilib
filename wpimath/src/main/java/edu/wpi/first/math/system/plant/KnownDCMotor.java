package edu.wpi.first.math.system.plant;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RPM;
import static edu.wpi.first.units.Units.Volts;

/** Holds the constants for known DC motors. */
public enum KnownDCMotor {
    /** TODO: CIM DOCUMENTATION */
    CIM(12, 2.42, 133, 2.7, 5310),
    /** TODO: 775Pro DOCUMENTATION */
    Vex775Pro(12, 0.71, 134, 0.7, 18730),
    /** TODO: NEO DOCUMENTATION */
    NEO(12, 2.6, 105, 1.8, 5676),
    /** TODO: MiniCIM DOCUMENTATION */
    MiniCIM(12, 1.41, 89, 3, 5840),
    /** TODO: Bag DOCUMENTATION */
    Bag(12, 0.43, 53, 1.8, 13180),
    /** TODO: Andymark RS775-125 DOCUMENTATION */
    AndymarkRs775_125(12, 0.28, 18, 1.6, 5800),
    /** TODO: Banebots RS775 DOOCUMENTATION */
    BanebotsRs775(12, 0.72, 97, 2.7, 13050),
    /** TODO: Andymark 9015 DOCUMENTATION */
    Andymark9015(12, 0.36, 71, 3.7, 14270),
    /** TODO: Banebots RS 550 DOCUMENTATION */
    BanebotsRS550(12, 0.38, 84, 0.4, 19000),
    /** TODO: Neo 550 DOCUMENTATION */
    Neo550(12, 0.97, 100, 1.4, 11000),

    /**
     * @see <a href=
     *      "https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/">Falcon 500 Documentation</a>
     */
    Falcon500(12, 4.69, 257, 1.5, 6380),

    /**
     * @see <a href=
     *      "https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/">Falcon
     *      500 FOC (Field-Oriented Control) Documentation</a>
     */
    Falcon500Foc(12, 5.84, 304, 1.5, 6080),

    /**
     * @see <a href="https://www.pololu.com/product/1520/specs">Romi Built-In
     *      Documentation</a>
     */
    RomiBuiltIn(4.5, 0.1765, 1.25, 0.13, 150),

    /**
     * @see <a href=
     *      "https://store.ctr-electronics.com/announcing-kraken-x60/">Kraken X60
     *      Documentation</a>
     */
    KrakenX60(12, 7.09, 366, 2, 6000),

    /**
     * @see <a href=
     *      "https://store.ctr-electronics.com/announcing-kraken-x60/">Kraken X60
     *      FOC (Field-Oriented Control) Documentation</a>
     */
    KrakenX60Foc(12, 9.37, 483, 2, 5800),

    /**
     * @see <a href="https://www.revrobotics.com/next-generation-spark-neo/">NEO
     *      Vortex Documentation</a>
     */
    NeoVortex(12, 3.60, 211, 3.6, 6784);

    /** DCMotor object for this known type. */
    public final DCMotor dcMotor;

    private KnownDCMotor(
            double nominalVoltageVolts,
            double stallTorqueNewtonMeters,
            double stallCurrentAmps,
            double freeCurrentAmps,
            double freeSpeedRPM) {
        dcMotor = new DCMotor(
                Volts.of(nominalVoltageVolts),
                NewtonMeters.of(stallTorqueNewtonMeters),
                Amps.of(stallCurrentAmps),
                Amps.of(freeCurrentAmps),
                RPM.of(freeSpeedRPM));

    }
}
