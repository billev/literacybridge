package org.literacybridge.authoring.schema.actions {
	import org.literacybridge.authoring.schema.helper.IBaseActionVisitor;
	
	[Bindable]
	public class SimpleAction extends Action {
		public static const NoOp:int = 1;
		public static const Stop:int = 2;
		public static const Pause:int = 3;
		public static const Return:int = 4;
		public static const PlayPause:int = 5;
		
		public var action:int;
		
		public function SimpleAction(action:int):void {
			this.action = action;
		}
		
		override public function accept(baseActionVisitor:IBaseActionVisitor):void {
			baseActionVisitor.visitSimpleAction(this);
		}
	}
}