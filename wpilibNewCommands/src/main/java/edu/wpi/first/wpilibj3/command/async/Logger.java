package edu.wpi.first.wpilibj3.command.async;

import edu.wpi.first.util.WPIUtilJNI;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

public class Logger {
  private static final List<LogRecord> records = new ArrayList<>();

  public static void log(String context, String message) {
    var now = WPIUtilJNI.now() / 1e6;
    LogRecord record = new LogRecord(now, Thread.currentThread().getName(), context, message);

    records.add(record);
  }

  public static void clear() {
    records.clear();
  }

  /**
   * Prints a table of threads
   */
  public static String formattedLogTable() {
    // Operate on a copy to avoid concurrency issues
    var records = List.copyOf(Logger.records);
    /*
     |-------------|-------------------|-------------------|
     | Timestamp   | Thread1           | Thread2           |
     |-------------|-------------------|-------------------|
     | 0.000000    | COMMAND: blah     |                   |
     | 0.002000    |                   | COMMAND: blargh   |
     |-------------|-------------------|-------------------|
     */
    // Use a sorted map implementation so threads are printed alphabetically
    var threadNames = records.stream().map(LogRecord::threadName).distinct().sorted().toList();
    int threadCount = threadNames.size();

    var timestampColumnWidth =
        records
            .stream()
            .mapToDouble(LogRecord::timestamp)
            .mapToInt(d -> String.format(" %.6f ", d).length())
            .max()
            .orElse(" Timestamp ".length());
    var threadColumnWidth =
        records
            .stream()
            .map(LogRecord::printedMessage)
            .mapToInt(m -> m.length() + 1)
            .max()
            .orElse(1);

    StringBuilder table = new StringBuilder();

    // First row: |---|---|---|...|
    table.append('|');
    table.append("-".repeat(timestampColumnWidth + 1));

    for (int i = 0; i < threadCount; i++) {
      table.append('|');
      table.append("-".repeat(threadColumnWidth + 1));
    }
    table.append('|');
    table.append('\n');
    // end first row

    // Second row: |   Timestamp |   threadA |    threadB |
    table.append('|');
    table.append(("%" + timestampColumnWidth + "s ").formatted("Timestamp"));

    for (int i = 0; i < threadCount; i++) {
      var threadName = threadNames.get(i);
      table.append('|');
      table.append(("%" + threadColumnWidth + "s ").formatted(threadName));
    }
    table.append('|');
    table.append('\n');
    // End second row

    Consumer<LogRecord> printRow = (record) -> {
      int threadIndex = threadNames.indexOf(record.threadName);
      table.append('|');
      table.append(("%" + timestampColumnWidth + "s ").formatted("%.6f".formatted(record.timestamp)));

      for (int thread = 0; thread < threadCount; thread++) {
        table.append('|');
        if (thread == threadIndex) {
          table.append(("%" + threadColumnWidth + "s ").formatted(record.printedMessage()));
        } else {
          table.append(" ".repeat(threadColumnWidth + 1));
        }
      }
      table.append('|');
      table.append('\n');
    };

    records.forEach(printRow);

    return table.toString();
  }

  public record LogRecord(double timestamp, String threadName, String context, String message) {
    @Override
    public String toString() {
      return "[%.6f][%s] - %s: %s".formatted(timestamp, threadName, context, message);
    }

    public String printedMessage() {
      return context + ": " + message;
    }
  }
}
