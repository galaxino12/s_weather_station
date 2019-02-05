#include <DHT.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <MQ135.h>
#include <SPI.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

SFE_BMP180 bmp180;
double pressure_sea_level=1013.25;

int UVOUT = A0;
int REF_3V3 = A1;

#define PIN_MQ135 A2
MQ135 mq135_sensor = MQ135(PIN_MQ135);

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
  Serial.print("Temperature(DHT22): ");
  Serial.print(t);
  Serial.print(" °C ");
  Serial.println();
  Serial.print("Humedity(DHT22): ");
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
       
  Serial.print("Temperature(BMP180): ");
  Serial.print(T);
  Serial.print(" °C , ");
  Serial.println();
  Serial.print("Pressure(BMP180): ");
  Serial.print(P*0.0145038);
  Serial.print(" psi"); 
  Serial.println();
  Serial.print("Altitude(BMP180): ");
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
  Serial.print("UV Intensity(ML8511): ");
  Serial.print(uvIntensity);
  Serial.print("mW/cm^2");
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
  Serial.print("Fixed Resistance: ");
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
  Serial.print("Corrected Pollution (MQ-135, DHT22): ");
  Serial.print(correctedPPM*3.19);
  Serial.print(" mg/m^3");
  Serial.println();
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

