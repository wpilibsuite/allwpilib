/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.popup;

import edu.wpi.first.wpilibj.tables.ITable;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Control;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.stage.Modality;

/**
 *
 * @author Sam
 */
class AddValuePopup extends Popup {

    public enum InputType {
        BOOLEAN, NUMBER, STRING, SUBTABLE
    }
    
    private final InputType type;
    private final ITable table;

    public AddValuePopup(ITable table, InputType type) {
        super();
        
        this.table = table;
        this.type = type;

        initModality(Modality.WINDOW_MODAL);
    }

    @Override
    public void showPopup() {
        initComponents();
        showAndWait();
    }

    @Override
    public void initComponents() {
        initComponents(type);
    }

    private void initComponents(final InputType type) {
        final Scene scene = new Scene(new Group());
        scene.setFill(Color.WHITESMOKE);
        scene.getStylesheets().add(
                "edu/wpi/first/tableviewer/stylesheets/ViewerStyleSheet.css");
        final Label messageLabel = new Label();
        final Label keyLabel = new Label("Key:");
        final Label valueLabel = new Label("Value:");
        final TextField keyField = new TextField();
        final Button confirm = new Button("Add");

        keyField.setPromptText("Key");
        keyField.setPrefWidth(100);

        final Control valueControl;
        ObservableList<String> booleanOptions = FXCollections.observableArrayList("True", "False");
        switch (type) {
            case BOOLEAN:
                messageLabel.setText("Add boolean");
                valueControl = new ComboBox(booleanOptions);
                ((ComboBox) valueControl).getSelectionModel().select(0);
                confirm.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent t) {
                        if (table.containsKey(keyField.getText())) {
                            PopupFactory.showErrorDialog("Key already in use!");
                        } else {
                            table.putBoolean(keyField.getText(), ((ComboBox) valueControl).getSelectionModel().isSelected(0));
                            close();
                        }
                    }
                });
                break;
            case NUMBER:
                messageLabel.setText("Add number");
                valueControl = new TextField();
                ((TextField) valueControl).setPromptText("Number");
                confirm.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent t) {
                        try {
                            if (table.containsKey(keyField.getText())) {
                                PopupFactory.showErrorDialog("Key already in use!");
                            } else {
                                table.putNumber(keyField.getText(), Double.parseDouble(((TextField) valueControl).getText()));
                                close();
                            }
                        } catch (NumberFormatException ex) {
                            PopupFactory.showErrorDialog("Invalid number!");
                        }
                    }
                });
                break;
            case STRING:
                messageLabel.setText("Add string");
                valueControl = new TextField();
                ((TextField) valueControl).setPromptText("Text");
                confirm.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent t) {
                        if (table.containsKey(keyField.getText())) {
                            PopupFactory.showErrorDialog("Key already in use!");
                        } else {
                            table.putString(keyField.getText(), ((TextField) valueControl).getText());
                            close();
                        }
                    }
                });
                break;
            case SUBTABLE:
                keyLabel.setText("Table name");
                keyField.setPromptText("Name");
                valueControl = null;
                confirm.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent t) {
                        //TODO make a subtable show up without putting a value
                        ITable i = table.getSubTable(keyField.getText());
                        close();
                    }
                });
                break;
            default:
                return;
        }

        final GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(5);
        grid.add(keyLabel, 1, 1);
        grid.add(keyField, 2, 1);
        if (type != InputType.SUBTABLE) {
            grid.add(valueLabel, 1, 2);
            grid.add(valueControl, 2, 2);
            valueControl.setPrefWidth(100);
        }
        grid.add(confirm, 2, 3);

        ((Group) scene.getRoot()).getChildren().add(grid);
        setScene(scene);

    }
}
