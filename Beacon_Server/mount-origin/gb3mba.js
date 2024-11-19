var float_buffer;
let colourMap;
let wf;
let sp;
let blocking_flag;

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
	
	blocking_flag=0;
        var ws = new_ws(get_appropriate_ws_url("beaconclient/"), "Beacon-Server");
	ws.binaryType = "arraybuffer"; /* we don't want a blob */
        try {
		ws.onopen = function() {
		};
	
		ws.onmessage = function got_packet(msg) {
                while (blocking_flag==1) {
                   console.log("Blocking");
                };
                blocking_flag=1;
                var byte_buffer = new Uint8Array(513); //msg.data); //Float32Array(msg.data);
                var tmp_buf = new Uint8Array(msg.data); //Float32Array(msg.data);
                for (let i=0;i<512;i++) {
                //for (let i=0;i<513;i++) {
                   byte_buffer[i+1]=tmp_buf[i+32];
                //   byte_buffer[i]=tmp_buf[i];
                }
                byte_buffer[0]=tmp_buf[8];
                //if (msg.data.byteLength!=513) console.log("bad len "+msg.data.byteLength);
                if (msg.data.byteLength!=544) console.log("bad len "+msg.data.byteLength);
                     if (byte_buffer[0] == 0x01) wf1.addLine(0,byte_buffer);
                else if (byte_buffer[0] == 0x02) wf2.addLine(1,byte_buffer);
                else if (byte_buffer[0] == 0x03) wf3.addLine(2,byte_buffer);
                else if (byte_buffer[0] == 0x04) wf4.addLine(3,byte_buffer);
                else if (byte_buffer[0] == 0x05) wf5.addLine(4,byte_buffer);
                else if (byte_buffer[0] == 0x06) wf6.addLine(5,byte_buffer);
                else console.log("bad client number "+byte_buffer[0]);
                //draw_pretty();
                //console.log("client number= "+byte_buffer[0]+" first= "+byte_buffer[1]);
                blocking_flag=0;
                }

		ws.onclose = function(){
		};
	} catch(exception) {
		alert("<p>Error " + exception);  
	}
	
        colourMap = new ColourMap();
        wf1 = new Waterfall("waterfall1", colourMap);
        wf2 = new Waterfall("waterfall2", colourMap);
        wf3 = new Waterfall("waterfall3", colourMap);
        wf4 = new Waterfall("waterfall4", colourMap);
        wf5 = new Waterfall("waterfall5", colourMap);
        wf6 = new Waterfall("waterfall6", colourMap);

//  sp = new Spectrum("spectrum", colourMap);
}, false);

