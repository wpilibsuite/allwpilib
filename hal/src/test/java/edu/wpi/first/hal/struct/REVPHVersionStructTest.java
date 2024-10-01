package edu.wpi.first.hal.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.REVPHVersion;
import edu.wpi.first.wpilibj.StructTestBase;

public class REVPHVersionStructTest extends StructTestBase<REVPHVersion> {

    public REVPHVersionStructTest() {
        super(
            new REVPHVersion(
                1,
                2,
                3,
                4,
                5,
                999
            ),
            REVPHVersion.struct
        );
    }

    @Override
    public void checkEquals(REVPHVersion testData, REVPHVersion data) {
        assertEquals(testData.firmwareMajor, data.firmwareMajor);
        assertEquals(testData.firmwareMinor, data.firmwareMinor);
        assertEquals(testData.firmwareFix, data.firmwareFix);
        assertEquals(testData.hardwareMinor, data.hardwareMinor);
        assertEquals(testData.hardwareMajor, data.hardwareMajor);
        assertEquals(testData.uniqueId, data.uniqueId);
    }
}
