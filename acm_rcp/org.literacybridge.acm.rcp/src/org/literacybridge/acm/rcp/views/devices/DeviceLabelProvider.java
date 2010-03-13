package org.literacybridge.acm.rcp.views.devices;

import org.eclipse.jface.viewers.LabelProvider;

public class DeviceLabelProvider extends LabelProvider {

      public String getText(Object element) {
        return element.toString();
      }
}
