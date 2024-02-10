//Bismillahirrahmanirrahim.
//Elhamdulillahirabbilalemin.

//Arduino Pin bağlantıları:
//Lcd malumat pinleri, yeşilden siyaha, A5'den A1'e bağlı. 
//Boştaki beyaz kablo 3.3v'a, siyah kablo gnd'ye bağlı.
//Beraber bantlanmış 6 kablo beyazdan maviye 2'den 7'ye bağlı.
//Beraber bantlanmış 4 kablo griden kahveye 8'den 11'e bağlı. 

//GELİŞTİRMELER:
//librarylerdeki gereksizleri silmek ve eeprom kullanmak çok hafıza kazandırabilir.
//Uyku moduna alınırken parametre 2 olursa uyandırmak için sıfırlamaya gerek kalmaz. Ama altında yazıyor şartları var.

#include <Keypad.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
SoftwareSerial sim(3, 2); //SIM800L Tx & Rx
//(CLK,DIN,D/C,CE,RST)
Adafruit_PCD8544 lcd = Adafruit_PCD8544(A1, A2, A3, A4, A5);

const char keys[4][3]  = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
const byte rowPins[4] = {4, 5, 6, 7};
const byte colPins[3] = {8, 9, 10};
Keypad tuslar = Keypad( makeKeymap(keys), rowPins, colPins, 4, 3);

char arm[3][11];
bool a = 0;
byte as = 0, sec = 0, ekr;
unsigned long mk = millis();

void setup()
{
  Serial.begin(9600);
  //LCD
  lcd.begin();
  lcd.setContrast(56);
  lcd.setTextColor(BLACK);
  lcd.setTextSize(1);
  anaEkr();
  //SIM
  sim.begin(9600);
  Serial.println(F("Baslatiliyor..."));
  delay(100);
  sim.println(F("AT"));
  pinMode(11, OUTPUT);
}

void loop()
{
  porGun();
  tusGun();
}



void armGor () {
  Serial.println(F("Son Aramalar:"));
  Serial.println(String(arm[2]));
  Serial.println(String(arm[1]));
  Serial.println(String(arm[0]));
}

void anaEkr() {
  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.println(F("Ara"));
  lcd.println(F("Arama Kaydi"));
  lcd.println(F("Uyku Hali"));
  lcd.println(F("Kapat"));
  lcd.display();
  ekr = 0;
  sec = 1;
  nok(sec);
}

void armEkr() {
  lcd.clearDisplay();
  lcd.setCursor(0, 0);
  lcd.print(F("Aramalar:"));
  lcd.print(as);
  lcd.setCursor(0, 8);
  for (int i = 0; i < as; i++)
    lcd.println(arm[as - i - 1]);
  lcd.display();
  if (as > 0) {
    sec = 2;
    nok(sec);
  }
  ekr = 1;
}

