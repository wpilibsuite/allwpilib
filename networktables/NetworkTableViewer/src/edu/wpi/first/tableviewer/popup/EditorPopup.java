/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.popup;

import edu.wpi.first.tableviewer.treeview.TableLeaf;
import javafx.collections.FXCollections;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;

/**
 *
 * @author Sam
 */
public class EditorPopup extends Popup {

    private final Label inputLabel = new Label();
    private final TextField inputField = new TextField();
    private final ComboBox booleanBox = new ComboBox(FXCollections.observableArrayList("True", "False"));
    private final Button okayButton = new Button("OK"),
                         cancelButton = new Button("Cancel");
    
    private final String itemType;
    private final TableLeaf leaf;

    public EditorPopup(TableLeaf leaf) {
        super("Edit Value", null, null);
        this.leaf = leaf;

        String tableValue = leaf.getTableValue().toString();
        String tableKey = leaf.getTableKey();

        inputLabel.setText(tableKey+":");

        inputField.setText(tableValue);
        inputField.setPromptText("New value");
        inputField.setPrefWidth(100);
        
        booleanBox.getSelectionModel().selectFirst();

        itemType = leaf.getType().toString();
        cancelButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                close();
            }
        });
    }

    @Override
    public void initComponents() {
        initComponents(itemType);
    }

    private void initComponents(final String type) {
        final Scene scene = new Scene(new Group());
        scene.setFill(Color.WHITESMOKE);
        scene.getStylesheets().add(
                "edu/wpi/first/tableviewer/stylesheets/ViewerStyleSheet.css");

        final GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(15);
        grid.setPadding(new Insets(5, 5, 5, 5));
        grid.setAlignment(Pos.CENTER);
        grid.add(inputLabel, 0, 1);

        ((Group) scene.getRoot()).getChildren().add(grid);
        setScene(scene);

        switch (type.toLowerCase()) {
            case "boolean":
                grid.add(booleanBox, 1, 1);
                okayButton.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent t) {
                        leaf.updateValue(booleanBox.getSelectionModel().isSelected(0));
                        leaf.setExpanded(false); // refresh the leaf to updateValue the shown value
                        leaf.setExpanded(true);
                        close();
                    }
                });
                break;
            case "number":
                grid.add(inputField, 1, 1);
                okayButton.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent t) {
                        try {
                            leaf.updateValue(Double.parseDouble(inputField.getText()));
                            leaf.setExpanded(false); // refresh the leaf to updateValue the shown value
                            leaf.setExpanded(true);
                            close();
                        } catch (NumberFormatException ex) {
                            PopupFactory.showErrorDialog("Invalid number value!");
                        }
                    }
                });
                break;
            case "string":
                grid.add(inputField, 1, 1);
                okayButton.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent t) {
                        leaf.updateValue(inputField.getText());
                        leaf.setExpanded(false); // refresh the leaf to updateValue the shown value
                        leaf.setExpanded(true);
                        close();
                    }
                });
                break;
        }
        grid.add(okayButton, 0, 2);
        grid.add(cancelButton, 1, 2);

    }
}
