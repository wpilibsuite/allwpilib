package edu.wpi.first.table_viewer;

import edu.wpi.first.table_viewer.tree.NetworkTableTree;
import java.awt.*;

import javax.swing.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.tables.*;

public class TableViewerFrame extends JFrame implements IRemoteConnectionListener {

    private final JLabel statusLabel;
    private final NetworkTableTree tree;

    public TableViewerFrame(NetworkTableNode node) {
        super(node.toString());
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        add(statusLabel = new JLabel(), BorderLayout.NORTH);
        
        node.addConnectionListener(this, true);
        
        tree = new NetworkTableTree(node);
        add(new JScrollPane(tree));
        setSize(600, 600);
    }

    @Override
    public void connected(IRemote remote) {
        statusLabel.setText("Connected");
    }

    @Override
    public void disconnected(IRemote remote) {
        statusLabel.setText("Disconnected");
    }
}
