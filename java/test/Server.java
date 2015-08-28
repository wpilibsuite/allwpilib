import edu.wpi.first.wpilibj.networktables.*;
import edu.wpi.first.wpilibj.tables.*;

public class Server {
  private static class MyLogger implements NetworkTablesJNI.LoggerFunction {
    public void apply(int level, String file, int line, String msg) {
      System.err.println(msg);
    }
  }

  public static void main(String[] args) {
    NetworkTablesJNI.setLogger(new MyLogger(), 0);
    NetworkTable.setIPAddress("127.0.0.1");
    NetworkTable.setPort(10000);
    NetworkTable.setServerMode();
    NetworkTable nt = NetworkTable.getTable("");
    try { Thread.sleep(1000); } catch (InterruptedException e) {}
    nt.putNumber("foo", 0.5);
    nt.setFlags("foo", NetworkTable.PERSISTENT);
    nt.putNumber("foo2", 0.5);
    nt.putNumber("foo2", 0.7);
    nt.putNumber("foo2", 0.6);
    nt.putNumber("foo2", 0.5);
    try { Thread.sleep(10000); } catch (InterruptedException e) {}
  }
}
