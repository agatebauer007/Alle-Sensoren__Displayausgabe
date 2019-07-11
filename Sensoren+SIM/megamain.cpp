//----------------------------------------------------------------------------------------------------------------------------//
#include "Arduino.h"
#include "LiquidCrystal.h"
#include "DHT_U.h"
#include "DHT.h"
#include "DS3231.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_CCS811.h"
#include "SoftwareSerial.h"


//----------------------------------------------------------------------------------------------------------------------------//
#define DHTPIN 6 //Input des "Data"-Kabels auf Pin-6 --> Luftfeuchte
#define DHTTYPE DHT22 // DHT11, DHT21, DHT22 --> Typ des Sensors


//----------------------------------------------------------------------------------------------------------------------------//
DHT dht(DHTPIN, DHTTYPE); // Initialisierung des Luftfeuchte Sensors

DS3231  rtc(SDA, SCL);

LiquidCrystal lcd(13, 12, 5, 4, 3, 2); // Initialisieren mit den verwendeten Pins des Displays am Arduino Board

Adafruit_BMP280 bmp(34, 32, 30, 28);

Adafruit_CCS811 ccs; // Initialisierung

SoftwareSerial SIM900(10, 11); // Konfiguration des Software Serial Ports auf Pins 10 und 11

String incoming_char_str;

float sensor_volt; // Deklaration der nötigen Variablen

int CO2_Gehalt;
int TVOC_Gehalt;


//----------------------------------------------------------------------------------------------------------------------------//
void send_SMS_Status() {

  SIM900.print("AT+CMGF=1\r");  // AT Kommando um SIM900 in SMS Modus zu versetzen
  delay(100);

  SIM900.println("AT + CMGS = \"+4915734064246\""); // Sende SMS an folgende Nummer --> Internationales Format
  delay(100);
  
  
  SIM900.println("STATUS WOHNUNG"); // SMS Inhalt -- Zeilenumbrüche über neue println moeglich
  
  SIM900.println();

  SIM900.print(rtc.getDOWStr(FORMAT_SHORT));
  SIM900.print(". ");
  SIM900.print(rtc.getDateStr(FORMAT_LITTLEENDIAN));
  SIM900.print(", ");
  SIM900.print(rtc.getTimeStr());
  SIM900.print("\n");
  
  SIM900.println();
  SIM900.println();
  
  SIM900.print("Temperatur: ");
  SIM900.print(dht.readTemperature());
  SIM900.print(" *C\n");
   
  SIM900.print("Luftfeuchte: ");
  SIM900.print(dht.readHumidity());
  SIM900.print(" %\n");
   
  SIM900.print("MQ9-Spannung: ");
  SIM900.print(sensor_volt);
  SIM900.print(" V\n");
  
  SIM900.print("Luftdruck: ");
  SIM900.print(bmp.readPressure());
  SIM900.print(" Pa\n");
  
  SIM900.print("CO2: ");
  SIM900.print(CO2_Gehalt);
  SIM900.print(" ppm\n");

  SIM900.print("TVOC: ");
  SIM900.print(TVOC_Gehalt);
  SIM900.print(" ppbb");

  delay(100);

  
  SIM900.println((char)26); // AT Kommandos beenden mit ^Z, ASCII Tabelle Nummer 26
  delay(100);

  
  //delay(5000); // Zeit für das Modul um Nachricht zu senden
}


//----------------------------------------------------------------------------------------------------------------------------//
void SIM900power() // Funktion zum starten des Shields über des Arduino 
{
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH); // Strom für 5 Sekunden, davor und danach wieder aus
  delay(3000);
  digitalWrite(9,LOW);
  delay(3000);
}


//----------------------------------------------------------------------------------------------------------------------------//
void check_SMS_Status()
{
  if(SIM900.available() >0) { // Zeige alle einkommenden Nachrichten auf dem Serial Monitor
    
    incoming_char_str=SIM900.readString(); // SIM wird ausgelesen und in vorher deklarierte Variable abgespeichert

    incoming_char_str.toUpperCase(); // Konvertieren der Nachricht in eine Nachricht aus reinen Großbuchstaben
  
    
    if (incoming_char_str.indexOf("+CMT")>=0) // AT Kommando CMT steht für empfangene Nachrichten --> soll enthalten sein im String damit Schleife ausgeführt wird
    {
      if (incoming_char_str.indexOf("STATUS")>=0) // Nachricht soll "STATUS" enthalten, damit if Schleife ausgeführt wird
      {
        send_SMS_Status();
      }
      else{}
    }
    else{}
  }
  else{}
}





