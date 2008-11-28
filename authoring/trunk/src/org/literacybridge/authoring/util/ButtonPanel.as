package org.literacybridge.authoring.util {
    import flash.events.MouseEvent;
    
    import mx.containers.Panel;
    import mx.controls.Button;

    [Event(name="buttonClick", type="flash.events.Event")]

    public class ButtonPanel extends Panel {
        [Bindable] public var buttonLabel:String;
        [Bindable] public var buttonPadding:Number = 10;

        private var mybtn:Button;

        public function ButtonPanel() {
            super();
        }
                
        protected override function createChildren():void {
            super.createChildren();
            
            if( ! buttonLabel ) return;
            
            mybtn = new Button();
            mybtn.label = buttonLabel;
            mybtn.visible = true;
            mybtn.includeInLayout = true;
            mybtn.addEventListener( MouseEvent.CLICK, buttonClickHandler );
            rawChildren.addChild( mybtn );
            
            
        }
        
        protected override function updateDisplayList(unscaledWidth:Number, unscaledHeight:Number):void {
            super.updateDisplayList(unscaledWidth,unscaledHeight);
            var x:int = width - ( mybtn.width + buttonPadding );
            mybtn.width = mybtn.measuredWidth;
            mybtn.height = mybtn.measuredHeight;
            this.setStyle('headerHeight', mybtn.height + buttonPadding );            
            mybtn.move( x, 5 );
        }
        
        private function buttonClickHandler(event:MouseEvent):void {
            this.dispatchEvent( new Event( 'buttonClick', false ) );
        }
        
    }
}