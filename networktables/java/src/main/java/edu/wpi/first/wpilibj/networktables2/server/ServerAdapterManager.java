package edu.wpi.first.wpilibj.networktables2.server;


/**
 * A class that manages connections to a server
 * 
 * @author Mitchell
 *
 */
public interface ServerAdapterManager {
    /**
     * Called when a connection adapter has been closed
     * @param connectionAdapter the adapter that was closed
     */
    public void close(ServerConnectionAdapter connectionAdapter, boolean closeStream);
}
