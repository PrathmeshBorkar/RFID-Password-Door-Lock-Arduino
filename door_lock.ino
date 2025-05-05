#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

MFRC522::MIFARE_Key key;
byte block = 16;
byte trailerBlock = 7;
byte sector = 4;

// Stored key to access block 16
byte knownKeyA[6] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};

// Stored password in block 16
String correctPassword = "Security";

// Authorized UID(s)
String allowedUIDs[] = {
  "81 E3 01 7C"
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.begin();
  lcd.backlight();
  servo.attach(3);
  servo.write(0); // Locked position

  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uidStr = getUIDString();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID: ");
  lcd.setCursor(0, 1);
  lcd.print(uidStr);

  delay(1000);

  if (isAuthorizedUID(uidStr)) {
    if (rfid.PICC_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid))) {
      byte buffer[18];
      byte size = sizeof(buffer);

      // Set key
      for (byte i = 0; i < 6; i++) key.keyByte[i] = knownKeyA[i];

      if (rfid.MIFARE_Read(block, buffer, &size) == MFRC522::STATUS_OK) {
        String readPassword = "";
        for (int i = 0; i < 16; i++) {
          if (buffer[i] != 0x00) {
            readPassword += (char)buffer[i];
          }
        }

        if (readPassword == correctPassword) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Granted");
          servo.write(90); // Open
          delay(5000);
          servo.write(0); // Close
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password");
        }
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Read Failed");
      }

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Auth Failed");
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unauthorized");
  }

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

String getUIDString() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

bool isAuthorizedUID(String uid) {
  for (String id : allowedUIDs) {
    if (uid == id) return true;
  }
  return false;
}
