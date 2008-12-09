package org.literacybridge.authoring.events {
	import flash.events.Event;
	
	import mx.controls.Menu;
	import mx.controls.MenuBar;
	import mx.controls.listClasses.IListItemRenderer;
	import mx.events.MenuEvent;

	public class ExtendedMenuEvent extends MenuEvent {
		public static const MENU_OPEN:int = 0;
		public static const MENU_SAVE:int = 1;
		public static const MENU_SAVE_AS:int = 2;
		public static const MENU_QUIT:int = 3;
		public static const MENU_PREFERENCES:int = 4;
		public static const MENU_ABOUT:int = 5;
		public static const MENU_SHOW_VIRTUAL_PLAYER:int = 6;
		
		public var id:int;
		
		public function ExtendedMenuEvent(id:int, type:String, bubbles:Boolean=false, cancelable:Boolean=true, 
				menuBar:MenuBar=null, menu:Menu=null, item:Object=null, itemRenderer:IListItemRenderer=null, 
				label:String=null, index:int=-1) {
			super(type, bubbles, cancelable, menuBar, menu, item, itemRenderer, label, index);
			this.id = id;
		}
		
		override public function clone():Event {
			 return new ExtendedMenuEvent(id, type, bubbles, cancelable, menuBar, menu, item, itemRenderer, label, index);
		}
		
	}
}