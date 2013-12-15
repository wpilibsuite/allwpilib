package edu.wpi.first.table_viewer;

import java.awt.*;
import java.awt.event.*;
import java.util.prefs.Preferences;

import javax.swing.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.client.*;
import edu.wpi.first.wpilibj.networktables2.server.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;

public class TableViewerModePrompt extends JDialog implements ActionListener {

    private JTextField hostField;
    private JButton launchClientButton, launchServerButton;
    private JButton cancelButton;
    private Preferences prefs;

    public TableViewerModePrompt() {
        setTitle("Preferences");
        try {
            UIManager.setLookAndFeel("javax.swing.plaf.nimbus.NimbusLookAndFeel");
            setBackground(Color.white);
        } catch (Exception e) {
            e.printStackTrace();
        }
        prefs = Preferences.userNodeForPackage(getClass());
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        JPanel contentPane = new JPanel();
        contentPane.setBorder(BorderFactory.createEmptyBorder(10, 20, 5, 20));
        setContentPane(contentPane);
        setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();

        c.gridwidth = 1;

        c.gridy = 1;
        add(new JLabel("Host: "), c);
        c.gridx = 1;
        String host;
        try {
            host = prefs.get("host", "localhost");
        } catch (NullPointerException e) {
            host = "localhost";
        }
        add(hostField = new JTextField(host), c);
        hostField.addActionListener(this);
        hostField.setColumns(20);

        c.gridx = 0;
        c.gridy = 3;
        c.gridwidth = 2;

        JPanel buttonPanel = new JPanel();
        buttonPanel.add(launchClientButton = new JButton("Start client"));
        launchClientButton.addActionListener(this);
        buttonPanel.add(launchServerButton = new JButton("Start server"));
        launchServerButton.addActionListener(this);
        buttonPanel.add(cancelButton = new JButton("Cancel"));
        cancelButton.addActionListener(this);

        add(buttonPanel, c);
        
        setSize(320, 120);
        setMinimumSize(getSize());
        
        pack();
    }

    public String getHost() {
        return hostField.getText();
    }

    public int getPort() {
        return 1735;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() != cancelButton) {
            try {
                NetworkTableNode node = null;
                if (e.getSource() == launchServerButton) {//server
                    IOStreamProvider streamProvider = SocketStreams.newStreamProvider(getPort());
                    node = new NetworkTableServer(streamProvider);
                    prefs.put("host", "");
                } else if (e.getSource() == launchClientButton) {
                    String host = getHost();
                    IOStreamFactory streamFactory = SocketStreams.newStreamFactory(host, getPort());
                    NetworkTableClient client = new NetworkTableClient(streamFactory);
                    client.reconnect();
                    node = client;
                    prefs.put("host", host);
                }
                TableViewerFrame frame = new TableViewerFrame(node);
                frame.setVisible(true);
                dispose();
            } catch (Exception ex) {
                JOptionPane.showMessageDialog(null, ex.getClass() + ": " + ex.getMessage(), "Error creating table node", JOptionPane.ERROR_MESSAGE);
            }
        } else {
            System.exit(0);
        }
    }
}
