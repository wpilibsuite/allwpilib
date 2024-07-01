package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringEntry;
import edu.wpi.first.wpilibj.DataLogManager;
import edu.wpi.first.wpilibj.shuffleboard.EventImportance;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

import java.lang.invoke.MethodHandles;
import java.util.HashMap;
import java.util.stream.Collectors;

public class CommandSchedulerLog 
{
    private static final String fullClassName = MethodHandles.lookup().lookupClass().getCanonicalName();
    static
    {
        System.out.println("Loading: " + fullClassName);
    }

    private final HashMap<String, Integer> currentCommands = new HashMap<String, Integer>();
    private final NetworkTable nt;    
    private final StringEntry initializeCommandLogEntry;
    private final StringEntry interruptCommandLogEntry;
    private final StringEntry finishCommandLogEntry;
    private final StringEntry executeCommandLogEntry;
    private final boolean useConsole;
    private final boolean useDataLog;
    private final boolean useShuffleBoardLog;

    /**
     * Command Event Loggers
     * 
     * <p>Set the command scheduler to log events for command initialize, interrupt, finish, and execute.
     * 
     * <p>Log to the Console/Terminal, ShuffleBoard or the WPILib DataLog tool.
     * 
     * <p>If ShuffleBoard is recording (start it manually), these events are added to the recording.
     * Convert recording to csv and they show nicely in Excel.
     * 
     * <p>If using DataLog tool, the recording is via NT so tell NT to send EVERYTHING to the DataLog.
     * Run DataLog tool to retrieve log from roboRIO and convert the log to csv.
     * 
     * <p>Note the comment in execute logging that only the first execute is logged unless changed.
     */ 
    CommandSchedulerLog(boolean useConsole, boolean useDataLog, boolean useShuffleBoardLog)
    {
        this.useConsole = useConsole;
        this.useDataLog = useDataLog;
        this.useShuffleBoardLog = useShuffleBoardLog;

        // DataLog via NT so establish NT and the connection to DataLog
        if (useDataLog) {
            DataLogManager.logNetworkTables(true); // WARNING - this puts all NT to the DataLog
        }

        final String networkTableName = "Team4237";
        nt = NetworkTableInstance.getDefault().getTable(networkTableName);
        initializeCommandLogEntry = nt.getStringTopic("Commands/initialize").getEntry("");
        interruptCommandLogEntry = nt.getStringTopic("Commands/interrupt").getEntry("");
        finishCommandLogEntry = nt.getStringTopic("Commands/finish").getEntry("");
        executeCommandLogEntry = nt.getStringTopic("Commands/execute").getEntry("");        
    }

    /**
     * Log commands that run the initialize method.
     */
    public void logCommandInitialize()
    {
        CommandScheduler.getInstance().onCommandInitialize(
            (command) -> 
            {
                String key = command.getClass().getSimpleName() + "/" + command.getName();
                String requirements = command.getRequirements().stream()
                    .map(subsystem -> subsystem.getClass().getSimpleName())
                    .collect(Collectors.joining(", ", "{", "}"));

                if(useConsole) {
                    System.out.println("Command initialized : " + key + " " + requirements);                    
                }
                if(useDataLog) {
                    initializeCommandLogEntry.set(key + " " + requirements);                    
                }
                if(useShuffleBoardLog) {
                    Shuffleboard.addEventMarker("Command initialized",
                        key + " " + requirements, EventImportance.kNormal);                    
                }

                currentCommands.put(key, 0);
            }
        );
    }

    /**
     * Log commands that have been interrupted.
     */
    public void logCommandInterrupt()
    {
        CommandScheduler.getInstance().onCommandInterrupt(
            (command) ->
            {
                String key = command.getClass().getSimpleName() + "/" + command.getName();
                String runs = " after " + currentCommands.getOrDefault(key, 0) + " runs";

                if(useConsole) {
                    System.out.println("Command interrupted : " + key + runs);
                }
                if(useDataLog) {
                    interruptCommandLogEntry.set(key + runs);
                } 
                if(useShuffleBoardLog) {
                    Shuffleboard.addEventMarker("Command interrupted", key, EventImportance.kNormal);
                }

                currentCommands.put(key, 0);
            }
        );
    }

    /**
     * Log commands that run the finish method.
     */
    public void logCommandFinish()
    {
        CommandScheduler.getInstance().onCommandFinish(
            (command) ->
            {
                String key = command.getClass().getSimpleName() + "/" + command.getName();
                String runs = " after " + currentCommands.getOrDefault(key, 0) + " runs";

                if(useConsole) {
                    System.out.println("Command finished : " + key + runs);
                }
                if(useDataLog) {
                    finishCommandLogEntry.set(key + runs);
                } 
                if(useShuffleBoardLog) {
                    Shuffleboard.addEventMarker("Command finished", key, EventImportance.kNormal);
                }

                currentCommands.put(key, 0);
            }
        );
    }

    /**
     * Log commands that run the execute() method.
     * 
     * <p>This can generate a lot of events so logging is suppressed except for the first
     * occurrence of execute(). Total count of execute() is logged at command end.
     * 
     * <p>Recompile without the if/else to get all execute() logged.
     */
    public void logCommandExecute()
    {
        CommandScheduler.getInstance().onCommandExecute(
            (command) ->
            {
                String key = command.getClass().getSimpleName() + "/" + command.getName();

                if(currentCommands.getOrDefault(key, 0) == 0) // suppress all but first execute
                {
                    if(useConsole) {
                        System.out.println("Command executed : " + key);
                    }
                    if(useDataLog) {
                        executeCommandLogEntry.set(key);
                    }
                    if(useShuffleBoardLog) {
                        Shuffleboard.addEventMarker("Command executed", key, EventImportance.kNormal);
                    }

                    currentCommands.put(key, 1); // first time through count is 1
                }
                else
                    // Increment total count to log when the command ends.
                    currentCommands.put(key, currentCommands.get(key) + 1);
            }
        );
    }
}
