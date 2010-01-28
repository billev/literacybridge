
class ContainerMap {
    
    var map:Object;
	
	
	function ContainerMap() {
		map = new Object();
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
