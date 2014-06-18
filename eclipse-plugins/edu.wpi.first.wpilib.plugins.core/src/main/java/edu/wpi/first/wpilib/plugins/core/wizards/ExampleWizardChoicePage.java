package edu.wpi.first.wpilib.plugins.core.wizards;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.jface.dialogs.IDialogPage;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

/**
 * The "New" wizard page allows setting the container for the new file as well
 * as the file name. The page will only accept file name without the extension
 * OR with the extension that matches the expected one (mpe).
 */

public class ExampleWizardChoicePage extends WizardPage {
	private Tree exampleTree;
	private Browser descriptionText;
	private IExampleProject selectedExample;
	private ExampleWizard parent;

	/**
	 * Constructor for SampleNewWizardPage.
	 * 
	 * @param pageName
	 */
	public ExampleWizardChoicePage(ExampleWizard parent, ISelection selection) {
		super("wizardPage");
		this.parent = parent;
		setTitle("Select Example Project to Create");
		setDescription("This wizard creates a new example project based on your selection.");
	}

	/**
	 * @see IDialogPage#createControl(Composite)
	 */
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NULL);
		GridLayout layout = new GridLayout();
		container.setLayout(layout);
		layout.numColumns = 2;
		layout.verticalSpacing = 9;

		exampleTree = new Tree(container, SWT.BORDER);
		GridData gd = new GridData(GridData.FILL_HORIZONTAL | GridData.FILL_VERTICAL);
		gd.heightHint = 400;
		exampleTree.setLayoutData(gd);
		exampleTree.addSelectionListener(new SelectionListener() {
			@Override
			public void widgetSelected(SelectionEvent arg0) {
				dialogChanged();
			}
			@Override
			public void widgetDefaultSelected(SelectionEvent arg0) {
				dialogChanged();
			}
		});

		descriptionText = new Browser(container, SWT.BORDER);
		gd = new GridData(GridData.FILL_HORIZONTAL | GridData.FILL_VERTICAL);
		gd.heightHint = 400;
		gd.widthHint = 300;
		descriptionText.setLayoutData(gd);
		
		initialize();
		dialogChanged();
		setControl(container);
	}

	/**
	 * Tests if the current workbench selection is a suitable container to use.
	 */

	private void initialize() {
		Document doc = loadXMLResource(parent.getXMLFile());

		// Generate all of the tags
		List<Tag> tags = new ArrayList<Tag>();
		NodeList nList = doc.getElementsByTagName("tagDescription");
		for (int i = 0; i < nList.getLength(); i++) {
			if (nList.item(i).getNodeType() == Node.ELEMENT_NODE) {
				tags.add(generateTagFromElement((Element) nList.item(i)));
			}
		}
		
		// Generate all of the example projects
		List<IExampleProject> examples = new ArrayList<IExampleProject>();
		nList = doc.getElementsByTagName("example");
		for (int i = 0; i < nList.getLength(); i++) {
			if (nList.item(i).getNodeType() == Node.ELEMENT_NODE) {
				examples.add(generateExampleFromElement((Element) nList.item(i)));
			}
		}

		WPILibCore.logInfo(examples.toString());
		WPILibCore.logInfo(tags.toString());
		
		// Generate the tags tree
		for (Tag tag : tags) {
			TreeItem tagItem = new TreeItem(exampleTree, SWT.NONE);
			tagItem.setData(tag);
			tagItem.setText(tag.getName());
			for (IExampleProject ex : examples) {
				if (ex.getTags().contains(tag.getName())) {
					TreeItem exItem = new TreeItem(tagItem, SWT.NONE);
					exItem.setData(ex);
					exItem.setText(ex.getName());
				}
			}
			tagItem.setExpanded(true);
		}
	}

	/**
	 * Ensures that both text fields are set.
	 */

	private void dialogChanged() {
		if (exampleTree.getSelection().length > 0) {
			Object selectedData = exampleTree.getSelection()[0].getData();
			if (selectedData instanceof IExampleProject) {
				IExampleProject example = (IExampleProject) selectedData;
				descriptionText.setText(example.getContent());
				selectedExample = example;
			} else if (selectedData instanceof Tag) {
				Tag tag = (Tag) selectedData;
				descriptionText.setText(tag.getContent());
				selectedExample = null;
			} else selectedExample = null;
		}
		
		if (selectedExample == null) {
			updateStatus("Must select a valid example before continuing.");
			return;
		}
		
		updateStatus(null);
	}

	private void updateStatus(String message) {
		setErrorMessage(message);
		setPageComplete(message == null);
	}

	private Document loadXMLResource(String filename) {
		final URL installURL = parent.getResourceURL();
		URL url = null;
		try {
			url = new URL(installURL, filename);
		} catch (final MalformedURLException e) {
            WPILibCore.logError("loadXMLResource()", e);
			return null;
		}
		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder dBuilder;
		Document doc;
		try {
			dBuilder = dbFactory.newDocumentBuilder();
			doc = dBuilder.parse(url.openStream());
		} catch (ParserConfigurationException e) {
            WPILibCore.logError("Error parsing "+filename, e);
			return null;
		} catch (SAXException e) {
            WPILibCore.logError("SAX issue with "+filename, e);
			return null;
		} catch (IOException e) {
            WPILibCore.logError("Error reading "+filename, e);
			return null;
		}
		doc.getDocumentElement().normalize();
		return doc;
	}

	private Tag generateTagFromElement(Element element) {
		String name = element.getElementsByTagName("name").item(0).getTextContent();
		String description = element.getElementsByTagName("description").item(0).getTextContent();
		return new Tag(name, description);
	}

	private IExampleProject generateExampleFromElement(Element element) {
		String name = element.getElementsByTagName("name").item(0).getTextContent();
		String description = element.getElementsByTagName("description").item(0).getTextContent();
		List<String> tags = new ArrayList<String>();
		Node tagsElement = element.getElementsByTagName("tags").item(0);
		if (tagsElement.getNodeType() == Node.ELEMENT_NODE) {
			NodeList tagElementList = ((Element) tagsElement).getElementsByTagName("tag");
			for (int i = 0; i < tagElementList.getLength(); i++) {
				tags.add(tagElementList.item(i).getTextContent());
			}
		}
		List<String> packages = new ArrayList<String>();
		tagsElement = element.getElementsByTagName("packages").item(0);
		if (tagsElement.getNodeType() == Node.ELEMENT_NODE) {
			NodeList elementList = ((Element) tagsElement).getElementsByTagName("package");
			for (int i = 0; i < elementList.getLength(); i++) {
				packages.add(elementList.item(i).getTextContent());
			}
		}
		List<IExampleProject.ExportFile> files = new ArrayList<IExampleProject.ExportFile>();
		tagsElement = element.getElementsByTagName("files").item(0);
		if (tagsElement.getNodeType() == Node.ELEMENT_NODE) {
			NodeList elementList = ((Element) tagsElement).getElementsByTagName("file");
			for (int i = 0; i < elementList.getLength(); i++) {
				if (elementList.item(i).getNodeType() == Node.ELEMENT_NODE) {
					element = (Element) elementList.item(i);
					files.add(new IExampleProject.ExportFile(element.getAttribute("source"),
								element.getAttribute("destination")));
				}
			}
		}
		return parent.makeExampleProject(name, description, tags, packages, files);
	}

	public IExampleProject getExampleProject() {
		return selectedExample;
	}
}
