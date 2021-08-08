// 11-05-18
/*******************************
   SARSAT decodeur 
   Oled Display 128x64
   F4GMU
   Jan_2018
   
   Detresse/AutoTest      OK
   Longueur Trame         OK
   Pays                   OK
   4 protocoles           OK   
   Coord Nat location     OK
   Offset coord           OK
   Coord Sdt location     OK
   Offset coord           OK
   Coord User loction     OK   
   HEX Id                 OK
   Buzzer trame suivante  OK

   Corriger N49.232 N49.0232 OK std loc prot
   
   *Loop Test406 sur trame courte
   *Alarme Batterie
   *AUX 121.5
********************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*******************************
  Configuration Oled
********************************/
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

/********************************
  Definitions des constantes
*********************************/
const int ReceiverPin = 2;
unsigned long microseconds;
unsigned long duree_palier, pays;
long latdeg, latmin, londeg, lonmin, latofmin, latofsec, lonofmin, lonofsec, protflag;
boolean longtrame, protocole, latflag, lonflag, latoffset, lonoffset;
boolean der_bit = 1; // debut de trame à 1
boolean etat = 0; // debut de trame à 1
byte start_flag = 0;
byte count;
byte count_oct;
byte data_demod;
const byte Nb_octet = 17; //3 + 15 **18 octets 1er FF  = octet 0**
byte octet[18];
float vout = 0.0;   // pour lecture tension batterie
float vin = 0.0;    // pour lecture tension batterie
int value = 0;      // pour lecture tension batterie

/*******************************
  Setup
********************************/
void setup()
{
  pinMode(ReceiverPin, INPUT);          // entree sur interruption 0
  pinMode(13, OUTPUT);                  // sortie LED pin 19
  pinMode(3, INPUT);                    // entree batterie pin 26
  pinMode(7, OUTPUT);                   // sortie Buzzer pin 13
  
  Serial.begin(9600);
  attachInterrupt(0, analyze, CHANGE);  // interruption sur Rise et Fall

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialise Oled I2C addr 0x3C
  display.clearDisplay();  
}

/********************************
   Loop
*********************************/
void loop()
{
  if (count_oct == Nb_octet)
  {
    /*for ( byte i = 0; i < Nb_octet; i++) // RAW data
    {
      if (octet[i] < 16)
      Serial.print('0');
      Serial.print (octet[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");*/

    if (((octet[1] == 254) && (octet[2] == 208)) || ((octet[1] == 254) && (octet[2] == 47)))// 0xFE 0xD0 autotest
    {
      test406();
      ledblink();
      voltmetre();

      count_oct = 0;     // repart pour trame suivante
      count = 0;
      start_flag = 0;
      data_demod = 0;
      der_bit = 1;
      etat = 1;
      
      return;
    } 
  }  
}

/***********************************************
  Extraction des bits
***********************************************/
void analyze(void)
{
  duree_palier = micros() - microseconds;
  microseconds = micros();

  if (duree_palier > 1500 && duree_palier < 1700) {
    return;
  }
  if (duree_palier > 1150 && duree_palier < 1350 && der_bit == 0) {   // si T = 125ms ET der_bit == 0; der_bit = 1
    der_bit = 1;
    return;
  }
  if (duree_palier > 1150 && duree_palier < 1350 && der_bit == 1) {    // si T = 125ms ET der_bit == 1; bit = etat
    data_demod = data_demod << 1 | etat; // ajout du 1 en fin
    der_bit = 0;
  }
  if (duree_palier > 2450 && duree_palier < 2650) {    // si T = 250ms; bit = !etat
    etat = !etat;
    data_demod = data_demod << 1 | etat; // ajout du 1 en fin
    der_bit = 0;
  }
  Test();
}


/***************************************
  Test octets recu et transfert data
  header 0xFF 0xFE 
****************************************/

