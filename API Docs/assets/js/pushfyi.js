/*!
*********************************************************
        This code is a proprietary of PushFYI Inc

Following may not be used in part on in whole without the
prior permission of PushFYI Inc

Author: Rahul Dey
Date: 20/09/2016
Purpose: PUSH FYI JAVASCRIPT API

API VERSION : 0.3b
*********************************************************
*/
(function() {
	/*!
	 * Pushfyi constructor
	 */ 
    this.Pushfyi = function(l, a, c) {
	  
		this.license = l;	//License key
	
		this.appsecret = a;	//App secret
	
		this.cluster = c;	//Cluster to connect
	
		this.websocket = null;	//Web socket
	
		this.queue = [];	//Out Queue

		this.subscription_callbacks = {};	//subscription callbacks

		this.status_callbacks = {};     //status callbacks

		/*!
		 * Make connection
		 */
		if(testWSS.call(this) == true) {
	 
		}
		authenticate.call(this);
		connect.call(this);
    }
    /*!
	 *	Browser Compatibility test
	 */    
    function testWSS() {
		if ("WebSocket" in window) {
			return true;
		}
		console.log("Your browser doesnt support websockets");
		return false;
	}
	/*!
	 *	Initialize the pushfyi_websocket
	 */
	function connect() {
		this.websocket = new WebSocket(this.cluster, "websocket");	
						
		if( (this.websocket == null) || 
				(this.websocket.readyState === 3) || 
				(this.websocket.readyState === 2) ) {
					console.log("<pushfyi>: connection failed");
		} else {
			this.websocket.onclose = close.bind(this);
			this.websocket.onmessage = incoming.bind(this);
			wait.call(this);
		}		
	}
	/*!
	 *	Close the connection
	 */
	function close() {
	    console.log("<pushfyi>: connection closed");
	    statusChange.call(this, "disconnected");
	}

	/*!
     * Callback processor method
     */
	function incoming(evt) {
    	var event = xmlToDom(evt.data);
    	var params = event.getElementsByTagName("params");

    	//find the channel and call the callback
    	var channel = getChannel(event);
    	this.subscription_callbacks[String(channel)](params[0]);		
	}

	/*!
     * wait loop during connection is established to the pushfyi cluster
     */
	function wait() {
	    setTimeout(
	        function () {
				if (this.websocket.readyState === 2 || this.websocket.readyState ===3) {
					console.log("<pushfyi>: Could not connect.");
					return;
				} 
		        if (this.websocket.readyState === 1) {
			        console.log("<pushfyi>: connection open");
			        process.call(this);
			        statusChange.call(this, "connected");
		        }  
				
				else {
				    console.log("<pushfyi>: connecting");	//connection is in progress
					wait.call(this);
				}
	        }.bind(this), 5);	// wait 5 milisecond for the connection...		
	}
	/*!
	 * Validate License and Appsecret keys
	 */
	function authenticate() {
		var publication = toPushfyiMessage("pushfyi", "authentication", 
					{'license': this.license, 
					 'appsecret': this.appsecret}, "hello");
		
		this.queue.push(publication);
	}
	
	/*!
	 * Send to pushfyi websocket
	 */
    function send (data) {
		//check the connection progress
		if( (this.websocket !== null) && (this.websocket.readyState === 0) ) {
			console.log("connection is in progress. Can not send at this moment");
		}
		else if( (this.websocket == null) || 
					(this.websocket.readyState === 3) || 
					(this.websocket.readyState === 2) ) {
			console.log("connection is in progress. Can not send at this moment");		
		} 
		else if (this.websocket.readyState === 1) {
			this.websocket.send(data);
		}
    }


    /*!
     * Convert message to Push FYI XML Format
     * @param params this is a JSON string of input key value pairs
     */
    var toPushfyiMessage = function (topic, subtopic, params, command) {
	var event = "<ims-event>\n";
        if(topic) {
            event += "<topic>"    + topic    + "</topic>\n";
        }
        if(subtopic) {
            event += "<subtopic>" + subtopic + "</subtopic>\n";
        }
        event += "<params>\n";
 
        if(params) {
            if (typeof(params) == "object") {
		//this should be JSON
		//var obj = JSON.parse(params);
		var obj = params;
		for(var key in obj) {
		    if(obj.hasOwnProperty(key)) {
		        var val = obj[key];
		        event += '<var name="' + key + '">' + val + '</var>\n';
		    }
		}
	    }
            else if (typeof(params) == "string") {
		//optional. should be discouraged.
                if(params.indexOf('<var ') === 0) {
	            //might already be encoded. do nothing.
                    event += params;
                }
                else {
                    var args = {};
                    args = params.split('|$|');
                    for (var i = 0; i < args.length; i++) {
                        var arg = args[i].split(':=');
                        if (arg.length == 2) {
                            event += '<var name="' + arg[0] + '">' + arg[1] + '</var>\n';
                        }
                    }
                }
            }
        }

	event += '<var name="command">' + command + '</var>\n';
        event += "</params>\n</ims-event>";
        return event;
    }
    
    /*!
     * Convert XML string to DOM for parsing
     */
    function xmlToDom(xml) {
        if(DOMParser) {
	    var dom = new DOMParser();
	    var doc = dom.parseFromString(xml, "text/xml");
	    return doc;
        } else {
	    alert("your browser does not support xml parsing");
	    return null;
        }
    }

    function getChannel(event) {
    	var sub = event.getElementsByTagName("subtopic");
    	return sub[0].firstChild.nodeValue;
    }    
    
    function process() {
		while(this.queue.length != 0) {
			var event = this.queue.shift();
			if(event) {
				console.log("<pushfyi>: sending");
				send.call(this, event);
			}
		}
	}

    /*!
     * Subscribe for a topic and subtopic
     */
    Pushfyi.prototype.subscribe = function (event, params, callback) {
		if(typeof callback === 'function') {
			var subscription = toPushfyiMessage(this.appsecret, event, 
									params, "subscribe");
			console.log("subscribing to event = " + event + " appsecret = " + this.appsecret);
			this.queue.push(subscription);
			this.subscription_callbacks[event] = callback;
			
			if(this.websocket.readyState === 1) {
				process.call(this);
			}			
		}
		else
			console.log('<pushfyi> invalid subscription callback.');		
	}
	
    /*!
     * Publish a message to the data channel on the Push FYI Cluster
     */
    Pushfyi.prototype.publish = function(event, params) {
		var publication = toPushfyiMessage(this.appsecret, event, 
									params, "publish");
		console.log("publishing event = " + event);
		this.queue.push(publication);
			
		if(this.websocket.readyState === 1) {
			process.call(this);
		}
    }

    /*!
     * Unsubscribe for a message from the specified data channel
     */
    Pushfyi.prototype.unsubscribe = function(event) {
		var unsubscription = toPushfyiMessage(this.appsecret, event, 
							null, "unsubscribe");
		this.queue.push(unsubscription);
		
		if(this.websocket.readyState === 1) {
			process.call(this);
		}										
    }

    /*!
     * Extract key value pair
     */
    Pushfyi.prototype.getParam = function (params, name)
    {
        var vars = params.getElementsByTagName("var");
        for (var i = 0; i < vars.length; i++) {
            var param = vars[i];
            var key   = param.getAttribute("name");

            if (key == name &&		
                param.firstChild) {
                if(param.firstChild.nodeName=='#text'){
                    return param.firstChild.nodeValue;
                }
                else {
                    return param.firstChild;
                }
            }
        }
        return "";
    }
	
    //experimental
    /*!
     * Subscribe for a topic and subtopic
     */
    Pushfyi.prototype.on = function (event, callback) {
        if (typeof callback === 'function') {
            if(event === "connected" || event === "disconnected")
                this.status_callbacks[event] = callback;
            else
                console.log('<pushfyi> invalid event.');
        }
        else
            console.log('<pushfyi> invalid status callback.');
    }

    function statusChange(status) {
        var cb = this.status_callbacks[status];

        if(typeof cb === 'function')
            cb();
    }

}());

