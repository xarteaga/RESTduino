#include <Arduino.h>
#include <EthernetClient.h>

inline void fileRequest (const __FlashStringHelper* filePathP, EthernetClient * client) {
  #define NBUFF 8
  #define BUFFLEN 16 // 8*128 = 1024 (Size of packets in bytes)
  char  filePath[16];
  strcpy_P(filePath, (const prog_char*)filePathP); // Convert File path in RAM path from Flash
  size_t i;                // Counter
  uint8_t buffer[BUFFLEN]; // Buffer, the data from the SD and Ethernet are comming and going throught
                           // SPI, therefore, it is needed a buffer
  File file = SD.open(filePath);  // Open SD file
  
  if (file) {
    // read from the file until there's nothing else in it:
    while (file.available()) {
      for (i = 0; i < NBUFF && file.available(); i++){
        // Read BUFFLEN characters and push it in the Socket buffer
        client->pushTx(buffer, file.read(buffer, BUFFLEN));
      }
      // Empty the tx buffer (send buffered data)
      client->flushTx();
    }

    // Close the file:
    file.close();
  } else {
    Serial.print(F("File '"));
    Serial.print(filePath);
    Serial.println(F("' not found!"));
  }
}

int8_t serveIndex (uint8_t method, char* path, EthernetClient *client){
	Serial.println(method);
	Serial.println(path);
	fileRequest(F("/HTMLHEAD.HDR"), client);
	fileRequest(F("/INDEX.TXT"), client);
	client->stop();
	Serial.println(F("Success!"));
	return 0;
}

int8_t serveCss (uint8_t method, char* path, EthernetClient *client){
	Serial.println(method);
	Serial.println(path);
	fileRequest(F("/CSSHEAD.HDR"), client);
	fileRequest(F("/BS.CSS"), client);
	client->stop();
	return 0;
}

int8_t serveCssRes (uint8_t method, char* path, EthernetClient *client){
	Serial.println(method);
	Serial.println(path);
	fileRequest(F("/CSSHEAD.HDR"), client);
	fileRequest(F("/BSR.CSS"), client);
	client->stop();
	return 0;
}

int8_t serveJQuery (uint8_t method, char* path, EthernetClient *client){
	Serial.println(method);
	Serial.println(path);
	fileRequest(F("/JSHEAD.HDR"), client);
	fileRequest(F("/JQUERY.TXT"), client);
	client->stop();
	return 0;
}

int8_t serveControl (uint8_t method, char* path, EthernetClient *client){
	Serial.println(method);
	Serial.println(path);
	fileRequest(F("/JSHEAD.HDR"), client);
	fileRequest(F("/CTRL.TXT"), client);
	client->stop();
	return 0;
}

int8_t serveBs (uint8_t method, char* path, EthernetClient *client){
	Serial.println(method);
	Serial.println(path);
	fileRequest(F("/JSHEAD.HDR"), client);
	fileRequest(F("/BSJS.TXT"), client);
	Serial.println("Success!");
	client->stop();
	return 0;
}

int8_t serveImg (uint8_t method, char* path, EthernetClient *client){
	Serial.println(method);
	Serial.println(path);
	fileRequest(F("/SHIELD.TXT"), client);
	client->stop();
	return 0;
}