void tusGun() {
  char tus = char(tuslar.getKey());
  if (tus) {
    if (ekr == 0) {//--------------------------ANA EKRAN DURUMLARI---------------------------
      switch (tus) {
        case '4': //---------------------------4'SE ZİL SESİNİ AZALT-------------------------
          sim.println(F("AT+CRSL=20"));
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.println(F("Ses seviyesi:1"));
          lcd.display();
          delay(2000);
          anaEkr();
          break;
        case '6'://---------------------------6'YSA ZİL SESİNİ ARTIR------------------------
          sim.println(F("AT+CRSL=100"));
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.println(F("Ses seviyesi:3"));
          lcd.display();
          delay(2000);
          anaEkr();
          break;
        case '8'://---------------------------8-2 İMLEÇ KONTROLÜ------------------------
          sec++;
          nok(sec);
          break;
        case '2':
          sec--;
          nok(sec);
          break;
        case '7'://---------------------------7'YSE ANAMI ARA--------------------------
          sim.println(F("ATD+ +905304601837;"));
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.println(F("Anam\naraniyor..."));
          lcd.display();
          while (tus != '\0') tus = tuslar.getKey();
          while (tus == '0') tus = tuslar.getKey();
          anaEkr();
          break;
        case '9'://---------------------------9'SA BABAMI ARA--------------------------
          sim.println(F("ATD+ +905378738499;"));
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.println(F("Babam\naraniyor..."));
          lcd.display();
          while (tus != '\0') tus = tuslar.getKey();
          while (tus == '0') tus = tuslar.getKey();
          anaEkr();
          break;
        case '5'://---------------------------5 - MİKROFON AÇ/KAPAT -------------------
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          if (a) {
            sim.println(F("AT+CEXTERNTONE=0"));
            lcd.println(F("Mikrofon\nacildi."));
            a = 0;
          } else {
            sim.println(F("AT+CEXTERNTONE=1"));
            lcd.println(F("Mikrofon\nkapatildi."));
            a = 1;
          }
          lcd.display();
          delay(2000);
          anaEkr();
          break;
        case '#'://---------------------------# - MENÜDEN SEÇME İŞLEMLERİ--------------
          switch (sec) {
            case 1: { //----------------------1 - NUMARA ARA---------------------------
                byte has = 8;
                char etus;
                String numlar = String(F("ATD+ +900000000000;"));
                while (tus != '\0') tus = char(tuslar.getKey());
                lcd.clearDisplay();
                lcd.setCursor(0, 0);
                lcd.print(F("Numara:\n+90"));
                lcd.display();
                etus = tus;
                while (has < 18) {
                  while (tus == etus) tus = char(tuslar.getKey());
                  while (tus == '\0') tus = char(tuslar.getKey());
                  if (tus == '*') {
                    has--;
                    lcd.fillRect((has - 5) * 6, 8, 5, 7, WHITE);
                    lcd.setCursor((has - 5) * 6, 8);
                  } else {
                    numlar[has] = tus;
                    lcd.print(tus);
                    has++;
                  }
                  lcd.display();
                  etus = tus;
                }
                while (tus != '\0') tus = char(tuslar.getKey());
                lcd.print('\n');
                lcd.print(F("Ara?"));
                lcd.display();
                while (tus == '\0') tus = char(tuslar.getKey());
                if (tus == '#')
                  sim.println(numlar);
                anaEkr();
                break;
              }
            case 2://-------------------------2 - ARAMA KAYDI--------------------------
              armEkr();
              break;
            case 3://-------------------------3 - UYKU HALİ----------------------------
              lcd.clearDisplay();
              lcd.setCursor(0, 0);
              sim.println(F("AT+CSCLK=1"));
              lcd.println(F("Sim uykuya\nalindi."));
              lcd.display();
              delay(2000);
              anaEkr();
              break;
            case 4://-------------------------4 - SİM'İ KAPAT--------------------------
              lcd.clearDisplay();
              lcd.setCursor(0, 0);
              sim.println(F("AT+CPOWD=1"));
              lcd.println(F("Sim\nkapatiliyor..."));
              lcd.display();
              delay(2000);
              anaEkr();
              break;
          }
          break;
      }
    } else if (ekr == 1) {//------------------ARAMA EKRANI DURUMLARI------------------
      switch (tus) {
        case '8'://---------------------------8-2 İMLEÇ KONTROLÜ-----------------------
          sec++;
          nok(sec);
          break;
        case '2':
          sec--;
          nok(sec);
          break;
        case '*'://---------------------------* - CEVAPSIZ ARAMALARI SİL---------------
          for (int m = 0; m < 3; m++)
            for (int i = 0; i < 10; i++)
              arm[m][i] = "";
          as = 0;
          armEkr();
          break;
        case '5'://---------------------------5 - ANA EKRANA DÖN-----------------------
          anaEkr();
          break;
        case '#': //--------------------------# - GELEN ARAMALARDAN SEÇ----------------
          String numara = String(F("+90")) + (String(arm[as + 1 - sec]));
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.println(numara);
          lcd.println(F("Ara?"));
          lcd.display();
          while (tus != '\0') tus = char(tuslar.getKey());
          while (tus == '\0') tus = char(tuslar.getKey());
          if (tus == '#') {
            numara = String(F("ATD+ ")) += (numara += ';');
            sim.println(numara);
            as--;
            for (int i = 0; i < 10; i++)
              arm[as][i] = "";
            armEkr();
          } else {
            armEkr();
          }
          break;
      }
    }
    switch (tus) {//--------------------------GENEL DURUMLAR---------------------------
      case '1'://-----------------------------1 - ARAMAYI KAPAT------------------------
        sim.println(F("ATH"));
        break;
      case '3'://-----------------------------3 - ARAMAYI AÇ---------------------------
        sim.println(F("ATA"));
        break;
      case '0'://-----------------------------0 - SIFIRLA------------------------------
        lcd.clearDisplay();
        lcd.setCursor(0, 0);
        lcd.println(F("Sifirlaniyor.."));
        lcd.display();
        digitalWrite(11, HIGH);
        delay(120);
        digitalWrite(11, LOW);
        delay(9000);
        anaEkr();
        break;
    }
  }
}

