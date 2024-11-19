# Meteor Beacon Server Software Architecture

## Introduction
Six Radio Clients, spread around the country, receive radio signals from a central transmitter (in Sherwood Observatory), rate reduce, down convert and transmit IQ samples to a central server in Goonhilly Earth Station. From this data the server performs Fourier transforms (via the Goertzel algorithm) to create 6 waterfalls displays. Clients can view these in real time via the gb3mba.html page. Transformed data is transferred to the clients via a websockets interface.

Also, when the server receives a data packet from a client, it re-streams this out to another page (restream.html), also via websockets. This page will collect data from all streams, convert it into an audio stream (12 channels, I and Q for each of the 6 streams), and present it to the Client’s PC via the browser audio playback mechanism.

## key info
http://185.83.169.27:7682/gb3mba.html – this is the waterfall display page direct from the server. Note we use port 7682 for the outgoing websocket pages.

http://185.83.169.27:7682/restream.html – this is the re-streamer access.
data rate from each receiver is 7812 16 bit I and Q samples per second.

Data comms from the radio clients to the server are via port 7680.

## main.c
In main we create a display thread that will create new threads for any incoming stream and will display data from those threads. We then create the websocket which never returns so the websocket is running in the main thread.

Data format from Radio Client to Server
Data is sent as 4096 + 32 bytes, first 32 bytes are identification codes starting with the string ‘Heather’ for the first 7 bytes then 0, then a client code (currently 1 to 6, denoted as C below). The remaining 23 bytes of the header are all zero. This is followed by 4096 data bytes which represent 1024 samples stored as I[0:7], I[8:15], Q[0:7], Q[8:15] (I.e. 16 bit sample sent as LSByte first).

‘H’ ‘e’ ‘a’ ‘t’ ‘h’ ‘e’ ‘r’ 0x00 C 0x00 … 0x00

We have now added a feature to program the title and location into the header section. Instead of the above “Heather” string etc. we now define the first 8 bytes as:

‘H’ 0b101ddddd dddmmmmm mmmmssss sssssDDD DDDDDDMM MMMMMMMS SSSSSSSS C T1 T2 … T23

Where d is the 8 bit latitude Degrees (degrees latitude = dddddddd - 90), then m is 9 bit minutes, s is 9 bit seconds, D is 9 bit longitude degrees (degrees longitude = D - 180) followed by 9 bit longitude minutes (M) and seconds (S). Note the 2nd byte has first 3 bits as 0b101.

We then have the client code (C) stored in the 9th byte as with the old mechanism.

The remaining 23 bytes of the header (T1 to T23) contain the title for waterfall display (sent as a string, no null termination needed).

#### display thread
Here we listen for new connections and if we find one, we create a new client thread for it, then look through all the possible connections and check to see if data is ready to display. If it is ready then we copy the data into the transmit buffer and send it to the waterfall clients via the websocket 

  lws_callback_on_writable_all_protocol(context, protocol);

#### client thread
Here we check for a timeout and kill everything if it has timed out. Then we see if there is data to read in, if there is then we read it into a circular buffer. Once we have a full buffer we re-stream the data out for analysis via

  lws_callback_on_writable_all_protocol(context, protocol_restreamer);

Note that the client machine receiving re-streamed data) will receive all the buffers that arrive at the server, pretty much in random order, so they need to be unpacked at the other end of the websocket (client).

After re-streaming, we fill in the I and Q buffers, perform hamming window, wait for the display to process the last data set and then goertzel it. Finally we tell the display thread it can display the results.

## websocket_server.c
This sets up some structures, creates a sig trap for ctrl-c etc. 

In websocket create we set up our list of protocols (one for the waterfall display and another for the restreamer), set up the port, mount location (where the html files live – currently mount-origin) and then create the context. We then go into an infinite loop servicing the web sockets and checking for a ctrl-c to get us out and end the whole program.

## protocol.c
This handles the normal waterfall streaming out to displays. When something needs to happen, we get a callback into callback_beacon_server. The main action in here is on writeable, where we send out the data

## protocol_restream.c
This is the bit where we send all the incoming data back out to a client who will do data analysis.

## gb3mba.html
Here we draw the screen, formatting the waterfalls etc. Note that the js files are included here at the top level. We also set up the web sockets via gb3mba.js

## gb3mba.js
Most of the work is done by an event listener that runs when the DOM is loaded. Importantly this creates the websocket with the appropriate protocol (by name) so that the server executes the correct protocol, in this case Beacon-Server is the protocol for the waterfall display. This creates an array, looks at the incoming data and chooses which display to add the line of waterfall info into. 

## restream.html
This will serve a web page that gets all the server incoming data and outputs it on the client machine as a 12 channel audio stream (6 streams of I and Q). Because auto play audio is not allowed in the DOM model, we have to have a button to press that allows us to start the audio stream by hand.

## restream.js
Here we just set up the correct protocol and call into add_into_buffer() once we have a set of data.

Data Format of re-streamed data
4097 bytes, first byte is the radio number (1 to 6), then 4096 bytes representing 1024 samples stored as I[0:7], I[8:15], Q[0:7], Q[8:15] (I.e. 16 bit sample sent as LSByte first).

## restreamer_audio.js
This is where we set everything up when the user clicks the start button which calls init(). The audio is stored as 12 channels (6 radios with I and Q). Each channel has a 10 deep buffer to allow audio buffering of the information, each of these has 4096 data points (floats) for the actual audio data.

In init we create an audio context based on the correct sample rate, and an output node with a buffer size of 4096 samples for all 12 channels.

In add_into_buffer (called from restreamer.js) we receive a data set for one radio stored as 1024 I, Q interleaved 16bit samples, these we extract, convert to floats and add into the correct buffer. It takes 4 such input data sets to fill one radio’s output audio buffer as each audio channel (I/Q) has 4096 samples and we only send 1024 samples each data set).

When we get a request for more data from the audio system, we copy out the data from the buffering buffers into the audio output buffers. We extract data from buffer 5 first so that we have a full 5 buffers worth of audio data to overcome internet and PC latency.

## fft.js
Once we have suitable amounts of data collected by gb3mba.js we then have to display it as a waterfall. This is done 
Note this file also contains an oscilloscope type function which is currently unused.

## make_pretty.js
This file has a global script (that runs at startup) that grabs the context of each of the 6 waterfalls and calls a function to create the starting image for each one.

This function creates the blue box for no signal, labels it and adds the scale. Note that it does not do the titles for each box or the location – these are added in fft.js when they input stream changes.



