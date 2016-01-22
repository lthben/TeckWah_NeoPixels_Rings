/* Author: Benjamin Low
 * Last updated: 20 Jan 2016
 * Description: Teck Wah gallery NeoPixel Rings
 *
 *   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
 *   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
 *   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
 *   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
 *   
 * IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
 * pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
 * and minimize distance between Arduino and first pixel.  Avoid connecting
 * on a live circuit...if you must, connect GND first.
 */

#include <Adafruit_NeoPixel.h> 

// Pattern types supported:
enum  pattern { NONE, SCANNER };
// Pattern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:
 
    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use 
    uint16_t TotalSteps;  // total number of steps in the pattern
    int Index;  // current step within the pattern

    uint32_t pinkColor = Color(200, 40, 60);
    uint32_t greenColor = Color(10, 220, 20 );
    uint32_t blueColor = Color(20, 20, 220);
    uint32_t yellowColor = Color( 160, 160, 10); 
    uint32_t whiteColor = Color(255, 255, 255);
    uint32_t redColor = Color(255, 0, 0);
    uint32_t noColor = Color(0,0,0);
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }
    
    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            
            switch(ActivePattern)
            {
                case SCANNER:
                    ScannerUpdate();
                    break;
                default:
                    break;
            }
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD)
        {
           Index++;
           
           if (Index >= TotalSteps) 
            {
                Index = 0;
                
                if (OnComplete != NULL)
                {
                    OnComplete(); 
                }
            }
        }
        else // Direction == REVERSE
        {
            --Index;
            
            if (Index < 0)
            {
                Index = TotalSteps-1;
                
                if (OnComplete != NULL)
                {
                    OnComplete();
                }
            }
        }
    }

    void TurnOff() {
        ColorSet(Color(0,0,0));
    }
    
    void Scanner(uint32_t color1, uint8_t interval, int my_dir)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        
        if (my_dir == 1) 
        {
            Direction = FORWARD;
            Index = 0;
        } 
        else if (my_dir == -1) 
        {
            Direction = REVERSE;
            Index = numPixels() - 1;
        }
    }
 
    // Update the Scanner Pattern
    void ScannerUpdate()
    { 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
//                 setPixelColor(i, Wheel( ( (i * 256 / numPixels() ) + Index ) & 255) );
//                byte my_index = ( (i + Index) * 256 / numPixels() ) & 255;

//                Serial.println(my_index);
                
//                setPixelColor(i, Wheel(my_index) );

                    setPixelColor(i, Color1);
            }
           /*else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
            }*/
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
    }
    
   
    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
        // Shift R, G and B components one bit to the right
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }
 
    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        ActivePattern = NONE;
        
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }
 
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }
 
    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }
 
    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }
    
    // Input a value 0 to 255 to get a color value.
    
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        
        if(WheelPos < 85)
        {
            return Color( 255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }

}; //end of class definition

// ------------------------------------------------------
// Function prototypes for completion callback routines 
// ------------------------------------------------------
void Strip1Complete();
void Strip2Complete();
void Strip3Complete();

// -----------------------------
// Object declarations
// -----------------------------
NeoPatterns Strip1(300, 2, NEO_GRB + NEO_KHZ800, &Strip1Complete);
NeoPatterns Strip2(60, 3, NEO_GRB + NEO_KHZ800, &Strip2Complete);
//NeoPatterns Strip3(300, 6, NEO_GRB + NEO_KHZ800, &Strip3Complete);

//------------------------------
// setup
//------------------------------
void setup() {
  Serial.begin(9600);

  Strip1.begin();
  Strip2.begin();
//  Strip3.begin();

//Strip1.RainbowCycle(2);
//  Strip1.Scanner( Strip1.Color(255,0,0), 100, 1);
}

//-------------------------
// The main loop
//-------------------------

void loop() {

  Strip1.Update();
//  Strip2.Update();
//  Strip3.Update();
  
  read_from_serial();
}

//-----------------------
// supporting functions
//-----------------------

void read_from_serial() {

  unsigned char incomingbyte = 0;

  if (Serial.available() > 0) {
        
    incomingbyte = Serial.read();

    if (incomingbyte == '0') {

        Strip1.ColorSet( Strip1.noColor );

    } else if (incomingbyte == '1') {

        Strip1.ColorSet( Strip1.whiteColor );
      
    } else if (incomingbyte == '2') {

        Strip1.ColorSet( Strip1.greenColor );
      
    } else if (incomingbyte == '3') {

        Strip1.ColorSet( Strip1.blueColor );
      
    } else if (incomingbyte == '4') {

        Strip1.ColorSet( Strip1.yellowColor );

    } else if (incomingbyte == '5') {

        Strip1.Scanner( Strip1.Color(2,2,2), 80, 1 );
    }
  }
}

// -----------------------------
// Completion callback routines 
// -----------------------------

void Strip1Complete() {
  Serial.println("strip 1 complete");
//  Strip1.ColorSet( Strip1.noColor );
  //Strip1.Scanner( Strip1.Color(255,0,0), 100, 1 );
}

void Strip2Complete() {
  Serial.println("strip 2 complete");
}

void Strip3Complete() {
  Serial.println("strip 3 complete");
}






