#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <TextFinder.h>

#define RST_PIN		4		        // reset pin for RC522 (RST)
#define RFID_SELECT_PIN		3		// select pin for RC522 (SDA)
#define LOCK_PIN        5                       // door lock pin

MFRC522 mfrc522(RFID_SELECT_PIN, RST_PIN);	// Create MFRC522 instance
// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,0,177);
IPAddress gw(192,168,0,1);
IPAddress mask(255,255,255,0);

// fill in your Domain Name Server address here:
IPAddress myDns(192,168,0,1);

// initialize the library instance:
EthernetClient client;

char server[] = "tomsik.eu"; //change the server address to your server!!!
unsigned long timeWhenDoorsOpened = 0;
unsigned long howLongKeepDoorsOpen = 0;



void setup() {
  pinMode(10, OUTPUT); //ethetnet
  pinMode(ETH_SPE, OUTPUT); //ethetnet
  pinMode(RFID_SELECT_PIN, OUTPUT);       
  pinMode(LOCK_PIN, OUTPUT);       

  Serial.begin(9600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();			// Init SPI bus        

  selectRC522();
  mfrc522.PCD_Init();		// Init MFRC522
  ShowReaderDetails();	// Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));        

  selectEthernet();
  Ethernet.begin(mac, ip, myDns,gw,mask);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());        
}

void loop() {
      if (howLongKeepDoorsOpen > millis() - timeWhenDoorsOpened){
        digitalWrite(LOCK_PIN,HIGH);
        
      }
      else{
        digitalWrite(LOCK_PIN,LOW);
      }
      
  selectEthernet();
  if(!client.connected()) {
    selectRC522();
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

    Serial.println();
    selectEthernet();

    httpRequest(mfrc522.uid.uidByte,mfrc522.uid.size);
  }

  TextFinder  finder(client );   
  if (client.connected()) {
    //    char c = client.read();
    //    Serial.print(c);
    if( finder.find("open:")){
      howLongKeepDoorsOpen = finder.getValue()*1000;
      timeWhenDoorsOpened = millis();
      Serial.print(F("Doors will be open: "));
      Serial.print(howLongKeepDoorsOpen);
      Serial.println(F(" milliseconds"));
    }
    client.stop();
  }
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}

void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void selectEthernet(){        
  digitalWrite(10, LOW);
  digitalWrite(RFID_SELECT_PIN,HIGH);
  delay(10);
}
void selectRC522(){        
  digitalWrite(10, HIGH);
  digitalWrite(RFID_SELECT_PIN,LOW);
  delay(10);
}

void httpRequest(byte *card_id, byte id_size) {
  Serial.println("creating request...");
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print(F("GET /rfid.php?door=1&cardnr="));
    for (byte i = 0; i < id_size; i++) {
      client.print(card_id[i] < 0x10 ? "0" : "");
      client.print(card_id[i], HEX);
    }

    client.println(F(" HTTP/1.1"));
    client.println(F("Host: tomsik.eu"));
    client.println(F("User-Agent: arduino-ethernet"));
    client.println(F("Connection: close"));
    client.println();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}





