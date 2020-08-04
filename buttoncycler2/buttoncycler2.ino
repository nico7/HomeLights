// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <string.h>

SoftwareSerial mySerial(10, 11); // RX, TX

#define ANALOG_REF 8
#define DIGITAL_REF 9


// Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.
#define UP_BUTTON_PIN   2    // YELLOW
#define RIGHT_BUTTON_PIN  3                          
#define DOWN_BUTTON_PIN   5
#define ENTER_BUTTON_PIN  4
#define VOLUME            A0
#define FREQ              A2

#define PIXEL_PIN    6   // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 180
#define PIXEL_OPTIONS 3

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


typedef enum LightMode // This is an enum of the selectable modes (big picture)
{
  IDLE_MODE,
  ALL_WHITE,
  RGB,
  XMAS,
  GAME1,
  THEATER_CHASE,
  RAINBOW,
  XMAS2,
  NUMBER_OF_CHOICES,
} LightMode_E;

typedef enum SelectionPixelColor  // This is an enum of the colors of pixels for mode selection UI
{
  BLACK,
  WHITE,
  RED,
  GREEN,
  BLUE,
  NUMBER_OF_PIXEL_COLORS,
} SelectionPixelColor_E;


typedef enum Colors
{
  BLACK_COLOR,
  WHITE_COLOR = 0xFFFFFF,
  RED_COLOR   = 0xFF0000,
  GREEN_COLOR = 0x00FF00,
  BLUE_COLOR  = 0x0000FF,
} COLOR_E;

uint32_t pixel_colors[NUMBER_OF_PIXEL_COLORS] = {BLACK_COLOR, WHITE_COLOR, RED_COLOR, GREEN_COLOR, BLUE_COLOR};
unsigned long time;
uint16_t t = 0;
LightMode_E choice;
uint8_t digit = 0;
COLOR_E option_array[PIXEL_OPTIONS] = {0};
uint8_t option_index = 0;
uint8_t color_index = 0;

uint8_t index_helper = 0;

char debug_string[20];

// ----Variables used for RGB mode----
uint8_t rgb_r = 0;
uint8_t rgb_g = 0;
uint8_t rgb_b = 0;
uint8_t rgb_index = 0;

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



LightMode_E decodeLightMode(COLOR_E * optionArray)
{
    LightMode_E retMode = IDLE_MODE;
    
    if(getColorIndex(optionArray[2]) == BLACK)
    {
      if(getColorIndex(optionArray[1]) == BLACK)
      {
          if(getColorIndex(optionArray[0]) == BLACK)
          {
              retMode = IDLE_MODE;        // I know, nothing changes, but for sake of consistency I put this here  
          } 
          else if(getColorIndex(optionArray[0]) == WHITE)
          {
              retMode = ALL_WHITE;
          }
          else if(getColorIndex(optionArray[0]) == RED)
          {
              retMode = RGB;
          }
          else if(getColorIndex(optionArray[0]) == GREEN)
          {
              retMode = XMAS;
          }
          else if(getColorIndex(optionArray[0]) == BLUE)
          {
              retMode = GAME1;
          }
      }
      else if (getColorIndex(optionArray[1]) == WHITE)
      {
          if(getColorIndex(optionArray[0]) == BLACK)
          {
              retMode = THEATER_CHASE;
          }
          else if(getColorIndex(optionArray[0]) == WHITE)
          {
              retMode = RAINBOW;
          }
          else if(getColorIndex(optionArray[0]) == RED)
          {
              retMode = XMAS2;
          }
      }
    }

    return retMode;
}


uint8_t getColorIndex(COLOR_E currentColor)
{
    uint8_t retColor;
    
    switch(currentColor)
    {
        case BLACK_COLOR:
            retColor = (uint8_t) BLACK;
            strcpy(debug_string, "BLACK");
            break;
        case WHITE_COLOR:
            retColor = (uint8_t) WHITE;
            strcpy(debug_string, "WHITE");
            break;
        case RED_COLOR:
            retColor = (uint8_t) RED;
            strcpy(debug_string, "RED");
            break;
        case GREEN_COLOR:
            retColor = (uint8_t) GREEN;
            strcpy(debug_string, "GREEN");
            break;
        case BLUE_COLOR:
            retColor = (uint8_t) BLUE;
            strcpy(debug_string, "BLUE");
            break;
        case DEFAULT:
            strcpy(debug_string, "NONE");
            return 255;
            break;
    }  

    return retColor;
}
void debug_color_index(void)
{
  Serial.println("");
  getColorIndex(option_array[2]);
  Serial.print( debug_string);
  Serial.print( " | ");
  getColorIndex(option_array[1]);
  Serial.print( debug_string);
  Serial.print( " | ");
  getColorIndex(option_array[0]);
  Serial.print( debug_string);
  Serial.println( "");
  Serial.print("VOLUME = " );
  Serial.print(analogRead(VOLUME), DEC);
  Serial.print("FREQ = " );
  Serial.print(analogRead(FREQ), DEC);
}

