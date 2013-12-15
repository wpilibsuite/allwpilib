/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.popup;

import edu.wpi.first.tableviewer.NetworkTableViewer;
import edu.wpi.first.tableviewer.treeview.TableTreeView;

import edu.wpi.first.wpilibj.networktables2.NetworkTableNode;
import edu.wpi.first.wpilibj.networktables2.client.NetworkTableClient;
import edu.wpi.first.wpilibj.networktables2.server.NetworkTableServer;
import edu.wpi.first.wpilibj.networktables2.stream.SocketStreams;

import java.io.IOException;
import java.util.prefs.Preferences;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 *
 * @author Sam
 */
public class PreferencesDialog {

    public static final int DEFAULT_PORT = 1735;
    private final Preferences prefs = Preferences.userNodeForPackage(NetworkTableViewer.class);
    private String host = "localhost";
    private NetworkTableNode node;
    @FXML public TextField addressField;
    private Stage stage;
    
    public NetworkTableNode getNode() {
        return node;
    }
    
    public void setStage(Stage stage) {
        this.stage = stage;
    }

    private void setServer(int port) throws IOException {
        this.node = new NetworkTableServer(SocketStreams.newStreamProvider(port));
    }

    private void setClient(String host, int port) throws IOException {
        this.node = new NetworkTableClient(SocketStreams.newStreamFactory(host, port));
        ((NetworkTableClient) node).reconnect();
    }

    @FXML
    void handleStartClientAction(ActionEvent event) {
        try {
            setClient(host, DEFAULT_PORT);
            System.out.println("Connecting client to " + host + ":" + DEFAULT_PORT);
            handleStartAction();
        } catch (IOException e) {
        }
    }

    @FXML
    void handleStartServerAction(ActionEvent event) {
        try {
            setServer(DEFAULT_PORT);
            System.out.println("Starting server on port " + DEFAULT_PORT);
            handleStartAction();
        } catch (IOException e) {
        }
    }
    
    private void handleStartAction() {
        TableTreeView.setRootNode(node);
        stage.close();
    }

    @FXML
    void handleAddressEntered(ActionEvent event) {
        host = addressField.getText();
        prefs.put("host", host);
        System.out.println("Set host to "+addressField.getText());
    }
    
    @FXML
    void handleCancelAction(ActionEvent event) {
        System.exit(0);
    }
}
