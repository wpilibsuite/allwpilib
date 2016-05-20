/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Hashtable;
import java.util.Vector;

import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

/**
 * The preferences class provides a relatively simple way to save important values to the RoboRIO to
 * access the next time the RoboRIO is booted.
 *
 * <p>This class loads and saves from a file inside the RoboRIO. The user can not access the file
 * directly, but may modify values at specific fields which will then be saved to the file when
 * {@link Preferences#save() save()} is called.</p>
 *
 * <p>This class is thread safe.</p>
 *
 * <p>This will also interact with {@link NetworkTable} by creating a table called "Preferences"
 * with all the key-value pairs. To save using {@link NetworkTable}, simply set the boolean at
 * position ~S A V E~ to true. Also, if the value of any variable is " in the {@link NetworkTable},
 * then that represents non-existence in the {@link Preferences} table</p>
 *
 * @author Joe Grinstead
 */
public class Preferences {

  /**
   * The Preferences table name
   */
  private static final String TABLE_NAME = "Preferences";
  /**
   * The value of the save field
   */
  private static final String SAVE_FIELD = "~S A V E~";
  /**
   * The file to save to
   */
  private static final String FILE_NAME = "wpilib-preferences.ini";
  /**
   * The characters to put between a field and value
   */
  private static final byte[] VALUE_PREFIX = {'=', '\"'};
  /**
   * The characters to put after the value
   */
  private static final byte[] VALUE_SUFFIX = {'\"', '\n'};
  /**
   * The newline character
   */
  private static final byte[] NEW_LINE = {'\n'};
  /**
   * The singleton instance
   */
  private static Preferences instance;

  /**
   * Returns the preferences instance.
   *
   * @return the preferences instance
   */
  public synchronized static Preferences getInstance() {
    if (instance == null) {
      instance = new Preferences();
    }
    return instance;
  }

  /**
   * The semaphore for beginning reads and writes to the file
   */
  private final Object fileLock = new Object();
  /**
   * The semaphore for reading from the table
   */
  private final Object lock = new Object();
  /**
   * The actual values (String->String)
   */
  private Hashtable values;
  /**
   * The keys in the order they were read from the file
   */
  private Vector keys;
  /**
   * The comments that were in the file sorted by which key they appeared over (String->Comment)
   */
  private Hashtable comments;
  /**
   * The comment at the end of the file
   */
  private Comment endComment;

  /**
   * Creates a preference class that will automatically read the file in a different thread. Any
   * call to its methods will be blocked until the thread is finished reading.
   */
  private Preferences() {
    values = new Hashtable();
    keys = new Vector();

    // We synchronized on fileLock and then wait
    // for it to know that the reading thread has started
    synchronized (fileLock) {
      new Thread() {
        public void run() {
          read();
        }
      }.start();
      try {
        fileLock.wait();
      } catch (InterruptedException ex) {
      }
    }
  }

  /**
   * @return a vector of the keys
   */
  public Vector getKeys() {
    synchronized (lock) {
      return keys;
    }
  }

  /**
   * Puts the given value into the given key position
   *
   * @param key   the key
   * @param value the value
   * @throws ImproperPreferenceKeyException if the key contains an illegal character
   */
  private void put(String key, String value) {
    synchronized (lock) {
      if (key == null) {
        throw new NullPointerException();
      }
      ImproperPreferenceKeyException.confirmString(key);
      if (values.put(key, value) == null) {
        keys.addElement(key);
      }
      NetworkTable.getTable(TABLE_NAME).putString(key, value);
    }
  }