//----------------------------------------------------------------------------------------------------------------------------//
void setup() {
 // SIM900power();

 //----------------------------------------------------------------------------------------------------------------------------//
  lcd.begin(16, 2); // Setup des Displays mit Größe
  dht.begin(); // Setup des Feuchte Sensors
  rtc.begin();
  ccs.begin();
  SIM900.begin(19200); // Arduino kommuniziert bei einer Baud Rate von 19200

 //----------------------------------------------------------------------------------------------------------------------------//
  if (!bmp.begin()) {
    lcd.setCursor(0,0);
    lcd.print("  BMP280 nicht"); // 
    lcd.setCursor(0,1);          // Sollte der BMP280-Sensor nicht gefunden werden Fehlermeldung ausgeben 
    lcd.print("    gefunden");   //
    while (1);
  }

 //----------------------------------------------------------------------------------------------------------------------------//
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
  Adafruit_BMP280::SAMPLING_X2,                     /* Temp. oversampling */
  Adafruit_BMP280::SAMPLING_X16,                    /* Pressure oversampling */
  Adafruit_BMP280::FILTER_X16,                      /* Filtering. */
  Adafruit_BMP280::STANDBY_MS_500);                 /* Standby time. */

 //----------------------------------------------------------------------------------------------------------------------------//                            
  if(!ccs.begin()){
    lcd.setCursor(0,0);
    lcd.print("  CCS811 nicht"); // 
    lcd.setCursor(0,1);          // Sollte der CCS811-Sensor nicht gefunden werden Fehlermeldung ausgeben 
    lcd.print("    gefunden");   //
  while(1);
  }

 //----------------------------------------------------------------------------------------------------------------------------//
  // delay(15000);
  SIM900.print("AT+CMGF=1\r"); // AT Kommando um Shield in SMS Modus zu setzen
  delay(500);
}





//----------------------------------------------------------------------------------------------------------------------------//
void loop(){


 //----------------------------------------------------------------------------------------------------------------------------//
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.println("  Temperatur:   ");
  lcd.setCursor(0,1);
  lcd.print("    ");
  lcd.print(dht.readTemperature());
  lcd.print(" *C");

  check_SMS_Status();

  delay(3000); 


 //----------------------------------------------------------------------------------------------------------------------------//
  lcd.clear(); 
  lcd.setCursor(0, 0); 
  lcd.println("  Luftfeuchte:  "); 
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(dht.readHumidity());
  lcd.print(" %");

  check_SMS_Status();

  delay(3000);


 //----------------------------------------------------------------------------------------------------------------------------//
  lcd.clear();
  lcd.setCursor(0, 0); // Cursor auf erstes Feld des Display --> oben links
  lcd.print("  "); // Leerzeichen weil schön
  lcd.print(rtc.getDOWStr(FORMAT_SHORT)); // Ausgabe des Wochentags im kurzen Format --> siehe library
  lcd.print(".   "); // Punkt und Leerzeichen weil schön
  lcd.print(rtc.getDateStr(FORMAT_LITTLEENDIAN)); // Ausgabe des Datums im kurzen Format --> siehe library
  
  lcd.setCursor(0, 1); // Cursor auf erstes Feld der zweiten Zeile
  lcd.print("    "); // Leerzeichen weil schön
  lcd.print(rtc.getTimeStr(FORMAT_SHORT)); // Ausgabe der Zeit im Standart Format

  check_SMS_Status();

  delay(3000);
  

 //----------------------------------------------------------------------------------------------------------------------------//
  
  int sensorValue = analogRead(A0); // Definition der nötigen Variablen
  sensor_volt = ((float)sensorValue / 1024) * 5.0; // Definition einer bereits deklarierten Variable
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" MQ9-Spannung:");
  lcd.setCursor(0,1);
  lcd.print("      ");
  lcd.print(sensor_volt); // Ausgabe der Spannung auf dem Display
  lcd.print("V"); // Ungefährer Richtwert für ein Ausschlagen des Alarms wären etwa 0,4 - 0,5 V

  check_SMS_Status();

  delay(3000);
  

 //----------------------------------------------------------------------------------------------------------------------------//
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Luftdruck:");
  lcd.setCursor(0,1);
  lcd.print("   ");
  lcd.print(bmp.readPressure());
  lcd.print("Pa");

  check_SMS_Status();

  delay(3000);
 

 //----------------------------------------------------------------------------------------------------------------------------//
  if(ccs.available()){ // Falls Sensor gefunden, Block ausführen
    if(!ccs.readData()){ // Empfange Daten
      lcd.clear();
      lcd.setCursor(0,0); 
      lcd.print("      CO2:");
      lcd.setCursor(0,1);
      lcd.print("     ");
      lcd.print(ccs.geteCO2()); // Ausgabe des CO2 Gehalts
      lcd.print("ppm");
      CO2_Gehalt = ccs.geteCO2();
    }

    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   Keine Daten"); // Fehlermeldung für den Fall, dass Sensor gefunden wurde aber Daten nicht ausgelesen werden konnen
    }
  }

  else
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Nicht erreichbar"); // Fehlermeldung für den Fall, dass Sensor nicht gefunden wird
  }

  check_SMS_Status();

  delay(3000);
 

 //----------------------------------------------------------------------------------------------------------------------------//
  if(ccs.available()){ // Falls Sensor gefunden, Block ausführen
    if(!ccs.readData()){ // Empfange Daten
      lcd.clear();
      lcd.setCursor(0,0); 
      lcd.print("     TVOC:");
      lcd.setCursor(0,1);
      lcd.print("     ");
      lcd.print(ccs.getTVOC()); // Ausgabe des CO2 Gehalts
      lcd.print("ppb");
      TVOC_Gehalt = ccs.getTVOC();
    }
  }

  else
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Keine Daten!"); // Fehlermeldung für den Fall, dass Sensor gefunden wurde aber Daten nicht ausgelesen werden konnen
  }

  check_SMS_Status();

  delay(3000);
  
}
