package edu.wpi.first.wpiutil;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

public class WPIUtilJNITest {

    @Test
    public void testNow() {
        assertDoesNotThrow(WPIUtilJNI::now);
    }
}
