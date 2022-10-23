import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.controller.DifferentialDriveFeedforward;
import edu.wpi.first.math.controller.DifferentialDriveWheelVoltages;
import edu.wpi.first.math.numbers.*;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.LinearSystemId;
import org.junit.jupiter.api.Test;

class DifferentialDriveFeedforwardTest {
  private final double kVLinear = 1.0;
  private final double kALinear = 1.0;
  private final double kVAngular = 1.0;
  private final double kAAngular = 1.0;
  private final double trackwidth = 1.0;
  private final double dtSeconds = .02;

  @Test
  public void testCalculate() {
    DifferentialDriveFeedforward differentialDriveFeedforward =
        new DifferentialDriveFeedforward(kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
    LinearSystem<N2, N2, N2> plant =
        LinearSystemId.identifyDrivetrainSystem(
            kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
    for (int cLVelocity = -4; cLVelocity <= 4; cLVelocity += 2) {
      for (int cRVelocity = -4; cRVelocity <= 4; cRVelocity += 2) {
        for (int nLVelocity = -4; nLVelocity <= 4; nLVelocity += 2) {
          for (int nRVelocity = -4; nRVelocity <= 4; nRVelocity += 2) {
            DifferentialDriveWheelVoltages u =
                differentialDriveFeedforward.calculate(
                    cLVelocity, nLVelocity, cRVelocity, nRVelocity, dtSeconds);
            Matrix<N2, N1> y =
                plant.calculateY(
                    VecBuilder.fill(cLVelocity, cRVelocity), VecBuilder.fill(u.left, u.right));
            // left drivetrain check
            assertEquals(y.get(0, 0), nLVelocity, 0.0);
            // right drivetrain check);
            assertEquals(y.get(1, 0), nRVelocity, 0.0);
          }
        }
      }
    }
  }
}
