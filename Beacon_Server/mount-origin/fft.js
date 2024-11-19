'use strict';
let w_blocking;

let itles = [
  "Normal Lockyer Observatory",
  "nr. Armagh, IO64RH",
  "NW Hampshire V Pol",
  "NW Hampshire H Pol",
  "Nr. Malvern, IO82UC",
  "Test Stream"
];

let cations=[
  "50.688296N, 3.218986W",
  "",
  "51.248083N, 1.572W",
  "51.248083N, 1.572W",
  "",
  "52.556708N, 2.664975W"
];


function Spectrum(spectrumCanvasId, colourMap)
{
    this.width = document.getElementById(spectrumCanvasId).clientWidth;
    this.height = document.getElementById(spectrumCanvasId).clientHeight;

    this.ctx = document.getElementById(spectrumCanvasId).getContext("2d");

    this.map = colourMap;
    this.updateData = function(data)
    {
      var dataLength = 300;//data.length;
      var i;
      var sample_index;
      var sample_index_f;
      var sample;
      var sample_fraction;

      this.ctx.clearRect(0, 0, this.width, this.height);

      for(i=0; i<this.width; i++)
        {
          sample_index = (i*dataLength)/ this.width;
          sample_index_f = sample_index | 0;
          sample = data[sample_index_f]
             + (sample_index - sample_index_f) * (data[sample_index_f+1] - data[sample_index_f]);
          sample_fraction = sample / 65535;
          sample = (sample * (256.0 / 65536)) | 0;
          this.ctx.fillStyle = "rgba("+this.map[sample][0]+","+this.map[sample][1]+","+this.map[sample][2]+",255)";
          this.ctx.fillRect(i, this.height-(sample_fraction * this.height), 2, 2);
        }
    };
}

const waterfall_newcolour =  [
[0,27,32],
[0,29,37],
[0,29,39],
[0,30,40],
[0,31,41],
[0,32,42],
[0,33,43],
[0,34,46],
[0,35,48],
[0,35,49],
[0,36,52],
[0,37,53],
[0,38,55],
[0,38,57],
[0,40,59],
[0,41,61],
[0,42,64],
[0,42,65],
[0,43,68],
[0,43,70],
[0,44,72],
[0,45,74],
[0,46,76],
[0,46,79],
[0,47,81],
[0,47,84],
[0,48,86],
[0,48,88],
[2,49,91],
[4,50,91],
[6,50,95],
[9,50,98],
[11,49,100],
[13,50,102],
[17,50,104],
[19,51,107],
[21,52,109],
[24,52,112],
[28,52,116],
[29,51,118],
[32,53,120],
[33,52,121],
[35,52,124],
[39,53,126],
[40,52,131],
[42,52,132],
[45,52,133],
[47,52,134],
[50,52,137],
[51,53,139],
[55,52,142],
[57,51,144],
[59,50,146],
[61,51,148],
[64,51,150],
[67,50,153],
[69,50,155],
[70,50,155],
[75,49,157],
[76,48,159],
[79,49,160],
[81,48,161],
[83,48,164],
[85,47,166],
[87,46,166],
[90,46,167],
[92,45,168],
[96,46,171],
[97,45,172],
[99,43,172],
[102,43,172],
[104,43,173],
[107,42,175],
[110,42,177],
[112,42,177],
[113,41,177],
[114,40,177],
[117,40,178],
[119,39,178],
[122,39,179],
[124,39,179],
[126,39,178],
[129,38,178],
[130,37,178],
[133,37,179],
[134,37,178],
[137,37,179],
[139,36,179],
[141,36,180],
[142,36,178],
[145,36,179],
[147,36,177],
[148,36,178],
[152,35,177],
[153,35,176],
[155,36,176],
[158,35,175],
[159,36,173],
[161,36,172],
[162,36,171],
[164,36,171],
[166,37,169],
[168,37,167],
[170,37,167],
[171,38,166],
[172,38,166],
[175,39,164],
[177,40,162],
[179,40,161],
[180,41,159],
[181,42,157],
[183,42,156],
[185,43,156],
[187,43,155],
[190,44,153],
[192,46,152],
[193,46,150],
[193,47,147],
[194,47,146],
[197,49,145],
[199,49,143],
[200,50,141],
[201,51,140],
[202,52,138],
[204,53,136],
[206,55,136],
[208,55,134],
[209,57,132],
[210,59,130],
[212,59,129],
[213,59,128],
[214,61,126],
[215,63,124],
[217,65,122],
[218,65,121],
[219,66,120],
[220,67,117],
[221,68,112],
[222,70,111],
[225,71,111],
[226,72,110],
[227,74,108],
[228,75,106],
[229,77,104],
[230,79,102],
[231,80,99],
[232,81,98],
[233,82,95],
[234,83,94],
[235,84,93],
[236,85,90],
[237,87,89],
[238,88,89],
[239,89,88],
[240,91,84],
[241,93,82],
[242,94,81],
[243,95,80],
[243,96,78],
[244,97,77],
[245,99,74],
[246,101,72],
[247,104,70],
[248,106,69],
[249,107,67],
[250,109,66],
[250,109,64],
[250,111,62],
[251,113,60],
[252,115,58],
[252,116,56],
[252,118,55],
[253,120,53],
[254,123,51],
[255,125,50],
[255,126,49],
[255,128,47],
[255,129,46],
[255,130,44],
[255,133,42],
[255,135,41],
[255,138,40],
[255,139,36],
[255,140,36],
[255,142,36],
[255,144,36],
[255,146,35],
[255,150,35],
[255,152,34],
[253,152,34],
[254,153,35],
[253,156,35],
[253,161,36],
[252,160,37],
[251,164,38],
[250,165,39],
[250,168,40],
[249,170,42],
[248,173,44],
[247,174,46],
[247,176,50],
[246,178,53],
[245,179,56],
[245,182,58],
[244,186,60],
[243,187,64],
[241,189,67],
[240,190,69],
[240,192,73],
[239,194,75],
[237,196,78],
[236,199,82],
[234,201,85],
[235,203,92],
[235,205,93],
[233,207,97],
[231,208,92],
[231,211,106],
[230,212,112],
[229,213,115],
[228,216,120],
[228,216,124],
[227,218,127],
[226,220,132],
[226,221,137],
[224,221,140],
[224,222,145],
[224,224,150],
[225,226,156],
[225,228,159],
[224,231,164],
[223,232,167],
[223,233,171],
[225,234,177],
[227,235,183],
[227,236,186],
[228,237,190],
[229,238,195],
[230,238,197],
[231,239,203],
[233,240,207],
[234,241,210],
[236,242,216],
[237,242,220],
[238,243,223],
[240,245,225],
[241,245,230],
[242,246,232],
[245,247,236],
[246,247,239],
[247,248,243],
[248,248,245],
[251,250,248],
[253,252,252],
[255,255,255],
[255,255,255]
];