void Test()
{
  if (start_flag == 0) {
    if (data_demod == 255) {         // si 0xFF recu, start_flag = 1
      octet[0] = data_demod;
      start_flag = 1;
      count = 0;
      count_oct = 1;
      data_demod = 0;
    }
    else {
      return;
    }
  }

  if (start_flag == 1) {   //si 0xFF recu
    if (data_demod == 254) { // si 0xFE
      octet[1]= data_demod;
      start_flag = 2;
      count = 0;
      count_oct = 2;
      data_demod = 0;
    }
    else {
      return;
    }    
  }  
   
  if (start_flag == 2) {   //si 0xFE recu
    if (data_demod == 208 || data_demod == 47) { // si 0xD0 ou 2F
      octet[2]= data_demod;
      start_flag = 3;
      count = 0;
      count_oct = 3;
      data_demod = 0;
    }
    else {
      return;
    }    
  }
  
  else if (start_flag == 3) {           //si 0xFE recu 
    if (count == 7) {                   //si nombre de bits = 8
      octet[count_oct] = data_demod;    //data dans octet numero xx
      count_oct ++;
      data_demod = 0;
      count = 0;
    }
    else if (count < 7) {
      count ++;
    }
  }
}

/*****************************
  Routine autotest 406
******************************/
void test406()
{
  longtrame = (octet[3] & 0x80) >> 7;      // taille         bit 25
  protocole = (octet[3] & 0x40) >> 6;      // protocole      bit 26
  //protflag = (octet[4] & 0x08) >> 3;       // protocole flag bit 37
  protflag = (octet[4] & 0x0F);            // protocole flag bit 37-40
  pays = ((octet[3] & 0x3F) << 4 | (octet[4] & 0xF0) >> 4); // pays bit 27-36
  pays = pays & 0x3FF; // pays
    
  display.clearDisplay();        // rafraichissement Oled
  display.setTextSize(1);
  display.setTextColor(WHITE); 
  display.setCursor(0, 0);       // Balise TEST ou DETRESSE
  if (octet[2] == 208) {
    display.println("AutoTest 406 F4GMU");
    Serial.println("AutoTest 406 F4GMU");    
  }
  else if (octet[2] == 47) {
    display.println("DETRESSE 406 F4GMU");
    Serial.println("DETRESSE 406 F4GMU");
  }
           
  if (longtrame == 1 && protocole == 1) {                    // User loc protocol trame longue
    
    latflag = (octet[13] & 0x10) >> 4;   
    latdeg = ((octet[13] & 0x0F) << 3 | (octet[14] & 0xE0) >> 5);
    latmin = (octet[14] & 0x1E) >> 1;
    latmin = (latmin * 4);
    lonflag = (octet[14] & 0x01);           
    londeg = (octet[15]);
    lonmin = (octet[16] & 0xF0) >> 4;
    lonmin = (lonmin * 4);

    
    display.setCursor(0, 10);
    display.println("User loc. Protocol");
    Serial.println("User location Protocol");   

    display.setCursor(0, 30); // Oled Latitude N/S 
    if (latdeg == 127 || londeg == 255) {
      display.print("GPS non synchronise");
    }
    else {
    if (latflag == 0) {
      display.print("N");
      Serial.print("N");
    }
    else {
      display.print("S");
      Serial.print("S");
    }
    display.print(latdeg, DEC);
    display.print(".");
    display.print(latmin, DEC);
    Serial.print(latdeg, DEC);
    Serial.print(".");
    Serial.print(latmin, DEC);
    
    if (lonflag == 0) {
      display.print("  E");
      Serial.print("  E");
    }
    else {
      display.print("  W");
      Serial.print("  W");
    }
    display.print(londeg, DEC);
    display.print(".");
    display.print(lonmin, DEC);
    Serial.print(londeg, DEC);
    Serial.print(".");
    Serial.println(lonmin, DEC);
    }

    display.setCursor(0, 40);      // HEX ID 30 Hexa
    for ( byte i = 3; i < 18; i++) {
      if (octet[i] < 16)
      display.print('0');
      display.print (octet[i], HEX);
      display.print(" ");
    }
  }
  else if (longtrame == 1 && protocole == 0 && (protflag == 8 || protflag == 10 || protflag == 11)) { //Nat loc protocole trame longue

    latoffset = (octet[14] & 0x80) >> 7;   // Latitude NAT LOCATION
    latflag = (octet[7] & 0x20) >> 5;   
    latdeg = ((octet[7] & 0x1F) << 2 | (octet[8] & 0xC0) >> 6);
    latmin = (octet[8] & 0x3E) >> 1;
    latmin = (latmin * 2);
    latofmin = (octet[14] & 0x60) >> 5;
    latofsec = (octet[14] & 0x1E) >> 1;
    latofsec = (latofsec * 4);
    if (latoffset == 1) {
      latmin = (latmin + latofmin);
      latmin = (latmin * 10000);
      latmin = (latmin / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      latmin = (latmin + latofsec);
    }
    else if (latoffset == 0) {
      latmin = (latmin - latofmin);
      if (latmin < 0) {
        latmin = (60 - abs(latmin));
        latdeg = (latdeg - 1);
      }
      latmin = (latmin * 10000);
      latmin = (latmin / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      latmin = (latmin - latofsec);
    }

    lonoffset = (octet[14] & 0x01);       //Longitude NAT LOCATION
    lonflag = (octet[8] & 0x01);           
    londeg = (octet[9]);
    lonmin = (octet[10] & 0xF8) >> 3;
    lonmin = (lonmin * 2);
    lonofmin = (octet[15] & 0xC0) >> 6;
    lonofsec = (octet[15] & 0x3C) >> 2;
    lonofsec = (lonofsec * 4);
    if (lonoffset == 1) {
      lonmin = (lonmin + lonofmin);
      lonmin = (lonmin * 10000);
      lonmin = (lonmin / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      lonmin = (lonmin + lonofsec);
    }
    else if (lonoffset == 0) {
      lonmin = (lonmin - lonofmin);
      if (lonmin < 0) {
        lonmin = (60 - abs(lonmin));
        londeg = (londeg - 1);
      }
      lonmin = (lonmin * 10000);
      lonmin = (lonmin / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      lonmin = (lonmin - lonofsec);
    }
    
    
    
    display.println("National Protocol");
    Serial.println("National Location Protocol");
    
    display.setCursor(0, 30); // Oled Latitude N/S 
    if (latdeg == 127 || londeg == 255) {
      display.print("GPS non synchronise");
    }
    else {
    if (latflag == 0) {
      display.print("N");
      Serial.print("N");
    }
    else {
      display.print("S");
      Serial.print("S");
    }
    display.print(latdeg, DEC);
    display.print(".");
    display.print(latmin, DEC);
    Serial.print(latdeg, DEC);
    Serial.print(".");
    Serial.print(latmin, DEC);
    
    if (lonflag == 0) {
      display.print("  E");
      Serial.print("  E");
    }
    else {
      display.print("  W");
      Serial.print("  W");
    }
    display.print(londeg, DEC);
    display.print(".");
    display.print(lonmin, DEC);
    Serial.print(londeg, DEC);
    Serial.print(".");
    Serial.println(lonmin, DEC);
    }

    display.setCursor(0, 40);      // HEX ID 30 Hexa
    for ( byte i = 3; i < 18; i++) {
      if (octet[i] < 16)
      display.print('0');
      display.print (octet[i], HEX);
      display.print(" ");
    }
  }
  else if (longtrame == 1 && protocole == 0 && (protflag < 8 || protflag == 14)) { //Std loc protocol trame longue

    latoffset = (octet[14] & 0x80) >> 7;  //Latitude STD LOCATION
    latflag = (octet[8] & 0x80) >> 7; 
    latdeg = (octet[8] & 0x7F);
    latmin = (octet[9] & 0xC0) >> 6;
    latmin = (latmin * 15);
    latofmin = (octet[14] & 0x7C) >> 2;
    latofsec = ((octet[14] & 0x03) << 2 | (octet[15] & 0xC0) >> 6);
    latofsec = (latofsec * 4);
    if (latoffset == 1) {
      latmin = (latmin + latofmin);
      latmin = (latmin * 10000);
      latmin = (latmin / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      latmin = (latmin + latofsec);
    }
    else if (latoffset == 0) {
      latmin = (latmin - latofmin);
      if (latmin < 0) {
        latmin = (60 - abs(latmin));
        latdeg = (latdeg - 1);
      }
      latmin = (latmin * 10000);
      latmin = (latmin / 60);
      latofsec = (latofsec * 10000);
      latofsec = (latofsec / 3600);
      latmin = (latmin - latofsec);
    }
    
    lonoffset = (octet[15] & 0x20) >> 5;  //Longitude STD LOCATION
    lonflag = (octet[9] & 0x20) >> 5; 
    londeg = ((octet[9] & 0x1F) << 3 | (octet[10] & 0xE0) >> 5);
    lonmin = (octet[10] & 0x18) >> 3;
    lonmin = (lonmin * 15);
    lonofmin = (octet[15] & 0x1F);
    lonofsec = (octet[16] & 0xF0) >> 4;
    lonofsec = (lonofsec * 4);
    if (lonoffset == 1) {
      lonmin = (lonmin + lonofmin);
      lonmin = (lonmin * 10000);
      lonmin = (lonmin / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      lonmin = (lonmin + lonofsec);
    }
    else if (lonoffset == 0) {
      lonmin = (lonmin - lonofmin);
      if (lonmin < 0) {
        lonmin = (60 - abs(lonmin));
        londeg = (londeg - 1);
      }
      lonmin = (lonmin * 10000);
      lonmin = (lonmin / 60);
      lonofsec = (lonofsec * 10000);
      lonofsec = (lonofsec / 3600);
      lonmin = (lonmin - lonofsec);
    }
    
    display.println("Standard Protocol");
    Serial.println("Standard Location Protocol");
    
    display.setCursor(0, 30); // Oled Latitude N/S 
    if (latdeg == 127 || londeg == 255) {
      display.print("GPS non synchronise");
    }
    else {
    if (latflag == 0) {
      display.print("N");
      Serial.print("N");
    }
    else {
      display.print("S");
      Serial.print("S");
    }
    display.print(latdeg, DEC);
    display.print(".");
      if (latmin < 0x0A) {
        display.print("000");
        display.print(latmin, DEC);
      }
      if (latmin < 0x64) {
        display.print("00");
        display.print(latmin, DEC);
      }
      if (latmin < 0x3E8) {
        display.print("0");
        display.print(latmin, DEC);
      }
      else {
        display.print(latmin, DEC);
      }
    Serial.print(latdeg, DEC);
    Serial.print(".");
    Serial.print(latmin, DEC);
    if (lonflag == 0) {
      display.print("  E");
      Serial.print("  E");
    }
    else {
      display.print("  W");
      Serial.print("  W");
    }
    display.print(londeg, DEC);
    display.print(".");
      if (lonmin < 0x0A) {
        display.print("000");
        display.print(lonmin, DEC);
      }
      if (lonmin < 0x64) {
        display.print("00");
        display.print(lonmin, DEC);
      }
      if (lonmin < 0x3E8) {
        display.print("0");
        display.print(lonmin, DEC);
      }
      else {
        display.print(lonmin, DEC);
      }
    Serial.print(londeg, DEC);
    Serial.print(".");
    Serial.println(lonmin, DEC);
    }

    display.setCursor(0, 40);      // HEX ID 30 Hexa
    for ( byte i = 3; i < 18; i++) {
      if (octet[i] < 16)
      display.print('0');
      display.print (octet[i], HEX);
      display.print(" ");
    }
    
  }
  else {                                // User protocol trame courte
    display.println("User Protocol");
    Serial.println("User Protocol");
    display.setCursor(5, 30);
    display.println("22 HEX. No location");
    
    display.setCursor(0, 40);      // HEX ID 22 Hexa bit 26 to 112
    for ( byte i = 3; i < 14; i++) {
      if (octet[i] < 16)
      display.print('0');
      display.print (octet[i], HEX);
      display.print(" ");
    }
    
  }
  display.setCursor(0, 20);     // Oled Pays
  if (pays == 226 || pays == 227 || pays == 228) {
    display.println("Pays= FRANCE");
    Serial.println("Pays= FRANCE");
  }
  else {
    display.print("Pays= ");
    Serial.print("Pays : ");
    display.println(pays, DEC);
    Serial.println(pays, DEC);
  }
  
  display.setCursor(80, 20); // Oled Voltmetre
  display.print("V= ");
  display.print(vin);

  display.display();

  count_oct = 0;     // repart pour trame suivante
  count = 0;
  start_flag = 0;
  data_demod = 0;
  der_bit = 1;
}

/****************************************
 * LED trame recu
 ****************************************/
void ledblink()
  {
  digitalWrite(13, HIGH);  // Clignotement LED Trame décodée
  delay(100);
  digitalWrite(13, LOW); 
  /*delay(45000);            // Alarme Buzzer Trame suivante
  digitalWrite(7, HIGH);  
  delay(1500);
  digitalWrite(7, LOW);*/
  }

/***************************************
 * Voltmetre sur A5
 ***************************************/
void voltmetre()
  {
    value = analogRead(3);
    vout = (value * 5.0) / 1024.0;
    vin = vout / (0.088); // vin = vout / (R2/(R1+R2)) R1=100k R2=10k
    if (vin<0.09) {
      vin=0.0;//Condition pour eviter des lectures indesirable !
    }
  }
