package edu.wpi.first.wpiutil.math;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

public class StateSpaceUtilsTest {

    @Test
    public void testCostArray() {
        var mat = StateSpaceUtils.makeCostMatrix(Nat.N3(),
                new MatBuilder<>(Nat.N3(), Nat.N1()).fill(1.0, 2.0, 3.0));

        Assertions.assertEquals(1.0, mat.get(0, 0), 1e-3);
        Assertions.assertEquals(0.0, mat.get(0, 1), 1e-3);
        Assertions.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assertions.assertEquals(0.0, mat.get(1, 0), 1e-3);
        Assertions.assertEquals(1.0 / 4.0, mat.get(1, 1), 1e-3);
        Assertions.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assertions.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assertions.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assertions.assertEquals(1.0 / 9.0, mat.get(2, 2), 1e-3);
    }

    @Test
    public void testCovArray() {
        var mat = StateSpaceUtils.makeCovMatrix(Nat.N3(),
                new MatBuilder<>(Nat.N3(), Nat.N1()).fill(1.0, 2.0, 3.0));

        Assertions.assertEquals(1.0, mat.get(0, 0), 1e-3);
        Assertions.assertEquals(0.0, mat.get(0, 1), 1e-3);
        Assertions.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assertions.assertEquals(0.0, mat.get(1, 0), 1e-3);
        Assertions.assertEquals(4.0, mat.get(1, 1), 1e-3);
        Assertions.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assertions.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assertions.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assertions.assertEquals(9.0, mat.get(2, 2), 1e-3);
    }

    @Test
    public void testIsStabilizable() {
        var A = new MatBuilder<>(Nat.N2(), Nat.N2())
                .fill(1.2, 0, 0, 0.5,
                        1, 0, 0, 0.5,
                        0.2, 0, 0, 0.5);
        var B = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0, 1);
    }

}
