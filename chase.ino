#include <SPI.h>
#include <TCL.h>

int potValue = 0;        // value read from the pot

const int LEDS = 25;
const int COLORS = 7;
const int BLACK = 0;
const int RED = 1;
const int ORANGE = 2;
const int YELLOW = 3;
const int GREEN = 4;
const int BLUE = 5;
const int WHITE = 6;

byte color_values[COLORS][3];
unsigned long change_time;
int current_color[LEDS];
byte currentFrame[LEDS][3];
int currentLED;


int numColors = 255;
int colorCounter = 0;

void setup() {
  TCL.setupDeveloperShield();
  Serial.begin(9600);
  
  color_values[BLACK][0] = 0x00;
  color_values[BLACK][1] = 0x00;
  color_values[BLACK][2] = 0x00;
  
  color_values[RED][0]=0xff;
  color_values[RED][1]=0x00;
  color_values[RED][2]=0x00;
  
  color_values[ORANGE][0]=0xff;
  color_values[ORANGE][1]=0x60;
  color_values[ORANGE][2]=0x00;
  
  color_values[YELLOW][0]=0xff;
  color_values[YELLOW][1]=0xb0;
  color_values[YELLOW][2]=0x00;
  
  color_values[GREEN][0]=0x00;
  color_values[GREEN][1]=0x80;
  color_values[GREEN][2]=0x00;
  
  color_values[BLUE][0]=0x00;
  color_values[BLUE][1]=0x00;
  color_values[BLUE][2]=0xff;
  
  color_values[WHITE][0]=0xff;
  color_values[WHITE][1]=0xff;
  color_values[WHITE][2]=0xff;
    
  
  int i;
  unsigned long time;
  
  TCL.begin();
  currentLED = 0;
  time = millis();
  change_time = time+1000;
  for(i=0;i<LEDS;i++) {
    currentFrame[i][0] =   color_values[WHITE][0];
    currentFrame[i][1] =   color_values[WHITE][1];
    currentFrame[i][2] =   color_values[WHITE][2];
  }
    currentFrame[currentLED][0] =   color_values[BLUE][0];
    currentFrame[currentLED][1] =   color_values[BLUE][1];
    currentFrame[currentLED][2] =   color_values[BLUE][2];
  update_strand();
}
  
  
void loop() {

  int i;
  unsigned long time;
  
  time=millis();
  
  potValue= analogRead(TCL_POT1);
  
  if(change_time<time) {
    change_time = time + (potValue/2);
     float colorNumber = colorCounter > numColors ? colorCounter - numColors: colorCounter;
    
    // Play with the saturation and brightness values
    // to see what they do
    float saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
    float brightness = .05; // Between 0 and 1 (0 = dark, 1 is full brightness)
    float hue = (colorNumber / float(numColors)) * 360; // Number between 0 and 360
    long color = HSBtoRGB(hue, saturation, brightness); 
    
    //Serial.println(colorNumber);
    
    // Get the red, blue and green parts from generated color
    int red = color >> 16 & 255;
    int green = color >> 8 & 255;
    int blue = color & 255;
    
    
    for(i=0;i<LEDS;i++) { 
      currentFrame[i][0] =   red;
      currentFrame[i][1] =   green;
      currentFrame[i][2] =   blue;
    }
    currentFrame[currentLED+1][0] =   color_values[BLUE][0];
    currentFrame[currentLED+1][1] =   color_values[BLUE][1];
    currentFrame[currentLED+1][2] =   color_values[BLUE][2];
    
    if (currentLED>=LEDS) {
      currentLED = 0;
    }
    else {
      currentLED = currentLED + 1;
    }

    colorCounter = (colorCounter + 1) % (numColors * 2);  
  }
  
  update_strand();
  // print the results to the serial monitor:
}

void update_strand() {
  int i;
  int color;
  
  TCL.sendEmptyFrame();
  for(i=0;i<LEDS;i++) {
    TCL.sendColor(currentFrame[i][0],currentFrame[i][1],currentFrame[i][2]);
  }
  TCL.sendEmptyFrame();
}

long HSBtoRGB(float _hue, float _sat, float _brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    
    if (_sat == 0.0) {
        red = _brightness;
        green = _brightness;
        blue = _brightness;
    } else {
        if (_hue == 360.0) {
            _hue = 0;
        }

        int slice = _hue / 60.0;
        float hue_frac = (_hue / 60.0) - slice;

        float aa = _brightness * (1.0 - _sat);
        float bb = _brightness * (1.0 - _sat * hue_frac);
        float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
        
        switch(slice) {
            case 0:
                red = _brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = _brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = _brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = _brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = _brightness;
                break;
            case 5:
                red = _brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    long ired = red * 255.0;
    long igreen = green * 255.0;
    long iblue = blue * 255.0;
    
    return long((ired << 16) | (igreen << 8) | (iblue));
}
