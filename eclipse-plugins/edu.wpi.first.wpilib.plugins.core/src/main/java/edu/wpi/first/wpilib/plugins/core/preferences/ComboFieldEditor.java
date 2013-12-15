package edu.wpi.first.wpilib.plugins.core.preferences;

import java.util.List;

import org.eclipse.jface.preference.FieldEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

public class ComboFieldEditor extends FieldEditor {
	private List<String> choices;
	private Composite parent;
	Combo comboDropDown;

	public ComboFieldEditor(String preference, String label, Composite parent, List<String> choices) {
		super(preference, label, parent);
		this.choices = choices;
		for (String choice : choices) {
			comboDropDown.add(choice);
		}
	}
	
	public void setChoice(String item) {
		comboDropDown.select(choices.indexOf(item));
	}

	public String getChoice() {
		return choices.get(comboDropDown.getSelectionIndex());
	}
	
	@Override protected void adjustForNumColumns(int numColumns) {
		((GridData) parent.getLayoutData()).horizontalSpan = numColumns;
	}

	@Override protected void doFillIntoGrid(Composite parent, int numColumns) {
		this.parent = parent;
		
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.horizontalSpan = numColumns;
		parent.setLayoutData(gd);
		
		Label label = getLabelControl(parent);
		label.setText(getLabelText());
		
		comboDropDown = new Combo(parent, SWT.DROP_DOWN | SWT.BORDER);
		gd = new GridData(GridData.FILL_HORIZONTAL);
		comboDropDown.setLayoutData(gd);
	}

	@Override protected void doLoad() {
		String item = getPreferenceStore().getString(getPreferenceName());
		setChoice(item);
	}

	@Override protected void doLoadDefault() {
		String item = getPreferenceStore().getDefaultString(getPreferenceName());
		setChoice(item);
	}

	@Override protected void doStore() {
		getPreferenceStore().setValue(getPreferenceName(), getChoice());
	}

	@Override public int getNumberOfControls() {
		return 2;
	}
}
