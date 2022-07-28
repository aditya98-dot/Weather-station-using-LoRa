/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

// Include LoRa library using SPI communication
#include <SPI.h>
#include <LoRa.h>
#define LoRa_SS 5
#define LoRa_RST 14
#define LoRa_DIO0 2

// Include LCD library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);           // initialize LCD

// int buz = 15;                                  // initialize the buzzer pin 15
String dataIn, dt[5];                         // initialize variable dataIn and dt[] as a String Type
int i;
boolean parsing = false;
float gTemperature, gHumidity, gLux, gIrradiance, gCurahHujan;

// Replace with your network credentials
const char* ssid = "Masterpiece";
const char* password = "handstand";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName  = "https://weather.smart-monitoring.my.id/weather-data.php"; //Hostname

// Keep this API Key value to be compatible with the PHP code provided in the project page.
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key
String apiKeyValue = "sjdbsi96jsbb1352";
const char* rootCACertificate = \

  "-----BEGIN CERTIFICATE-----\n"
  "MIIFazCCBFOgAwIBAgISBGrNftfzNZIHU461aDDyzAyqMA0GCSqGSIb3DQEBCwUA\n" \
  "MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n" \
  "EwJSMzAeFw0yMjA1MTcwNjUxMjBaFw0yMjA4MTUwNjUxMTlaMC0xKzApBgNVBAMT\n" \
  "Ind3dy53ZWF0aGVyLnNtYXJ0LW1vbml0b3JpbmcubXkuaWQwggEiMA0GCSqGSIb3\n" \
  "DQEBAQUAA4IBDwAwggEKAoIBAQCg56EQtV9OX4c6ZinZHjkjT8U7SmrEAkZEHYvz\n" \
  "Jh/ADqPs4yQTwHK85B/II9AN9XZPQnURe0JLpigW4wnCSWSNvoswUdUtEpqksErw\n" \
  "sP3JnDzml+B2peeKKo2xDBFmhdu3mJyvDGs/GXRUE8pITlCS8yLFmjL5/DT0jmc8\n" \
  "3Rdvze7roP5I9LgyNtaDonXJ7J5xheBJwahzIrJR4gHAc3smHE+ZzCtp6ECCu5S9\n" \
  "hZ9KazPiIROL42uVC6dmyPsuHD8rcS5o+xeVJLa5Nt3Dw4ekJEYgbXKLoY+71kRU\n" \
  "lul1p7GbWFQlS74/cqrTT2m0XGE2ch+6J5CJSn+IwVxF+UZPAgMBAAGjggJ+MIIC\n" \
  "ejAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMC\n" \
  "MAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFJV0xr6rK0yalFDhvbbdh21DQvgFMB8G\n" \
  "A1UdIwQYMBaAFBQusxe3WFbLrlAJQOYfr52LFMLGMFUGCCsGAQUFBwEBBEkwRzAh\n" \
  "BggrBgEFBQcwAYYVaHR0cDovL3IzLm8ubGVuY3Iub3JnMCIGCCsGAQUFBzAChhZo\n" \
  "dHRwOi8vcjMuaS5sZW5jci5vcmcvME0GA1UdEQRGMESCHndlYXRoZXIuc21hcnQt\n" \
  "bW9uaXRvcmluZy5teS5pZIIid3d3LndlYXRoZXIuc21hcnQtbW9uaXRvcmluZy5t\n" \
  "eS5pZDBMBgNVHSAERTBDMAgGBmeBDAECATA3BgsrBgEEAYLfEwEBATAoMCYGCCsG\n" \
  "AQUFBwIBFhpodHRwOi8vY3BzLmxldHNlbmNyeXB0Lm9yZzCCAQUGCisGAQQB1nkC\n" \
  "BAIEgfYEgfMA8QB2AEalVet1+pEgMLWiiWn0830RLEF0vv1JuIWr8vxw/m1HAAAB\n" \
  "gNEABSsAAAQDAEcwRQIgXVAAKOwx9HvGJ/D3KVVtCYYeIu5aw3kllUCo+XdktowC\n" \
  "IQCRoB854yYdT1qyNEvpST/TerzapQGx3wdxpDgR4HflmQB3AEHIyrHfIkZKEMah\n" \
  "OglCh15OMYsbA+vrS8do8JBilgb2AAABgNEAB64AAAQDAEgwRgIhAJWDgQJl5WN1\n" \
  "BJHZS1sANhKmGJjp9kngRXLgaMNTnOM0AiEA6iHE17n7wmbFvLTDzqlIE4fu2o/F\n" \
  "cg9DOkvGE3/YO+kwDQYJKoZIhvcNAQELBQADggEBAJI5DAWg8Ns2jm64jHXD5iMD\n" \
  "tdfY7kbn39lkMooZK2+ybDU+jVux+Vm+TZhh2HI8RNd0Q4yKKVvMzaavvtGba/6E\n" \
  "Y5Uxwg7SBodNBUUaQhTrWVvtwBtUQMv1nxJzuBBkvGXKK8dp7gVanRe7U7PNiaer\n" \
  "MVboi9L+IRzzKhHIpmagu9eGY+PLNwP45eirgxN1zCKQAn6xu2ATVTgGCeOyhzMR\n" \
  "+oxvFRN+g505EKeUEzqVdFyb9nG2NrIU203Esp7ELm00jLcAZ2PnZHF50oCGPfzw\n" \
  "wUzLRuljeV0F/DZJjj08aNM0P6TXKNWQWAUO6MI2fRdj7GfdcSdiJYd2vAuT3ds=\n" \
  "-----END CERTIFICATE-----\n";

