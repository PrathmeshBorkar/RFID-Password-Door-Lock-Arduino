# RFID + Password Based Secure Door Lock System (Arduino)
This project is a secure door access system using an RFID reader and a password check stored directly on the RFID card. Access is granted only when both the UID and the stored password are correct.

# Features
Secure Authentication using:

RFID UID check

Password stored on RFID card (Block 16)

LCD Display (16x2) via I2C for instructions and feedback

Servo Motor to control physical locking/unlocking

Built with Arduino Uno R3 and MFRC522 RFID module

Compact and easy-to-use design

# Components Used
Arduino Uno R3

MFRC522 RFID Module

16x2 LCD Display with I2C

Servo Motor SG90

Breadboard & Jumper wires

RFID Tag/Card (MIFARE Classic 1K)

# How it Works
When powered on, the LCD displays: Mark your attendance or Scan your card.

Upon scanning, the system reads the UID and checks if it's authorized.

It then reads Block 16 of the card and compares the stored password (e.g., Security).

If both UID and password match:

Access is granted

Servo motor rotates to unlock

If verification fails, access is denied.

# Folder Structure
/code/               → Arduino .ino file(s)  
/circuit-diagram/    → Wiring diagram (image or PDF)  
/presentation/       → Project presentation (PPTX or PDF)  

# Setup Instructions
Wire your components as shown in the circuit diagram.

Upload the code from /code/ to your Arduino using the Arduino IDE.

Make sure the RFID card has the password stored in Block 16.

Test by scanning the RFID card.

# Future Improvements
Add Bluetooth/Cloud logging of entries

Admin card or button to add new UIDs

OLED Display instead of LCD

Buzzer and LED indicators

# Troubleshooting

- If the system doesn't respond to your RFID card, try scanning the card using an **NFC reader app** on your smartphone (e.g., “NFC Tools” on Android) to verify:
  - UID is readable
  - Data is stored correctly in Block 19

- Make sure you've written the **correct password** (`Security`) in **Block 19** of your RFID card.

- The code uses a **custom key (`A1 B2 C3 D4 E5 F6`)** to access Block 19. You must ensure this key is written to that block.

- Double-check that you're powering the **RFID module with 3.3V**, not 5V.

- If using a clone MFRC522 module and facing inconsistent readings, try another power source or reduce wire length.
