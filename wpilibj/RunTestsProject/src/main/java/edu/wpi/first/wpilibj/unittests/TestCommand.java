package edu.wpi.first.wpilibj.unittests;

import edu.wpi.first.wpilibj.command.Command;

/**
 * A class to simulate a simple command
 * The command keeps track of how many times each method was called
 *
 * @author mwills
 */
public class TestCommand extends Command{
    private int initializeCount = 0;
    private int executeCount = 0;
    private int isFinishedCount = 0;
    private boolean hasFinished = false;
    private int endCount = 0;
    private int interruptedCount = 0;
    
    protected void initialize() {
        ++initializeCount;
    }

    protected void execute() {
        ++executeCount;
    }

    protected boolean isFinished() {
        ++isFinishedCount;
        return isHasFinished();
    }

    protected void end() {
        ++endCount;
    }

    protected void interrupted() {
        ++interruptedCount;
    }
    
    
    
    
    
    

    /**
     * @return how many times the initialize method has been called
     */
    public int getInitializeCount() {
        return initializeCount;
    }
    /**
     * @return if the initialize method has been called at least once
     */
    public boolean hasInitialized(){
        return getInitializeCount()>0;
    }

    /**
     * @return how many time the execute method has been called
     */
    public int getExecuteCount() {
        return executeCount;
    }

    /**
     * @return how many times the isFinished method has been called
     */
    public int getIsFinishedCount() {
        return isFinishedCount;
    }

    /**
     * @return what value the isFinished method will return
     */
    public boolean isHasFinished() {
        return hasFinished;
    }

    /**
     * @param set what value the isFinished method will return
     */
    public void setHasFinished(boolean hasFinished) {
        this.hasFinished = hasFinished;
    }

    /**
     * @return how many times the end method has been called
     */
    public int getEndCount() {
        return endCount;
    }
    /**
     * @return if the end method has been called at least once
     */
    public boolean hasEnd(){
        return getEndCount()>0;
    }

    /**
     * @return how many times the interrupted method has been called
     */
    public int getInterruptedCount() {
        return interruptedCount;
    }
    /**
     * @return if the interrupted method has been called at least once
     */
    public boolean hasInterrupted(){
        return getInterruptedCount()>0;
    }
    
}
