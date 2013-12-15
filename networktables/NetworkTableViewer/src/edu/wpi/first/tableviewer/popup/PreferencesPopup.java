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
import javafx.event.EventHandler;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.stage.WindowEvent;

/**
 *
 * @author Sam
 */
public class PreferencesPopup extends Popup {

    public static final int DEFAULT_PORT = 1735;
    private final Preferences prefs = Preferences.userNodeForPackage(NetworkTableViewer.class);
    private String host = "localhost";
    private NetworkTableNode node;

    public PreferencesPopup() {
        super("Preferences", null, null);
        setWidth(215);
        setHeight(160);
    }

    public NetworkTableNode getNode() {
        return node;
    }

    @Override
    public void initComponents() {
        try {
            host = prefs.get("host", "localhost");
        } catch (NullPointerException e) {
            host = "localhost";
        }

        final Scene scene = new Scene(new Group());
        scene.getStylesheets().add("edu/wpi/first/tableviewer/stylesheets/ViewerStyleSheet.css");

        final Label hostLabel = new Label("Host:");
        final CheckBox clientBox = new CheckBox("Client");
        clientBox.setSelected(true); // default to client
        final TextField hostField = new TextField("localhost");
        final Button okayButton = new Button("OK");
        final Button cancelButton = new Button("Cancel");

        hostField.setMaxWidth(100);

        hostField.setPromptText("Host");

        hostField.setText(prefs.get("host", "localhost"));

        okayButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                try {
                    host = hostField.getText();
                    if (clientBox.isSelected()) { // client
                        setClient(host, DEFAULT_PORT);
                        System.out.println("Connecting client to " + host + ":" + DEFAULT_PORT);
                    } else { // server
                        setServer(DEFAULT_PORT);
                        System.out.println("Starting server on port " + DEFAULT_PORT);
                    }
                    TableTreeView.setRootNode(node);
                    prefs.put("host", host);
                    close();
                } catch (NumberFormatException ex) { // invalid team number
                    PopupFactory.showErrorDialog("Invalid team number!");
                } catch (IOException ex) {
                    System.err.println("Error creating table node: "
                            + ex.getClass().toString().substring(ex.getClass().toString().lastIndexOf(".") + 1) + ": "
                            + ex.getMessage());
                }
            }
        });

        cancelButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                System.exit(0);
            }
        });

        setOnCloseRequest(new EventHandler<WindowEvent>() {
            @Override
            public void handle(WindowEvent t) {
                System.exit(0);
            }
        });

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(5);
        grid.add(clientBox, 1, 1);
        grid.add(hostLabel, 1, 2);
        grid.add(hostField, 2, 2);
        grid.add(okayButton, 1, 3);
        grid.add(cancelButton, 2, 3);

        ((Group) scene.getRoot()).getChildren().addAll(grid);

        setScene(scene);
    }
    
    private void setServer(int port) throws IOException {
        this.node = new NetworkTableServer(SocketStreams.newStreamProvider(port));
    }
    
    private void setClient(String host, int port) throws IOException {
        this.node = new NetworkTableClient(SocketStreams.newStreamFactory(host, port));
        ((NetworkTableClient)node).reconnect();
    }
    
}
