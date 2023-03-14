/*
  Read RFID based payment system Prototype

  RC522 Interfacing with NodeMCU

   Typical pin layout used:
   ----------------------------------
               MFRC522      Node
               Reader/PCD   MCU
   Signal      Pin          Pin
   ----------------------------------
   SPI MOSI    MOSI         23 (GPIO23)
   SPI MISO    MISO         19 (GPIO19)
   SPI SCK     SCK          18 (GPIO18)
   SPI SS      SDA(SS)      21 (GPIO21)
   RST/Reset   RST          22 (GPIO22)
   IRQ         IRQ          N/C
   3.3V        3.3V         3.3V
   GND         GND          GND

*/

#include <SPI.h>
#include <MFRC522.h>
#include <WiFiManager.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define LCD_SDA 13
#define LCD_SCL 14

#define RST_PIN 22
#define SS_PIN 21

#define SUCCESS_LED_PIN 2
#define ERR_LED_PIN 4
#define TRIGGER_PIN 32

LiquidCrystal_I2C lcd(0x27, 20, 4);

const long interval = 2000; // interval at which to lit (milliseconds)
unsigned int timeout = 120; // seconds to run for wifi connection
unsigned long turnOnTime;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  pinMode(SUCCESS_LED_PIN, OUTPUT);
  digitalWrite(SUCCESS_LED_PIN, LOW);
  pinMode(ERR_LED_PIN, OUTPUT);
  digitalWrite(ERR_LED_PIN, LOW);

  Serial.print("\n The status of Trigger pin is = ");
  Serial.print(digitalRead(TRIGGER_PIN));

  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.home();
  lcd.setCursor(0, 0); // Move the cursor at origin
  lcd.print("STARTING...");

  // WiFiManager wm;
  // if (digitalRead(TRIGGER_PIN) == LOW)
  // {

  //   // reset settings - for testing
  //   // wm.resetSettings();

  //   // set configportal timeout
  //   wm.setConfigPortalTimeout(timeout);

  //   if (!wm.startConfigPortal("swivtIOT", "123456789"))
  //   {
  //     Serial.println("failed to connect and hit timeout");
  //     delay(3000);
  //     // reset and try again, or maybe put it to deep sleep
  //     ESP.restart();
  //     delay(5000);
  //   }
  //   // if you get here you have connected to the WiFi
  //   Serial.println("connected.. :)");
  // }
  // else
  // {
  //   // connect to existing wifi credentials
  //   wm.autoConnect();
  // }

  Serial.println(WiFi.SSID());

  lcd.setCursor(0, 0); // Move the cursor at origin
  lcd.print("Connected to");
  lcd.setCursor(0,1); 
  lcd.print(WiFi.SSID());

  Serial.println("\n *************Starting*************");

  SPI.begin();     // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (rfid.PICC_ReadCardSerial())
  {
    for (byte i = 0; i < 4; i++)
    {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);

    lcd.clear();
    lcd.setCursor(0, 0); // Move the cursor at origin
    lcd.print("RFID TAG FOUND");

    if (tag == "14723814217")
    {
      digitalWrite(SUCCESS_LED_PIN, HIGH);

      lcd.setCursor(0, 2); // Move the cursor at origin
      lcd.print("TRANSACTION SUCCESS");
    }
    else
    {
      digitalWrite(ERR_LED_PIN, HIGH);

      lcd.setCursor(0, 2); // Move the cursor at origin
      lcd.print("TRANSACTION FAILED");
    }
    turnOnTime = millis();
    Serial.println(turnOnTime);

    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  if (digitalRead(SUCCESS_LED_PIN))
  {
    if (millis() - turnOnTime >= interval)
      digitalWrite(SUCCESS_LED_PIN, LOW);
  }
  else if (digitalRead(ERR_LED_PIN))
  {
    if (millis() - turnOnTime >= interval)
      digitalWrite(ERR_LED_PIN, LOW);
  }

  if (!rfid.PICC_IsNewCardPresent())
    return;
}