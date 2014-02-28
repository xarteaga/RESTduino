// Definitions
#define SERIAL_BAUDRATE 115200
#define REQUEST_MAXBUFFER 30
#define HEADER_MAXBUFFER 30
#define DEV_NAME_MAX_LEN 16
// Compilation options
//#define DEBUG_EEPROM
//#define DEBUG
#define PRINT_HEADERS
#define VERBOSE
#define ALLOW_CACHE

// Includes
#include <inttypes.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include "HttpServer.h"

HttpServer server;

void fileRequest (const __FlashStringHelper* filePathP, EthernetClient * client) {
  #define NBUFF 8
  #define BUFFLEN 64 // 8*128 = 1024 (Size of packets in bytes)
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

void setup() {
	Serial.begin(SERIAL_BAUDRATE);

	byte mac [] = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED};
	IPAddress ip = IPAddress((uint8_t)192,(uint8_t)168,(uint8_t)10,(uint8_t)2);
	IPAddress dns = IPAddress((uint8_t)147,(uint8_t)83,(uint8_t)2,(uint8_t)3);
	IPAddress gw = IPAddress((uint8_t)192,(uint8_t)168,(uint8_t)10,(uint8_t)1);
	IPAddress subnet = IPAddress((uint8_t)255,(uint8_t)255,(uint8_t)255,(uint8_t)0);
	uint16_t port = 80;

	server.pushEntry(GET, "/index.html", serveIndex);
	server.pushEntry(GET, "/bs.css", serveCss);
	server.pushEntry(GET, "/bs-res.css", serveCssRes);
	server.pushEntry(GET, "/jquery.js", serveJQuery);
	server.pushEntry(GET, "/control.js", serveControl);
	server.pushEntry(GET, "/bs.js", serveBs);
	server.pushEntry(GET, "/shield.jpg", serveBs);
	server.pushEntry(GET, "/", serveIndex);
	server.start(mac, ip, dns, gw, subnet, port);

	// Create SD
	digitalWrite(10, LOW);
	digitalWrite(10, HIGH);
	if (!SD.begin(4)) {
		while(true){
			Serial.println(F("SD Initialization failed!"));
			delay(1000);
		}
	}
}

void loop() {
  server.proccess();
}