  /**
   * Puts the given string into the preferences table.
   *
   * <p>The value may not have quotation marks, nor may the key have any whitespace nor an equals
   * sign</p>
   *
   * <p>This will <b>NOT</b> save the value to memory between power cycles, to do that you must call
   * {@link Preferences#save() save()} (which must be used with care). at some point after calling
   * this.</p>
   *
   * @param key   the key
   * @param value the value
   * @throws NullPointerException           if value is null
   * @throws IllegalArgumentException       if value contains a quotation mark
   * @throws ImproperPreferenceKeyException if the key contains any whitespace or an equals sign
   */
  public void putString(String key, String value) {
    if (value == null) {
      throw new NullPointerException();
    }
    if (value.indexOf('"') != -1) {
      throw new IllegalArgumentException("Can not put string:" + value + " because it contains " +
          "quotation marks");
    }
    put(key, value);
  }

  /**
   * Puts the given int into the preferences table.
   *
   * <p>The key may not have any whitespace nor an equals sign</p>
   *
   * <p>This will <b>NOT</b> save the value to memory between power cycles, to do that you must call
   * {@link Preferences#save() save()} (which must be used with care) at some point after calling
   * this.</p>
   *
   * @param key   the key
   * @param value the value
   * @throws ImproperPreferenceKeyException if the key contains any whitespace or an equals sign
   */
  public void putInt(String key, int value) {
    put(key, String.valueOf(value));
  }

  /**
   * Puts the given double into the preferences table.
   *
   * <p>The key may not have any whitespace nor an equals sign</p>
   *
   * <p>This will <b>NOT</b> save the value to memory between power cycles, to do that you must call
   * {@link Preferences#save() save()} (which must be used with care) at some point after calling
   * this.</p>
   *
   * @param key   the key
   * @param value the value
   * @throws ImproperPreferenceKeyException if the key contains any whitespace or an equals sign
   */
  public void putDouble(String key, double value) {
    put(key, String.valueOf(value));
  }

  /**
   * Puts the given float into the preferences table.
   *
   * <p>The key may not have any whitespace nor an equals sign</p>
   *
   * <p>This will <b>NOT</b> save the value to memory between power cycles, to do that you must call
   * {@link Preferences#save() save()} (which must be used with care) at some point after calling
   * this.</p>
   *
   * @param key   the key
   * @param value the value
   * @throws ImproperPreferenceKeyException if the key contains any whitespace or an equals sign
   */
  public void putFloat(String key, float value) {
    put(key, String.valueOf(value));
  }

  /**
   * Puts the given boolean into the preferences table.
   *
   * <p>The key may not have any whitespace nor an equals sign</p>
   *
   * <p>This will <b>NOT</b> save the value to memory between power cycles, to do that you must call
   * {@link Preferences#save() save()} (which must be used with care) at some point after calling
   * this.</p>
   *
   * @param key   the key
   * @param value the value
   * @throws ImproperPreferenceKeyException if the key contains any whitespace or an equals sign
   */
  public void putBoolean(String key, boolean value) {
    put(key, String.valueOf(value));
  }

  /**
   * Puts the given long into the preferences table.
   *
   * <p>The key may not have any whitespace nor an equals sign</p>
   *
   * <p>This will <b>NOT</b> save the value to memory between power cycles, to do that you must call
   * {@link Preferences#save() save()} (which must be used with care) at some point after calling
   * this.</p>
   *
   * @param key   the key
   * @param value the value
   * @throws ImproperPreferenceKeyException if the key contains any whitespace or an equals sign
   */
  public void putLong(String key, long value) {
    put(key, String.valueOf(value));
  }

  /**
   * Returns the value at the given key.
   *
   * @param key the key
   * @return the value (or null if none exists)
   * @throws NullPointerException if the key is null
   */
  private String get(String key) {
    synchronized (lock) {
      if (key == null) {
        throw new NullPointerException();
      }
      return (String) values.get(key);
    }
  }

  /**
   * Returns whether or not there is a key with the given name.
   *
   * @param key the key
   * @return if there is a value at the given key
   * @throws NullPointerException if key is null
   */
  public boolean containsKey(String key) {
    return get(key) != null;
  }

