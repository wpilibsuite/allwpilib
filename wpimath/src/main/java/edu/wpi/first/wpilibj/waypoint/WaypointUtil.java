// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.waypoint;

import edu.wpi.first.pathweaver.path.Path;
import edu.wpi.first.pathweaver.path.wpilib.WpilibPath;
import org.apache.commons.csv.CSVFormat;
import org.apache.commons.csv.CSVParser;
import org.apache.commons.csv.CSVPrinter;
import org.apache.commons.csv.CSVRecord;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.Reader;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

import javafx.geometry.Point2D;

public final class WaypointUtil {
  private static final Logger LOGGER = Logger.getLogger(WaypointUtil.class.getName());

  private WaypointUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Exports path object to csv file.
   *
   * @param fileLocation the directory and filename to write to
   * @param path         Path object to save
   *
   * @return true if successful file write was preformed
   */
  public static boolean export(String fileLocation, Path path, Double yoffset) {
    try (
        BufferedWriter writer = Files.newBufferedWriter(Paths.get(fileLocation + path.getPathName()));

        CSVPrinter csvPrinter = new CSVPrinter(writer, CSVFormat.DEFAULT
            .withHeader("X", "Y", "Tangent X", "Tangent Y", "Fixed Theta", "Reversed", "Name"))
    ) {
      for (Waypoint wp : path.getWaypoints()) {
        double xPos = wp.getX();
        double yPos = wp.getY() + yoffset;
        double tangentX = wp.getTangentX();
        double tangentY = wp.getTangentY();
        String name = wp.getName();
        csvPrinter.printRecord(xPos, yPos, tangentX, tangentY, wp.isLockTangent(), wp.isReversed(), name);
      }
      csvPrinter.flush();
    } catch (IOException except) {
      LOGGER.log(Level.WARNING, "Could not save Path file", except);
      return false;
    }
    return true;
  }

  /**
   * Imports Path object from disk.
   *
   * @param fileLocation Folder with path file
   * @param fileName     Name of path file
   *
   * @return Path object saved in Path file
   */
  public static Path importPath(String fileLocation, String fileName) {
    try(Reader reader = Files.newBufferedReader(java.nio.file.Path.of(fileLocation, fileName));
        CSVParser csvParser = new CSVParser(reader, CSVFormat.DEFAULT
                .withFirstRecordAsHeader()
                .withIgnoreHeaderCase()
                .withTrim())) {
      ArrayList<Waypoint> waypoints = new ArrayList<>();
      for (CSVRecord csvRecord : csvParser) {
        Point2D position = new Point2D(
                Double.parseDouble(csvRecord.get("X")),
                Double.parseDouble(csvRecord.get("Y"))
        );
        Point2D tangent = new Point2D(
                Double.parseDouble(csvRecord.get("Tangent X")),
                Double.parseDouble(csvRecord.get("Tangent Y"))
        );
        boolean locked = Boolean.parseBoolean(csvRecord.get("Fixed Theta"));
        boolean reversed = Boolean.parseBoolean(csvRecord.get("Reversed"));
        Waypoint point = new Waypoint(position, tangent, locked, reversed);
        if (csvRecord.isMapped("Name")) {
          String name = csvRecord.get("Name");
          point.setName(name);
        }
        waypoints.add(point);
      }
      return new WpilibPath(waypoints, fileName);
    } catch (IOException except) {
      LOGGER.log(Level.WARNING, "Could not read Path file", except);
      return null;
    }
  }

  /**
   * Imports Path object and creates a trajectory with that info.
   *
   * @param fileLocation Folder with path file
   * @param fileName     Name of path file
   *
   * @return Trajectory created from the waypoints in the file.
   */
  public static Trajectory importPathToTrajectory(String fileLocation, String fileName, TrajectoryConfig config) {
    try(Reader reader = Files.newBufferedReader(java.nio.file.Path.of(fileLocation, fileName));
        CSVParser csvParser = new CSVParser(reader, CSVFormat.DEFAULT
                .withFirstRecordAsHeader()
                .withIgnoreHeaderCase()
                .withTrim())) {
      int loop = 0;
      Pose2d start = new Pose2d();
      Pose2d end = new Pose2d();
      var interiorWaypoints = new ArrayList<Translation2d>();
      for (CSVRecord csvRecord : csvParser) {
        if (loop == 0) {
          start = createPoseWaypoint(csvRecord);
        }
        else if (loop == csvParser.Length - 1) {
          end = createPoseWaypoint(csvRecord);
        }
        else {
          interiorWaypoints.add(createTranslationWaypoint(csvRecord));
        }
        config.setReversed(Boolean.parseBoolean(csvRecord.get("Reversed")));
      }
      return TrajectoryGenerator.generateTrajectory(
            start,
            interiorWaypoints,
            end,
            config);
    } catch (IOException except) {
      LOGGER.log(Level.WARNING, "Could not read Path file", except);
      return null;
    }
  }

  private static Pose2d createPoseWaypoint(CSVRecord csvRecord) {
    return new Pose2d(new Translation2d(Double.parseDouble(csvRecord.get("X")), Double.parseDouble(csvRecord.get("Y"))),
       new Rotation2d(Double.parseDouble(csvRecord.get("Tangent X")), Double.parseDouble(csvRecord.get("Tangent Y"))));
  }

  private static Translation2d createTranslationWaypoint(CSVRecord csvRecord) {
    return new Translation2d(Double.parseDouble(csvRecord.get("X")), Double.parseDouble(csvRecord.get("Y")));
  }
}