// Not sure if WiFiClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...

void setup() {
  Serial.begin(115200);
  // Clear value variable dataIn for parsing data
  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("LCD Ready!");
  delay(2000);
  lcd.clear();
  dataIn = "";
//  pinMode(buz, OUTPUT);            // Create variable buz as a OUTPUT

  // Connect the LoRa
  LoRa.setPins(LoRa_SS, LoRa_RST, LoRa_DIO0);
  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    // Serial.println("Starting LoRa Failed!");
    lcd.setCursor(0, 0);
    lcd.print("Failed Connect");
    lcd.setCursor(0, 1);
    lcd.print("to Node Receiver");
    delay(2000);
    lcd.clear();
    while (1);
  } lcd.clear();

  // check WiFi connection
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 0);
    lcd.print("Check WiFi");
    lcd.setCursor(0,1);
    lcd.print("Connection!");
    delay(2000);
//    digitalWrite(buz, HIGH);
//    delay(500);
//    digitalWrite(buz, LOW);
//    delay(500);
  }
  lcd.clear();
  Serial.println("");
  Serial.println("Connected to WiFi network with IP Address: ");
  Serial.print(WiFi.localIP());
}

void parsingData() {
  int j = 0;

  // kirim data yang telah diterima sebelumnya lalu menampilkannya pada serial monitor
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ada data masuk!");
  Serial.print("data masuk : ");
  Serial.println(dataIn);
  delay(1000);
  dt[j] = "";                //inisialisasi variabel, (reset isi variabel)

  //proses parsing data
  for (i = 1; i < dataIn.length(); i++) {
    // pengecekan tiap karakter dengan karakter (#) dan (,)
    if ((dataIn[i] == '#') || (dataIn[i] == ','))
    {
      //increment variabel j, digunakan untuk merubah index array penampung
      j++;
      dt[j] = "";     // Inisialisasi variabel array dt[j]
    }
    else {
      dt[j] = dt[j] + dataIn[i];    // proses tampung data saat pengecekan karakter selesai.
    }
  }
  
  // tampung data ke variabel
  gTemperature += dt[0].toInt()/100.0;
  gHumidity += dt[1].toInt()/100.0;
  gLux += dt[2].toInt()/100.0;
  gIrradiance += dt[3].toInt()/100.0;
  gCurahHujan += dt[4].toInt()/100.0;
  delay(500);
}

void loop() {
    // parsing data
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      while (LoRa.available() > 0) {
        char inChar = (char)LoRa.read();
        dataIn += inChar;
        if (inChar == '\n') {
          parsing = true;
        }
      }
      if (parsing) {
        parsingData();
        parsing = false;
        dataIn = "";
      }
       //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client) {
      client -> setCACert(rootCACertificate);
      {
        HTTPClient https;

        // Your Domain name with URL path or IP address with path
        https.begin(serverName);

        // Specify content-type header
        https.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Prepare your HTTP POST request data
        String httpRequestData = "api_key=" + apiKeyValue + "&Temperature=" + String(gTemperature) + "&Humidity=" + String(gHumidity)
                                 + "&Light_intensity=" + String(gLux) + "&Irradiance=" + String(gIrradiance) + "&Rain_intensity=" + String(gCurahHujan) + "";
        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);

        // You can comment the httpRequestData variable above
        // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
        // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";// Send HTTP POST request
        int httpResponseCode = https.POST(httpRequestData);

        // If you need an HTTP request with a content type: text/plain
        // http.addHeader("Content-Type", "text/plain");
        // int httpResponseCode = http.POST("Hello, World!");

        // If you need an HTTP request with a content type: application/json, use the following:
        // http.addHeader("Content-Type", "application/json");
        // int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

        if (httpResponseCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Resp. code: ");
          lcd.print(httpResponseCode);
          lcd.setCursor(0, 1);
          lcd.print("Server Connected");
          delay(2000);
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.print("NODE SINK");
          delay(2000);
        } else {
          lcd.clear();
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
          lcd.setCursor(0, 0);
          lcd.print("Resp. code: ");
          lcd.print(httpResponseCode);
          lcd.setCursor(0, 1);
          lcd.print("Server Failed!");
          delay(2000);
        }
        // Free resources
        https.end();
      }
    } else {
      Serial.println ("Unable to create clinet");
    }
  } else if (WiFi.status() != WL_CONNECTED) {
    // if you didn't get a connection to the server:
    Serial.println("Connection Failed!, please check the network!");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
//      digitalWrite(buz, HIGH);
//      delay(200);
//      digitalWrite(buz, LOW);
//      delay(800);
//      digitalWrite(buz, HIGH);
//      delay(200);
//      digitalWrite(buz, LOW);
//      delay(800);
      
      Serial.println ("Send data to mikro SD card");
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("NODE SINK");
      lcd.setCursor(4, 1);
      lcd.print("OFFLINE!");
      delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("NODE SINK");
    lcd.setCursor(4, 1);
    lcd.print("ONLINE!!");
    delay(2000);
  }
    setData();
  }
}

// empty the variable of sensor data
void setData(){
  gTemperature = 0;
  gHumidity = 0;
  gLux = 0;
  gIrradiance = 0;
  gCurahHujan = 0;
}
