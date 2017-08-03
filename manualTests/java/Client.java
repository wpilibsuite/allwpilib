import edu.wpi.first.wpilibj.networktables.*;
import edu.wpi.first.wpilibj.tables.*;

public class Client {
  private static class MyLogger implements NetworkTablesJNI.LoggerFunction {
    public void apply(int level, String file, int line, String msg) {
      System.err.println(msg);
    }
  }

  public static void main(String[] args) {
    NetworkTablesJNI.setLogger(new MyLogger(), 0);
    NetworkTable.setIPAddress("127.0.0.1");
    NetworkTable.setPort(10000);
    NetworkTable.setClientMode();
    NetworkTable nt = NetworkTable.getTable("");
    try { Thread.sleep(2000); } catch (InterruptedException e) {}
    try {
      System.out.println("Got foo: " + nt.getNumber("foo"));
    } catch(TableKeyNotDefinedException ex) {
    }
    nt.putBoolean("bar", false);
    nt.setFlags("bar", NetworkTable.PERSISTENT);
    nt.putBoolean("bar2", true);
    nt.putBoolean("bar2", false);
    nt.putBoolean("bar2", true);
    nt.putString("str", "hello world");
    double[] nums = new double[3];
    nums[0] = 0.5;
    nums[1] = 1.2;
    nums[2] = 3.0;
    nt.putNumberArray("numarray", nums);
    String[] strs = new String[2];
    strs[0] = "Hello";
    strs[1] = "World";
    nt.putStringArray("strarray", strs);
    try { Thread.sleep(10000); } catch (InterruptedException e) {}
  }
}
