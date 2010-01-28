class Container implements IXmlSerializable {
    
    var id:String;
    var name:String;
    var className:String ;
    var parent:Container;
	var onStart:EventHandler;
	var onFinish:EventHandler;
	var buttonHandlers:Array;
    var childContainers:Array;
	var previousSibling:Container;
	var nextSibling:Container;
    
    
    function Container() {
    
		buttonHandlers = new Array();
		childContainers = new Array();
		
		}

		
    function get startPosition():Position {

		return childContainers[0].startPosition;

		}
		
	function get previous():Container {
	
		if (this.previousSibling!=null)
			return previousSibling.last;
			
		if (parent!=null)
			return parent.previous;
			
		return null;
		
		}
		
	function get next():Container {
	
		if (this.nextSibling!=null)
			return nextSibling.first;
			
		if (parent!=null)
			return parent.next;
			
		return null;
		
		}


	function get first():Container {
		return childContainers[0].first;
		}
		
	function get last():Container {
		return childContainers[childContainers.length-1].last;
		}
		
	
	function handleButton(context:PlayerContext,evt:ButtonEventArgs) {
		
		for(var i=0;i < this.buttonHandlers.length;i++) 
			if (buttonHandlers[i].handleEvent(context,evt))
				return;
			
		// If button has not handled, try the parent
		if (this.parent != null)
			this.parent.handleButton(context,evt);
		
		}
		
	function handleOnStart(context:PlayerContext) {
		
		if (this.onStart != null) {
			if (!this.onStart.handleEvent(context))
				return;
			}	
			
		if (this.parent != null)
			this.parent.handleOnStart(context);

		}
		
	function handleOnFinish(context:PlayerContext) {
		
		if (this.onFinish != null) {
			if (!this.onFinish.handleEvent(context))
				return;
			}	
			
		if (this.parent != null)
			this.parent.handleOnFinish(context);

		}


	function get scopeName():String {
		return parent.scopeName();
		}	
    
    function addChild(newChild) {

		newChild.parent = this;
		
		if (childContainers.length > 0) {
			var prevItem:Container =  Container(childContainers[childContainers.length-1]);
			prevItem.nextSibling = newChild;
			newChild.previousSibling = prevItem;
			}

		childContainers.push(newChild);
		}
		
		
		
	function findChild(pos:Position) : Container {
	
		//_root.addLog("FindChild: " + this.name);
		
		for (var i=0;i<childContainers.length;i++) {
			var foundChild:Container = childContainers[i].findChild(pos);
			if (foundChild != null) {
				return Container(foundChild);
				}
			}
			
		return this;
		
		}
		

	function readXml(node:XMLNode) {
	
		var atts = node.attributes;
		name = atts["Name"];
		id = atts["ID"];
		className = atts["Class"];
		
		// -- Parse children
		var childNodes = node.childNodes;
		var childCount = childNodes.length;
		for (var i=0;i<childCount;i++) {
			var cNode = node.childNodes[i];
			var nName = cNode.nodeName;
			var childContainer = null;
			if (nName == "Package") {
				childContainer = new Package();
				}
			if (nName == "Folder") {
				childContainer = new Folder();
				}
			if (nName == "File") {
				childContainer = new File();
				childContainer.href = cNode.attributes.Href;
				}
			if (nName == "Block") {
				childContainer = new Block();
				}
			if (nName == "OnStart") {
				this.onStart = new EventHandler();
				this.onStart.parent = this;
				this.onStart.readXml(cNode);
				}
			if (nName == "OnFinish") {
				this.onFinish = new EventHandler();
				this.onFinish.parent = this;
				this.onFinish.readXml(cNode);
				}
			if (nName == "OnButton") {
				var newButton = new ButtonEventHandler();
				newButton.readXml(cNode);
				this.buttonHandlers.push(newButton);
				}


			
			// Add Child
			if (childContainer != null) {
				addChild(childContainer);
				childContainer.readXml(cNode);
				}
		
			}
	
	
		}
    
    }
    
