function get_appropriate_ws_url(extra_url)
{
	var pcol;
	var u = document.URL;

	/*
	 * We open the websocket encrypted if this page came on an
	 * https:// url itself, otherwise unencrypted
	 */

	if (u.substring(0, 5) === "https") {
		pcol = "wss://";
		u = u.substr(8);
	} else {
		pcol = "ws://";
		if (u.substring(0, 4) === "http")
			u = u.substr(7);
	}

	u = u.split("/");

	/* + "/xxx" bit is for IE10 workaround */

	return pcol + u[0] + "/" + extra_url;
}

function new_ws(urlpath, protocol)
{
	return new WebSocket(urlpath, protocol);
}

document.addEventListener("DOMContentLoaded", function() {
	
	var ws = new_ws(get_appropriate_ws_url(""), "Beacon-Restream");
	ws.binaryType = "arraybuffer"; /* we don't want a blob */
    try {
			
		ws.onopen = function() {
		};
	
		ws.onmessage = function got_packet(msg) {
                    byte_buffer = new Uint8Array(msg.data);
                    size = msg.data.byteLength;
                    add_into_buffer(byte_buffer);
                };

		ws.onclose = function(){
		};
		
	} catch(exception) {
		alert("<p>Error " + exception);  
	}
	
}, false);


