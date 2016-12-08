

// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>

#define UP_BUTTON_PIN   2    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.
#define DOWN_BUTTON_PIN 3

#define PIXEL_PIN    6   // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 180


// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
void startShow(int i);
void christmassCycle(uint8_t wait);
void christmassCycle2(uint8_t wait);
int onButton()
{
         // Short delay to debounce button.
         delay(20);
          // Check if button is still low after debounce.
         if(digitalRead(UP_BUTTON_PIN) == LOW)
         {
            return 1;
         }
         else if (digitalRead(DOWN_BUTTON_PIN) == LOW)
         {
            return 2; 
         }
         return 0;
   
}
int showType = 0;

void setup() {
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  startShow(0);
}

void loop() {

int direction = 0;
    direction = onButton();
    if (direction == 1) 
    {
      showType++;
      if (showType > 9)
        showType=0;
      startShow(showType);
    }
    if(direction == 2)
    {
      showType--;
      if (showType < 0)
        showType=9;
      startShow(showType);
    }
    else
    {
      startShow(showType);
    }
}

void startShow(int i) {
 
  switch(i){
    case 0: 
            delay(1000);
            while(onButton()== 0)
            {
              christmassCycle2(20);
            }    
            break;
           
    case 1: 
            delay(100);
            while(onButton() == 0)
            {
              infiniteRainbowCycle(20);
            }
            break;
    case 2: 
            delay(100);
            while(onButton()== 0)
            {
              breath(20);
            }
            break;
    case 3: 
            delay(100);
            while(onButton()== 0)
            {
              christmassCycle(20);
            }
            break;
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: colorWipe(strip.Color(255, 255, 255), 25);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
    case 10: infiniteRainbowCycle(20);
    break;        
         
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
      if(onButton()>0)
      {
        return;
      }
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      if(onButton()>0)
      {
        return;
      }
    }
    strip.show();
    delay(wait);
  }
}

void infiniteRainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  while(1)
  { //for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    j++;
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    if(onButton() > 0)
    {
      return;
    }
  }
}

void christmassCycle(uint8_t wait)
{
  uint16_t i, k = 0, t = 0;
  double f, f1, r, g, w;
  f = 0.002*2*PI;
  f1 = 0.0013*PI;
  while(1)
  {
      r = 255 * sin(f*t);
      g = -255 * sin(f*t);
      w = 250 * sin(f1*t);

      if(r <0)
      { 
        r = 0;
      }
      if(g <0)
      {
        g = 0;
      }
      if(w<0)
      {
        w = -w;
      }
      for(i=0; i<strip.numPixels(); i+=4) //draw one frame of the pixels
      {
        if(i<= strip.numPixels())
        {
          strip.setPixelColor(i, r, g, 0);
        }
        if((i+1) <= strip.numPixels())
        {
          strip.setPixelColor(i+1, w, w, w);
        } 
        if((i+2) <= strip.numPixels())
        {
          strip.setPixelColor(i+2, g, r, 0);
        }
        if((i+3)<= strip.numPixels())
        {
          strip.setPixelColor(i+3, w,w, w);
        }
        
      }

      strip.show();
      
      if(onButton() > 0)
      {
        return;
      }
      
      t++;
      
  }

}

void christmassCycle2(uint8_t wait)
{
  uint16_t i, k = 0, t = 0, j;
  double f, f1, r, g, w;
  f = 0.002*2*PI;
  f1 = 0.0013*PI;
  while(1)
  {
      r = 255 * sin(f*t);
      g = -255 * sin(f*t);
      w = 250 * sin(f1*t);
      if(t%(strip.numPixels()/10)==0)
      {
        k++;
      }
      if(r <0)
      { 
        r = 0;
      }
      if(g <0)
      {
        g = 0;
      }
      if(w<0)
      {
        w = -w;
      }
      if(k>strip.numPixels())
      {
        k = 0;
      }
      for(i=0; i<strip.numPixels(); i+=4) //draw one frame of the pixels
      {
      
        if(i+k<= strip.numPixels())
        {
          strip.setPixelColor(i+k, r, g, 0);
        }
        if((i+1+k) <= strip.numPixels())
        {
          strip.setPixelColor(i+1+k, w, w, w);
        } 
        if((i+2+k) <= strip.numPixels())
        {
          strip.setPixelColor(i+2+k, g, r, 0);
        }
        if((i+3+k)<= strip.numPixels())
        {
          strip.setPixelColor(i+3+k, w,w, w);
        }
        
      }
      for(i=0; i<k; i+=4)  //for the inverse
      {
        if(i<= strip.numPixels())
        {
          strip.setPixelColor(k-i, r, g, 0);
        }
        if((i+1) <= strip.numPixels())
        {
          strip.setPixelColor(k-i+1, w, w, w);
        } 
        if((i+2) <= strip.numPixels())
        {
          strip.setPixelColor(k-i+2, g, r, 0);
        }
        if((i+3)<= strip.numPixels())
        {
          strip.setPixelColor(k-i+3, 0,0, w);
        }
      }

      strip.show();
      
      if(onButton() > 0)
      {
        return;
      }
      
      t++;
      
  }

}

void breath(uint8_t pattern)
{
  int t = 0;
  double f = 0.003*PI;
  int8_t w, x;
  
  while(1)
  {
    delay(20);
      t++;
      w= 120 * sin(f*t);// +120;
       
      x = -120 * sin(f*t);//+120;

      if(x<0)
      {
        x = 0; 
      }
      if(w<0)
      {
        w = 0; 
      }
      for(int i = 0; i<strip.numPixels(); i++)
      {
        if(i%pattern == 0)
        {
            strip.setPixelColor(i, w, w, w);
            strip.setPixelColor(i+pattern/2, x, x, x);  
        }
        
      }


      strip.show();

      if(onButton() > 0)
      {
        return;
      }
  }
    
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      if(onButton()>0)
      {
        return;
      }
      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();
      if(onButton()>0)
      {
        return;
      }
      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


