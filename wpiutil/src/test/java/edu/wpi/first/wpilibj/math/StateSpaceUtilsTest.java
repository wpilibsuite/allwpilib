package edu.wpi.first.wpilibj.math;

import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Nat;
import org.junit.Assert;
import org.junit.Test;

public class StateSpaceUtilsTest {

    @Test
    public void testCostArray() {
        var mat = StateSpaceUtils.makeCostMatrix(Nat.N3(),
                new MatBuilder<>(Nat.N3(), Nat.N1()).fill(1.0, 2.0, 3.0));

        Assert.assertEquals(1.0, mat.get(0, 0), 1e-3);
        Assert.assertEquals(0.0, mat.get(0, 1), 1e-3);
        Assert.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assert.assertEquals(0.0, mat.get(1, 0), 1e-3);
        Assert.assertEquals(1.0 / 4.0, mat.get(1, 1), 1e-3);
        Assert.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assert.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assert.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assert.assertEquals(1.0 / 9.0, mat.get(2, 2), 1e-3);
    }

    @Test
    public void testCovArray() {
        var mat = StateSpaceUtils.makeCovMatrix(Nat.N3(),
                new MatBuilder<>(Nat.N3(), Nat.N1()).fill(1.0, 2.0, 3.0));

        Assert.assertEquals(1.0, mat.get(0, 0), 1e-3);
        Assert.assertEquals(0.0, mat.get(0, 1), 1e-3);
        Assert.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assert.assertEquals(0.0, mat.get(1, 0), 1e-3);
        Assert.assertEquals(4.0, mat.get(1, 1), 1e-3);
        Assert.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assert.assertEquals(0.0, mat.get(0, 2), 1e-3);
        Assert.assertEquals(0.0, mat.get(1, 2), 1e-3);
        Assert.assertEquals(9.0, mat.get(2, 2), 1e-3);
    }

    @Test
    public void testIsStabilizable() {
//        var A = new MatBuilder<>(Nat.N2(), Nat.N2())
//                .fill(1.2, 0, 0, 0.5,
//                        1, 0, 0, 0.5,
//                        0.2, 0, 0, 0.5);
//        var B = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0, 1);
    }

}
