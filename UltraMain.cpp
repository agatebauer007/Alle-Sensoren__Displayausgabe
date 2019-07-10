#include "Arduino.h"
#include "LiquidCrystal.h"
#include "DHT_U.h"
#include "DHT.h"
#include "DS3231.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_CCS811.h"



#define DHTPIN 6 //Input des "Data"-Kabels auf Pin-6 --> Luftfeuchte
#define DHTTYPE DHT22 // DHT11, DHT21, DHT22 --> Typ des Sensors

 
DHT dht(DHTPIN, DHTTYPE); // Initialisierung des Luftfeuchte Sensors

DS3231  rtc(SDA, SCL);

LiquidCrystal lcd(13, 12, 5, 4, 3, 2); // Initialisieren mit den verwendeten Pins des Displays am Arduino Board


Adafruit_BMP280 bmp(34, 32, 30, 28);

Adafruit_CCS811 ccs; // Initialisierung





//----------------------------------------------------------------------------------------------------------------------------//

void setup() {
  lcd.begin(16, 2); // Setup des Displays mit Größe
  
  dht.begin(); // Setup des Feuchte Sensors
  rtc.begin();
  ccs.begin();

  if (!bmp.begin()) {
    lcd.setCursor(0,0);
    lcd.print("  BMP280 nicht"); // 
    lcd.setCursor(0,1);          // Sollte der Sensor nicht gefunden werden Fehlermeldung ausgeben 
    lcd.print("    gefunden");   //
    while (1);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
                
  if(!ccs.begin()){
    lcd.setCursor(0,0);
    lcd.print("  CCS811 nicht"); // 
    lcd.setCursor(0,1);          // Sollte der Sensor nicht gefunden werden Fehlermeldung ausgeben 
    lcd.print("    gefunden");   //
  while(1);
  }
}

void loop(){
//--------------------------------------------------------------------------//
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.println("  Temperatur:   ");
  lcd.setCursor(0,1);
  lcd.print("    ");
  lcd.print(dht.readTemperature());
  lcd.print(" *C");

  delay(3000); 
//--------------------------------------------------------------------------//
 
  lcd.clear(); 
  lcd.setCursor(0, 0); 
  lcd.println("  Luftfeuchte:  "); 
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(dht.readHumidity());
  lcd.print(" %");

  delay(3000);
// --------------------------------------------------------------------------//

  lcd.clear();
  lcd.setCursor(0, 0); // Cursor auf erstes Feld des Display --> oben links
  lcd.print("  "); // Leerzeichen weil schön
  lcd.print(rtc.getDOWStr(FORMAT_SHORT)); // Ausgabe des Wochentags im kurzen Format --> siehe library
  lcd.print(".   "); // Punkt und Leerzeichen weil schön
  lcd.print(rtc.getDateStr(FORMAT_LITTLEENDIAN)); // Ausgabe des Datums im kurzen Format --> siehe library
  
  lcd.setCursor(0, 1); // Cursor auf erstes Feld der zweiten Zeile
  lcd.print("    "); // Leerzeichen weil schön
  lcd.print(rtc.getTimeStr(FORMAT_SHORT)); // Ausgabe der Zeit im Standart Format

  delay(3000);
  //-------------------------------------------------------------------------//

  float sensor_volt; // Deklaration der nötigen Variablen
  int sensorValue = analogRead(A0); // Definition der nötigen Variablen
  sensor_volt = ((float)sensorValue / 1024) * 5.0; // Definition einer bereits deklarierten Variable
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Spannung: ");
  lcd.setCursor(0,1);
  lcd.print("      ");
  lcd.print(sensor_volt); // Ausgabe der Spannung auf dem Display
  lcd.print("V"); // Ungefährer Richtwert für ein Ausschlagen des Alarms wären etwa 0,4 - 0,5 V

  delay(3000);
  //-------------------------------------------------------------------------//

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Luftdruck:");
  lcd.setCursor(0,1);
  lcd.print("   ");
  lcd.print(bmp.readPressure());
  lcd.print("Pa");

  delay(3000);
  //-------------------------------------------------------------------------//

    if(ccs.available()){ // Falls Sensor gefunden, Block ausführen
      if(!ccs.readData()){ // Empfange Daten
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("      CO2:");
        lcd.setCursor(0,1);
        lcd.print("     ");
        lcd.print(ccs.geteCO2()); // Ausgabe des CO2 Gehalts
        lcd.print("ppm");
      }
    }

    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  Keine Daten!"); // Fehlermeldung für den Fall, dass Sensor gefunden wurde aber Daten nicht ausgelesen werden konnen
    }

     delay(3000);
  //------------------------------------------------------------------------//

   if(ccs.available()){ // Falls Sensor gefunden, Block ausführen
      if(!ccs.readData()){ // Empfange Daten
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("     TVOC:");
        lcd.setCursor(0,1);
        lcd.print("     ");
        lcd.print(ccs.getTVOC()); // Ausgabe des CO2 Gehalts
        lcd.print("ppb");
      }
    }

    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("  Keine Daten!"); // Fehlermeldung für den Fall, dass Sensor gefunden wurde aber Daten nicht ausgelesen werden konnen
    }

     delay(3000);
  //------------------------------------------------------------------------//
  }
