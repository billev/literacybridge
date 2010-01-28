class ContainerNavigator {
    
	var current:Container;
	
	function ContainerNavigator(context) {
	
		this.current = context;
		
		}
		
	function clone():ContiainerNavigator {
		return new ContainerNavigator(this.current);
		}
		
		
	function goto(ref:String):Boolean {
	
		
	
		}
		
	function moveToChildByID(id:String) {
	
		for (var i=0;i < current.childContainers.length;i++) {
			var childTest = current.childContainers[i];
			if (childTest.id == id) {
				current = childTest;
				return true;
				}
			}
			
		return false;
		
	
		}
		
	function moveToFirstChild():Boolean {
	
		if (this.current.childContainers.length ==0) 
			return false;
			
		this.current = this.current.childContainers[0];
		return true;
			
		
	
		}
		
	function moveToNext():Boolean {
	
		if (this.current.nextSibling == null)
			return false;
			
		this.current = this.current.nextSibling;
		return true;
		}
		
	function moveToPrevious():Boolean {
	
		if (this.current.previousSibling == null)
			return false;
			
		this.current = this.current.previousSibling;
		}
		
	function moveToParent():Boolean {
	
		if (this.current.parent == null)
			return false;
			
		this.current = current.parent;
	
		}
		
		
	function moveToPackage():Boolean {

		if (this.current instanceof Package)
			return true;
	
		while (this.moveToParent()) {
			if (this.current instanceof Package)
				return true;
			}

		return false;			
	
		}
	

			
	function getContainer(fullPath):Container {
	
		_root.addLog("Map: " + fullPath);
		return Container(map[fullPath]);
		
		}
		
	function loadMap(dev:Device) {
	
		for (var i=0;i<dev.childContainers.length;i++) {
			var name = String(i + 1);
			var childCon:Container = Container(dev.childContainers[i]);
			if (childCon.id != null)
				name = childCon.id;

			name = name + ":";
				
			map[name] = childCon;
			
			addChildContainers(name + "/",childCon);
			
			}
	
		}

		
	function addChildContainers(rootName:String,con:Container) {
	
		for (var i=0;i<con.childContainers.length;i++) {
			var name = String(i +  1);
			var childCon:Container = Container(con.childContainers[i]);
			if (childCon.id != null)
				name = childCon.id;
				
			var fullName:String = rootName + name;
			map[fullName] = childCon;
			
			addChildContainers(fullName + "/",childCon);
			
		
			}
	
		}
	
	
	
	}
