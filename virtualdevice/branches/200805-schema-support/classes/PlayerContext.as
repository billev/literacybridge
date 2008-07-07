import mx.utils.Delegate;

class PlayerContext {

	var playStack:Array;
	var currentSound:Sound;
	var isPlaying:Boolean;
	var variables:Object;
	var baseVolume:Number;
	var map:ContainerMap;
	private var _xml:XML; 
	private var _pendingPlaylistUrl:String;
	
	private	var _device:Device;
	private var _currentPos:Position;
	
	function PlayerContext() {

	
		playStack = new Array();
		variables = new Object();
		this.map = new ContainerMap();
		currentSound = null;
		this.baseVolume = 100;
		_device = null;
		_currentPos = null;
		_pendingPlaylistUrl = null;
		isPlaying=false;

		}
		

		

	function getVariable(name:String):Number {
	
		//-- Handle predefined Flags
		if (name == "#isPlaying") {
			if (this.isPlaying)
				return 1;
			else
				return 0;
		}
		
		if (name == "#offset") {
			if (currentSound == null)
				return 0;
			else
				return currentSound.position;
		}
		
		if (name == "#volume") {
			return this.volume;
			
		}
		
				
	
		//-- Return code flag		
		var outVal:Number = this.variables[currentContainer.scopeName + "." + name];
		if (outVal == null)
			return 0;
		else
			return outVal;
		}
		
		
	function loadDevice(deviceUrl:String,playlistUrl:String) {
		this._pendingPlaylistUrl = playlistUrl;
		_xml = new XML(); 
		_xml.ignoreWhite = true; 
		_xml.onLoad = Delegate.create(this, onDeviceXmlLoad); 
		_xml.load(deviceUrl); 		
		

					
	}
	
	
	function onDeviceXmlLoad(success:Boolean):Void {
		
		var _device:Device = new Device();		
		for (var i=0;i<_xml.childNodes.length;i++)
			if (_xml.childNodes[i].localName=='Device')
				_device.readXml(_xml.childNodes[i]);

		this.device = _device;
		
		if (this._pendingPlaylistUrl != null)
			loadPlaylist(_pendingPlaylistUrl);
	}
	
	
	function loadPlaylist(url:String) {
		trace("Loading Playlist " + url);
		var pl:PlaylistLoader = new PlaylistLoader(_device);
		pl.loadPlaylist(url);
		
	}
	
	function setVariable(varName:String, val:Number) {
		
		switch (varName) {
			
			case "#volume" :
				this.volume = val;
				break;
				
			default:
				this.variables[currentContainer.scopeName + "." + varName] = val;
				
			}
		
		}


	function get currentContainer():Container {
	
		return _device.findChild(position);
		
		}
		
	function get volume():Number {
		return this.baseVolume;
		}
		
	function set volume(val:Number) {
		
		this.baseVolume = val;
		if (this.baseVolume < 0)
			this.baseVolume = 0;
		if (this.baseVolume>100)
			this.baseVolume = 100;
		
		if (currentSound != null)
			currentSound.setVolume(this.baseVolume);
		}
		
		
	function get device():Device {
		return _device;
		}
		
	function set device(dev:Device) {
		_device = dev;
		this.map.loadMap(_device);
		this.position = _device.startPosition;
		}

	
	function evaluateLocation(loc:Location):Position {
		
		var outPosition:Position = position.clone();
		
		var targetContainer:Container = currentContainer;
		
		// Evaluate Ref
		if (loc.ref != null) {
			var newTarg:Container = Container(this.map.getContainer(loc.ref));
			if (newTarg != null)
				targetContainer = newTarg;
			}
		
		
		// Evaluate Target
		switch (loc.target)  {
			case TargetType.Next:
				targetContainer = currentContainer.next;
				break;
			case TargetType.Previous:
				targetContainer = currentContainer.previous;
				break;
			case TargetType.Beginning:
				outPosition.offset = 0;
				break;
			case TargetType.End:
				if (currentSound != null)
					outPosition.offset = currentSound.duration;
				break;
			}
		if (targetContainer != currentContainer)
			outPosition = targetContainer.startPosition;
		

		// Evaluate Offset
		if (loc.offset!=0) {
			outPosition.offset = Number(outPosition.offset) + Number(loc.offset);
			if (outPosition.offset < 0)
				outPosition.offset = 0;
			}

		return outPosition;
		
		}
		
		
	function get position():Position {
		
		if (isPlaying) 
			return new Position(_currentPos.url,currentSound.position);
		else 
			return _currentPos;

		}
		

		
	function set position(pos:Position) {
		
		if (currentSound!=null)
			currentSound.stop();
			
		var oldUrl = _currentPos.url;
		this._currentPos = pos.clone();
			
		if (_currentPos.url != oldUrl) {
		
			// Load New Sound
			currentSound = new Sound(_root);
			currentSound.onSoundComplete = function() {
				_root.addLog("Handling Finish");
				_root._playerContext.currentContainer.handleOnFinish(_root._playerContext);
				}
			if (isPlaying) {
				_root._newPos = _currentPos.clone();
				currentSound.onLoad = function(success) {
					if (_currentPos.offset == 0)
						_root._playerContext.currentContainer.handleOnStart(_root._playerContext);
					_root._playerContext.position = _root._newPos;
					}
			} else {
				currentSound.onLoad = function(success) {this.stop();}
				currentSound.stop();
				}
			currentSound.loadSound(_currentPos.url,true);
			currentSound.setVolume(this.baseVolume);
			
		} else {
		
			// restart in new location
			if (isPlaying) {
				_root.addLog("Starting");
				currentSound.start(_currentPos.offset / 1000);
				}
		
			}

			
		}

	function playPause() {

		if (isPlaying) {
			pause();
		} else {
			play();
			}

		}
		
		
	function setLight(light,mode) {
		
		var target = null;
		if (light == LightColorType.Green) {
			target = _root.greenLight;
			}
		if (light == LightColorType.Red) {
			target = _root.redLight;
			}
			
		if (target == null)
			return;
			
		target.setLightMode(mode);
		
		}
	
	function pause() {
		
		if (isPlaying) {
			_currentPos = position;
			currentSound.stop();
			isPlaying = false;
			}
	
		}
			
	function play() {

		if (!isPlaying) {
			isPlaying = true;
			this.position = _currentPos;
			} 
	
		}

	function popStack():Boolean {
		
		if (playStack.length==0)
			return false;

		_root.addLog("Stack: Popping Position");
		var newPos:Position = Position(playStack.pop()); 
		_root.addLog(newPos.offset);
		position = newPos.clone();
		return true;
		

		}

	function pushStack(pos:Position) {

		_root.addLog("Stack: Pushing Position");
		playStack.push(pos.clone());

		}

	function clearStack() {
	
		playStack = new Array();
		
		}

		
	function handleButton(evt:ButtonEventArgs) {
	
		this.currentContainer.handleButton(this,evt);
		
		}
	

	}