  /**
   * Remove a preference
   *
   * @param key the key
   * @throws NullPointerException if key is null
   */
  public void remove(String key) {
    synchronized (lock) {
      if (key == null) {
        throw new NullPointerException();
      }
      values.remove(key);
      keys.removeElement(key);
    }
  }

  /**
   * Returns the string at the given key. If this table does not have a value for that position,
   * then the given backup value will be returned.
   *
   * @param key    the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   * @throws NullPointerException if the key is null
   */
  public String getString(String key, String backup) {
    String value = get(key);
    return value == null ? backup : value;
  }

  /**
   * Returns the int at the given key. If this table does not have a value for that position, then
   * the given backup value will be returned.
   *
   * @param key    the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   * @throws IncompatibleTypeException if the value in the table can not be converted to an int
   */
  public int getInt(String key, int backup) {
    String value = get(key);
    if (value == null) {
      return backup;
    } else {
      try {
        return Integer.parseInt(value);
      } catch (NumberFormatException e) {
        throw new IncompatibleTypeException(value, "int");
      }
    }
  }

  /**
   * Returns the double at the given key. If this table does not have a value for that position,
   * then the given backup value will be returned.
   *
   * @param key    the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   * @throws IncompatibleTypeException if the value in the table can not be converted to an double
   */
  public double getDouble(String key, double backup) {
    String value = get(key);
    if (value == null) {
      return backup;
    } else {
      try {
        return Double.parseDouble(value);
      } catch (NumberFormatException e) {
        throw new IncompatibleTypeException(value, "double");
      }
    }
  }

  /**
   * Returns the boolean at the given key. If this table does not have a value for that position,
   * then the given backup value will be returned.
   *
   * @param key    the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   * @throws IncompatibleTypeException if the value in the table can not be converted to a boolean
   */
  public boolean getBoolean(String key, boolean backup) {
    String value = get(key);
    if (value == null) {
      return backup;
    } else {
      if (value.equalsIgnoreCase("true")) {
        return true;
      } else if (value.equalsIgnoreCase("false")) {
        return false;
      } else {
        throw new IncompatibleTypeException(value, "boolean");
      }
    }
  }

  /**
   * Returns the float at the given key. If this table does not have a value for that position, then
   * the given backup value will be returned.
   *
   * @param key    the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   * @throws IncompatibleTypeException if the value in the table can not be converted to a float
   */
  public float getFloat(String key, float backup) {
    String value = get(key);
    if (value == null) {
      return backup;
    } else {
      try {
        return Float.parseFloat(value);
      } catch (NumberFormatException e) {
        throw new IncompatibleTypeException(value, "float");
      }
    }
  }

  /**
   * Returns the long at the given key. If this table does not have a value for that position, then
   * the given backup value will be returned.
   *
   * @param key    the key
   * @param backup the value to return if none exists in the table
   * @return either the value in the table, or the backup
   * @throws IncompatibleTypeException if the value in the table can not be converted to a long
   */
  public long getLong(String key, long backup) {
    String value = get(key);
    if (value == null) {
      put(key, String.valueOf(backup));
      return backup;
    } else {
      try {
        return Long.parseLong(value);
      } catch (NumberFormatException e) {
        throw new IncompatibleTypeException(value, "long");
      }
    }
  }

  /**
   * Saves the preferences to a file on the RoboRIO.
   *
   * <p>This should <b>NOT</b> be called often. Too many writes can damage the RoboRIO's flash
   * memory. While it is ok to save once or twice a match, this should never be called every run of
   * {@link IterativeRobot#teleopPeriodic()}.</p>
   *
   * <p>The actual writing of the file is done in a separate thread. However, any call to a get or
   * put method will wait until the table is fully saved before continuing.</p>
   */
  public void save() {
    synchronized (fileLock) {
      new Thread() {
        public void run() {
          write();
        }
      }.start();
      try {
        fileLock.wait();
      } catch (InterruptedException ex) {
      }
    }
  }

