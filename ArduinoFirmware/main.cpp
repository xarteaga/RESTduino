// Definitions
#define SERIAL_BAUDRATE 115200

// Compilation options
//#define DEBUG_EEPROM
//#define DEBUG
#define PRINT_HEADERS
#define VERBOSE
#define ALLOW_CACHE

// Includes
#include <inttypes.h>
#include <IPAddress.h>
#include <SD.h>
#include <HttpServer.h>

#include "FileResources.h"
#include "DeviceResource.h"
#include "SensorPlatform.h"

HttpServer server;

void notFound (uint8_t method, char* path, EthernetClient *client){
    client->println("HTTP/1.1 404 - File not Found\n");
	client->stop();
	return;
}

void setup() {
	// Start serial port
	Serial.begin(SERIAL_BAUDRATE);

	// Setup IP interfaces
	byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
	IPAddress ip = IPAddress(192, 168, 1, (uint8_t) 2);
	IPAddress dns = IPAddress(147, 83, 2, 3);
	IPAddress gw = IPAddress(192, 168, 1, 1);
	IPAddress subnet = IPAddress(255, 255, 255, 0);
	uint16_t port = 80;

	// Push server entries
	server.pushEntry(GET, F("/index.html"), serveIndex);
	server.pushEntry(GET, F("/bs.css"), serveCss);
	server.pushEntry(GET, F("/bs-res.css"), serveCssRes);
	server.pushEntry(GET, F("/jquery.js"), serveJQuery);
	server.pushEntry(GET, F("/control.js"), serveControl);
	server.pushEntry(GET, F("/bs.js"), serveBs);
	server.pushEntry(GET, F("/shield.jpg"), serveImg);
	server.pushEntry(GET, F("/ports"), portsRequest);
	server.pushEntry(GET, F("/config"), configRequest);
	server.pushEntry(GET, F("/set"), configSet);
	server.pushEntry(GET, F("/out"), setOutput);
	server.pushEntry(GET, F("/favicon.ico"), notFound);
	server.pushEntry(GET, F("/"), serveIndex);

	// Start server
	server.start(mac, ip, dns, gw, subnet, port);

	// Start SD service
	digitalWrite(10, LOW);
	digitalWrite(10, HIGH);
	if (!SD.begin(4))
		Serial.println(F("SD Initialization failed!"));

	else {
		Serial.println(F("SD Initialization Ok!"));
	}

	// Load configuration
	loadConfig();
}

void loop() {
	char c;
	if (Serial.available()){
		c = Serial.read();
		Serial.print(c);
	}
	server.proccess();
	updateValues();
}

