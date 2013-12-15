/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.tableviewer.treeview;

import edu.wpi.first.tableviewer.EntryData;
import edu.wpi.first.tableviewer.popup.PopupFactory;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TreeCell;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;

/**
 *
 * @author Sam
 */
public class TableCell extends TreeCell<EntryData> {

    private GridPane grid = new GridPane();
    private Label key = new Label(),
            value = new Label(),
            type = new Label();
     
    // Very hackish but it almost works. Just don't collapse tables. 
    // I blame JavaFX/Patrick for making me do this.
    private static List<TableCell> cells = new ArrayList<>();
    private static int currentCell = 0;
    private static int maxCells = 100;
    
    private static HashMap<TableCell, EntryData> cellMap = new HashMap<>();

    static {
        for (int i = 0; i < maxCells; i++) {
            cells.add(new TableCell());
        }
    }

    public static TableCell getCell() {
        if (currentCell >= maxCells) {
            currentCell = 0;
        }
        return cells.get(currentCell++);
    }

    private TableCell() {
        super();
        genGrid();
        this.setOnMouseClicked(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent t) {
                ITableItem selected = getItem().getTableItem();
                if (selected != null && selected instanceof TableLeaf && t.getClickCount() == 2) {
                    TableLeaf leaf = (TableLeaf) selected;
                    PopupFactory.showEditorDialog(leaf);
                }
            }
        });
    }

    private void genGrid() {
        key.setPrefWidth(50);
        value.setPrefWidth(125);
        type.setPrefWidth(75);
        grid.add(key, 0, 0);
        grid.add(value, 1, 0);
        grid.add(type, 2, 0);
        grid.setHgap(15);
    }

    @Override
    public void updateItem(EntryData data, boolean empty) {
        super.updateItem(data, empty);
        if (data != null && !empty) {
            cellMap.put(this, data);
            if (!data.representsLeaf()) {
                setContextMenu(getBranchMenu((TableBranch) data.getTableItem()));
                setText(data.getKey());
                setGraphic(null);
            } else {
                key.setText(data.getKey());
                value.setText(data.getValue().toString());
                type.setText(data.getType().toString());

                setText(null);
                setGraphic(grid);
                setContextMenu(getLeafMenu((TableLeaf)data.getTableItem()));
            }
        }
    }

    private ContextMenu getBranchMenu(final TableBranch branch) {
        MenuItem addBoolean = new MenuItem("Add boolean");
        MenuItem addNumber = new MenuItem("Add number");
        MenuItem addString = new MenuItem("Add string");

        addBoolean.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                PopupFactory.showAddBooleanDialog(branch.getTable());
            }
        });

        addNumber.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                PopupFactory.showAddNumberDialog(branch.getTable());
            }
        });

        addString.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                PopupFactory.showAddStringDialog(branch.getTable());
            }
        });

        ContextMenu cMenu = new ContextMenu();
        cMenu.getItems().addAll(addBoolean, addNumber, addString);
        return cMenu;
    }

    private ContextMenu getLeafMenu(final TableLeaf leaf) {
        MenuItem edit = new MenuItem("Edit");

        edit.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                PopupFactory.showEditorDialog(leaf);
            }
        });

        ContextMenu cMenu = new ContextMenu(edit);
        return cMenu;

    }
}