  /**
   * Internal method that actually writes the table to a file. This is called in its own thread when
   * {@link Preferences#save() save()} is called.
   */
  private void write() {
    synchronized (lock) {
      synchronized (fileLock) {
        fileLock.notifyAll();
      }

      File file = null;
      FileOutputStream output = null;
      try {
        file = new File(FILE_NAME);

        if (file.exists())
          file.delete();

        file.createNewFile();

        output = new FileOutputStream(file);

        for (int i = 0; i < keys.size(); i++) {
          String key = (String) keys.elementAt(i);
          String value = (String) values.get(key);

          if (comments != null) {
            Comment comment = (Comment) comments.get(key);
            if (comment != null) {
              comment.write(output);
            }
          }

          output.write(key.getBytes());
          output.write(VALUE_PREFIX);
          output.write(value.getBytes());
          output.write(VALUE_SUFFIX);
        }

        if (endComment != null) {
          endComment.write(output);
        }
      } catch (IOException ex) {
        ex.printStackTrace();
      } finally {
        if (output != null) {
          try {
            output.close();
          } catch (IOException ex) {
          }
        }
        NetworkTable.getTable(TABLE_NAME).putBoolean(SAVE_FIELD, false);
      }
    }
  }

  /**
   * The internal method to read from a file. This will be called in its own thread when the
   * preferences singleton is first created.
   */
  private void read() {
    class EndOfStreamException extends Exception {
    }

    class Reader {

      InputStream stream;

      Reader(InputStream stream) {
        this.stream = stream;
      }

      public char read() throws IOException, EndOfStreamException {
        int input = stream.read();
        if (input == -1) {
          throw new EndOfStreamException();
        } else {
          // Check for carriage returns
          return input == '\r' ? '\n' : (char) input;
        }
      }

      char readWithoutWhitespace() throws IOException, EndOfStreamException {
        while (true) {
          char value = read();
          switch (value) {
            case ' ':
            case '\t':
              continue;
            default:
              return value;
          }
        }
      }
    }

    synchronized (lock) {
      synchronized (fileLock) {
        fileLock.notifyAll();
      }

      Comment comment = null;


      File file = null;
      FileInputStream input = null;
      try {
        file = new File(FILE_NAME);

        if (file.exists()) {
          input = new FileInputStream(file);
          Reader reader = new Reader(input);

          StringBuffer buffer;

          while (true) {
            char value = reader.readWithoutWhitespace();

            if (value == '\n' || value == ';') {
              if (comment == null) {
                comment = new Comment();
              }

              if (value == '\n') {
                comment.addBytes(NEW_LINE);
              } else {
                buffer = new StringBuffer(30);
                for (; value != '\n'; value = reader.read()) {
                  buffer.append(value);
                }
                buffer.append('\n');
                comment.addBytes(buffer.toString().getBytes());
              }
            } else {
              buffer = new StringBuffer(30);
              for (; value != '='; value = reader.readWithoutWhitespace()) {
                buffer.append(value);
              }
              String name = buffer.toString();
              buffer = new StringBuffer(30);

              boolean shouldBreak = false;

              value = reader.readWithoutWhitespace();
              if (value == '"') {
                for (value = reader.read(); value != '"'; value = reader.read()) {
                  buffer.append(value);
                }
                // Clear the line
                while (reader.read() != '\n') ;
              } else {
                try {
                  for (; value != '\n'; value = reader.readWithoutWhitespace()) {
                    buffer.append(value);
                  }
                } catch (EndOfStreamException e) {
                  shouldBreak = true;
                }
              }

              String result = buffer.toString();

              keys.addElement(name);
              values.put(name, result);
              NetworkTable.getTable(TABLE_NAME).putString(name, result);

              if (comment != null) {
                if (comments == null) {
                  comments = new Hashtable();
                }
                comments.put(name, comment);
                comment = null;
              }


              if (shouldBreak) {
                break;
              }
            }
          }
        }
      } catch (IOException ex) {
        ex.printStackTrace();
      } catch (EndOfStreamException ex) {
        System.out.println("Done Reading");
      }

      if (input != null) {
        try {
          input.close();
        } catch (IOException ex) {
        }
      }

      if (comment != null) {
        endComment = comment;
      }
    }

    NetworkTable.getTable(TABLE_NAME).putBoolean(SAVE_FIELD, false);
    // TODO: Verify that this works even though it changes with subtables.
    //       Should work since preferences shouldn't have subtables.
    NetworkTable.getTable(TABLE_NAME).addTableListener(new ITableListener() {
      public void valueChanged(ITable source, String key, Object value, boolean isNew) {
        if (key.equals(SAVE_FIELD)) {
          if (((Boolean) value).booleanValue()) {
            save();
          }
        } else {
          synchronized (lock) {
            if (!ImproperPreferenceKeyException.isAcceptable(key) || value.toString().indexOf
                ('"') != -1) {
              if (values.contains(key) || keys.contains(key)) {
                values.remove(key);
                keys.removeElement(key);
                NetworkTable.getTable(TABLE_NAME).putString(key, "\"");
              }
            } else {
              if (values.put(key, value.toString()) == null) {
                keys.addElement(key);
              }
            }
          }
        }
      }
    });
  }

