package edu.wpi.first.wpilibj.networktables2.stream;

import java.io.*;
import java.net.*;

/**
 * An object that will provide socket connections when a client connects to the server on the given port (for standard java VM)
 * 
 * @author mwills
 *
 */
public class SocketServerStreamProvider implements IOStreamProvider{

    private ServerSocket server = null;
    public SocketServerStreamProvider(int port) throws IOException{
            server = new ServerSocket(port);
    }

    public IOStream accept() throws IOException {
            Socket socket = server.accept();
            return new SocketStream(socket);
    }
    public void close() throws IOException {
            if(server!=null)
                    server.close();
    }
}
