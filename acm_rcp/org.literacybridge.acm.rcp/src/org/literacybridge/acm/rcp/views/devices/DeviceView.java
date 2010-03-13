package org.literacybridge.acm.rcp.views.devices;

import org.eclipse.jface.viewers.ListViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
//import org.eclipse.ui.forms.widgets.Form;
//import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.part.ViewPart;

public class DeviceView extends ViewPart {

	private ListViewer deviceListViewer = null;
//	private FormToolkit toolkit;
//	private Form form;
	
	
	public DeviceView() {
	}

	@Override
	public void createPartControl(Composite parent) {
//		toolkit = new FormToolkit(parent.getDisplay());
//		form = toolkit.createForm(parent);
//		form.setText("Hello, Eclipse Forms");
		
		
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 1;
		parent.setLayout(gridLayout);
		
		Label deviceLabel = new Label(parent, SWT.LEFT);
		deviceLabel.setText("Devices:");
		GridData gridData = new GridData();
		gridData.horizontalAlignment = GridData.FILL_HORIZONTAL;
		
		deviceListViewer = new ListViewer(parent);
		deviceListViewer.setContentProvider(new DeviceContentProvider());
		deviceListViewer.setLabelProvider(new DeviceLabelProvider());
		deviceListViewer.setInput(new Object());
		deviceListViewer.getControl().setLayoutData(gridData);
		deviceListViewer.getControl().setSize(new Point(100,100));
	}

	@Override
	public void setFocus() {
	}

}
