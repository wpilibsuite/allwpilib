package edu.wpi.first.math.util;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.UtilityClassTest;
import org.junit.jupiter.api.Test;
public class FalconUnitConversionTest extends UtilityClassTest<FalconUnitConversion> {
    FalconUnitConversionTest() {
        super(FalconUnitConversion.class);
    }

    @Test
    void falcon500VelocityToRadiansPerSecond() {
        assertEquals(29.1456349698, FalconUnitConversion.falcon500VelocityToRadiansPerSecond(950), 1e-2);
    }

    @Test
    void radiansPerSecondToFalcon500Velocity() {
        assertEquals(0.894176881888, FalconUnitConversion.falcon500VelocityToRadiansPerSecond(29.1456349698), 1e-2);
    }

    @Test
    void falcon500VelocityToMetersPerSecond() {
        assertEquals(349.75, FalconUnitConversion.falcon500VelocityToMetersPerSecond(950, 12), 1e-2);
    }

    @Test
    void metersPerSecondToFalcon500Velocity() {
        assertEquals(950, FalconUnitConversion.metersPerSecondToFalcon500Velocity(349.75, 12), 1e-2);
    }

    @Test
    void falcon500VelocityToRotationsPerMinute() {
        assertEquals(950, FalconUnitConversion.falcon500VelocityToRotationsPerMinute(3242.66666667), 1e-2);
    }

    @Test
    void rotationsPerMinuteToFalcon500Velocity() {
        assertEquals(3242.66666667, FalconUnitConversion.rotationsPerMinuteToFalcon500Velocity(950), 1e-2);
    }

    @Test
    void falcon500PositionToDegrees() {
        assertEquals(14.0625, FalconUnitConversion.falcon500PositionToDegrees(80), 1e-2);
    }

    @Test
    void falcon500PositionToRadians() {
        assertEquals(0.245436926062, FalconUnitConversion.falcon500PositionToRadians(80), 1e-2);
    }

    @Test
    void degreesToFalcon500Position() {
        assertEquals(1177.6, FalconUnitConversion.degreesToFalcon500Position(207), 1e-2);
    }

    @Test
    void radiansToFalcon500Position() {
        assertEquals(488.923985178, FalconUnitConversion.radiansToFalcon500Position(1.5), 1e-2);
    }
}
