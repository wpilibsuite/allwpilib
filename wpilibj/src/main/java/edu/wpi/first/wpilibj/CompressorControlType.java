package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.REVPHJNI;

public enum CompressorControlType {
    Disabled(REVPHJNI.COMPRESSOR_CONFIG_TYPE_DISABLED), Digital(REVPHJNI.COMPRESSOR_CONFIG_TYPE_DIGITAL),
    Analog(REVPHJNI.COMPRESSOR_CONFIG_TYPE_ANALOG), Hybrid(REVPHJNI.COMPRESSOR_CONFIG_TYPE_HYBRID);

    public final int value;

    private CompressorControlType(int value) {
        this.value = value;
    }

    public static CompressorControlType getValue(int value) {
        switch (value) {
        case REVPHJNI.COMPRESSOR_CONFIG_TYPE_HYBRID:
            return Hybrid;
        case REVPHJNI.COMPRESSOR_CONFIG_TYPE_ANALOG:
            return Analog;
        case REVPHJNI.COMPRESSOR_CONFIG_TYPE_DIGITAL:
            return Digital;
        default:
            return Disabled;
        }
    }
}
