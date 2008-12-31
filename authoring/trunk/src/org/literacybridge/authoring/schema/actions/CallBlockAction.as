package org.literacybridge.authoring.schema.actions {
	import org.literacybridge.authoring.schema.helper.IBaseActionVisitor;
	
	[Bindable]
	public class CallBlockAction extends Action {
		public var blockName:String;
		public var returnRewindSeconds:int;
		
		
		override public function accept(baseActionVisitor:IBaseActionVisitor):void {
			baseActionVisitor.visitCallBlockAction(this);
		}
	}
}