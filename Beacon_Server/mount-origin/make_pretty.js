function make_pretty(ctx, name, location) {
  const indent_x=30;
  const indent_y=75;
  //make it look prety
  //ctx.fillStyle = "#000000";
  //ctx.fillRect(indent_x-25,indent_y-45,512+10+40,494);
  // default to blue for actual fft area
  ctx.fillStyle = "#0000ff";
  ctx.fillRect(indent_x,indent_y,512,300);

  //all lines are going to be in white
  ctx.fillStyle = "#FFFFFF";
  ctx.font = "32px Arial";
  ctx.fillText("No Signal", indent_x+180, indent_y+160);
  // Define a new Path:
  ctx.beginPath();
  ctx.strokeStyle = "white";
  ctx.font = "10px Arial";
  var x_offset;

  for (let x=-100; x<=100; x+=20) {
    // Define a start Point
    ctx.moveTo(indent_x+257+256/512*x*5, indent_y+300);
    // draw a line
    ctx.lineTo(indent_x+257+256/512*x*5, indent_y+315);
    // label the point on the x axis
    if (x==0) x_offset=-3; 
  //  else if (x==-5000) x_offset=-3;
  //	else if (x==5000) x_offset=-20;
      else x_offset=-10;
    ctx.fillText(x, indent_x+256+x_offset+256/512*x*5, indent_y+325);
  }

  // Stroke it (Do the Drawing)
  ctx.stroke();
  // fill in centre freq
  ctx.fillText("50.408 MHz", indent_x+230, indent_y+338);

}

    const canvas1 = document.getElementById("waterfall1");
    const ctx1 = canvas1.getContext("2d");
    make_pretty(ctx1, "Norman Lockyer Observatory", "50.688296N, 3.218986W");

    const canvas2 = document.getElementById("waterfall2");
    const ctx2 = canvas2.getContext("2d");
    make_pretty(ctx2, "nr Armagh, IO64RH", "");

    const canvas3 = document.getElementById("waterfall3");
    const ctx3 = canvas3.getContext("2d");
    make_pretty(ctx3, "NW Hampshire V Pol", "51.248083N, 1.572W");

    const canvas4 = document.getElementById("waterfall4");
    const ctx4 = canvas4.getContext("2d");
    make_pretty(ctx4, "NW Hampshire H Pol", "51.248083N, 1.572W");

    const canvas5 = document.getElementById("waterfall5");
    const ctx5 = canvas5.getContext("2d");
    make_pretty(ctx5, "Nr. Malvern, IO82UC", "");

    const canvas6 = document.getElementById("waterfall6");
    const ctx6 = canvas6.getContext("2d");
    make_pretty(ctx6, Titles.w6, "52.556708N, 2.664975W");
//
//                /* location is stored as 8 bits for latitude degrees (lat = val - 90), 9 bits for mins, 9 bits for secs, */
//                /* then similar for long but starting with 9 bits for long degress (lon = val - 180)                       */
//                /* so 53 bits in total. 7 bytes. Starting from the 3rd bit of the second byte                             */
//                //lat_d = ((int16_t)(in_dat[i][1] & 0x1f) << 3) | ((uint16_t)(in_dat[2] & 0xe0) >> 5);
//                lat_m = ((int16_t)(in_dat[i][2] & 0x1f) << 4) | ((uint16_t)(in_dat[3] & 0xf0) >> 4);
//                lat_s = ((int16_t)(in_dat[i][3] & 0x0f) << 5) | ((uint16_t)(in_dat[4] & 0xf8) >> 3);
//                lon_d = ((int16_t)(in_dat[i][4] & 0x07) << 6) | ((uint16_t)(in_dat[5] & 0xfc) >> 2);
//                lon_m = ((int16_t)(in_dat[i][5] & 0x03) << 7) | ((uint16_t)(in_dat[6] & 0xfe) >> 1);
//                lon_s = ((int16_t)(in_dat[i][6] & 0x01) << 8) | ((uint16_t)(in_dat[7]       )     );
//                lat = ((double)lat_d-90)  + ((double)lat_m/60) + ((double)lat_s/60/60);
//                lon = ((double)lon_d-180) + ((double)lon_m/60) + ((double)lon_s/60/60);