//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________


void porGun() {//-----------------------------BİLGİSAYAR - SİM MUHABERATI--------------
  //------------------------------------------SERIAL'DAN BİR ŞEY GELMİŞSE--------------
  if (Serial.available() > 0) {
    char gln[35];
    delay(50);
    byte i = 0;
    int ta = Serial.available();
    while (Serial.available() > 0 && i <= 34) {
      gln[i] = Serial.read();
      i++;
    }
    //----------------------------------------GELENİ MURAKEBE ET-----------------------
    String kod = String(gln).substring(0, 3);
    if (kod == F("SFR")) { //-----------------"SFR" - SIFIRLA--------------------------
      Serial.println(F("Sifirlaniyor..."));
      digitalWrite(11, HIGH);
      delay(120);
      digitalWrite(11, LOW);
    } else if (kod == F("UYK")) { //----------"UYK" - UYKU HALİ------------------------
      Serial.println(F("Uykuya alindi."));
      sim.println(F("AT+CSCLK=1"));
      porGun();
    } else if (kod == F("ARM")) { //----------"ARM" - ARAMA KAYDI----------------------
      armGor();
    } else if (kod == F("MIK")) { //----------"MIK" - MİKROFONU AÇ/KAPA----------------
      if (a) {
        sim.println(F("AT+CEXTERNTONE=0"));
        Serial.println(F("Mikrofon açildi."));
        a = 0;
      } else {
        sim.println(F("AT+CEXTERNTONE=1"));
        Serial.println(F("Mikrofon kapatildi."));
        a = 1;
      }
    } else { //-------------------------------KOD DEĞİLSE AYNEN GÖNDER-----------------
      i = 0;
      while (i <= 34 && i < ta) {
        sim.write(gln[i]);
        i++;
      }
      while (Serial.available() > 0) {
        delay(2);
        sim.write(Serial.read());
      }
    }
  }
  //------------------------------------------SIM'DEN BİR ŞEY GELMİŞSE------------------------
  if (sim.available() > 0) {
    char gln[35];
    delay(60);
    byte i = 0;
    int ta = sim.available();
    while (sim.available() > 0 && i <= 34) {
      gln[i] = sim.read();
      i++;
    }
    //----------------------------------------SIM'DEN GELENİ MURAKEBE ET-----------------------
    //----------------------------------------TELEFON ÇALIYORSA NUMARAYI KAYDET----------------
    byte d = String(gln).indexOf('R');
    if (String(gln).substring(d, d + 4) == F("RING") && ta == 48) {
      if ((millis() - mk) > 10000) {
        String numara = String(gln).substring(d + 19, d + 29);
        for (int i = 0; i < 11; i++)
          arm[as][i] = numara[i];
        as++;
        armEkr();
      }
      mk = millis();
      //--------------------------------------ÇALAN TELEFON AÇILMIŞ/KAPATILMIŞSA KAYDEDİLENİ SİL
    } else if ((String(gln).substring(0, 3) == F("ATA") || String(gln).substring(0, 3) == F("ATH")) && (millis() - mk) < 10000) {
      as--;
      for (int i = 0; i < 10; i++)
        arm[as][i] = ' ';
      anaEkr();
    }
    //----------------------------------------SIM'DEN GELENİ GÖNDER-----------------------------
    i = 0;
    while (i <= 34 && i < ta) {
      Serial.write(gln[i]);
      i++;
    }
    while (sim.available() > 0) {
      delay(2);
      Serial.write(sim.read());
    }
  }
}

void nok(byte sira) {
  lcd.fillRect(79, 0, 5, 48, WHITE);
  sira = 8 * sira - 5;
  lcd.fillCircle(81, sira, 2, BLACK);
  lcd.display();
}

//HATALAR:
//Rastgele: KAPATIP AÇ.
//PUC: Soft Serial pinlerini input yaptıysan. Donanımla alakalı.

//Değişiklik kaydı:
//31.12.20 12.14: Tuştakımı ve sim reset kablolarının bağlandıkları pinler değiştirildi.
//                Kablolar bantlanıp bağlantılar yukarıya yazılarak Arduino çıkarıldı.
//28.12.20 21.44: LCD'nin pinlerinin bağlandığı pinler değiştirildi.
