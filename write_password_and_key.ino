#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte block = 19;
String password = "Security"; // 8 characters max for simplicity
byte customKeyA[6] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Scan your card to write password...");

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; // default factory key
  }
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  // Authenticate block 19
  if (rfid.PICC_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid)) != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return;
  }

  // Write password to block 19
  byte buffer[16];
  for (int i = 0; i < 16; i++) buffer[i] = 0x00; // Clear all first
  for (int i = 0; i < password.length(); i++) buffer[i] = password[i];

  MFRC522::StatusCode status = rfid.MIFARE_Write(block, buffer, 16);
  if (status == MFRC522::STATUS_OK) {
    Serial.println("Password written to block 19");
  } else {
    Serial.print("Write failed: "); Serial.println(rfid.GetStatusCodeName(status));
  }

  // Change Key A of block 19 to custom key
  byte trailerBlock = 23; // Trailer block for block 19
  byte sectorTrailer[16] = {
    customKeyA[0], customKeyA[1], customKeyA[2], customKeyA[3], customKeyA[4], customKeyA[5], // Key A
    0xFF, 0x07, 0x80, // Access Bits
    0x69, 0x69, 0x69, // Unused
    0xFF, 0xFF, 0xFF, 0xFF // Key B (optional, not used)
  };

  status = rfid.MIFARE_Write(trailerBlock, sectorTrailer, 16);
  if (status == MFRC522::STATUS_OK) {
    Serial.println("Key A updated for block 19");
  } else {
    Serial.print("Failed to write key: "); Serial.println(rfid.GetStatusCodeName(status));
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(3000);
}
