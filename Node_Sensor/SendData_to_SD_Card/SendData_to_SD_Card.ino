#include <SPI.h>
#include <SD.h>
#include "DS3231_Simple.h"
DS3231_Simple Clock;
#define SD_CS 10
File myFile;
               
String dataIn;
String dt[5];
int i;
boolean parsing = false;

String data="";
void setup()
{
  Serial.begin(9600);
  SD.begin(10);
  Clock.begin();
  dataIn = ""; // parsing data

  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
  Serial.println("initialization failed!");
  while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("weather.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print("Date(dd:mm:yy), Time(hr:min:sec), Temperature(°C), Humidity(%), Light Intensity(LUX), Irradiance(W/m2), Rain Intensity(mm) \r\n");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening weather.txt");
  }
}

// parsing data
void olahData() {
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
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
}

void parsingData() {
  int j = 0;

  //kirim data yang telah diterima sebelumnya lalu menampilkannya pada serial monitor
  Serial.print("data masuk : ");
  Serial.print(dataIn);
  Serial.print("\n");

  dt[j] = "";     //inisialisasi variabel, (reset isi variabel)

  //proses parsing data
  for (i = 1; i < dataIn.length(); i++) {
    //pengecekan tiap karakter dengan karakter (#) dan (,)
    if ((dataIn[i] == '#') || (dataIn[i] == ','))
    {
      //increment variabel j, digunakan untuk merubah index array penampung
      j++;
      dt[j] = "";     //inisialisasi variabel array dt[j]
    }
    else
    {

      dt[j] = dt[j] + dataIn[i];    //proses tampung data saat pengecekan karakter selesai.

    }
  }

    DateTime waktu;
    waktu = Clock.read();
    
    myFile = SD.open("weather.txt",FILE_WRITE);
    if(myFile) {
    myFile.print(waktu.Day);
    myFile.print("/");
    myFile.print(waktu.Month);
    myFile.print("/");
    myFile.print(waktu.Year);
    myFile.print(", ");
    myFile.print(waktu.Hour);
    myFile.print(":");
    myFile.print(waktu.Minute);
    myFile.print(":");
    myFile.print(waktu.Second);
    myFile.print(", ");
    myFile.print(dt[0].toFloat()/100.0);
    myFile.print(" , ");
    myFile.print(dt[1].toFloat()/100.0);
    myFile.print(" , ");
    myFile.print(dt[2].toFloat()/100.0);
    myFile.print(" , ");
    myFile.print(dt[3].toFloat()/100.0);
    myFile.print(" , ");
    myFile.println(dt[4].toFloat()/100.0);
    myFile.close();    
    Serial.println("Data berhasil disimpan");

    }else{
    Serial.println("Error menyimpan data");
    }
}

void loop (){
  olahData();
}
