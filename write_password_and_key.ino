#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key defaultKey;
MFRC522::MIFARE_Key newKey;

const byte block = 16;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Scan the RFID card to write password and set key...");

  // Default key (factory reset)
  for (byte i = 0; i < 6; i++) defaultKey.keyByte[i] = 0xFF;

  // Your custom key
  byte tempKey[6] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
  for (byte i = 0; i < 6; i++) newKey.keyByte[i] = tempKey[i];
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  MFRC522::StatusCode status;

  // Authenticate using default key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &defaultKey, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write "Security" to block 16
  byte dataBlock[16] = {'S', 'e', 'c', 'u', 'r', 'i', 't', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  status = mfrc522.MIFARE_Write(block, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  Serial.println("✅ 'Security' written to Block 16.");

  // Now write new key to sector trailer (Block 19 for sector 4)
  byte trailerBlock = 19;
  byte sectorTrailer[16] = {
    0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6,  // Key A
    0xFF, 0x07, 0x80,                    // Access bits
    0x69,                                // User data
    0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6   // Key B (optional)
  };

  status = mfrc522.MIFARE_Write(trailerBlock, sectorTrailer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Failed to write sector trailer: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("✅ New key written successfully to sector trailer (Block 19).");
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(3000);
}
