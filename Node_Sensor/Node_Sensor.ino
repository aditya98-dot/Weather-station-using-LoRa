/*****************************************************************************
  Created by: Aditya Pratama. email: adityapratama141198@gmail.com. July 2022
  Weather Monitoring Station Using LoRa Version 1.0
  This is open source code. Please include my name in copies of this code
  Thankyou ...
 *****************************************************************************/

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <DHT.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27, 16, 2);
//DS3231_Simple Clock;

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

BH1750 lightMeter;

const int pin_interrupt = 2;
long int jumlah_tip = 0;
long int temp_jumlah_tip = 0;
float gCurahHujan = 0.00;
float milimeter_per_tip = 0.70;
volatile bool flag = false;
float gTemperature, gHumidity, gLux, gIrradiance;

void hitung_gCurahhujan()
{
  flag = true;
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("LCD Ready!");
  delay(2000);
  dht.begin();
  lightMeter.begin();
  pinMode(pin_interrupt, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin_interrupt), hitung_gCurahhujan, FALLING); // Akan menghitung tip jika pin berlogika dari HIGH ke LOW
  // printSerial();

  //LORA
  while (!Serial);
  //Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // read BH1750
  // Wait a few seconds between measurements.
  delay(500);
  PROGMEM const float IRRADIANCE_CONST = 0.0079;

  gLux = lightMeter.readLightLevel();
  gIrradiance = gLux * IRRADIANCE_CONST; // conversion
  
  // read DHT22
  // Wait a few seconds between measurements.
  delay(500);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  gHumidity = dht.readHumidity(); // Read humidity
  gTemperature = dht.readTemperature(); // Read temperature as Celsius
  
//  //read Curah Hujan
    if (flag == true){ // don't really need the == true but makes intent clear for new users
      gCurahHujan += milimeter_per_tip; // Akan bertambah nilainya saat tip penuh
      jumlah_tip++;
      delay(500);
      flag = false; // reset flag
    }
    gCurahHujan = jumlah_tip * milimeter_per_tip;
    //printSerial();
  
  float Temp_send = gTemperature * 100;
  float Hum_send = gHumidity * 100;
  float Lux_send = gLux * 100;
  float Ir_send = gIrradiance * 100;
  float CurahHujan_send = gCurahHujan * 100;
  String valueSensor = "*" + String(Temp_send) + "," + String(Hum_send) + "," + String(Lux_send) + "," + String(Ir_send) + "," + String(CurahHujan_send) + "#";
  
  Serial.println(valueSensor);
  delay(2000);

  // send packet
  //Serial.println("Sending Packet....");
  LoRa.beginPacket();
  LoRa.println(valueSensor);
  LoRa.endPacket();
  //dataLogger();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Succeed sending");
  lcd.setCursor(0, 1);
  lcd.print("data!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("NODE SENSOR");
  delay(600000);
  //delay(10000);
}

#if defined DEBUG_BH1750 || defined DEBUG_ALL
  Serial.print(F("Light: "));
  Serial.print(gLux);
  Serial.println(F(" lx"));

  Serial.print(F("Irradiance: "));
  Serial.print(gIrradiance);
  Serial.println(F(" W/m2"));
#endif

#if defined DEBUG_DHT22 || defined DEBUG_ALL
  Serial.print("Humidity: ");
  Serial.print(gHumidity);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(gTemperature);
  Serial.println(" °C");
#endif

#if defined DEBUG_CURAH_HUJAN || defined DEBUG_ALL
  Serial.print("Jumlah tip= ");
  Serial.print(jumlah_tip);
  Serial.println(" kali");

  Serial.print("Curah hujan=");
  Serial.print(gCurahHujan, 1);
  Serial.print(" mm");
  Serial.println();
#endif
