import mx.utils.Delegate;

class PlaylistLoader {

	private var _playlistXml:XML; 
	private var _packageXml:XML;
	private var _containerUrls:Array;
	private var _device:Device;
	
	
	function PlaylistLoader(dev:Device) {
		
		this._device = dev;
		this._containerUrls = new Array();
	
		}
		
	function loadPlaylist(url:String) {
		_playlistXml = new XML(); 
		_playlistXml.ignoreWhite = true; 
		_playlistXml.onLoad = Delegate.create(this, onPlaylistLoad); 
		_playlistXml.load(url); 		
					
	}
	
	
	function onPlaylistLoad(success:Boolean):Void {
		
		for (var i=0;i<_playlistXml.childNodes.length;i++) {
			var cNode:XMLNode = _playlistXml.childNodes[i];
			if (cNode.nodeName ==  "Playlist") {
				scanPlayList(cNode);
			}
		}
		
		loadPackages();
			

	}
	
	private function scanPlayList(node:XMLNode) {
		
		for (var i=0;i<node.childNodes.length;i++) {
			var cNode:XMLNode = node.childNodes[i];
			if (cNode.nodeName == "Package") {
				_containerUrls.push(cNode.attributes.Url);
			}
		}
	}
		
	private function loadPackages() {
		
		if (_containerUrls.length > 0)
			loadPackage(String(_containerUrls.pop()));
		
		}
		
	private function loadPackage(url:String) {
		
		trace("Loading Package " + url);
		_packageXml = new XML(); 
		_packageXml.ignoreWhite = true; 
		_packageXml.onLoad = Delegate.create(this, onPackageLoad); 
		_packageXml.load(url); 		
		}
			

			
	function onPackageLoad(success:Boolean):Void {
		
		
		var newPackage:Package = new Package();
		
		for (var i=0;i<_packageXml.childNodes.length;i++) {
			var cNode:XMLNode = _packageXml.childNodes[i];
			if (cNode.nodeName == "Package") {
				newPackage.readXml(cNode);
			}
		}
		_device.addChild(newPackage);
		
		loadPackages();

	}
			

}