void setup() {
    // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(ANALOG_REF, OUTPUT);
  pinMode(DIGITAL_REF, OUTPUT);
  
  digitalWrite(ANALOG_REF, HIGH);
  digitalWrite(DIGITAL_REF, LOW);
  
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);                          
  pinMode(ENTER_BUTTON_PIN, INPUT_PULLUP);
  

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  startShow(0);
}



void loop() 
{
  
  //debug_color_index();
  time = millis();

   if (digitalRead(ENTER_BUTTON_PIN) == LOW && choice != IDLE_MODE)
        {
            while(digitalRead(ENTER_BUTTON_PIN) == LOW)
            {

                if(time - millis() >= 40)
                {
                   Serial.println("Pressed ENTER");
                   choice = IDLE_MODE;

                   while(digitalRead(ENTER_BUTTON_PIN) == LOW);
                }
            }
        }
  switch(choice)
  {
    case IDLE_MODE:
        time = millis();
        if(digitalRead(UP_BUTTON_PIN) == LOW)
        {
            while(digitalRead(UP_BUTTON_PIN) == LOW)
            {
                if(time - millis() >= 100)
                {
                  Serial.println("Pressed Up");
                  color_index = getColorIndex(option_array[option_index]);  // Get current color
                  color_index +=1;                                         // Go to the next index
                  color_index %= NUMBER_OF_PIXEL_COLORS;                   // Ensure the index exists
               
                  option_array[option_index] = pixel_colors[color_index];   // Change color of pixel to that of next index
                  while(digitalRead(UP_BUTTON_PIN) == LOW);
                  time = millis();
                }    
            }
        }
        else if (digitalRead(DOWN_BUTTON_PIN) == LOW)
        {
            while(digitalRead(DOWN_BUTTON_PIN) == LOW)
            {
                if(time - millis() >= 100)
                {
                  Serial.println("Pressed Down");
                  color_index = getColorIndex(option_array[option_index]);  // Get current color
                  if(color_index == 0)
                  {
                    color_index = NUMBER_OF_PIXEL_COLORS - 1;
                  }
                  else
                  {
                    color_index -=1;                                         // Go to the next index
                  }
               
                  option_array[option_index] = pixel_colors[color_index];   // Change color of pixel to that of next index
                  while(digitalRead(DOWN_BUTTON_PIN) == LOW);
                  time = millis();
                }    
            }
        }
        else if (digitalRead(RIGHT_BUTTON_PIN) == LOW)
        {
            while(digitalRead(RIGHT_BUTTON_PIN) == LOW)
            {
                if(time - millis() >= 100)
                {
                  Serial.println("Pressed ->");
                  option_index += 1;
                  option_index %= PIXEL_OPTIONS;
                  while(digitalRead(RIGHT_BUTTON_PIN) == LOW);
                  time = millis();
                }    
            }
        }
        else if (digitalRead(ENTER_BUTTON_PIN) == LOW)
        {
            while(digitalRead(ENTER_BUTTON_PIN) == LOW)
            {

                if(time - millis() >= 40)
                {
                   Serial.println("Pressed ENTER");
                   choice = decodeLightMode(option_array);

                   while(digitalRead(ENTER_BUTTON_PIN) == LOW);
                }
            }
        }
          
        for(uint16_t i=0; i<strip.numPixels(); i++) 
        {
            index_helper = i % 7;
            switch(index_helper)
            {
                case 0:
                    strip.setPixelColor(i, 255, 255, 255);
                    break;
                case 1:
                    strip.setPixelColor(i, 255, 255, 255);
                    break;
                case 2:
                    strip.setPixelColor(i, 255, 255, 255);
                    break;
                case 3:
                    strip.setPixelColor(i, 0, 0, 0);
                    break;
                case 4:
                    strip.setPixelColor(i, option_array[0]);
                    break;
                case 5:
                    strip.setPixelColor(i, option_array[1]);
                    break;
                case 6:
                    strip.setPixelColor(i, option_array[2]);
                    break;

                default:
                    break;
            }
            
            strip.show();
           
        }
        break;

    case ALL_WHITE:
    {
            uint32_t brightness32, delay32;
            uint8_t brightness8, delay8;

            brightness32 = analogRead(VOLUME);
            if(brightness32 > 1000) { brightness32 = 1000;}
            brightness32 *= 255;
            brightness32 /= 1000;
            brightness8 = brightness32;
            Serial.println();
            Serial.print("Brightness = ");
            Serial.println(brightness8, DEC);
            delay32 = analogRead(FREQ);
            if(delay32 > 1000) {delay32 = 1000;}
            delay32 *= 50;
            delay32 /= 1000;
            delay8 = delay32;
            
            colorWipe(strip.Color(brightness8, brightness8, brightness8), delay8);
            strip.show();
    }
        break;
    case RGB:
    {
        uint32_t totalBrightness32, colorBrightness32;  // colorBrightness only applies to one channel at a time; r, g, or b.
        uint8_t brightness8, colorBrightness8;
        uint8_t *rgb_color[3] = {&rgb_r, &rgb_g, &rgb_b};
        uint8_t eff_r, eff_g, eff_b;                    //  effective r, g, and b values after taking the total brightness into consideration


        
     /*       uint8_t rgb_r = 0;
uint8_t rgb_g = 0;
uint8_t rgb_b = 0;
uint8_t rgb_index = 0;
       */     
        if(digitalRead(RIGHT_BUTTON_PIN) == LOW)
        {
          while(digitalRead(RIGHT_BUTTON_PIN) == LOW)
          {
            if(time - millis() >= 100)
            {
              time = millis();
              rgb_index +=1 ;
              Serial.println("\n\r->");
              while(digitalRead(RIGHT_BUTTON_PIN) == LOW);
            }
          }
        }
        rgb_index %= 3;   // always make sure we don't break out of the 3-item array.
//---------------------------------------------------------
//      In this part, we use the potentiometer to change the color of one channel
//---------------------------------------------------------
        colorBrightness32 = analogRead(FREQ);
        if(colorBrightness32 > 1000)
        {
            colorBrightness32 = 1000;
        }
        colorBrightness32 *= 255;
        colorBrightness32 /= 1000;
        *rgb_color[rgb_index] = colorBrightness32;
//---------------------------------------------------------
//      In this part we use the other potentiometer to change the toal brightness of all lights.
//---------------------------------------------------------

        totalBrightness32 = analogRead(VOLUME);
        if(totalBrightness32 > 1000) {totalBrightness32 = 1000;}
        totalBrightness32 *= *rgb_color[0];
        totalBrightness32 /= 1000;
        eff_r = totalBrightness32;
        Serial.println();
        Serial.print("Total brightness RED = ");
        Serial.print(totalBrightness32, DEC);
        Serial.println();

        totalBrightness32 = analogRead(VOLUME);
        if(totalBrightness32 > 1000) {totalBrightness32 = 1000;}
        totalBrightness32 *= *rgb_color[1];
        totalBrightness32 /= 1000;
        eff_g = totalBrightness32;

        totalBrightness32 = analogRead(VOLUME);
        if(totalBrightness32 > 1000) {totalBrightness32 = 1000;}
        totalBrightness32 *= *rgb_color[2];
        totalBrightness32 /= 1000;
        eff_b = totalBrightness32;
//----------------------------------------------------------

        Serial.println();
        Serial.print("r = ");
        Serial.print(rgb_r, DEC);
        Serial.print(" g = ");
        Serial.print(rgb_g, DEC);
        Serial.print(" b = ");
        Serial.println(rgb_b, DEC);

        Serial.print("r = ");
        Serial.print(eff_r, DEC);
        Serial.print(" g = ");
        Serial.print(eff_g, DEC);
        Serial.print(" b = ");
        Serial.println(eff_b, DEC);
        Serial.print("rbg_index = ");
        Serial.println(rgb_index, DEC);
        colorWipe(strip.Color(eff_r, eff_g, eff_b), 0);
        strip.show();
      
    }
        break;
    case XMAS:
        {
            uint16_t i, j;
            double f, f1, r, g, w;
            f = 0.002*2*PI;
            f1 = 0.0013*PI;
 
            r = 255 * sin(f*t) * (analogRead(VOLUME)/1000.0);
            g = -255 * sin(f*t)* (analogRead(VOLUME)/1000.0);
            w = 250 * sin(f1*t)* (analogRead(FREQ)/1000.0);
           
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
      
            t++;
        }

        break;

    case 98: 
            delay(100);
            while(onButton() == 0)
            {
              infiniteRainbowCycle(20);
            }
            break;
    case 97: 
            delay(100);
            while(onButton()== 0)
            {
             // breath(20);
            }
            break;
    case 96: 
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
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
    case 10: infiniteRainbowCycle(20);
    break;        
         
  }


  


}

void startShow(int i) {
 
  
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) 
{           
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
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
