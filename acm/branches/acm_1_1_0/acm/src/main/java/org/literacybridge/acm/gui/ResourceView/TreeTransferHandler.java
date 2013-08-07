package org.literacybridge.acm.gui.ResourceView;

import java.awt.Container;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.File;
import java.io.IOException;
import java.util.List;

import javax.persistence.EntityManager;
import javax.persistence.EntityTransaction;
import javax.swing.JTree;
import javax.swing.TransferHandler;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;

import org.literacybridge.acm.content.AudioItem;
import org.literacybridge.acm.db.Persistence;
import org.literacybridge.acm.importexport.FileImporter;
import org.literacybridge.acm.gui.resourcebundle.LabelProvider;
import org.literacybridge.acm.gui.Application;
import org.literacybridge.acm.gui.ResourceView.CategoryView.CategoryTreeNodeObject;
import org.literacybridge.acm.gui.ResourceView.audioItems.AudioItemView;
import org.literacybridge.acm.gui.dialogs.BusyDialog;
import org.literacybridge.acm.gui.util.UIUtils;
import org.literacybridge.acm.gui.util.language.LanguageUtil;

public class TreeTransferHandler extends TransferHandler {
	private static final long serialVersionUID = 1L;

	static DataFlavor[] supportedFlavors = new DataFlavor[] {
		DataFlavor.javaFileListFlavor,
		AudioItemView.AudioItemDataFlavor
	};

	@Override
	public boolean canImport(TransferHandler.TransferSupport support) {
		if (!support.isDrop()) {
			return false;
		}

		boolean supported = false;
		for (DataFlavor flavor : supportedFlavors) {
			if (support.isDataFlavorSupported(flavor)) {
				supported = true;
				break;
			}
		}

		if (!supported) {
			return false;
		}
		
		// Get drop location info.
		JTree.DropLocation dl = (JTree.DropLocation) support.getDropLocation();
		TreePath dest = dl.getPath();
		DefaultMutableTreeNode parent = (DefaultMutableTreeNode) dest.getLastPathComponent();
		final CategoryTreeNodeObject target = (CategoryTreeNodeObject) parent.getUserObject();
		
		// only allow dropping on leaves
		return !target.getCategory().hasChildren();
	}

	@Override
	public boolean importData(TransferHandler.TransferSupport support) {
		if (!canImport(support)) {
			return false;
		}
		// Get drop location info.
		JTree.DropLocation dl = (JTree.DropLocation) support.getDropLocation();
		TreePath dest = dl.getPath();
		DefaultMutableTreeNode parent = (DefaultMutableTreeNode) dest.getLastPathComponent();
		final CategoryTreeNodeObject target = (CategoryTreeNodeObject) parent.getUserObject();
		
		// Extract transfer data.
		try {
			if (support.isDataFlavorSupported(DataFlavor.javaFileListFlavor)) {
				importExternalFiles(support, target);
				return true;
			} else if (support.isDataFlavorSupported(AudioItemView.AudioItemDataFlavor)) {
				assignCategory(support, target);
				return true;
			}
			
		} catch (UnsupportedFlavorException e) {
		} catch (IOException e) {}
		
		return false;
	}
	
	private void importExternalFiles(TransferHandler.TransferSupport support, final CategoryTreeNodeObject target) throws IOException, UnsupportedFlavorException {
		Transferable t = support.getTransferable();
		final List<File> files = (List<File>) t.getTransferData(DataFlavor.javaFileListFlavor);

		
		// don't piggyback on the drag&drop thread
		Runnable job = new Runnable() {

			@Override
			public void run() {
				Application parent = Application.getApplication();
				Container busy = UIUtils.showDialog(parent, new BusyDialog(LabelProvider.getLabel("IMPORTING_FILES", LanguageUtil.getUILanguage()), parent));
				try {
					for (File f : files) {
						if (f.isDirectory()) {
							FileImporter.getInstance().importDirectory(target.getCategory(), f, false);
						} else {
							FileImporter.getInstance().importFile(target.getCategory(), f);
						}
					}
				} catch (IOException e) {
					e.printStackTrace();
				} finally {
					UIUtils.hideDialog(busy);
					Application.getFilterState().updateResult(true);
				}
			}
		};
		
		new Thread(job).start();
	}
	
	private void assignCategory(TransferHandler.TransferSupport support, final CategoryTreeNodeObject target) throws IOException, UnsupportedFlavorException {
		Transferable t = support.getTransferable();
		final boolean move = support.getDropAction() == TransferHandler.MOVE;

		final AudioItem[] audioItems = (AudioItem[]) t.getTransferData(AudioItemView.AudioItemDataFlavor);
		// don't piggyback on the drag&drop thread
		Runnable job = new Runnable() {

			@Override
			public void run() {

				EntityManager em = null;
				try {
					em = Persistence.getEntityManager();
					EntityTransaction transaction = em.getTransaction();
					transaction.begin();
					try {
						for (AudioItem item : audioItems) {
							if (move) {
								item.removeAllCategories();
							}
							item.addCategory(target.getCategory());
							item.commit(em);
						}
						transaction.commit();
					} finally {
		                if (transaction.isActive()) {
		                	transaction.rollback();
		                }

						Application.getFilterState().updateResult(true);
					}
				} finally {
					if (em != null) {
						em.close();
					}
				}
			}
		};
		new Thread(job).start();
		
	}
}