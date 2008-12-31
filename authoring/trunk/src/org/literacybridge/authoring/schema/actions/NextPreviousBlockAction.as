package org.literacybridge.authoring.schema.actions {
	import org.literacybridge.authoring.schema.helper.IBaseActionVisitor;
	
	[Bindable]
	public class NextPreviousBlockAction extends Action {
		public static const Next:int = 1;
		public static const Previous:int = 2;
		
		public var mode:int;
		public var className:String;
		
		public function NextPreviousBlockAction(mode:int) {
			this.mode = mode;
		}
		
		override public function accept(baseActionVisitor:IBaseActionVisitor):void {
			baseActionVisitor.visitNextPreviousBlockAction(this);
		}
	}
}