# ArDoorLock
Door lock opener based on Arduino Uno, Ethernet Shield and RFID RC522

Arduino is asking server via REST api for how long should be the door unlocked for read Mifare card UID. The solution is working (tested) with Mifare Classic, Mifare Mini and DESFire.

Wiring is pretty straightforward:
Ethernet Shield (w5100) on top of Arduino Uno

RST_PIN		 4   //reset pin for RC522 (RST)
RFID_SELECT_PIN	 3   //select pin for RC522 (SDA)
LOCK_PIN       	 5   //door lock pin

RC522  | Ethernet Shield
MISO    | 12
MOSI    | 11
SCK      | 13

Power as usual.

Key lock is connected via simple relay module.

# Important note!
My old Ethernet Shield, probably version 2, has untreated W5100 design failure. Thus the SPI bus is kept used even when Slave Select pin (10) is HIGH. So the communication with RFID RC522 is not working. 

There are two possible solutions:
1) Solder one wire from W5100 pin SEN - pin 31 to some arduino pin and keep it down then you would like to use SPI for some other component.

2) Add logical negation between pin Ethernet Slave Select (Arduino Uno 10) and W5100 pin SEN - pin 31. With for example 7440 (B140D) or similar NAND. It's more complicated but you gonna save one pin and it makes the code bit easier. 
