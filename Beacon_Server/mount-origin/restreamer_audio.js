      let f;
      // Create AudioContext and buffer source
      let audioCtx;
      let source;
      // create the buffer. this is an array of 12 channels (actually 6 channels of I and Q data)
      //  each of which has a 10 deep buffer each of which has 4096 points in a Float32Arrays. filled with 0.0 to start
      let audioBuffer = new Array(12);
      for (var j = 0; j < 12; j++) {
          audioBuffer[j] = new Array(10);
          for (var i = 0; i < 10; i++) {
              audioBuffer[j][i] = new Float32Array(4096); // make each element an array
          }
      }
      let in_pointer = 0;
      let out_pointer = 0;
      let buffer_chunk = 0;

      // each input byte buffer of 4097 bytes contains radio client number followed by 1024 S16 stereo samples (IQ sample)
      //  I[0] = byte[0] + 256 * byte[1]
      //  Q[0] = byte[2] + 256 * byte[3]    and so on.
      // we need to extract this into two audio output buffers and convert to float32 as we go
      // each output buffer is 4096 floats. we have 2 buffers, one for left and one for right (I and Q)
      function add_into_buffer(byte_buffer) {
          var AudioGain = parseFloat(document.getElementById("AudioGain").value);
          /* deduce which array to put data into - radio client number * 2 as we need I and Q arrays */
          let radio_client = (byte_buffer[0] - 1) * 2;
          for (
              let posn = 0;
              posn < 4096;
              posn += 4
          ) {
              if ((posn==0) && (buffer_chunk==0)) {
                  audioBuffer[radio_client    ][in_pointer] = new Float32Array(4096); // make each element an array
                  audioBuffer[radio_client + 1][in_pointer] = new Float32Array(4096); // make each element an array
              }
              L = byte_buffer[posn + 1] + 256 * byte_buffer[posn + 2]; 
              R = byte_buffer[posn + 3] + 256 * byte_buffer[posn + 4];
              if (L > 32767) L -= 65536; 
              if (R > 32767) R -= 65536; 
              audioBuffer[radio_client    ][in_pointer][posn/4 + 1024 * buffer_chunk] = L / 32768 * AudioGain; 
              audioBuffer[radio_client + 1][in_pointer][posn/4 + 1024 * buffer_chunk] = R / 32768 * AudioGain; 
          }
          // move on to the next chunk and wrap if needed
          buffer_chunk++;
          if (buffer_chunk == 4) {
              buffer_chunk = 0;
              in_pointer++;
              if (in_pointer == 10) {
                  in_pointer = 0;
              }
          }
      }

      function init() {
          audioCtx = new AudioContext({sampleRate: 7812});
          source = audioCtx.createBufferSource();
          f = 0.0;
          // Create a ScriptProcessorNode with a bufferSize of 4096, no input and two output channel
          let scriptNode = audioCtx.createScriptProcessor(4096, 0, 12);
          console.log("Init", scriptNode.bufferSize);
  
          // we start with the output pointer half way through the buffer (i.e add 5)) we may need to wrap it
          out_pointer = in_pointer + 5;
          if (out_pointer > 9) {
              out_pointer -= 10;
          }

          // Give the node a function to process audio events
          // this is called each time new audio data is required
          // we handle this by filling the audio buffer from data we have stored in our circular buffer 
          scriptNode.onaudioprocess = function (audioProcessingEvent) {
              console.log("Audio Process", scriptNode.bufferSize, in_pointer, out_pointer);

              // The output buffer contains the samples that will be modified and played
              let outputBuffer = audioProcessingEvent.outputBuffer;

              // Loop through the output channels
              let outputData_0I = outputBuffer.getChannelData(0);
              let outputData_0Q = outputBuffer.getChannelData(1);
              let outputData_1I = outputBuffer.getChannelData(2);
              let outputData_1Q = outputBuffer.getChannelData(3);
              let outputData_2I = outputBuffer.getChannelData(4);
              let outputData_2Q = outputBuffer.getChannelData(5);
              let outputData_3I = outputBuffer.getChannelData(6);
              let outputData_3Q = outputBuffer.getChannelData(7);
              let outputData_4I = outputBuffer.getChannelData(8);
              let outputData_4Q = outputBuffer.getChannelData(9);
              let outputData_5I = outputBuffer.getChannelData(10);
              let outputData_5Q = outputBuffer.getChannelData(11);

              // Loop through the 4096 samples copying the data from the audio buffer into the outputBuffer
              for (let sample = 0; sample < outputBuffer.length; sample++) {
//                if (channel==1) {
//    	              outputData[sample] = Math.sin(f)* 0.1;
//	              f+=0.25;
//	              if (f>2*Math.PI) {f-=2*Math.PI;}
//	          } else {
//	              outputData[sample] = (Math.random() * 2 - 1) * 0.1;
//	          }
                  outputData_0I[sample] = audioBuffer[0 ][out_pointer][sample];
                  outputData_0Q[sample] = audioBuffer[1 ][out_pointer][sample];
                  outputData_1I[sample] = audioBuffer[2 ][out_pointer][sample];
                  outputData_1Q[sample] = audioBuffer[3 ][out_pointer][sample];
                  outputData_2I[sample] = audioBuffer[4 ][out_pointer][sample];
                  outputData_2Q[sample] = audioBuffer[5 ][out_pointer][sample];
                  outputData_3I[sample] = audioBuffer[6 ][out_pointer][sample];
                  outputData_3Q[sample] = audioBuffer[7 ][out_pointer][sample];
                  outputData_4I[sample] = audioBuffer[8 ][out_pointer][sample];
                  outputData_4Q[sample] = audioBuffer[9 ][out_pointer][sample];
                  outputData_5I[sample] = audioBuffer[10][out_pointer][sample];
                  outputData_5Q[sample] = audioBuffer[11][out_pointer][sample];
              }
          
              // move the output buffer on one once we have extracted the data (and wrap if needed)
              out_pointer++;
              if (out_pointer == 10) {
                  out_pointer = 0;
              }
          };

          source.connect(scriptNode);
          scriptNode.connect(audioCtx.destination);
          source.start();

          // When the buffer source stops playing, disconnect everything
          source.onended = function () {
              source.disconnect(scriptNode);
              scriptNode.disconnect(audioCtx.destination);
          };
      }

      document.addEventListener("DOMContentLoaded", function() {
          var ele = document.getElementById("playButton");
          ele.addEventListener("click", init);
      });


