package org.literacybridge.authoring.schema.actions {
	import org.literacybridge.authoring.schema.helper.IBaseActionVisitor;
	
	[Bindable]
	public class RelativeTimeJumpAction extends Action	{
		public var relatveTime:int;
		
		override public function accept(baseActionVisitor:IBaseActionVisitor):void {
			baseActionVisitor.visitRelativeTimeJumpAction(this);
		}
	}
}