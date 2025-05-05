#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address (0x27 is the default)
Servo myServo;

MFRC522::MIFARE_Key defaultKey;
MFRC522::MIFARE_Key newKey;

const byte block = 16;
const char expectedPassword[] = "Security"; // 8 characters
const byte servoPin = 3;  // Pin where the servo is connected
bool accessGranted = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Initialize the LCD
  lcd.begin();
  lcd.init();
  lcd.clear();
  lcd.print("Scan your card");

  // Initialize servo
  myServo.attach(servoPin);
  myServo.write(0);  // Set the servo to 0 degrees initially

  // Default key (factory reset)
  for (byte i = 0; i < 6; i++) defaultKey.keyByte[i] = 0xFF;

  // Your custom key
  byte tempKey[6] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
  for (byte i = 0; i < 6; i++) newKey.keyByte[i] = tempKey[i];
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  // Display the card UID on the Serial Monitor
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Authenticate using the new key
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &newKey, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    lcd.clear();
    lcd.print("Access Denied");
    Serial.println("Authentication failed");
    delay(2000);  // Show "Access Denied" for 2 seconds
    lcd.clear();
    lcd.print("Scan your card");  // Prompt again to scan
    return;
  }

  // Read Block 16 to verify the password
  byte buffer[18];
  byte size = sizeof(buffer);
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    lcd.clear();
    lcd.print("Access Denied");
    Serial.println("Reading Block failed");
    delay(2000);
    lcd.clear();
    lcd.print("Scan your card");
    return;
  }

  // Compare the read data with the expected password
  if (strncmp((char*)buffer, expectedPassword, 8) == 0) {
    accessGranted = true;
    lcd.clear();
    lcd.print("Access Granted");
    Serial.println("Password matched: ACCESS GRANTED");

    // Rotate servo to 90 degrees
    myServo.write(90);
    delay(6000);  // Stay in 90 degrees for 6 seconds

    // Rotate servo back to 0 degrees
    myServo.write(0);
    delay(1000);  // Wait for servo to return to 0 degrees
  } else {
    accessGranted = false;
    lcd.clear();
    lcd.print("Access Denied");
    Serial.println("Password mismatch");
    delay(2000);  // Show "Access Denied" for 2 seconds
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1500);  // Pause before scanning the next card
}
