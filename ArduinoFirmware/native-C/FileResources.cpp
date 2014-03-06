#include <Arduino.h>
#include <SD.h>
#include <EthernetClient.h>

int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void fileRequest (const __FlashStringHelper* filePathP, EthernetClient * client) {
  #define NBUFF 8
  #define BUFFLEN 128 // 8*128 = 1024 (Size of packets in bytes)
  char  filePath[16];
  strcpy_P(filePath, (const prog_char*)filePathP); // Convert File path in RAM path from Flash
  size_t i;                // Counter
  uint8_t buffer[BUFFLEN]; // Buffer, the data from the SD and Ethernet are comming and going throught
                           // SPI, therefore, it is needed a buffer
  File file = SD.open(filePath);  // Open SD file
  Serial.println(freeRam());
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

void serveIndex (uint8_t method, char* path, EthernetClient *client){
	fileRequest(F("/HTMLHEAD.HDR"), client);
	fileRequest(F("/INDEX.TXT"), client);
}

void serveCss (uint8_t method, char* path, EthernetClient *client){
	fileRequest(F("/CSSHEAD.HDR"), client);
	fileRequest(F("/BS.CSS"), client);
}

void serveCssRes (uint8_t method, char* path, EthernetClient *client){
	fileRequest(F("/CSSHEAD.HDR"), client);
	fileRequest(F("/BSR.CSS"), client);
}

void serveJQuery (uint8_t method, char* path, EthernetClient *client){
	fileRequest(F("/JSHEAD.HDR"), client);
	fileRequest(F("/JQUERY.TXT"), client);
}

void serveControl (uint8_t method, char* path, EthernetClient *client){
	fileRequest(F("/JSHEAD.HDR"), client);
	fileRequest(F("/CTRL.TXT"), client);
}

void serveBs (uint8_t method, char* path, EthernetClient *client){
	fileRequest(F("/JSHEAD.HDR"), client);
	fileRequest(F("/BSJS.TXT"), client);
}

void serveImg (uint8_t method, char* path, EthernetClient *client){
	fileRequest(F("/SHIELD.TXT"), client);
}