  /**
   * A class representing some comment lines in the ini file. This is used so that if a programmer
   * ever directly modifies the ini file, then his/her comments will still be there after {@link
   * Preferences#save() save()} is called.
   */
  private static class Comment {

    /**
     * A vector of byte arrays. Each array represents a line to write
     */
    private Vector bytes = new Vector();

    /**
     * Appends the given bytes to the comment.
     *
     * @param bytes the bytes to add
     */
    private void addBytes(byte[] bytes) {
      this.bytes.addElement(bytes);
    }

    /**
     * Writes this comment to the given stream
     *
     * @param stream the stream to write to
     * @throws IOException if the stream has a problem
     */
    private void write(OutputStream stream) throws IOException {
      for (int i = 0; i < bytes.size(); i++) {
        stream.write((byte[]) bytes.elementAt(i));
      }
    }
  }

  /**
   * This exception is thrown if the a value requested cannot be converted to the requested type.
   */
  public static class IncompatibleTypeException extends RuntimeException {

    /**
     * Creates an exception with a description based on the input
     *
     * @param value the value that can not be converted
     * @param type  the type that the value can not be converted to
     */
    public IncompatibleTypeException(String value, String type) {
      super("Cannot convert \"" + value + "\" into " + type);
    }
  }

  /**
   * Should be thrown if a string can not be used as a key in the preferences file. This happens if
   * the string contains a new line, a space, a tab, or an equals sign.
   */
  public static class ImproperPreferenceKeyException extends RuntimeException {

    /**
     * Instantiates an exception with a descriptive message based on the input.
     *
     * @param value  the illegal key
     * @param letter the specific character that made it illegal
     */
    public ImproperPreferenceKeyException(String value, char letter) {
      super("Preference key \""
          + value + "\" is not allowed to contain letter with ASCII code:" + (byte) letter);
    }

    /**
     * Tests if the given string is ok to use as a key in the preference table. If not, then a
     * {@link ImproperPreferenceKeyException} will be thrown.
     *
     * @param value the value to test
     */
    public static void confirmString(String value) {
      for (int i = 0; i < value.length(); i++) {
        char letter = value.charAt(i);
        switch (letter) {
          case '=':
          case '\n':
          case '\r':
          case ' ':
          case '\t':
            throw new ImproperPreferenceKeyException(value, letter);
        }
      }
    }

    /**
     * Returns whether or not the given string is ok to use in the preference table.
     */
    public static boolean isAcceptable(String value) {
      for (int i = 0; i < value.length(); i++) {
        char letter = value.charAt(i);
        switch (letter) {
          case '=':
          case '\n':
          case '\r':
          case ' ':
          case '\t':
            return false;
        }
      }
      return true;
    }
  }
}
