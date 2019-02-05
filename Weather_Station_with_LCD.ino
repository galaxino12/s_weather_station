#include <DHT.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <MQ135.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

SFE_BMP180 bmp180;
double pressure_sea_level=1013.25;

int UVOUT = A0;
int REF_3V3 = A1;

#define PIN_MQ135 A2
MQ135 mq135_sensor = MQ135(PIN_MQ135);

#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
  Serial.begin(57600);
  dht.begin();
  if (bmp180.begin())
          Serial.println("BMP180 started correctly");
        else {
          Serial.println("Error starting the BMP180");
        }
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK); 
  tft.setTextColor(ILI9341_DARKGREEN);
  tft.setTextSize(2);
  tft.setCursor(70,20);
  tft.print("Weather_Station");
}

void loop() {
  char a;
  double T,P,A;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(t, h);
  float resistance = mq135_sensor.getResistance();
  float correctedresistence = mq135_sensor.getCorrectedResistance(t, h);
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(t, h);
  
  delay(2000);
  Serial.println();
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C ");
  Serial.println();
  Serial.print("Humedity: ");
  Serial.print(h);
  Serial.print(" %RH");
  Serial.println();

  a = bmp180.startTemperature();
  if (a != 0) {   
    delay(a);
    a = bmp180.getTemperature(T);
     if (a != 0) {
         a = bmp180.startPressure(3);
        if (a != 0) {        
                  delay(a);      
                        a = bmp180.getPressure(P,T);
                 if (a != 0) {                   
                  A= bmp180.altitude(P,pressure_sea_level);

                 }      
              }      
           }   
       }
       
  Serial.print("Temperature: ");
  Serial.print(T);
  Serial.print(" °C , ");
  Serial.println();
  Serial.print("Pressure: ");
  Serial.print(P*0.0145038);
  Serial.print(" psi"); 
  Serial.println();
  Serial.print("Altitude: ");
  Serial.print(A);
  Serial.print(" m s.n.m.");  
  Serial.println();
                        
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  float outputVoltage = 3.3 / refLevel * uvLevel;
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 12.0);
  
  Serial.print("Output: ");
  Serial.print(refLevel);
  Serial.print(" v");
  Serial.println();
  Serial.print("ML8511 output: ");
  Serial.print(uvLevel);
  Serial.print(" v");
  Serial.println();
  Serial.print("ML8511 voltage: ");
  Serial.print(outputVoltage);
  Serial.print(" v");
  Serial.println();
  Serial.print("UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity);
  Serial.println();
  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.println();
  Serial.print("Fixed RZero: ");
  Serial.print(correctedRZero);
  Serial.println();
  Serial.print("Resistance: ");
  Serial.print(resistance);
  Serial.println();
  Serial.print("Fixed Resistencia: ");
  Serial.print(correctedresistence);
  Serial.println();
  Serial.print("PPM: ");
  Serial.print(ppm);
  Serial.print(" ppm");
  Serial.println();
  Serial.print("Fixed PPM: ");
  Serial.print(correctedPPM);
  Serial.print(" ppm");
  Serial.println();
  Serial.print("Corrected Pollution: ");
  Serial.print(correctedPPM*3.19);
  Serial.print(" mg/m^3");
  Serial.println();

  write(14,40,1,"ILI9341_GREEN","Temperature");
  write(14,60,0.6,"ILI9341_RED",String(t));
  write(24,60,0.6,"ILI9341_RED"," °C");
  write(14,80,1,"ILI9341_GREEN","Humedity");
  write(14,100,0.6,"ILI9341_BLUE",String(h));
  write(24,100,0.6,"ILI9341_BLUE"," %RH");
  write(14,120,1,"ILI9341_GREEN","Pressure");
  write(14,140,0.6,"ILI9341_LIGHTGREY",String(P*0.0145038));
  write(24,140,0.6,"ILI9341_LIGHTGREY"," psi");
  write(14,160,1,"ILI9341_GREEN","Air Quality");
  write(14,180,0.6,"ILI9341_CYAN",String(correctedPPM));
  write(24,180,0.6,"ILI9341_CYAN"," ppm");
  write(14,200,1,"ILI9341_GREEN","Radiation UV");
  write(14,220,0.6,"ILI9341_MAGENTA",String(uvIntensity));
  write(24,220,0.6,"ILI9341_MAGENTA"," mW/cm^2");
}

int averageAnalogRead(int pinToRead) {
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 
  for(int x = 0 ; x < numberOfReadings ; x++)
          runningValue += analogRead(pinToRead);
          runningValue /= numberOfReadings;
    return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void write(int X,int Y, int tam, short color, String mensaje){
  tft.setCursor(X,Y);
  tft.setTextSize(tam);
  tft.setTextColor(color);
  tft.print(mensaje);
}
