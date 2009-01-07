package org.literacybridge.authoring.schema.actions {
	import mx.collections.ArrayCollection;
	
	import org.literacybridge.authoring.schema.helper.IBaseActionVisitor;
	
	[Bindable]
	public class SetLightAction	extends Action {
		
		static private var colorList:ArrayCollection = null;
		static private var stateList:ArrayCollection = null;
		
		public static const Red:int = 0;
		public static const Green:int = 1;
		
		public static const On:int = 0;
		public static const Off:int = 1;
		
		public var color:int;
		public var mode:int;
		
		
		public static function getColorList():ArrayCollection {
			if (colorList == null) {
				colorList = new ArrayCollection();
				colorList.addItem(Red);
				colorList.addItem(Green);
			}
			return colorList;
		}

		public static function getStateList():ArrayCollection {
			if (stateList == null) {
				stateList = new ArrayCollection();
				stateList.addItem(On);
				stateList.addItem(Off);
			}
			return stateList;
		}
		
		override public function accept(baseActionVisitor:IBaseActionVisitor):void	{
			baseActionVisitor.visitSetLightAction(this);
		}	
	}
}