function Waterfall(canvasFrontId, colourMap)
{
    this.width = document.getElementById(canvasFrontId).clientWidth-60;
    this.height = document.getElementById(canvasFrontId).clientHeight-100;
    this.map = colourMap;

    this.imgFront = document.getElementById(canvasFrontId);
    this.ctxFront = this.imgFront.getContext("2d");

    this.lineImage = this.ctxFront.createImageData(this.width, 1);

    this.addLine = function(clientId, fft_dat)
    {
      while (w_blocking==1) {
          console.log("Waterfall blocking");
      }
      w_blocking=1;

      var imgdata = this.lineImage.data;
      var lookup = 0;
      var i = 0;

      for (lookup = 0; lookup < 512; lookup++)
      {
        var rgb = this.map[fft_dat[lookup] | 0];
//        imgdata[i++] = rgb[0];
//        imgdata[i++] = rgb[1];
//        imgdata[i++] = rgb[2];
        imgdata[i++] = waterfall_newcolour[fft_dat[lookup]] [0];
        imgdata[i++] = waterfall_newcolour[fft_dat[lookup]] [1];
        imgdata[i++] = waterfall_newcolour[fft_dat[lookup]] [2];
        imgdata[i++] = 255;
      }
      this.ctxFront.drawImage(this.imgFront, 
                    30, 74, 512, this.height - 24, 
                    30, 75, 512, this.height - 24);
      this.ctxFront.putImageData(this.lineImage, 30, 74);

      // add a timestamp
      const now = new Date();
      const year = now.getUTCFullYear();
      const month = now.getUTCMonth() + 1;
      const day = now.getUTCDate();
      const hours = now.getUTCHours();
      const minutes = now.getUTCMinutes();
      const seconds = now.getUTCSeconds();
      const milliseconds = now.getUTCMilliseconds();
      this.ctxFront.font = "12px Arial";
      const secs = seconds; //+milliseconds/1000;
      this.ctxFront.fillStyle = "#000000";
      this.ctxFront.fillRect(400,18,150,30);
      this.ctxFront.fillStyle = "#FFFFFF";
      this.ctxFront.fillText(day+" / "+month+" / "+year+"  "+hours+":"+minutes.toString().padStart(2,'0')+":"+secs.toString().padStart(2,'0'), 400, 38);


      this.ctxFront.font = "24px Arial";
      this.ctxFront.fillText(itles[clientId], 31, 38);
      this.ctxFront.font = "16px Arial";
      this.ctxFront.fillText(cations[clientId], 31, 62);

      w_blocking=0;
    };
}

function ColourMap()
{
  var map = new Array(256);

  var e;
  for (e = 0; 64 > e; e++)
  {
    map[e] = new Uint8Array(3);
    map[e][0] = 0;
    map[e][1] = 0;
    map[e][2] = 64 + e;
  }
  for (; 128 > e; e++)
  {
    map[e] = new Uint8Array(3);
    map[e][0] = 3 * e - 192;
    map[e][1] = 0;
    map[e][2] = 64 + e;
  }
  for (; 192 > e; e++)
  {
    map[e] = new Uint8Array(3);
    map[e][0] = e + 64;
    map[e][1] = 256 * Math.sqrt((e - 128) / 64);
    map[e][2] = 511 - 2 * e;
  }
  for (; 256 > e; e++)
  {
    map[e] = new Uint8Array(3);
    map[e][0] = 255;
    map[e][1] = 255;
    map[e][2] = 512 + 2 * e;
  }

  return map;
}
