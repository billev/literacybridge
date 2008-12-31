package org.literacybridge.authoring.schema.actions {
	import org.literacybridge.authoring.schema.helper.IBaseActionVisitor;
	
	[Bindable]
	public class LoadPackageAction extends Action {
		public var name:String;
		
		override public function accept(baseActionVisitor:IBaseActionVisitor):void {
			baseActionVisitor.visitLoadPackageAction(this);
		}
	}
}