/* PROGRAM MONITORING BMP 280 & MAX 30102
 * KELOMPOK 5
 *  NAMA : 
 *   AUDREY MARTIKA D.  3.32.18.0.03
 *   DIFA JIHAN S.      3.32.18.0.06
 *   FEBY ADIANTA       3.32.18.0.08
 *   NAUFAL DAFFA W.    3.13.18.0.16 
 */
 
#include <Wire.h>
#include <Adafruit_GFX.h>         //Librari untuk Oled dan BMP280
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>

#include "MAX30105.h"             //Librari MAX30105
#include "heartRate.h"            //Algoritma Perhitungan Detak Jantung

#define SCREEN_WIDTH 128          // Lebar Tampilan OLED, dalam pixels
#define SCREEN_HEIGHT 32          // Tinggi Tampilan OLED, dalam pixels
#define OLED_RESET    -1          // Reset pin # (atau -1 jika sharing Arduino reset pin)

MAX30105 particleSensor;

const byte RATE_SIZE = 4;         //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];            //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;                //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
int irValue;

static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,              //Logo2 and Logo3 are two bmp pictures that display on the OLED if called
  0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
  0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
  0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,
};

static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
  0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
  0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
  0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)
Adafruit_BMP280 bmp;


void setup() {
  Serial.begin(9600);
  bmp.begin();                                    //Start the bmp
  particleSensor.begin(Wire, I2C_SPEED_FAST);     //Use default I2C port, 400kHz speed
  particleSensor.setup();                         //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);      //Turn Red LED to low to indicate sensor is running
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);      //Start the OLED display
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("Kelompok 5");                    //Show the name, you can remove it or replace it
  display.setCursor(32, 12);
  display.setTextSize(2);
  display.println("BMP280");
  display.display();
  delay(2000);
}

void loop() {
  irValue = particleSensor.getIR();
  Serial.println(irValue);
  if (irValue < - 7000 || irValue > 7000) {
    MAX();
  }
  else {
    BMP();
    beatAvg = 0;
  }
}

void BMP() {

  display.clearDisplay();
  float T = bmp.readTemperature();              //Read temperature in C
  float P = bmp.readPressure() / 100;           //Read Pressure in Pa and conversion to hPa
  float A = bmp.readAltitude(1013.50);          //Calculating the Altitude, the "1019.66" is the pressure in (hPa) at sea level at day in your region
  //If you don't know it just modify it until you get the altitude of your place

  display.setCursor(0, 0);                      //Oled display, just playing with text size and cursor to get the display you want
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.print("Temp");
  display.setCursor(0, 18);
  display.print(T, 1);
  display.setCursor(50, 17);
  display.setTextSize(1);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(65, 0);
  display.print("Pres");
  display.setCursor(65, 10);
  display.print(P);
  display.setCursor(110, 10);
  display.print("hPa");

  display.setCursor(65, 25);
  display.print("Alt");
  display.setCursor(90, 25);
  display.print(A, 0);
  display.setCursor(110, 25);
  display.print("m");

  display.display();
  delay(2000);
}
void MAX() {

  display.clearDisplay();                                   //Clear the display
  display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);       //Draw the first bmp picture (little heart)
  display.setTextSize(2);                                   //Near it display the average BPM you can display the BPM if you want
  display.setTextColor(WHITE);
  display.setCursor(50, 0);
  display.println("BPM");
  display.setCursor(50, 18);
  display.println(beatAvg);
  display.display();

  if (checkForBeat(irValue) == true)                       //If a heart beat is detected
  {
    display.clearDisplay();                                //Clear the display
    display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE);    //Draw the second picture (bigger heart)
    display.setTextSize(2);                                //And still displays the average BPM
    display.setTextColor(WHITE);
    display.setCursor(50, 0);
    display.println("BPM");
    display.setCursor(50, 18);
    display.println(beatAvg);
    display.display();
    tone(3, 1000);                                        //And tone the buzzer for a 100ms you can reduce it it will be better
    delay(100);
    noTone(3);                                            //Deactivate the buzzer to have the effect of a "bip"
    //We sensed a beat!
    long delta = millis() - lastBeat;                     //Measure duration between two beats
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);              //Calculating the BPM

    if (beatsPerMinute < 255 && beatsPerMinute > 20)     //To calculate the average we strore some values (4) then do some math to calculate the average
    {
      rates[rateSpot++] = (byte)beatsPerMinute;          //Store this reading in the array
      rateSpot %= RATE_SIZE;                             //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;

    }
  }
}
